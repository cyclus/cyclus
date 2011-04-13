#include <gtest/gtest.h>
#include "Utility/BookKeeper.h"
#include <stdlib.h>
#include <stdio.h>


class BookKeeperTest : public ::testing::Test {
  protected:

    // these integers will be the dimensions of the databases
    int n, nrows, ncols, nlayers, hsrows, hscols, hslayers;
    int test_dims[2];

    // some test strings
    string nstr, test_filename, test_dsp_name, test_type, 
           test_group_name, test_nonsense;

    // make some data sets for writing and reading
    intData1d test_data_int1, test_out_int1;
    dblData1d test_data_dbl1, test_out_dbl1;
    strData1d test_data_str1, test_out_str1;
    intData2d test_data_int2, test_out_int2;
    dblData2d test_data_dbl2, test_out_dbl2;
    strData2d test_data_str2, test_out_str2;
    intData3d test_data_int3, test_out_int3;
    dblData3d test_data_dbl3, test_out_dbl3;
    strData3d test_data_str3, test_out_str3;

    // dataspaces will be used for hyperspace description
    DataSpace test_hs_int1, test_hs_dbl1, test_hs_str1,
      test_hs_int2, test_hs_dbl2, test_hs_str2,
      test_hs_int3, test_hs_dbl3, test_hs_str3;

    // this sets up the fixtures
    virtual void SetUp(){
      n=2;
      nrows=100;
      ncols=50;
      nlayers=10;
      hsrows=10;
      hscols=5;
      hslayers=1;
      test_dims[0]=ncols;
      test_dims[1]=nrows;
      test_dims[2]=nlayers;
      int test_data_int[nrows][ncols];
      double test_data_dbl[nrows][ncols];
      string test_data_str[nrows][ncols];
      test_filename = "testDB.h5";
      test_dsp_name = "testDSpace"; 
      test_group_name = "testGroup";
      test_nonsense = "nonsense";
      char buffer[16];
      char nbuff[16];
      sprintf( nbuff, "%i", 3*n) ;
      nstr = nbuff;
      for (int row=0; row<nrows; row++){
        test_data_int1[row] = row;
        for (int col=0; col<ncols; col++){ 
          test_data_int2[row][col] = row+col ;
          test_data_dbl2[row][col] = row + col*0.1 ;
          sprintf( buffer, "%i", row+col) ;
          test_data_str2[row][col] = buffer; 
          for (int layer=0; layer<nlayers; layer++){ 
            test_data_int3[row][col][layer] = row+col+layer ;
            test_data_dbl3[row][col][layer] = row + col*0.1 + layer*0.01 ;
            sprintf( buffer, "%i", row+col+layer) ;
            test_data_str3[row][col][layer] = buffer; 
          };
        };
      };
      BI->createDB(test_filename);
      BI->writeData(test_data_dbl1, test_dsp_name);
    };
};

TEST_F(BookKeeperTest, createDataBase) {
  EXPECT_EQ(       BI->getDBName(),               test_filename );
}

TEST_F(BookKeeperTest, isGroup) {
  EXPECT_EQ(       BI->isGroup(test_group_name),           true );
  EXPECT_EQ(       BI->isGroup(test_nonsense),            false );
}

TEST_F(BookKeeperTest, closeDB) {
  BI->openDB();
  EXPECT_EQ(       BI->isOpen(),                           true );
  BI->closeDB();
  EXPECT_EQ(       BI->isOpen(),                          false );
}

TEST_F(BookKeeperTest, ReadWriteIntegers) {
  BI->readData(test_hs_int1, test_out_int1);
  EXPECT_EQ(       test_out_int1[n],           test_data_int1[n] );
  ASSERT_EQ(       test_out_int1[n],                           n ); 
  BI->readData(test_hs_int2, test_out_int2);
  EXPECT_EQ(       test_out_int2[n][n],     test_data_int2[n][n] );
  ASSERT_EQ(       test_out_int2[n][n],                        n ); 
  BI->readData(test_hs_int3, test_out_int3);
  EXPECT_EQ(      test_out_int3[n][n][n], test_data_int3[n][n][n]);
  ASSERT_EQ(      test_out_int3[n][n][n],                      n ); 
}

TEST_F(BookKeeperTest, ReadWriteDoubles) {
  BI->readData(test_hs_dbl1, test_out_dbl1);
  EXPECT_EQ(       test_out_dbl1[n],           test_data_dbl1[n] );
  ASSERT_NEAR(     test_out_dbl1[n],       n + 0.1*n,      0.001 );  
  BI->readData(test_hs_dbl2, test_out_dbl2);
  EXPECT_EQ(       test_out_dbl2[n][n],     test_data_dbl2[n][n] );
  ASSERT_NEAR(     test_out_dbl2[n][n],    n + 0.1*n,      0.001 );  
  BI->readData(test_hs_dbl3, test_out_dbl3);
  EXPECT_EQ(      test_out_dbl3[n][n][n], test_data_dbl3[n][n][n]);
  ASSERT_NEAR(    test_out_dbl3[n][n][n],  n + 0.1*n,      0.001 );  
}

TEST_F(BookKeeperTest, ReadWriteStrings) {
  BI->readData(test_hs_str1, test_out_str1);
  EXPECT_EQ(       test_out_str1[n],           test_data_str1[n] );
  ASSERT_EQ(       test_out_str1[n],                        nstr );  
  BI->readData(test_hs_str2, test_out_str2);
  EXPECT_EQ(       test_out_str2[n][n],     test_data_str2[n][n] );
  ASSERT_EQ(       test_out_str2[n][n],                     nstr );  
  BI->readData(test_hs_str3, test_out_str3);
  EXPECT_EQ(      test_out_str3[n][n][n], test_data_str3[n][n][n]);
  ASSERT_EQ(      test_out_str3[n][n][n],                   nstr );  
}

