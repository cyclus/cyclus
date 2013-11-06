
package main

import (
	"testing"
	"os"

	"code.google.com/p/go-sqlite/go1/sqlite3"
)

const tmpDbFile = "/tmp/cyclus_inv_test_db.sqlite"

func TestRegression1(t *testing.T) {
	if err := os.RemoveAll(tmpDbFile); err != nil {
		t.Fatal(err)
	}

	conn, err := sqlite3.Open(tmpDbFile)
	if err != nil {
		t.Fatal(err)
	}

	for _, sql := range rawSimSql {
		if err := conn.Exec(sql); err != nil {
			t.Fatal(err)
		}
	}

	if err := Prepare(conn); err != nil {
		t.Fatal(err)
	}

	simids, err := GetSimIds(conn)
	if err != nil {
		t.Fatal(err)
	}

	ch := make(chan string, 1000)
	for _, simid := range simids {
		ctx := NewContext(conn, simid, ch)
		if err := ctx.WalkAll(); err != nil {
			t.Fatal(err)
		}
	}
	close(ch)

	i := 0
	for sql := range ch {
		if sql != inventorySql[i] {
			t.Errorf("[node %v] expected \"%s\", got \"%s\"", i, inventorySql[i], sql)
		}
		i++
	}

	if err := Finish(conn); err != nil {
		t.Fatal(err)
	}

}
