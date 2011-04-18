#include <gtest/gtest.h>
#include "Utility/BookKeeper.h"
#include <stdlib.h>
#include <stdio.h>


class BookKeeperTest : public ::testing::Test {
  protected:

    // these integers will be the dimensions of the databases
    int n, nrows, ncols, nlayers;

    // some test strings
    string nstr, test_filename, test_dsp_name, test_type, 
           test_group_name, test_nonsense;

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

    };
};

TEST_F(BookKeeperTest, createDataBase) {
  BI->createDB(test_filename);
  EXPECT_EQ(       BI->getDBName(),               test_filename );
}

TEST_F(BookKeeperTest, openDB) {
  EXPECT_EQ(       BI->isOpen(),                           true );
}

TEST_F(BookKeeperTest, closeDB){
  BI->closeDB();
  EXPECT_EQ(       BI->isOpen(),                          false );
}

// Integers
TEST_F(BookKeeperTest, ReadWrite1DIntegers) {
  BI->createDB(test_filename);
  intData1d test_data_int1(boost::extents[nrows]);
  intData1d test_out_int1(boost::extents[nrows]);
  for (int1didx row=0; row!=nrows; ++row){
    test_data_int1[row]=row;
  };
  BI->writeData( test_data_int1, "/output/test_int1" );
  BI->readData( "/output/test_int1", test_out_int1);
  EXPECT_EQ(       test_out_int1[n],            test_data_int1[n] );
  ASSERT_EQ(       n,                           test_out_int1[n] ); 
}
TEST_F(BookKeeperTest, ReadWrite2DIntegers) {
  intData2d test_data_int2(boost::extents[nrows][ncols]);
  intData2d test_out_int2(boost::extents[nrows][ncols]);
  for (int2didx row=0; row!=nrows; ++row){
    for (int2didx col=0; col!=ncols; ++col){ 
      test_data_int2[row][col]=row+col ;
    };
  };
  BI->writeData( test_data_int2, "/output/test_int2" );
  BI->readData("/output/test_int2", test_out_int2);
  ASSERT_EQ(       test_out_int2[0][0],     test_data_int2[0][0] );
  ASSERT_EQ(       test_out_int2[n][n],                       2*n  ); 
}

TEST_F(BookKeeperTest, ReadWrite3DIntegers) {
  intData3d test_data_int3(boost::extents[nrows][ncols][nlayers]);
  intData3d test_out_int3(boost::extents[nrows][ncols][nlayers]);
  for (int3didx row=0; row!=nrows; ++row){
    for (int3didx col=0; col!=ncols; ++col){ 
      for (int3didx layer=0; layer!=nlayers; ++layer){ 
        test_data_int3[row][col][layer]= row+col+layer ;
      };
    };
  };
  BI->writeData( test_data_int3, "/output/test_int3" );
  BI->readData("/output/test_int3", test_out_int3);
  EXPECT_EQ(      test_out_int3[0][0][0], test_data_int3[0][0][0]);
  ASSERT_EQ(      test_out_int3[n][n][n],                    3*n ); 
  ASSERT_EQ(      test_out_int3[0][0][n],                      n ); 
  ASSERT_EQ(      test_out_int3[0][1][n],                    1+n ); 
}

// Doubles
TEST_F(BookKeeperTest, ReadWrite1DDoubles) {
  dblData1d test_data_dbl1(boost::extents[nrows]);
  dblData1d test_out_dbl1(boost::extents[nrows]);
  for (dbl1didx row=0; row!=nrows; ++row){
    test_data_dbl1[row] = row*0.1 ;
  };
  BI->writeData( test_data_dbl1, "/output/test_dbl1" );
  BI->readData("/output/test_dbl1", test_out_dbl1);
  EXPECT_EQ(       test_out_dbl1[n],           test_data_dbl1[n] );
  ASSERT_NEAR(     test_out_dbl1[n],           0.1*n,      0.001 );  
}
TEST_F(BookKeeperTest, ReadWrite2DDoubles) {
  dblData2d test_data_dbl2(boost::extents[nrows][ncols]);
  dblData2d test_out_dbl2(boost::extents[nrows][ncols]);
  for (dbl2didx row=0; row!=nrows; ++row){
    for (dbl2didx col=0; col!=ncols; ++col){ 
      test_data_dbl2[row][col]=(row+col)*0.1 ;
    };
  };
  BI->writeData( test_data_dbl2, "/output/test_dbl2" );
  BI->readData("/output/test_dbl2", test_out_dbl2);
  EXPECT_EQ(       test_out_dbl2[n][n],     test_data_dbl2[n][n] );
  ASSERT_NEAR(     test_out_dbl2[n][n],       0.1*(n+n),   0.001 );  
}
TEST_F(BookKeeperTest, ReadWrite3DDoubles) {
  dblData3d test_data_dbl3(boost::extents[nrows][ncols][nlayers]);
  dblData3d test_out_dbl3(boost::extents[nrows][ncols][nlayers]);
  for (dbl3didx row=0; row!=nrows; ++row){
    for (dbl3didx col=0; col!=ncols; ++col){ 
      for (dbl3didx layer=0; layer!=nlayers; ++layer){ 
        test_data_dbl3[row][col][layer]= (row+col+layer)*0.1  ;
      };
    };
  };
  BI->writeData( test_data_dbl3, "/output/test_dbl3" );
  BI->readData("/output/test_dbl3", test_out_dbl3);
  EXPECT_EQ(      test_out_dbl3[n][n][n], test_data_dbl3[n][n][n]);
  EXPECT_NEAR(    test_out_dbl3[n][n][n],  0.1*(3*n),      0.001 );  
  BI->closeDB();
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

