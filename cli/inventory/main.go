// inventory creates a fast-queryable agent inventory table for a cyclus output db.
package main

import (
	"fmt"
	"flag"
	"log"

	"code.google.com/p/go-sqlite/go1/sqlite3"
)

var help = flag.Bool("h", false, "Print this help message.")

func main() {
	log.SetFlags(0)
	flag.Parse()

	if *help || flag.NArg() != 1 {
		fmt.Println("Usage: inventory [cyclus-db]")
		fmt.Println("Creates a fast queryable inventory table for a cyclus sqlite output file.\n")
		flag.PrintDefaults()
		return
	}


	fname := flag.Arg(0)

	conn, err := sqlite3.Open(fname)
	fatalif(err)
	defer conn.Close()

	fatalif(Prepare(conn))
	defer Finish(conn)

	simids, err := GetSimIds(conn)
	fatalif(err)

	for _, simid := range simids {
		ctx := NewContext(conn, simid, nil)
		fatalif(ctx.WalkAll())
	}
}
