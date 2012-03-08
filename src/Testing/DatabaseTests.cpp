#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "Env.h"
#include "Utility/Database.h"
#include "Utility/Table.h"

class DatabaseTest : public ::testing::Test {
  protected:
  // declare useful data members
  std::string dbName, dbPath, tbl_name, tst_query;
  Database* db;
  table_ptr tbl;
  query_result qr;
  int r1;
  double r2;
  std::string r3;
 
  // define the test table
  void define_table() {
    column int_col("int","INTEGER");
    column dbl_col("dbl","REAL");
    column str_col("str","VARCHAR(128)");
    tbl->setPrimaryKey(int_col);
    tbl->addColumn(int_col);
    tbl->addColumn(dbl_col);
    tbl->addColumn(str_col);
    tbl->tableDefined();
  }
  
  // add a row for testing
  void add_row_to_table() {
    data an_int(r1), a_dbl(r2), a_str(r3);
    entry i("int",an_int), d("dbl",a_dbl), s("str",a_str);
    row r;
    r.push_back(i);
    r.push_back(d);
    r.push_back(s);
    tbl->addRow(r);
  }

  // set up the table for this test
  void setUpTable() {
    define_table();
    add_row_to_table();
  };

  // this sets up the fixtures
  virtual void SetUp() {
    dbPath = Env::checkEnv("CYCLUS_OUT_DIR") + "/Testing/Temporary";
    dbName = "testDB.sqlite";
    db = new Database(dbName,dbPath);
    tbl_name = "test_table";
    tst_query = "select * from " + tbl_name;
    r1 = 0;
    r2 = 1.2;
    r3 = "hello world";
    tbl = new Table(tbl_name);
    setUpTable();
  };
  
  // this tears down the fixtures
  virtual void TearDown() {};
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DatabaseTest, initTest) {
  EXPECT_EQ( db->dbExists(), true );
  EXPECT_EQ( db->name(), dbName );
  EXPECT_EQ( db->isOpen(), false );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DatabaseTest, dbOpenClose) {
  EXPECT_EQ( db->isOpen(), false );
  EXPECT_NO_THROW( db->open() );
  EXPECT_EQ( db->isOpen(), true );
  EXPECT_NO_THROW( db->close() );
  EXPECT_EQ( db->isOpen(), false );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(DatabaseTest, testTableClassIntegration) {
  // creation
  std::string full_path = dbPath + "/" + dbName;
  if ( db->fexists(full_path.c_str()) ) {
    remove (full_path.c_str());
  }
  EXPECT_NO_THROW( db->open() );
  EXPECT_NO_THROW( db->registerTable(tbl) );
  EXPECT_EQ( db->nTables(), 1 );
  EXPECT_EQ( db->tablePtr(0), tbl );
  EXPECT_NO_THROW( db->createTable(tbl) );
  // writing
  EXPECT_NO_THROW( db->writeRows(tbl) );
  // querying
  qr = db->query(tst_query);
  query_row qrow = qr.at(0);
  EXPECT_EQ( atoi( qrow.at(0).c_str() ), r1 );
  EXPECT_EQ( atof( qrow.at(1).c_str() ), r2 );
  EXPECT_EQ( qrow.at(2), r3 );
  // clean up
  EXPECT_NO_THROW( db->removeTable(tbl) );
  EXPECT_EQ( db->nTables(), 0 );
  EXPECT_NO_THROW( db->close() );
}
