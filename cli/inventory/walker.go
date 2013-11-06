package main

import (
	"fmt"
	"io"
	"math"
	"strings"

	"code.google.com/p/go-sqlite/go1/sqlite3"
)

// The number of sql commands to buffer before dumping to the output database.
const DumpFreq = 100000

var (
	preExecStmts = []string{
		"DROP TABLE IF EXISTS Inventories",
		"CREATE TABLE Inventories (SimID TEXT,ResID INTEGER,AgentID INTEGER,StartTime INTEGER,EndTime INTEGER);",
		Index("Resources", "SimID", "ID"),
		Index("Resources", "Parent1"),
		Index("Resources", "Parent2"),
		Index("Resources", "StateID"),
		Index("Compositions", "ID"),
		Index("Compositions", "IsoID"),
		Index("Transactions", "ID"),
		Index("Transactions", "Time"),
		Index("Transactions", "ReceiverID"),
		Index("TransactedResources", "TransactionID"),
		Index("TransactedResources", "ResourceID"),
		Index("ResCreators", "SimID", "ResID"),
		Index("Agents", "Prototype"),
		Index("Agents", "ID"),
	}
	postExecStmts = []string{
		Index("Inventories", "SimID", "AgentID"),
		Index("Inventories", "SimID", "StartTime"),
		Index("Inventories", "SimID", "EndTime"),
	}
	dumpSql    = "INSERT INTO Inventories VALUES (?,?,?,?,?);"
	resSqlHead = "SELECT ID,TimeCreated FROM "
	resSqlTail = " WHERE Parent1 = ? OR Parent2 = ?;"

	ownerSql = `SELECT tr.ReceiverID, tr.Time FROM Transactions AS tr
				  INNER JOIN TransactedResources AS trr ON tr.ID = trr.TransactionID
				  WHERE trr.ResourceID = ? AND tr.SimID = ? AND trr.SimID = ?
				  ORDER BY tr.Time ASC;`
	rootsSql = `SELECT res.ID,res.TimeCreated,rc.ModelID FROM Resources AS res
				  INNER JOIN ResCreators AS rc ON res.ID = rc.ResID
				  WHERE res.SimID = ? AND rc.SimID = ?;`
)

// Prepare creates necessary indexes and tables required for efficient
// calculation of cyclus simulation inventory information.  Should be called
// once before walking begins.
func Prepare(conn *sqlite3.Conn) (err error) {
	fmt.Println("Creating indexes and inventory table...")
	for _, sql := range preExecStmts {
		if err := conn.Exec(sql); err != nil {
			fmt.Println("    ", err)
		}
	}
	return nil
}

// Finish should be called for a cyclus database after all walkers have
// completed processing inventory data. It creates final indexes and other
// finishing tasks.
func Finish(conn *sqlite3.Conn) (err error) {
	fmt.Println("Creating inventory indexes...")
	for _, sql := range postExecStmts {
		if err := conn.Exec(sql); err != nil {
			return err
		}
	}
	return nil
}

type Node struct {
	ResId     int
	OwnerId   int
	StartTime int
	EndTime   int
}

// Context encapsulates the logic for building a fast, queryable inventories
// table for a specific simulation from raw cyclus output database.
type Context struct {
	*sqlite3.Conn
	// Simid is the cyclus simulation id targeted by this context.  Must be
	// set.
	Simid       string
	mappednodes map[int32]struct{}
	tmpResTbl   string
	tmpResStmt  *sqlite3.Stmt
	dumpStmt    *sqlite3.Stmt
	ownerStmt   *sqlite3.Stmt
	resCount    int
	nodes       []*Node
	History chan string
}

func NewContext(conn *sqlite3.Conn, simid string, history chan string) *Context {
	return &Context{
		Conn: conn,
		Simid: simid,
		History: history,
	}
}

func (c *Context) init() {
	c.nodes = make([]*Node, 0, 10000)
	c.mappednodes = map[int32]struct{}{}

	// create temp res table without simid
	fmt.Println("Creating temporary resource table...")
	c.tmpResTbl = "tmp_restbl_" + strings.Replace(c.Simid, "-", "_", -1)
	err := c.Exec("DROP TABLE IF EXISTS " + c.tmpResTbl)
	panicif(err)

	sql := "CREATE TABLE " + c.tmpResTbl + " AS SELECT ID,TimeCreated,Parent1,Parent2 FROM Resources WHERE SimID = ?;"
	err = c.Exec(sql, c.Simid)
	panicif(err)

	fmt.Println("Indexing temporary resource table...")
	err = c.Exec(Index(c.tmpResTbl, "Parent1"))
	panicif(err)

	err = c.Exec(Index(c.tmpResTbl, "Parent2"))
	panicif(err)

	// create prepared statements
	c.tmpResStmt, err = c.Prepare(resSqlHead + c.tmpResTbl + resSqlTail)
	panicif(err)

	c.dumpStmt, err = c.Prepare(dumpSql)
	panicif(err)

	c.ownerStmt, err = c.Prepare(ownerSql)
	panicif(err)
}

