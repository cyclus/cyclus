package main

import (
	"bytes"
	"io"
	"log"
	"time"

	"code.google.com/p/go-sqlite/go1/sqlite3"
)

// Index builds an sql statement for creating a new index on the specified
// table over cols.  The index is named according to the table and cols.
func Index(table string, cols ...string) string {
	var buf bytes.Buffer
	buf.WriteString("CREATE INDEX IF NOT EXISTS ")
	buf.WriteString(table + "_" + cols[0])
	for _, c := range cols[1:] {
		buf.WriteString("_" + c)
	}
	buf.WriteString(" ON " + table + " (" + cols[0] + " ASC")
	for _, c := range cols[1:] {
		buf.WriteString("," + c + " ASC")
	}
	buf.WriteString(");")
	return buf.String()
}

// GetSimIds returns a list of all simulation ids in the cyclus database for
// conn.
func GetSimIds(conn *sqlite3.Conn) (ids []string, err error) {
	sql := "SELECT SimID FROM SimulationTimeInfo"
	var stmt *sqlite3.Stmt
	for stmt, err = conn.Query(sql); err == nil; err = stmt.Next() {
		var s string
		if err := stmt.Scan(&s); err != nil {
			return nil, err
		}
		ids = append(ids, s)
	}
	if err != io.EOF {
		return nil, err
	}
	return ids, nil
}

func fatalif(err error) {
	if err != nil {
		log.Fatal(err)
	}
}

func panicif(err error) {
	if err != nil {
		panic(err.Error())
	}
}

type Timer struct {
	starts map[string]time.Time
	Totals map[string]time.Duration
}

func NewTimer() *Timer {
	return &Timer{
		map[string]time.Time{},
		map[string]time.Duration{},
	}
}

func (t *Timer) Start(label string) {
	if _, ok := t.starts[label]; !ok {
		t.starts[label] = time.Now()
	}
}

func (t *Timer) Stop(label string) {
	if start, ok := t.starts[label]; ok {
		t.Totals[label] += time.Now().Sub(start)
	}
	delete(t.starts, label)
}
