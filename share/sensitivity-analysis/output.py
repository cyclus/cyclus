import sqlite3 as lite

def cursor(file_name):
    """Connects and returns a cursor to an sqlite output file
    Parameters
    ----------
    file_name: str
        name of the sqlite file
    Returns
    -------
    sqlite cursor3
    """
    con = lite.connect(file_name)
    con.row_factory = lite.Row
    return con.cursor()