// WalkAll constructs the inventories table in the cyclus database alongside
// other tables. Creates several indexes in the process.  Finish should be
// called on the database connection after all simulation id's have been
// walked.
func (c *Context) WalkAll() (err error) {
	defer func() {
		if r := recover(); r != nil {
			err = r.(error)
		}
	}()

	fmt.Printf("--- Building inventories for simid %v ---\n", c.Simid)
	c.init()

	fmt.Println("Retrieving root resource nodes...")
	roots := c.getRoots()

	fmt.Printf("Found %v root nodes\n", len(roots))
	for i, n := range roots {
		fmt.Printf("    Processing root %d...\n", i)
		c.walkDown(n)
	}

	fmt.Println("Dropping temporary resource table...")
	err = c.Exec("DROP TABLE " + c.tmpResTbl)
	panicif(err)

	c.dumpNodes()

	return nil
}

func (c *Context) getRoots() (roots []*Node) {
	sql := "SELECT COUNT(*) FROM ResCreators WHERE SimID = ?"
	stmt, err := c.Query(sql, c.Simid)
	panicif(err)

	n := 0
	err = stmt.Scan(&n)
	panicif(err)
	stmt.Reset()

	roots = make([]*Node, 0, n)
	for stmt, err = c.Query(rootsSql, c.Simid, c.Simid); err == nil; err = stmt.Next() {
		node := &Node{EndTime: math.MaxInt32}
		err := stmt.Scan(&node.ResId, &node.StartTime, &node.OwnerId)
		panicif(err)

		roots = append(roots, node)
	}
	if err != io.EOF {
		panic(err.Error())
	}
	return roots
}

func (c *Context) walkDown(node *Node) {
	if _, ok := c.mappednodes[int32(node.ResId)]; ok {
		return
	}
	c.mappednodes[int32(node.ResId)] = struct{}{}

	// dump if necessary
	c.resCount++
	if c.resCount%DumpFreq == 0 {
		c.dumpNodes()
	}

	// find resource's children
	kids := make([]*Node, 0, 2)
	err := c.tmpResStmt.Query(node.ResId, node.ResId)
	for ; err == nil; err = c.tmpResStmt.Next() {
		child := &Node{EndTime: math.MaxInt32}
		err := c.tmpResStmt.Scan(&child.ResId, &child.StartTime)
		panicif(err)
		node.EndTime = child.StartTime
		kids = append(kids, child)
	}
	if err != io.EOF {
		panic(err.Error())
	}

	// find resources owner changes (that occurred before children)
	owners, times := c.getNewOwners(node.ResId)

	childOwner := node.OwnerId
	if len(owners) > 0 {
		node.EndTime = times[0]
		childOwner = owners[len(owners)-1]

		lastend := math.MaxInt32
		if len(kids) > 0 {
			lastend = kids[0].StartTime
		}
		times = append(times, lastend)
		for i := range owners {
			n := &Node{ResId: node.ResId, OwnerId: owners[i], StartTime: times[i], EndTime: times[i+1]}
			c.nodes = append(c.nodes, n)
		}
	}

	c.nodes = append(c.nodes, node)

	// walk down resource's children
	for _, child := range kids {
		child.OwnerId = childOwner
		c.walkDown(child)
	}
}

func (c *Context) getNewOwners(id int) (owners, times []int) {
	var owner, t int
	err := c.ownerStmt.Query(id, c.Simid, c.Simid)
	for ; err == nil; err = c.ownerStmt.Next() {
		err := c.ownerStmt.Scan(&owner, &t)
		panicif(err)

		if id == owner {
			continue
		}
		owners = append(owners, owner)
		times = append(times, t)
	}
	if err != io.EOF {
		panic(err.Error())
	}
	return owners, times
}

func (c *Context) dumpNodes() {
	fmt.Printf("    Dumping inventories (%d resources done)...\n", c.resCount)
	err := c.Exec("BEGIN TRANSACTION;")
	panicif(err)

	for _, n := range c.nodes {
		err = c.dumpStmt.Exec(c.Simid, n.ResId, n.OwnerId, n.StartTime, n.EndTime)
		panicif(err)
		if c.History != nil {
			sql := fmt.Sprintf("INSERT INTO Inventories VALUES('%v',%v,%v,%v,%v);", c.Simid, n.ResId, n.OwnerId, n.StartTime, n.EndTime)
			c.History <- sql
		}
	}
	err = c.Exec("END TRANSACTION;")
	panicif(err)

	c.nodes = c.nodes[:0]
}
