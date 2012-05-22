from output_tools import Query
from numpy import cumsum
from file_io import list2file

def prep_output(dbname, receiver):
    outfile = dbname.strip('sqlite') + "txt"
    new_query = Query(dbname,'resource')
    list2file(outfile,'resource', list(cumsum(new_query.allReceivedBy(receiver))))
