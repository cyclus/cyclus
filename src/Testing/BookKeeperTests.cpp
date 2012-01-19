#include <gtest/gtest.h>
#include "Utility/BookKeeper.h"
#include <stdlib.h>
#include <stdio.h>

using namespace std;

class BookKeeperTest : public ::testing::Test {
  protected:

    // these integers will be the dimensions of the databases
    int n, nrows, ncols, nlayers;

    // some test strings
    string nstr, test_filename, test_dsp_name, test_type, 
           test_group_name, test_nonsense;

    // a buffer
    char buffer[16];

    // this sets up the fixtures
    virtual void SetUp(){
      n=1;
      nrows=10;
      ncols=5;
      nlayers=3;
      test_filename = "testDB.h5";
      test_dsp_name = "testDSpace"; 
      test_group_name = "testGroup";
      test_nonsense = "nonsense";
      char nbuff[16];
      sprintf( nbuff, "%i",  n) ;
      nstr = nbuff;

      if (BI->exists()){
        BI->openDB();
      }
      else{
        BI->createDB(test_filename);
      };
    };

    // this tears down the fixtures
    virtual void TearDown(){
      if (BI->isOpen()){
        BI->closeDB();
      };
    };
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, createDataBase) {
  BI->closeDB();
  BI->createDB(test_filename);
  EXPECT_EQ(       BI->getDBName(),               test_filename );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, openDB) {
  BI->openDB();
  EXPECT_EQ(       BI->isOpen(),                           true );
  BI->closeDB();
  EXPECT_EQ(       BI->isOpen(),                          false );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, closeDB){
  BI->closeDB();
  EXPECT_EQ(       BI->isOpen(),                          false );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(BookKeeperTest, DISABLED_WriteModelLists){
  BI->writeModelList();
}

