#include "Database.h"

#include <iostream>
#include <string>

#include "CycException.h"

Database::Database(std::string filename)
{
  database = NULL;
  open(filename);
}

Database::~Database()
{
}

bool Database::open(std::string filename)
{
  if(sqlite3_open(filename.c_str(), &database) == SQLITE_OK)
    return true;
  else 
    throw CycIOException("Unable to open database " + filename); 
  return false;   
}

query_result Database::query(std::string query)
{
  sqlite3_stmt *statement;
  query_result results;

  if(sqlite3_prepare_v2(database, query.c_str(), -1, &statement, 0) == SQLITE_OK)
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
                  std::string  val;
                  char * ptr = (char*)sqlite3_column_text(statement, col);
                  
                  if(ptr)
                    {
                      val = ptr;
                    }
                  else val = ""; // this can be commented out since std::string  val;
                  // initialize variable 'val' to empty string anyway
                  
                  values.push_back(val);  // now we will never push NULL
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
  
  std::string error = sqlite3_errmsg(database);
  if(error != "not an error") 
    throw CycIOException(query + " " + error);
  
  return results;  
}

void Database::close()
{
  sqlite3_close(database);   
}
