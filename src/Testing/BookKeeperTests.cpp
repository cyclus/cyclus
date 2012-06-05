#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>

#include "Utility/Env.h"
#include "Utility/BookKeeper.h"
#include "Utility/Table.h"


class BookKeeperTest : public ::testing::Test {
  protected:

  // some test strings
  file_path fpath;
  std::string test_filename;
  table_ptr test_table;
  
  // this sets up the fixtures
  virtual void SetUp() {
    BI->turnLoggingOn();
    fpath = Env::checkEnv("PWD");
    test_filename = "testBK.sqlite";
    table_ptr test_table = new Table("test_tbl"); 
  };
  
  // this tears down the fixtures
  virtual void TearDown() {
    if ( BI->dbIsOpen() ) {
      BI->closeDB();
    }
    BI->turnLoggingOff();
    std::string test_file = fpath + "/" + test_filename;
    if( std::remove( test_file.c_str() ) != 0 )
      std::cout << "Error deleting file " << test_filename ;
  };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, createDB) {
  EXPECT_NO_THROW( BI->createDB(test_filename,fpath) );
  EXPECT_EQ( BI->dbExists(), true );
  EXPECT_EQ( BI->dbName(), test_filename );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, openDB) {
  EXPECT_NO_THROW( BI->openDB() );
  EXPECT_EQ( BI->dbIsOpen(), true );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, closeDB) {
  EXPECT_NO_THROW( BI->openDB() );
  EXPECT_EQ( BI->dbIsOpen(), true );
  EXPECT_NO_THROW( BI->closeDB() );
  EXPECT_EQ( BI->dbIsOpen(), false );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, loggingOnOff){
  BI->turnLoggingOn();
  EXPECT_EQ( BI->loggingIsOn(), true );
  EXPECT_NO_THROW( BI->turnLoggingOff() );
  EXPECT_EQ( BI->loggingIsOn(), false );
  BI->turnLoggingOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, tableTests){
  EXPECT_NO_THROW( BI->registerTable(test_table) );
  EXPECT_EQ( BI->nTables(), 1 );
  EXPECT_NO_THROW( BI->tableAtThreshold(test_table) );
  EXPECT_NO_THROW( BI->removeTable(test_table) );
  EXPECT_EQ( BI->nTables(), 0 );
}
