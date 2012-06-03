#include <gtest/gtest.h>
#include <stdlib.h>
#include <stdio.h>

#include "Utility/Table.h"

class TableTest : public ::testing::Test {
protected:
  // member vars used in testing
  table_ptr test_table;
  std::string name,diff_name;
  std::string iLabel, dLabel, sLabel, diffLabel;
  std::string pkLabel, fkLabel;
  int ival;
  double dval1, dval2;
  std::string sval;
  command *createTest, *rowTest, *updateTest, 
    *fkTest, *pkTest;
  primary_key_ref* pkref;
  
    
  // this sets up the fixtures
  virtual void SetUp() {
    // create and define the test table
    name = "Test", diff_name = "OtherTable";
    test_table = new Table(name);
    pkref = new primary_key_ref();
    iLabel = "IntVal", dLabel = "DblVal", sLabel = "StrVal",
    pkLabel = iLabel, fkLabel = sLabel;
    diffLabel ="aKey";
    ival = 1, dval1 = 2.3, dval2 = 3.4, sval = "hello world";
    // define the testing commands
    createTest = new command("");
    (*createTest) << "CREATE TABLE " << name << " (" << iLabel << " INTEGER, "
                  << dLabel << " REAL, " << sLabel << " VARCHAR(128),"
                  << " PRIMARY KEY (" << iLabel << "), FOREIGN KEY"
                  << " (" << sLabel << ") REFERENCES " << diff_name 
                  << "(" << diffLabel << "));";
    rowTest = new command("");
    (*rowTest) << "INSERT INTO " << name << " (" << iLabel << ", " << dLabel 
               << ", " << sLabel << ")" << " VALUES (" << ival << ", " 
               << dval1 << ", '" << sval << "');";
    updateTest = new command("");
    (*updateTest) << "UPDATE " << name << " SET " << dLabel << "=" << dval2
                  << " WHERE " << iLabel << "=" << ival << ";";
    pkTest = new command("");
    (*pkTest) << "PRIMARY KEY (" << pkLabel << ")";
    fkTest = new command("");
    (*fkTest) << "FOREIGN KEY (" << fkLabel << ") REFERENCES "
              << diff_name << "(" << diffLabel <<")";
  };
  
  // this tears down the fixtures
  virtual void TearDown() {};
  
  // define the table for testing
  void define_test_table(table_ptr tbl) {
    tbl->addField(iLabel,"INTEGER");
    tbl->addField(dLabel,"REAL");
    tbl->addField(sLabel,"VARCHAR(128)");
    tbl->setPrimaryKey(iLabel);
    foreign_key_ref *fkref;
    foreign_key *fk;
    key myk, theirk;
    theirk.push_back(diffLabel);
    fkref = new foreign_key_ref(diff_name,theirk);
    myk.push_back(sLabel);
    fk = new foreign_key(myk, (*fkref) );
    tbl->addForeignKey( (*fk) );
    tbl->tableDefined();
  };
  
  // add a row to the table for testing
  void add_row_to_test_table(table_ptr tbl) {
    data int_val(ival), dbl_val(dval1), str_val(sval);
    entry i(iLabel,int_val), d(dLabel,dbl_val), s(sLabel,str_val);
    row aRow;
    aRow.push_back(i), aRow.push_back(d), aRow.push_back(s);
    tbl->addRow(aRow);
    pkref->push_back(i);
  };
  
  // update a row for testing
  void update_row_to_test_table(table_ptr tbl) {
    data dbl_val(dval2);
    entry d(dLabel,dbl_val);
    row aRow;
    aRow.push_back(d);
    tbl->updateRow( (*pkref),aRow );
  };
  
};

// to test
// fkey, pkey

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(TableTest, ConstructTable) {
  EXPECT_NO_THROW( define_test_table(test_table) );
  EXPECT_EQ( test_table->defined(), true );
  EXPECT_EQ( test_table->name(), name );
  EXPECT_EQ( test_table->create(), createTest->str() );
  EXPECT_EQ( test_table->p_key(), pkTest->str() );
  EXPECT_EQ( test_table->f_keys(), fkTest->str() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(TableTest, AddRow) {
  EXPECT_EQ( test_table->nRows(), 0 );
  EXPECT_NO_THROW( add_row_to_test_table(test_table) );
  EXPECT_EQ( test_table->nRows(), 1 );
  EXPECT_EQ( test_table->row_command(0)->str(), rowTest->str() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(TableTest, UpdateRow) {
  EXPECT_EQ( test_table->nRows(), 0 );
  EXPECT_NO_THROW( add_row_to_test_table(test_table) );
  EXPECT_EQ( test_table->row_command(0)->str(), rowTest->str() );
  EXPECT_EQ( test_table->nRows(), 1 );
  EXPECT_NO_THROW( update_row_to_test_table(test_table) );
  EXPECT_EQ( test_table->nRows(), 2 );
  EXPECT_EQ( test_table->row_command(1)->str(), updateTest->str() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(TableTest, FlushRows) {
  EXPECT_EQ( test_table->nRows(), 0 );
  EXPECT_NO_THROW( add_row_to_test_table(test_table) );
  EXPECT_EQ( test_table->nRows(), 1 );
  EXPECT_NO_THROW( update_row_to_test_table(test_table) );
  EXPECT_EQ( test_table->nRows(), 2 );
  EXPECT_NO_THROW( test_table->flush() );
  EXPECT_EQ( test_table->nRows(), 0 );
}
