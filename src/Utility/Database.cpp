#include "Database.h"
#include <iostream>

Database::Database(char* filename)
{
  database = NULL;
  open(filename);
}

Database::~Database()
{
}

bool Database::open(char* filename)
{
  if(sqlite3_open(filename, &database) == SQLITE_OK)
    return true;
  
  return false;   
}

query_result Database::query(char* query)
{
  sqlite3_stmt *statement;
  query_result results;

  if(sqlite3_prepare_v2(database, query, -1, &statement, 0) == SQLITE_OK)
    {
      int cols = sqlite3_column_count(statement);
      int result = 0;
      while(true)
	{
	  result = sqlite3_step(statement);
	  
	  if(result == SQLITE_ROW)
	    {
	      query_row values;
	      for(int col = 0; col < cols; col++)
		{
		  values.push_back((char*)sqlite3_column_text(statement, col));
		}
	      results.push_back(values);
	    }
	  else
	    {
	      break;   
	    }
	}
         
      sqlite3_finalize(statement);
    }
  
  string error = sqlite3_errmsg(database);
  if(error != "not an error") cout << query << " " << error << endl;
  
  return results;  
}

void Database::close()
{
  sqlite3_close(database);   
}
