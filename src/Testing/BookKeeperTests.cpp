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


// Strings
//TEST_F(BookKeeperTest, ReadWrite1DStrings) {
//  strData1d test_data_str1(boost::extents[nrows]);
//  strData1d test_out_str1(boost::extents[nrows]);
//  for (str1didx row=0; row!=nrows; ++row){
//    sprintf( buffer, "%i",row );
//    test_data_str1[row] = buffer;
//  };
//  BI->writeData( test_data_str1, "/output/test_str1" );
//  BI->readData("/output/test_str1", test_out_str1);
//  EXPECT_EQ(       test_out_str1[n],           test_data_str1[n] );
//  EXPECT_EQ(       test_out_str1[n],                        nstr );  
//  BI->closeDB();
//}
//
//TEST_F(BookKeeperTest, ReadWrite2DStrings) {
//  strData2d test_data_str2(boost::extents[nrows][ncols]);
//  strData2d test_out_str2(boost::extents[nrows][ncols]);
//  for (str2didx row=0; row!=nrows; ++row){
//    for (str2didx col=0; col!=ncols; ++col){ 
//      sprintf( buffer, "%i", row+col);
//      test_data_str2[row][col]= buffer; 
//    };
//  };
//  BI->writeData( test_data_str2, "/output/test_str2" );
//  BI->readData("/output/test_str2", test_out_str2);
//  EXPECT_EQ(       test_out_str2[n][n],     test_data_str2[n][n] );
//  ASSERT_EQ(       test_out_str2[0][n],                     nstr );  
//  ASSERT_EQ(       test_out_str2[n][0],                     nstr );  
//  BI->closeDB();
//}
//
//TEST_F(BookKeeperTest, ReadWrite3DStrings) {
//  strData3d test_data_str3(boost::extents[nrows][ncols][nlayers]);
//  strData3d test_out_str3(boost::extents[nrows][ncols][nlayers]);
//  for (str3didx row=0; row!=nrows; ++row){
//    for (str3didx col=0; col!=ncols; ++col){ 
//      for (str3didx layer=0; layer!=nlayers; ++layer){ 
//        sprintf( buffer, "%i", row+col+layer);
//        test_data_str3[row][col][layer]= buffer ; 
//      };
//    };
//  };
//  BI->writeData( test_data_str3, "/output/test_str3" );
//  BI->readData("/output/test_str3", test_out_str3);
//  EXPECT_EQ(      test_out_str3[n][n][n], test_data_str3[n][n][n]);
//  ASSERT_EQ(      test_out_str3[0][0][n],                   nstr );  
//  ASSERT_EQ(      test_out_str3[0][n][0],                   nstr );  
//  BI->closeDB();
//}

