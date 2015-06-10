var dbdata =
    '[["id", "name", "C++ type", "shape rank", "backend", "version", "supported"],' +
    '[0, "BOOL", "bool", 0, "SQLite", "v1.0", 1],' +
    '[1, "INT", "int", 0, "SQLite", "v1.0", 1],' +
    '[2, "FLOAT", "float", 0, "SQLite", "v1.0", 1],' +
    '[3, "DOUBLE", "double", 0, "SQLite", "v1.0", 1],' +
    '[4, "STRING", "std::string", 1, "SQLite", "v1.0", 1],' +
    '[5, "VL_STRING", "std::string", 1, "SQLite", "v1.0", 1],' +
    '[6, "BLOB", "cyclus::Blob", 0, "SQLite", "v1.0", 1],' +
    '[7, "UUID", "boost::uuids::uuid", 0, "SQLite", "v1.0", 1],' +
    '[8, "VECTOR_BOOL", "std::vector<bool>", 1, "SQLite", "v1.0", 0],' +
    '[9, "VL_VECTOR_BOOL", "std::vector<bool>", 1, "SQLite", "v1.0", 0],' +
    '[10, "VECTOR_INT", "std::vector<int>", 1, "SQLite", "v1.0", 1],' +
    '[11, "VL_VECTOR_INT", "std::vector<int>", 1, "SQLite", "v1.0", 1],' +
    '[12, "VECTOR_FLOAT", "std::vector<float>", 1, "SQLite", "v1.0", 0],' +
    '[13, "VL_VECTOR_FLOAT", "std::vector<float>", 1, "SQLite", "v1.0", 0],' +
    '[14, "VECTOR_DOUBLE", "std::vector<double>", 1, "SQLite", "v1.0", 0],' +
    '[15, "VL_VECTOR_DOUBLE", "std::vector<double>", 1, "SQLite", "v1.0", 0],' +
    '[16, "VECTOR_STRING", "std::vector<std::string>", 2, "SQLite", "v1.0", 1],' +
    '[17, "VL_VECTOR_STRING", "std::vector<std::string>", 2, "SQLite", "v1.0", 1],' +
    '[18, "VECTOR_VL_STRING", "std::vector<std::string>", 2, "SQLite", "v1.0", 1],' +
    '[19, "VL_VECTOR_VL_STRING", "std::vector<std::string>", 2, "SQLite", "v1.0", 1],' +
    '[20, "VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[21, "VL_VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[22, "VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[23, "VL_VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[24, "SET_BOOL", "std::set<bool>", 1, "SQLite", "v1.0", 0],' +
    '[25, "VL_SET_BOOL", "std::set<bool>", 1, "SQLite", "v1.0", 0],' +
    '[26, "SET_INT", "std::set<int>", 1, "SQLite", "v1.0", 0],' +
    '[27, "VL_SET_INT", "std::set<int>", 1, "SQLite", "v1.0", 0],' +
    '[28, "SET_FLOAT", "std::set<float>", 1, "SQLite", "v1.0", 0],' +
    '[29, "VL_SET_FLOAT", "std::set<float>", 1, "SQLite", "v1.0", 0],' +
    '[30, "SET_DOUBLE", "std::set<double>", 1, "SQLite", "v1.0", 0],' +
    '[31, "VL_SET_DOUBLE", "std::set<double>", 1, "SQLite", "v1.0", 0],' +
    '[32, "SET_STRING", "std::set<std::string>", 2, "SQLite", "v1.0", 0],' +
    '[33, "VL_SET_STRING", "std::set<std::string>", 2, "SQLite", "v1.0", 0],' +
    '[34, "SET_VL_STRING", "std::set<std::string>", 2, "SQLite", "v1.0", 0],' +
    '[35, "VL_SET_VL_STRING", "std::set<std::string>", 2, "SQLite", "v1.0", 0],' +
    '[36, "SET_BLOB", "std::set<cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[37, "VL_SET_BLOB", "std::set<cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[38, "SET_UUID", "std::set<boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[39, "VL_SET_UUID", "std::set<boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[40, "LIST_BOOL", "std::list<bool>", 1, "SQLite", "v1.0", 0],' +
    '[41, "VL_LIST_BOOL", "std::list<bool>", 1, "SQLite", "v1.0", 0],' +
    '[42, "LIST_INT", "std::list<int>", 1, "SQLite", "v1.0", 0],' +
    '[43, "VL_LIST_INT", "std::list<int>", 1, "SQLite", "v1.0", 0],' +
    '[44, "LIST_FLOAT", "std::list<float>", 1, "SQLite", "v1.0", 0],' +
    '[45, "VL_LIST_FLOAT", "std::list<float>", 1, "SQLite", "v1.0", 0],' +
    '[46, "LIST_DOUBLE", "std::list<double>", 1, "SQLite", "v1.0", 0],' +
    '[47, "VL_LIST_DOUBLE", "std::list<double>", 1, "SQLite", "v1.0", 0],' +
    '[48, "LIST_STRING", "std::list<std::string>", 2, "SQLite", "v1.0", 0],' +
    '[49, "VL_LIST_STRING", "std::list<std::string>", 2, "SQLite", "v1.0", 0],' +
    '[50, "LIST_VL_STRING", "std::list<std::string>", 2, "SQLite", "v1.0", 0],' +
    '[51, "VL_LIST_VL_STRING", "std::list<std::string>", 2, "SQLite", "v1.0", 0],' +
    '[52, "LIST_BLOB", "std::list<cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[53, "VL_LIST_BLOB", "std::list<cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[54, "LIST_UUID", "std::list<boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[55, "VL_LIST_UUID", "std::list<boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[56, "PAIR_INT_BOOL", "std::pair<int, bool>", 0, "SQLite", "v1.0", 0],' +
    '[57, "PAIR_INT_INT", "std::pair<int, int>", 0, "SQLite", "v1.0", 0],' +
    '[58, "PAIR_INT_FLOAT", "std::pair<int, float>", 0, "SQLite", "v1.0", 0],' +
    '[59, "PAIR_INT_DOUBLE", "std::pair<int, float>", 0, "SQLite", "v1.0", 0],' +
    '[60, "PAIR_INT_STRING", "std::pair<int, std::string>", 1, "SQLite", "v1.0", 0],' +
    '[61, "PAIR_INT_VL_STRING", "std::pair<int, std::string>", 1, "SQLite", "v1.0", 0],' +
    '[62, "PAIR_INT_BLOB", "std::pair<int, cyclus::Blob>", 0, "SQLite", "v1.0", 0],' +
    '[63, "PAIR_INT_UUID", "std::pair<int, boost::uuids::uuid>", 0, "SQLite", "v1.0", 0],' +
    '[64, "PAIR_STRING_BOOL", "std::pair<std::string, bool>", 1, "SQLite", "v1.0", 0],' +
    '[65, "PAIR_STRING_INT", "std::pair<std::string, int>", 1, "SQLite", "v1.0", 0],' +
    '[66, "PAIR_STRING_FLOAT", "std::pair<std::string, float>", 1, "SQLite", "v1.0", 0],' +
    '[67, "PAIR_STRING_DOUBLE", "std::pair<std::string, double>", 1, "SQLite", "v1.0", 0],' +
    '[68, "PAIR_STRING_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.0", 0],' +
    '[69, "PAIR_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.0", 0],' +
    '[70, "PAIR_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[71, "PAIR_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[72, "PAIR_VL_STRING_BOOL", "std::pair<std::string, bool>", 1, "SQLite", "v1.0", 0],' +
    '[73, "PAIR_VL_STRING_INT", "std::pair<std::string, int>", 1, "SQLite", "v1.0", 0],' +
    '[74, "PAIR_VL_STRING_FLOAT", "std::pair<std::string, float>", 1, "SQLite", "v1.0", 0],' +
    '[75, "PAIR_VL_STRING_DOUBLE", "std::pair<std::string, double>", 1, "SQLite", "v1.0", 0],' +
    '[76, "PAIR_VL_STRING_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.0", 0],' +
    '[77, "PAIR_VL_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.0", 0],' +
    '[78, "PAIR_VL_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[79, "PAIR_VL_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[80, "MAP_INT_BOOL", "std::map<int, bool>", 1, "SQLite", "v1.0", 0],' +
    '[81, "VL_MAP_INT_BOOL", "std::map<int, bool>", 1, "SQLite", "v1.0", 0],' +
    '[82, "MAP_INT_INT", "std::map<int, int>", 1, "SQLite", "v1.0", 0],' +
    '[83, "VL_MAP_INT_INT", "std::map<int, int>", 1, "SQLite", "v1.0", 0],' +
    '[84, "MAP_INT_FLOAT", "std::map<int, float>", 1, "SQLite", "v1.0", 0],' +
    '[85, "VL_MAP_INT_FLOAT", "std::map<int, float>", 1, "SQLite", "v1.0", 0],' +
    '[86, "MAP_INT_DOUBLE", "std::map<int, double>", 1, "SQLite", "v1.0", 0],' +
    '[87, "VL_MAP_INT_DOUBLE", "std::map<int, double>", 1, "SQLite", "v1.0", 0],' +
    '[88, "MAP_INT_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.0", 0],' +
    '[89, "VL_MAP_INT_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.0", 0],' +
    '[90, "MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.0", 0],' +
    '[91, "VL_MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.0", 0],' +
    '[92, "MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[93, "VL_MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "SQLite", "v1.0", 0],' +
    '[94, "MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[95, "VL_MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "SQLite", "v1.0", 0],' +
    '[96, "MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.0", 0],' +
    '[97, "VL_MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.0", 0],' +
    '[98, "MAP_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.0", 0],' +
    '[99, "VL_MAP_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.0", 0],' +
    '[100, "MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.0", 0],' +
    '[101, "VL_MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.0", 0],' +
    '[102, "MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.0", 0],' +
    '[103, "VL_MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.0", 0],' +
    '[104, "MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.0", 0],' +
    '[105, "VL_MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.0", 0],' +
    '[106, "MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.0", 0],' +
    '[107, "VL_MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.0", 0],' +
    '[108, "MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.0", 0],' +
    '[109, "VL_MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.0", 0],' +
    '[110, "MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.0", 0],' +
    '[111, "VL_MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.0", 0],' +
    '[112, "MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.0", 0],' +
    '[113, "VL_MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.0", 0],' +
    '[114, "MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.0", 0],' +
    '[115, "VL_MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.0", 0],' +
    '[116, "MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.0", 0],' +
    '[117, "VL_MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.0", 0],' +
    '[118, "MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.0", 0],' +
    '[119, "VL_MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.0", 0],' +
    '[120, "MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.0", 0],' +
    '[121, "VL_MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.0", 0],' +
    '[122, "MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.0", 0],' +
    '[123, "VL_MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.0", 0],' +
    '[124, "MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.0", 0],' +
    '[125, "VL_MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.0", 0],' +
    '[126, "MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.0", 0],' +
    '[127, "VL_MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.0", 0],' +
    '[0, "BOOL", "bool", 0, "HDF5", "v1.0", 1],' +
    '[1, "INT", "int", 0, "HDF5", "v1.0", 1],' +
    '[2, "FLOAT", "float", 0, "HDF5", "v1.0", 1],' +
    '[3, "DOUBLE", "double", 0, "HDF5", "v1.0", 1],' +
    '[4, "STRING", "std::string", 1, "HDF5", "v1.0", 1],' +
    '[5, "VL_STRING", "std::string", 1, "HDF5", "v1.0", 1],' +
    '[6, "BLOB", "cyclus::Blob", 0, "HDF5", "v1.0", 1],' +
    '[7, "UUID", "boost::uuids::uuid", 0, "HDF5", "v1.0", 1],' +
    '[8, "VECTOR_BOOL", "std::vector<bool>", 1, "HDF5", "v1.0", 0],' +
    '[9, "VL_VECTOR_BOOL", "std::vector<bool>", 1, "HDF5", "v1.0", 0],' +
    '[10, "VECTOR_INT", "std::vector<int>", 1, "HDF5", "v1.0", 1],' +
    '[11, "VL_VECTOR_INT", "std::vector<int>", 1, "HDF5", "v1.0", 1],' +
    '[12, "VECTOR_FLOAT", "std::vector<float>", 1, "HDF5", "v1.0", 0],' +
    '[13, "VL_VECTOR_FLOAT", "std::vector<float>", 1, "HDF5", "v1.0", 0],' +
    '[14, "VECTOR_DOUBLE", "std::vector<double>", 1, "HDF5", "v1.0", 0],' +
    '[15, "VL_VECTOR_DOUBLE", "std::vector<double>", 1, "HDF5", "v1.0", 0],' +
    '[16, "VECTOR_STRING", "std::vector<std::string>", 2, "HDF5", "v1.0", 1],' +
    '[17, "VL_VECTOR_STRING", "std::vector<std::string>", 2, "HDF5", "v1.0", 1],' +
    '[18, "VECTOR_VL_STRING", "std::vector<std::string>", 2, "HDF5", "v1.0", 1],' +
    '[19, "VL_VECTOR_VL_STRING", "std::vector<std::string>", 2, "HDF5", "v1.0", 1],' +
    '[20, "VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[21, "VL_VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[22, "VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[23, "VL_VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[24, "SET_BOOL", "std::set<bool>", 1, "HDF5", "v1.0", 0],' +
    '[25, "VL_SET_BOOL", "std::set<bool>", 1, "HDF5", "v1.0", 0],' +
    '[26, "SET_INT", "std::set<int>", 1, "HDF5", "v1.0", 1],' +
    '[27, "VL_SET_INT", "std::set<int>", 1, "HDF5", "v1.0", 1],' +
    '[28, "SET_FLOAT", "std::set<float>", 1, "HDF5", "v1.0", 0],' +
    '[29, "VL_SET_FLOAT", "std::set<float>", 1, "HDF5", "v1.0", 0],' +
    '[30, "SET_DOUBLE", "std::set<double>", 1, "HDF5", "v1.0", 0],' +
    '[31, "VL_SET_DOUBLE", "std::set<double>", 1, "HDF5", "v1.0", 0],' +
    '[32, "SET_STRING", "std::set<std::string>", 2, "HDF5", "v1.0", 0],' +
    '[33, "VL_SET_STRING", "std::set<std::string>", 2, "HDF5", "v1.0", 0],' +
    '[34, "SET_VL_STRING", "std::set<std::string>", 2, "HDF5", "v1.0", 0],' +
    '[35, "VL_SET_VL_STRING", "std::set<std::string>", 2, "HDF5", "v1.0", 0],' +
    '[36, "SET_BLOB", "std::set<cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[37, "VL_SET_BLOB", "std::set<cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[38, "SET_UUID", "std::set<boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[39, "VL_SET_UUID", "std::set<boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[40, "LIST_BOOL", "std::list<bool>", 1, "HDF5", "v1.0", 0],' +
    '[41, "VL_LIST_BOOL", "std::list<bool>", 1, "HDF5", "v1.0", 0],' +
    '[42, "LIST_INT", "std::list<int>", 1, "HDF5", "v1.0", 1],' +
    '[43, "VL_LIST_INT", "std::list<int>", 1, "HDF5", "v1.0", 1],' +
    '[44, "LIST_FLOAT", "std::list<float>", 1, "HDF5", "v1.0", 0],' +
    '[45, "VL_LIST_FLOAT", "std::list<float>", 1, "HDF5", "v1.0", 0],' +
    '[46, "LIST_DOUBLE", "std::list<double>", 1, "HDF5", "v1.0", 0],' +
    '[47, "VL_LIST_DOUBLE", "std::list<double>", 1, "HDF5", "v1.0", 0],' +
    '[48, "LIST_STRING", "std::list<std::string>", 2, "HDF5", "v1.0", 0],' +
    '[49, "VL_LIST_STRING", "std::list<std::string>", 2, "HDF5", "v1.0", 0],' +
    '[50, "LIST_VL_STRING", "std::list<std::string>", 2, "HDF5", "v1.0", 0],' +
    '[51, "VL_LIST_VL_STRING", "std::list<std::string>", 2, "HDF5", "v1.0", 0],' +
    '[52, "LIST_BLOB", "std::list<cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[53, "VL_LIST_BLOB", "std::list<cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[54, "LIST_UUID", "std::list<boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[55, "VL_LIST_UUID", "std::list<boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[56, "PAIR_INT_BOOL", "std::pair<int, bool>", 0, "HDF5", "v1.0", 0],' +
    '[57, "PAIR_INT_INT", "std::pair<int, int>", 0, "HDF5", "v1.0", 1],' +
    '[58, "PAIR_INT_FLOAT", "std::pair<int, float>", 0, "HDF5", "v1.0", 0],' +
    '[59, "PAIR_INT_DOUBLE", "std::pair<int, float>", 0, "HDF5", "v1.0", 0],' +
    '[60, "PAIR_INT_STRING", "std::pair<int, std::string>", 1, "HDF5", "v1.0", 0],' +
    '[61, "PAIR_INT_VL_STRING", "std::pair<int, std::string>", 1, "HDF5", "v1.0", 0],' +
    '[62, "PAIR_INT_BLOB", "std::pair<int, cyclus::Blob>", 0, "HDF5", "v1.0", 0],' +
    '[63, "PAIR_INT_UUID", "std::pair<int, boost::uuids::uuid>", 0, "HDF5", "v1.0", 0],' +
    '[64, "PAIR_STRING_BOOL", "std::pair<std::string, bool>", 1, "HDF5", "v1.0", 0],' +
    '[65, "PAIR_STRING_INT", "std::pair<std::string, int>", 1, "HDF5", "v1.0", 0],' +
    '[66, "PAIR_STRING_FLOAT", "std::pair<std::string, float>", 1, "HDF5", "v1.0", 0],' +
    '[67, "PAIR_STRING_DOUBLE", "std::pair<std::string, double>", 1, "HDF5", "v1.0", 0],' +
    '[68, "PAIR_STRING_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.0", 0],' +
    '[69, "PAIR_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.0", 0],' +
    '[70, "PAIR_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[71, "PAIR_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[72, "PAIR_VL_STRING_BOOL", "std::pair<std::string, bool>", 1, "HDF5", "v1.0", 0],' +
    '[73, "PAIR_VL_STRING_INT", "std::pair<std::string, int>", 1, "HDF5", "v1.0", 0],' +
    '[74, "PAIR_VL_STRING_FLOAT", "std::pair<std::string, float>", 1, "HDF5", "v1.0", 0],' +
    '[75, "PAIR_VL_STRING_DOUBLE", "std::pair<std::string, double>", 1, "HDF5", "v1.0", 0],' +
    '[76, "PAIR_VL_STRING_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.0", 0],' +
    '[77, "PAIR_VL_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.0", 0],' +
    '[78, "PAIR_VL_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[79, "PAIR_VL_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[80, "MAP_INT_BOOL", "std::map<int, bool>", 1, "HDF5", "v1.0", 0],' +
    '[81, "VL_MAP_INT_BOOL", "std::map<int, bool>", 1, "HDF5", "v1.0", 0],' +
    '[82, "MAP_INT_INT", "std::map<int, int>", 1, "HDF5", "v1.0", 1],' +
    '[83, "VL_MAP_INT_INT", "std::map<int, int>", 1, "HDF5", "v1.0", 1],' +
    '[84, "MAP_INT_FLOAT", "std::map<int, float>", 1, "HDF5", "v1.0", 0],' +
    '[85, "VL_MAP_INT_FLOAT", "std::map<int, float>", 1, "HDF5", "v1.0", 0],' +
    '[86, "MAP_INT_DOUBLE", "std::map<int, double>", 1, "HDF5", "v1.0", 0],' +
    '[87, "VL_MAP_INT_DOUBLE", "std::map<int, double>", 1, "HDF5", "v1.0", 0],' +
    '[88, "MAP_INT_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.0", 0],' +
    '[89, "VL_MAP_INT_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.0", 0],' +
    '[90, "MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.0", 0],' +
    '[91, "VL_MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.0", 0],' +
    '[92, "MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[93, "VL_MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "HDF5", "v1.0", 0],' +
    '[94, "MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[95, "VL_MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "HDF5", "v1.0", 0],' +
    '[96, "MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.0", 0],' +
    '[97, "VL_MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.0", 0],' +
    '[98, "MAP_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.0", 0],' +
    '[99, "VL_MAP_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.0", 0],' +
    '[100, "MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.0", 0],' +
    '[101, "VL_MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.0", 0],' +
    '[102, "MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.0", 0],' +
    '[103, "VL_MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.0", 0],' +
    '[104, "MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.0", 0],' +
    '[105, "VL_MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.0", 0],' +
    '[106, "MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.0", 0],' +
    '[107, "VL_MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.0", 0],' +
    '[108, "MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.0", 0],' +
    '[109, "VL_MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.0", 0],' +
    '[110, "MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.0", 0],' +
    '[111, "VL_MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.0", 0],' +
    '[112, "MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.0", 0],' +
    '[113, "VL_MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.0", 0],' +
    '[114, "MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.0", 0],' +
    '[115, "VL_MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.0", 0],' +
    '[116, "MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.0", 0],' +
    '[117, "VL_MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.0", 0],' +
    '[118, "MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.0", 0],' +
    '[119, "VL_MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.0", 0],' +
    '[120, "MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.0", 0],' +
    '[121, "VL_MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.0", 0],' +
    '[122, "MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.0", 0],' +
    '[123, "VL_MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.0", 0],' +
    '[124, "MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.0", 0],' +
    '[125, "VL_MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.0", 0],' +
    '[126, "MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.0", 0],' +
    '[127, "VL_MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.0", 0],' +
    '[0, "BOOL", "bool", 0, "SQLite", "v1.1", 1],' +
    '[1, "INT", "int", 0, "SQLite", "v1.1", 1],' +
    '[2, "FLOAT", "float", 0, "SQLite", "v1.1", 1],' +
    '[3, "DOUBLE", "double", 0, "SQLite", "v1.1", 1],' +
    '[4, "STRING", "std::string", 1, "SQLite", "v1.1", 1],' +
    '[5, "VL_STRING", "std::string", 1, "SQLite", "v1.1", 1],' +
    '[6, "BLOB", "cyclus::Blob", 0, "SQLite", "v1.1", 1],' +
    '[7, "UUID", "boost::uuids::uuid", 0, "SQLite", "v1.1", 1],' +
    '[8, "VECTOR_BOOL", "std::vector<bool>", 1, "SQLite", "v1.1", 0],' +
    '[9, "VL_VECTOR_BOOL", "std::vector<bool>", 1, "SQLite", "v1.1", 0],' +
    '[10, "VECTOR_INT", "std::vector<int>", 1, "SQLite", "v1.1", 1],' +
    '[11, "VL_VECTOR_INT", "std::vector<int>", 1, "SQLite", "v1.1", 1],' +
    '[12, "VECTOR_FLOAT", "std::vector<float>", 1, "SQLite", "v1.1", 0],' +
    '[13, "VL_VECTOR_FLOAT", "std::vector<float>", 1, "SQLite", "v1.1", 0],' +
    '[14, "VECTOR_DOUBLE", "std::vector<double>", 1, "SQLite", "v1.1", 1],' +
    '[15, "VL_VECTOR_DOUBLE", "std::vector<double>", 1, "SQLite", "v1.1", 1],' +
    '[16, "VECTOR_STRING", "std::vector<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[17, "VL_VECTOR_STRING", "std::vector<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[18, "VECTOR_VL_STRING", "std::vector<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[19, "VL_VECTOR_VL_STRING", "std::vector<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[20, "VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[21, "VL_VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[22, "VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[23, "VL_VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[24, "SET_BOOL", "std::set<bool>", 1, "SQLite", "v1.1", 0],' +
    '[25, "VL_SET_BOOL", "std::set<bool>", 1, "SQLite", "v1.1", 0],' +
    '[26, "SET_INT", "std::set<int>", 1, "SQLite", "v1.1", 1],' +
    '[27, "VL_SET_INT", "std::set<int>", 1, "SQLite", "v1.1", 1],' +
    '[28, "SET_FLOAT", "std::set<float>", 1, "SQLite", "v1.1", 0],' +
    '[29, "VL_SET_FLOAT", "std::set<float>", 1, "SQLite", "v1.1", 0],' +
    '[30, "SET_DOUBLE", "std::set<double>", 1, "SQLite", "v1.1", 0],' +
    '[31, "VL_SET_DOUBLE", "std::set<double>", 1, "SQLite", "v1.1", 0],' +
    '[32, "SET_STRING", "std::set<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[33, "VL_SET_STRING", "std::set<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[34, "SET_VL_STRING", "std::set<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[35, "VL_SET_VL_STRING", "std::set<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[36, "SET_BLOB", "std::set<cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[37, "VL_SET_BLOB", "std::set<cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[38, "SET_UUID", "std::set<boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[39, "VL_SET_UUID", "std::set<boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[40, "LIST_BOOL", "std::list<bool>", 1, "SQLite", "v1.1", 0],' +
    '[41, "VL_LIST_BOOL", "std::list<bool>", 1, "SQLite", "v1.1", 0],' +
    '[42, "LIST_INT", "std::list<int>", 1, "SQLite", "v1.1", 1],' +
    '[43, "VL_LIST_INT", "std::list<int>", 1, "SQLite", "v1.1", 1],' +
    '[44, "LIST_FLOAT", "std::list<float>", 1, "SQLite", "v1.1", 0],' +
    '[45, "VL_LIST_FLOAT", "std::list<float>", 1, "SQLite", "v1.1", 0],' +
    '[46, "LIST_DOUBLE", "std::list<double>", 1, "SQLite", "v1.1", 0],' +
    '[47, "VL_LIST_DOUBLE", "std::list<double>", 1, "SQLite", "v1.1", 0],' +
    '[48, "LIST_STRING", "std::list<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[49, "VL_LIST_STRING", "std::list<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[50, "LIST_VL_STRING", "std::list<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[51, "VL_LIST_VL_STRING", "std::list<std::string>", 2, "SQLite", "v1.1", 1],' +
    '[52, "LIST_BLOB", "std::list<cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[53, "VL_LIST_BLOB", "std::list<cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[54, "LIST_UUID", "std::list<boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[55, "VL_LIST_UUID", "std::list<boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[56, "PAIR_INT_BOOL", "std::pair<int, bool>", 0, "SQLite", "v1.1", 0],' +
    '[57, "PAIR_INT_INT", "std::pair<int, int>", 0, "SQLite", "v1.1", 0],' +
    '[58, "PAIR_INT_FLOAT", "std::pair<int, float>", 0, "SQLite", "v1.1", 0],' +
    '[59, "PAIR_INT_DOUBLE", "std::pair<int, float>", 0, "SQLite", "v1.1", 0],' +
    '[60, "PAIR_INT_STRING", "std::pair<int, std::string>", 1, "SQLite", "v1.1", 0],' +
    '[61, "PAIR_INT_VL_STRING", "std::pair<int, std::string>", 1, "SQLite", "v1.1", 0],' +
    '[62, "PAIR_INT_BLOB", "std::pair<int, cyclus::Blob>", 0, "SQLite", "v1.1", 0],' +
    '[63, "PAIR_INT_UUID", "std::pair<int, boost::uuids::uuid>", 0, "SQLite", "v1.1", 0],' +
    '[64, "PAIR_STRING_BOOL", "std::pair<std::string, bool>", 1, "SQLite", "v1.1", 0],' +
    '[65, "PAIR_STRING_INT", "std::pair<std::string, int>", 1, "SQLite", "v1.1", 0],' +
    '[66, "PAIR_STRING_FLOAT", "std::pair<std::string, float>", 1, "SQLite", "v1.1", 0],' +
    '[67, "PAIR_STRING_DOUBLE", "std::pair<std::string, double>", 1, "SQLite", "v1.1", 0],' +
    '[68, "PAIR_STRING_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.1", 0],' +
    '[69, "PAIR_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.1", 0],' +
    '[70, "PAIR_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[71, "PAIR_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[72, "PAIR_VL_STRING_BOOL", "std::pair<std::string, bool>", 1, "SQLite", "v1.1", 0],' +
    '[73, "PAIR_VL_STRING_INT", "std::pair<std::string, int>", 1, "SQLite", "v1.1", 0],' +
    '[74, "PAIR_VL_STRING_FLOAT", "std::pair<std::string, float>", 1, "SQLite", "v1.1", 0],' +
    '[75, "PAIR_VL_STRING_DOUBLE", "std::pair<std::string, double>", 1, "SQLite", "v1.1", 0],' +
    '[76, "PAIR_VL_STRING_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.1", 0],' +
    '[77, "PAIR_VL_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.1", 0],' +
    '[78, "PAIR_VL_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[79, "PAIR_VL_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[80, "MAP_INT_BOOL", "std::map<int, bool>", 1, "SQLite", "v1.1", 0],' +
    '[81, "VL_MAP_INT_BOOL", "std::map<int, bool>", 1, "SQLite", "v1.1", 0],' +
    '[82, "MAP_INT_INT", "std::map<int, int>", 1, "SQLite", "v1.1", 1],' +
    '[83, "VL_MAP_INT_INT", "std::map<int, int>", 1, "SQLite", "v1.1", 1],' +
    '[84, "MAP_INT_FLOAT", "std::map<int, float>", 1, "SQLite", "v1.1", 0],' +
    '[85, "VL_MAP_INT_FLOAT", "std::map<int, float>", 1, "SQLite", "v1.1", 0],' +
    '[86, "MAP_INT_DOUBLE", "std::map<int, double>", 1, "SQLite", "v1.1", 1],' +
    '[87, "VL_MAP_INT_DOUBLE", "std::map<int, double>", 1, "SQLite", "v1.1", 1],' +
    '[88, "MAP_INT_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.1", 1],' +
    '[89, "VL_MAP_INT_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.1", 1],' +
    '[90, "MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.1", 1],' +
    '[91, "VL_MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.1", 1],' +
    '[92, "MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[93, "VL_MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "SQLite", "v1.1", 0],' +
    '[94, "MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[95, "VL_MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "SQLite", "v1.1", 0],' +
    '[96, "MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.1", 0],' +
    '[97, "VL_MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.1", 0],' +
    '[98, "MAP_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.1", 1],' +
    '[99, "VL_MAP_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.1", 1],' +
    '[100, "MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.1", 0],' +
    '[101, "VL_MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.1", 0],' +
    '[102, "MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.1", 1],' +
    '[103, "VL_MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.1", 1],' +
    '[104, "MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.1", 1],' +
    '[105, "VL_MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.1", 1],' +
    '[106, "MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.1", 1],' +
    '[107, "VL_MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.1", 1],' +
    '[108, "MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.1", 0],' +
    '[109, "VL_MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.1", 0],' +
    '[110, "MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.1", 0],' +
    '[111, "VL_MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.1", 0],' +
    '[112, "MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.1", 0],' +
    '[113, "VL_MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.1", 0],' +
    '[114, "MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.1", 1],' +
    '[115, "VL_MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.1", 1],' +
    '[116, "MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.1", 0],' +
    '[117, "VL_MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.1", 0],' +
    '[118, "MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.1", 1],' +
    '[119, "VL_MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.1", 1],' +
    '[120, "MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.1", 1],' +
    '[121, "VL_MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.1", 1],' +
    '[122, "MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.1", 1],' +
    '[123, "VL_MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.1", 1],' +
    '[124, "MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.1", 0],' +
    '[125, "VL_MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.1", 0],' +
    '[126, "MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.1", 0],' +
    '[127, "VL_MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.1", 0],' +
    '[1, "INT", "int", 0, "HDF5", "v1.1", 1],' +
    '[2, "FLOAT", "float", 0, "HDF5", "v1.1", 1],' +
    '[3, "DOUBLE", "double", 0, "HDF5", "v1.1", 1],' +
    '[4, "STRING", "std::string", 1, "HDF5", "v1.1", 1],' +
    '[5, "VL_STRING", "std::string", 1, "HDF5", "v1.1", 1],' +
    '[6, "BLOB", "cyclus::Blob", 0, "HDF5", "v1.1", 1],' +
    '[7, "UUID", "boost::uuids::uuid", 0, "HDF5", "v1.1", 1],' +
    '[8, "VECTOR_BOOL", "std::vector<bool>", 1, "HDF5", "v1.1", 0],' +
    '[9, "VL_VECTOR_BOOL", "std::vector<bool>", 1, "HDF5", "v1.1", 0],' +
    '[10, "VECTOR_INT", "std::vector<int>", 1, "HDF5", "v1.1", 1],' +
    '[11, "VL_VECTOR_INT", "std::vector<int>", 1, "HDF5", "v1.1", 1],' +
    '[12, "VECTOR_FLOAT", "std::vector<float>", 1, "HDF5", "v1.1", 1],' +
    '[13, "VL_VECTOR_FLOAT", "std::vector<float>", 1, "HDF5", "v1.1", 1],' +
    '[14, "VECTOR_DOUBLE", "std::vector<double>", 1, "HDF5", "v1.1", 1],' +
    '[15, "VL_VECTOR_DOUBLE", "std::vector<double>", 1, "HDF5", "v1.1", 1],' +
    '[16, "VECTOR_STRING", "std::vector<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[17, "VL_VECTOR_STRING", "std::vector<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[18, "VECTOR_VL_STRING", "std::vector<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[19, "VL_VECTOR_VL_STRING", "std::vector<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[20, "VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[21, "VL_VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[22, "VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[23, "VL_VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[24, "SET_BOOL", "std::set<bool>", 1, "HDF5", "v1.1", 0],' +
    '[25, "VL_SET_BOOL", "std::set<bool>", 1, "HDF5", "v1.1", 0],' +
    '[26, "SET_INT", "std::set<int>", 1, "HDF5", "v1.1", 1],' +
    '[27, "VL_SET_INT", "std::set<int>", 1, "HDF5", "v1.1", 1],' +
    '[28, "SET_FLOAT", "std::set<float>", 1, "HDF5", "v1.1", 0],' +
    '[29, "VL_SET_FLOAT", "std::set<float>", 1, "HDF5", "v1.1", 0],' +
    '[30, "SET_DOUBLE", "std::set<double>", 1, "HDF5", "v1.1", 0],' +
    '[31, "VL_SET_DOUBLE", "std::set<double>", 1, "HDF5", "v1.1", 0],' +
    '[32, "SET_STRING", "std::set<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[33, "VL_SET_STRING", "std::set<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[34, "SET_VL_STRING", "std::set<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[35, "VL_SET_VL_STRING", "std::set<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[36, "SET_BLOB", "std::set<cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[37, "VL_SET_BLOB", "std::set<cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[38, "SET_UUID", "std::set<boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[39, "VL_SET_UUID", "std::set<boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[40, "LIST_BOOL", "std::list<bool>", 1, "HDF5", "v1.1", 0],' +
    '[41, "VL_LIST_BOOL", "std::list<bool>", 1, "HDF5", "v1.1", 0],' +
    '[42, "LIST_INT", "std::list<int>", 1, "HDF5", "v1.1", 1],' +
    '[43, "VL_LIST_INT", "std::list<int>", 1, "HDF5", "v1.1", 1],' +
    '[44, "LIST_FLOAT", "std::list<float>", 1, "HDF5", "v1.1", 0],' +
    '[45, "VL_LIST_FLOAT", "std::list<float>", 1, "HDF5", "v1.1", 0],' +
    '[46, "LIST_DOUBLE", "std::list<double>", 1, "HDF5", "v1.1", 0],' +
    '[47, "VL_LIST_DOUBLE", "std::list<double>", 1, "HDF5", "v1.1", 0],' +
    '[48, "LIST_STRING", "std::list<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[49, "VL_LIST_STRING", "std::list<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[50, "LIST_VL_STRING", "std::list<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[51, "VL_LIST_VL_STRING", "std::list<std::string>", 2, "HDF5", "v1.1", 1],' +
    '[52, "LIST_BLOB", "std::list<cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[53, "VL_LIST_BLOB", "std::list<cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[54, "LIST_UUID", "std::list<boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[55, "VL_LIST_UUID", "std::list<boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[56, "PAIR_INT_BOOL", "std::pair<int, bool>", 0, "HDF5", "v1.1", 0],' +
    '[57, "PAIR_INT_INT", "std::pair<int, int>", 0, "HDF5", "v1.1", 1],' +
    '[58, "PAIR_INT_FLOAT", "std::pair<int, float>", 0, "HDF5", "v1.1", 0],' +
    '[59, "PAIR_INT_DOUBLE", "std::pair<int, float>", 0, "HDF5", "v1.1", 0],' +
    '[60, "PAIR_INT_STRING", "std::pair<int, std::string>", 1, "HDF5", "v1.1", 0],' +
    '[61, "PAIR_INT_VL_STRING", "std::pair<int, std::string>", 1, "HDF5", "v1.1", 0],' +
    '[62, "PAIR_INT_BLOB", "std::pair<int, cyclus::Blob>", 0, "HDF5", "v1.1", 0],' +
    '[63, "PAIR_INT_UUID", "std::pair<int, boost::uuids::uuid>", 0, "HDF5", "v1.1", 0],' +
    '[64, "PAIR_STRING_BOOL", "std::pair<std::string, bool>", 1, "HDF5", "v1.1", 0],' +
    '[65, "PAIR_STRING_INT", "std::pair<std::string, int>", 1, "HDF5", "v1.1", 0],' +
    '[66, "PAIR_STRING_FLOAT", "std::pair<std::string, float>", 1, "HDF5", "v1.1", 0],' +
    '[67, "PAIR_STRING_DOUBLE", "std::pair<std::string, double>", 1, "HDF5", "v1.1", 0],' +
    '[68, "PAIR_STRING_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.1", 0],' +
    '[69, "PAIR_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.1", 0],' +
    '[70, "PAIR_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[71, "PAIR_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[72, "PAIR_VL_STRING_BOOL", "std::pair<std::string, bool>", 1, "HDF5", "v1.1", 0],' +
    '[73, "PAIR_VL_STRING_INT", "std::pair<std::string, int>", 1, "HDF5", "v1.1", 0],' +
    '[74, "PAIR_VL_STRING_FLOAT", "std::pair<std::string, float>", 1, "HDF5", "v1.1", 0],' +
    '[75, "PAIR_VL_STRING_DOUBLE", "std::pair<std::string, double>", 1, "HDF5", "v1.1", 0],' +
    '[76, "PAIR_VL_STRING_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.1", 0],' +
    '[77, "PAIR_VL_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.1", 0],' +
    '[78, "PAIR_VL_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[79, "PAIR_VL_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[80, "MAP_INT_BOOL", "std::map<int, bool>", 1, "HDF5", "v1.1", 0],' +
    '[81, "VL_MAP_INT_BOOL", "std::map<int, bool>", 1, "HDF5", "v1.1", 0],' +
    '[82, "MAP_INT_INT", "std::map<int, int>", 1, "HDF5", "v1.1", 1],' +
    '[83, "VL_MAP_INT_INT", "std::map<int, int>", 1, "HDF5", "v1.1", 1],' +
    '[84, "MAP_INT_FLOAT", "std::map<int, float>", 1, "HDF5", "v1.1", 0],' +
    '[85, "VL_MAP_INT_FLOAT", "std::map<int, float>", 1, "HDF5", "v1.1", 0],' +
    '[86, "MAP_INT_DOUBLE", "std::map<int, double>", 1, "HDF5", "v1.1", 1],' +
    '[87, "VL_MAP_INT_DOUBLE", "std::map<int, double>", 1, "HDF5", "v1.1", 1],' +
    '[88, "MAP_INT_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.1", 1],' +
    '[89, "VL_MAP_INT_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.1", 1],' +
    '[90, "MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.1", 1],' +
    '[91, "VL_MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.1", 1],' +
    '[92, "MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[93, "VL_MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "HDF5", "v1.1", 0],' +
    '[94, "MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[95, "VL_MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "HDF5", "v1.1", 0],' +
    '[96, "MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.1", 0],' +
    '[97, "VL_MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.1", 0],' +
    '[98, "MAP_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.1", 1],' +
    '[99, "VL_MAP_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.1", 1],' +
    '[100, "MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.1", 0],' +
    '[101, "VL_MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.1", 0],' +
    '[102, "MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.1", 1],' +
    '[103, "VL_MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.1", 1],' +
    '[104, "MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.1", 1],' +
    '[105, "VL_MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.1", 1],' +
    '[106, "MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.1", 1],' +
    '[107, "VL_MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.1", 1],' +
    '[108, "MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.1", 0],' +
    '[109, "VL_MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.1", 0],' +
    '[110, "MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.1", 0],' +
    '[111, "VL_MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.1", 0],' +
    '[112, "MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.1", 0],' +
    '[113, "VL_MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.1", 0],' +
    '[114, "MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.1", 1],' +
    '[115, "VL_MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.1", 1],' +
    '[116, "MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.1", 0],' +
    '[117, "VL_MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.1", 0],' +
    '[118, "MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.1", 1],' +
    '[119, "VL_MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.1", 1],' +
    '[120, "MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.1", 1],' +
    '[121, "VL_MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.1", 1],' +
    '[122, "MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.1", 1],' +
    '[123, "VL_MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.1", 1],' +
    '[124, "MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.1", 0],' +
    '[125, "VL_MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.1", 0],' +
    '[126, "MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.1", 0],' +
    '[127, "VL_MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.1", 0],' +
    '[0, "BOOL", "bool", 0, "HDF5", "v1.1", 1],' +
    '[0, "BOOL", "bool", 0, "SQLite", "v1.2", 1],' +
    '[1, "INT", "int", 0, "SQLite", "v1.2", 1],' +
    '[2, "FLOAT", "float", 0, "SQLite", "v1.2", 1],' +
    '[3, "DOUBLE", "double", 0, "SQLite", "v1.2", 1],' +
    '[4, "STRING", "std::string", 1, "SQLite", "v1.2", 1],' +
    '[5, "VL_STRING", "std::string", 1, "SQLite", "v1.2", 1],' +
    '[6, "BLOB", "cyclus::Blob", 0, "SQLite", "v1.2", 1],' +
    '[7, "UUID", "boost::uuids::uuid", 0, "SQLite", "v1.2", 1],' +
    '[8, "VECTOR_BOOL", "std::vector<bool>", 1, "SQLite", "v1.2", 0],' +
    '[9, "VL_VECTOR_BOOL", "std::vector<bool>", 1, "SQLite", "v1.2", 0],' +
    '[10, "VECTOR_INT", "std::vector<int>", 1, "SQLite", "v1.2", 1],' +
    '[11, "VL_VECTOR_INT", "std::vector<int>", 1, "SQLite", "v1.2", 1],' +
    '[12, "VECTOR_FLOAT", "std::vector<float>", 1, "SQLite", "v1.2", 0],' +
    '[13, "VL_VECTOR_FLOAT", "std::vector<float>", 1, "SQLite", "v1.2", 0],' +
    '[14, "VECTOR_DOUBLE", "std::vector<double>", 1, "SQLite", "v1.2", 1],' +
    '[15, "VL_VECTOR_DOUBLE", "std::vector<double>", 1, "SQLite", "v1.2", 1],' +
    '[16, "VECTOR_STRING", "std::vector<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[17, "VL_VECTOR_STRING", "std::vector<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[18, "VECTOR_VL_STRING", "std::vector<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[19, "VL_VECTOR_VL_STRING", "std::vector<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[20, "VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[21, "VL_VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[22, "VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[23, "VL_VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[24, "SET_BOOL", "std::set<bool>", 1, "SQLite", "v1.2", 0],' +
    '[25, "VL_SET_BOOL", "std::set<bool>", 1, "SQLite", "v1.2", 0],' +
    '[26, "SET_INT", "std::set<int>", 1, "SQLite", "v1.2", 1],' +
    '[27, "VL_SET_INT", "std::set<int>", 1, "SQLite", "v1.2", 1],' +
    '[28, "SET_FLOAT", "std::set<float>", 1, "SQLite", "v1.2", 0],' +
    '[29, "VL_SET_FLOAT", "std::set<float>", 1, "SQLite", "v1.2", 0],' +
    '[30, "SET_DOUBLE", "std::set<double>", 1, "SQLite", "v1.2", 0],' +
    '[31, "VL_SET_DOUBLE", "std::set<double>", 1, "SQLite", "v1.2", 0],' +
    '[32, "SET_STRING", "std::set<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[33, "VL_SET_STRING", "std::set<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[34, "SET_VL_STRING", "std::set<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[35, "VL_SET_VL_STRING", "std::set<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[36, "SET_BLOB", "std::set<cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[37, "VL_SET_BLOB", "std::set<cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[38, "SET_UUID", "std::set<boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[39, "VL_SET_UUID", "std::set<boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[40, "LIST_BOOL", "std::list<bool>", 1, "SQLite", "v1.2", 0],' +
    '[41, "VL_LIST_BOOL", "std::list<bool>", 1, "SQLite", "v1.2", 0],' +
    '[42, "LIST_INT", "std::list<int>", 1, "SQLite", "v1.2", 1],' +
    '[43, "VL_LIST_INT", "std::list<int>", 1, "SQLite", "v1.2", 1],' +
    '[44, "LIST_FLOAT", "std::list<float>", 1, "SQLite", "v1.2", 0],' +
    '[45, "VL_LIST_FLOAT", "std::list<float>", 1, "SQLite", "v1.2", 0],' +
    '[46, "LIST_DOUBLE", "std::list<double>", 1, "SQLite", "v1.2", 0],' +
    '[47, "VL_LIST_DOUBLE", "std::list<double>", 1, "SQLite", "v1.2", 0],' +
    '[48, "LIST_STRING", "std::list<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[49, "VL_LIST_STRING", "std::list<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[50, "LIST_VL_STRING", "std::list<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[51, "VL_LIST_VL_STRING", "std::list<std::string>", 2, "SQLite", "v1.2", 1],' +
    '[52, "LIST_BLOB", "std::list<cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[53, "VL_LIST_BLOB", "std::list<cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[54, "LIST_UUID", "std::list<boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[55, "VL_LIST_UUID", "std::list<boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[56, "PAIR_INT_BOOL", "std::pair<int, bool>", 0, "SQLite", "v1.2", 0],' +
    '[57, "PAIR_INT_INT", "std::pair<int, int>", 0, "SQLite", "v1.2", 0],' +
    '[58, "PAIR_INT_FLOAT", "std::pair<int, float>", 0, "SQLite", "v1.2", 0],' +
    '[59, "PAIR_INT_DOUBLE", "std::pair<int, float>", 0, "SQLite", "v1.2", 0],' +
    '[60, "PAIR_INT_STRING", "std::pair<int, std::string>", 1, "SQLite", "v1.2", 0],' +
    '[61, "PAIR_INT_VL_STRING", "std::pair<int, std::string>", 1, "SQLite", "v1.2", 0],' +
    '[62, "PAIR_INT_BLOB", "std::pair<int, cyclus::Blob>", 0, "SQLite", "v1.2", 0],' +
    '[63, "PAIR_INT_UUID", "std::pair<int, boost::uuids::uuid>", 0, "SQLite", "v1.2", 0],' +
    '[64, "PAIR_STRING_BOOL", "std::pair<std::string, bool>", 1, "SQLite", "v1.2", 0],' +
    '[65, "PAIR_STRING_INT", "std::pair<std::string, int>", 1, "SQLite", "v1.2", 0],' +
    '[66, "PAIR_STRING_FLOAT", "std::pair<std::string, float>", 1, "SQLite", "v1.2", 0],' +
    '[67, "PAIR_STRING_DOUBLE", "std::pair<std::string, double>", 1, "SQLite", "v1.2", 0],' +
    '[68, "PAIR_STRING_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.2", 0],' +
    '[69, "PAIR_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.2", 0],' +
    '[70, "PAIR_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[71, "PAIR_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[72, "PAIR_VL_STRING_BOOL", "std::pair<std::string, bool>", 1, "SQLite", "v1.2", 0],' +
    '[73, "PAIR_VL_STRING_INT", "std::pair<std::string, int>", 1, "SQLite", "v1.2", 0],' +
    '[74, "PAIR_VL_STRING_FLOAT", "std::pair<std::string, float>", 1, "SQLite", "v1.2", 0],' +
    '[75, "PAIR_VL_STRING_DOUBLE", "std::pair<std::string, double>", 1, "SQLite", "v1.2", 0],' +
    '[76, "PAIR_VL_STRING_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.2", 0],' +
    '[77, "PAIR_VL_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.2", 0],' +
    '[78, "PAIR_VL_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[79, "PAIR_VL_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[80, "MAP_INT_BOOL", "std::map<int, bool>", 1, "SQLite", "v1.2", 0],' +
    '[81, "VL_MAP_INT_BOOL", "std::map<int, bool>", 1, "SQLite", "v1.2", 0],' +
    '[82, "MAP_INT_INT", "std::map<int, int>", 1, "SQLite", "v1.2", 1],' +
    '[83, "VL_MAP_INT_INT", "std::map<int, int>", 1, "SQLite", "v1.2", 1],' +
    '[84, "MAP_INT_FLOAT", "std::map<int, float>", 1, "SQLite", "v1.2", 0],' +
    '[85, "VL_MAP_INT_FLOAT", "std::map<int, float>", 1, "SQLite", "v1.2", 0],' +
    '[86, "MAP_INT_DOUBLE", "std::map<int, double>", 1, "SQLite", "v1.2", 1],' +
    '[87, "VL_MAP_INT_DOUBLE", "std::map<int, double>", 1, "SQLite", "v1.2", 1],' +
    '[88, "MAP_INT_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.2", 1],' +
    '[89, "VL_MAP_INT_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.2", 1],' +
    '[90, "MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.2", 1],' +
    '[91, "VL_MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.2", 1],' +
    '[92, "MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[93, "VL_MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "SQLite", "v1.2", 0],' +
    '[94, "MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[95, "VL_MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "SQLite", "v1.2", 0],' +
    '[96, "MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.2", 0],' +
    '[97, "VL_MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.2", 0],' +
    '[98, "MAP_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.2", 1],' +
    '[99, "VL_MAP_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.2", 1],' +
    '[100, "MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.2", 0],' +
    '[101, "VL_MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.2", 0],' +
    '[102, "MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.2", 1],' +
    '[103, "VL_MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.2", 1],' +
    '[104, "MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.2", 1],' +
    '[105, "VL_MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.2", 1],' +
    '[106, "MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.2", 1],' +
    '[107, "VL_MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.2", 1],' +
    '[108, "MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.2", 0],' +
    '[109, "VL_MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.2", 0],' +
    '[110, "MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.2", 0],' +
    '[111, "VL_MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.2", 0],' +
    '[112, "MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.2", 0],' +
    '[113, "VL_MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.2", 0],' +
    '[114, "MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.2", 1],' +
    '[115, "VL_MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.2", 1],' +
    '[116, "MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.2", 0],' +
    '[117, "VL_MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.2", 0],' +
    '[118, "MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.2", 1],' +
    '[119, "VL_MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.2", 1],' +
    '[120, "MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.2", 1],' +
    '[121, "VL_MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.2", 1],' +
    '[122, "MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.2", 1],' +
    '[123, "VL_MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.2", 1],' +
    '[124, "MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.2", 0],' +
    '[125, "VL_MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.2", 0],' +
    '[126, "MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.2", 0],' +
    '[127, "VL_MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.2", 0],' +
    '[1, "INT", "int", 0, "HDF5", "v1.2", 1],' +
    '[2, "FLOAT", "float", 0, "HDF5", "v1.2", 1],' +
    '[3, "DOUBLE", "double", 0, "HDF5", "v1.2", 1],' +
    '[4, "STRING", "std::string", 1, "HDF5", "v1.2", 1],' +
    '[5, "VL_STRING", "std::string", 1, "HDF5", "v1.2", 1],' +
    '[6, "BLOB", "cyclus::Blob", 0, "HDF5", "v1.2", 1],' +
    '[7, "UUID", "boost::uuids::uuid", 0, "HDF5", "v1.2", 1],' +
    '[8, "VECTOR_BOOL", "std::vector<bool>", 1, "HDF5", "v1.2", 0],' +
    '[9, "VL_VECTOR_BOOL", "std::vector<bool>", 1, "HDF5", "v1.2", 0],' +
    '[10, "VECTOR_INT", "std::vector<int>", 1, "HDF5", "v1.2", 1],' +
    '[11, "VL_VECTOR_INT", "std::vector<int>", 1, "HDF5", "v1.2", 1],' +
    '[12, "VECTOR_FLOAT", "std::vector<float>", 1, "HDF5", "v1.2", 1],' +
    '[13, "VL_VECTOR_FLOAT", "std::vector<float>", 1, "HDF5", "v1.2", 1],' +
    '[14, "VECTOR_DOUBLE", "std::vector<double>", 1, "HDF5", "v1.2", 1],' +
    '[15, "VL_VECTOR_DOUBLE", "std::vector<double>", 1, "HDF5", "v1.2", 1],' +
    '[16, "VECTOR_STRING", "std::vector<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[17, "VL_VECTOR_STRING", "std::vector<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[18, "VECTOR_VL_STRING", "std::vector<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[19, "VL_VECTOR_VL_STRING", "std::vector<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[20, "VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[21, "VL_VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[22, "VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[23, "VL_VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[24, "SET_BOOL", "std::set<bool>", 1, "HDF5", "v1.2", 0],' +
    '[25, "VL_SET_BOOL", "std::set<bool>", 1, "HDF5", "v1.2", 0],' +
    '[26, "SET_INT", "std::set<int>", 1, "HDF5", "v1.2", 1],' +
    '[27, "VL_SET_INT", "std::set<int>", 1, "HDF5", "v1.2", 1],' +
    '[28, "SET_FLOAT", "std::set<float>", 1, "HDF5", "v1.2", 0],' +
    '[29, "VL_SET_FLOAT", "std::set<float>", 1, "HDF5", "v1.2", 0],' +
    '[30, "SET_DOUBLE", "std::set<double>", 1, "HDF5", "v1.2", 0],' +
    '[31, "VL_SET_DOUBLE", "std::set<double>", 1, "HDF5", "v1.2", 0],' +
    '[32, "SET_STRING", "std::set<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[33, "VL_SET_STRING", "std::set<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[34, "SET_VL_STRING", "std::set<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[35, "VL_SET_VL_STRING", "std::set<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[36, "SET_BLOB", "std::set<cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[37, "VL_SET_BLOB", "std::set<cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[38, "SET_UUID", "std::set<boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[39, "VL_SET_UUID", "std::set<boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[40, "LIST_BOOL", "std::list<bool>", 1, "HDF5", "v1.2", 0],' +
    '[41, "VL_LIST_BOOL", "std::list<bool>", 1, "HDF5", "v1.2", 0],' +
    '[42, "LIST_INT", "std::list<int>", 1, "HDF5", "v1.2", 1],' +
    '[43, "VL_LIST_INT", "std::list<int>", 1, "HDF5", "v1.2", 1],' +
    '[44, "LIST_FLOAT", "std::list<float>", 1, "HDF5", "v1.2", 0],' +
    '[45, "VL_LIST_FLOAT", "std::list<float>", 1, "HDF5", "v1.2", 0],' +
    '[46, "LIST_DOUBLE", "std::list<double>", 1, "HDF5", "v1.2", 0],' +
    '[47, "VL_LIST_DOUBLE", "std::list<double>", 1, "HDF5", "v1.2", 0],' +
    '[48, "LIST_STRING", "std::list<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[49, "VL_LIST_STRING", "std::list<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[50, "LIST_VL_STRING", "std::list<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[51, "VL_LIST_VL_STRING", "std::list<std::string>", 2, "HDF5", "v1.2", 1],' +
    '[52, "LIST_BLOB", "std::list<cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[53, "VL_LIST_BLOB", "std::list<cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[54, "LIST_UUID", "std::list<boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[55, "VL_LIST_UUID", "std::list<boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[56, "PAIR_INT_BOOL", "std::pair<int, bool>", 0, "HDF5", "v1.2", 0],' +
    '[57, "PAIR_INT_INT", "std::pair<int, int>", 0, "HDF5", "v1.2", 1],' +
    '[58, "PAIR_INT_FLOAT", "std::pair<int, float>", 0, "HDF5", "v1.2", 0],' +
    '[59, "PAIR_INT_DOUBLE", "std::pair<int, float>", 0, "HDF5", "v1.2", 0],' +
    '[60, "PAIR_INT_STRING", "std::pair<int, std::string>", 1, "HDF5", "v1.2", 0],' +
    '[61, "PAIR_INT_VL_STRING", "std::pair<int, std::string>", 1, "HDF5", "v1.2", 0],' +
    '[62, "PAIR_INT_BLOB", "std::pair<int, cyclus::Blob>", 0, "HDF5", "v1.2", 0],' +
    '[63, "PAIR_INT_UUID", "std::pair<int, boost::uuids::uuid>", 0, "HDF5", "v1.2", 0],' +
    '[64, "PAIR_STRING_BOOL", "std::pair<std::string, bool>", 1, "HDF5", "v1.2", 0],' +
    '[65, "PAIR_STRING_INT", "std::pair<std::string, int>", 1, "HDF5", "v1.2", 0],' +
    '[66, "PAIR_STRING_FLOAT", "std::pair<std::string, float>", 1, "HDF5", "v1.2", 0],' +
    '[67, "PAIR_STRING_DOUBLE", "std::pair<std::string, double>", 1, "HDF5", "v1.2", 0],' +
    '[68, "PAIR_STRING_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.2", 0],' +
    '[69, "PAIR_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.2", 0],' +
    '[70, "PAIR_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[71, "PAIR_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[72, "PAIR_VL_STRING_BOOL", "std::pair<std::string, bool>", 1, "HDF5", "v1.2", 0],' +
    '[73, "PAIR_VL_STRING_INT", "std::pair<std::string, int>", 1, "HDF5", "v1.2", 0],' +
    '[74, "PAIR_VL_STRING_FLOAT", "std::pair<std::string, float>", 1, "HDF5", "v1.2", 0],' +
    '[75, "PAIR_VL_STRING_DOUBLE", "std::pair<std::string, double>", 1, "HDF5", "v1.2", 0],' +
    '[76, "PAIR_VL_STRING_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.2", 0],' +
    '[77, "PAIR_VL_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.2", 0],' +
    '[78, "PAIR_VL_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[79, "PAIR_VL_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[80, "MAP_INT_BOOL", "std::map<int, bool>", 1, "HDF5", "v1.2", 0],' +
    '[81, "VL_MAP_INT_BOOL", "std::map<int, bool>", 1, "HDF5", "v1.2", 0],' +
    '[82, "MAP_INT_INT", "std::map<int, int>", 1, "HDF5", "v1.2", 1],' +
    '[83, "VL_MAP_INT_INT", "std::map<int, int>", 1, "HDF5", "v1.2", 1],' +
    '[84, "MAP_INT_FLOAT", "std::map<int, float>", 1, "HDF5", "v1.2", 0],' +
    '[85, "VL_MAP_INT_FLOAT", "std::map<int, float>", 1, "HDF5", "v1.2", 0],' +
    '[86, "MAP_INT_DOUBLE", "std::map<int, double>", 1, "HDF5", "v1.2", 1],' +
    '[87, "VL_MAP_INT_DOUBLE", "std::map<int, double>", 1, "HDF5", "v1.2", 1],' +
    '[88, "MAP_INT_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.2", 1],' +
    '[89, "VL_MAP_INT_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.2", 1],' +
    '[90, "MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.2", 1],' +
    '[91, "VL_MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.2", 1],' +
    '[92, "MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[93, "VL_MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "HDF5", "v1.2", 0],' +
    '[94, "MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[95, "VL_MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "HDF5", "v1.2", 0],' +
    '[96, "MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.2", 0],' +
    '[97, "VL_MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.2", 0],' +
    '[98, "MAP_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.2", 1],' +
    '[99, "VL_MAP_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.2", 1],' +
    '[100, "MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.2", 0],' +
    '[101, "VL_MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.2", 0],' +
    '[102, "MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.2", 1],' +
    '[103, "VL_MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.2", 1],' +
    '[104, "MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.2", 1],' +
    '[105, "VL_MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.2", 1],' +
    '[106, "MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.2", 1],' +
    '[107, "VL_MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.2", 1],' +
    '[108, "MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.2", 0],' +
    '[109, "VL_MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.2", 0],' +
    '[110, "MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.2", 0],' +
    '[111, "VL_MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.2", 0],' +
    '[112, "MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.2", 0],' +
    '[113, "VL_MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.2", 0],' +
    '[114, "MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.2", 1],' +
    '[115, "VL_MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.2", 1],' +
    '[116, "MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.2", 0],' +
    '[117, "VL_MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.2", 0],' +
    '[118, "MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.2", 1],' +
    '[119, "VL_MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.2", 1],' +
    '[120, "MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.2", 1],' +
    '[121, "VL_MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.2", 1],' +
    '[122, "MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.2", 1],' +
    '[123, "VL_MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.2", 1],' +
    '[124, "MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.2", 0],' +
    '[125, "VL_MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.2", 0],' +
    '[126, "MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.2", 0],' +
    '[127, "VL_MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.2", 0],' +
    '[0, "BOOL", "bool", 0, "HDF5", "v1.2", 1],' +
    '[0, "BOOL", "bool", 0, "SQLite", "v1.3", 1],' +
    '[1, "INT", "int", 0, "SQLite", "v1.3", 1],' +
    '[2, "FLOAT", "float", 0, "SQLite", "v1.3", 1],' +
    '[3, "DOUBLE", "double", 0, "SQLite", "v1.3", 1],' +
    '[4, "STRING", "std::string", 1, "SQLite", "v1.3", 1],' +
    '[5, "VL_STRING", "std::string", 1, "SQLite", "v1.3", 1],' +
    '[6, "BLOB", "cyclus::Blob", 0, "SQLite", "v1.3", 1],' +
    '[7, "UUID", "boost::uuids::uuid", 0, "SQLite", "v1.3", 1],' +
    '[8, "VECTOR_BOOL", "std::vector<bool>", 1, "SQLite", "v1.3", 0],' +
    '[9, "VL_VECTOR_BOOL", "std::vector<bool>", 1, "SQLite", "v1.3", 0],' +
    '[10, "VECTOR_INT", "std::vector<int>", 1, "SQLite", "v1.3", 1],' +
    '[11, "VL_VECTOR_INT", "std::vector<int>", 1, "SQLite", "v1.3", 1],' +
    '[12, "VECTOR_FLOAT", "std::vector<float>", 1, "SQLite", "v1.3", 0],' +
    '[13, "VL_VECTOR_FLOAT", "std::vector<float>", 1, "SQLite", "v1.3", 0],' +
    '[14, "VECTOR_DOUBLE", "std::vector<double>", 1, "SQLite", "v1.3", 1],' +
    '[15, "VL_VECTOR_DOUBLE", "std::vector<double>", 1, "SQLite", "v1.3", 1],' +
    '[16, "VECTOR_STRING", "std::vector<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[17, "VL_VECTOR_STRING", "std::vector<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[18, "VECTOR_VL_STRING", "std::vector<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[19, "VL_VECTOR_VL_STRING", "std::vector<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[20, "VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[21, "VL_VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[22, "VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[23, "VL_VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[24, "SET_BOOL", "std::set<bool>", 1, "SQLite", "v1.3", 0],' +
    '[25, "VL_SET_BOOL", "std::set<bool>", 1, "SQLite", "v1.3", 0],' +
    '[26, "SET_INT", "std::set<int>", 1, "SQLite", "v1.3", 1],' +
    '[27, "VL_SET_INT", "std::set<int>", 1, "SQLite", "v1.3", 1],' +
    '[28, "SET_FLOAT", "std::set<float>", 1, "SQLite", "v1.3", 0],' +
    '[29, "VL_SET_FLOAT", "std::set<float>", 1, "SQLite", "v1.3", 0],' +
    '[30, "SET_DOUBLE", "std::set<double>", 1, "SQLite", "v1.3", 0],' +
    '[31, "VL_SET_DOUBLE", "std::set<double>", 1, "SQLite", "v1.3", 0],' +
    '[32, "SET_STRING", "std::set<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[33, "VL_SET_STRING", "std::set<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[34, "SET_VL_STRING", "std::set<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[35, "VL_SET_VL_STRING", "std::set<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[36, "SET_BLOB", "std::set<cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[37, "VL_SET_BLOB", "std::set<cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[38, "SET_UUID", "std::set<boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[39, "VL_SET_UUID", "std::set<boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[40, "LIST_BOOL", "std::list<bool>", 1, "SQLite", "v1.3", 0],' +
    '[41, "VL_LIST_BOOL", "std::list<bool>", 1, "SQLite", "v1.3", 0],' +
    '[42, "LIST_INT", "std::list<int>", 1, "SQLite", "v1.3", 1],' +
    '[43, "VL_LIST_INT", "std::list<int>", 1, "SQLite", "v1.3", 1],' +
    '[44, "LIST_FLOAT", "std::list<float>", 1, "SQLite", "v1.3", 0],' +
    '[45, "VL_LIST_FLOAT", "std::list<float>", 1, "SQLite", "v1.3", 0],' +
    '[46, "LIST_DOUBLE", "std::list<double>", 1, "SQLite", "v1.3", 0],' +
    '[47, "VL_LIST_DOUBLE", "std::list<double>", 1, "SQLite", "v1.3", 0],' +
    '[48, "LIST_STRING", "std::list<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[49, "VL_LIST_STRING", "std::list<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[50, "LIST_VL_STRING", "std::list<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[51, "VL_LIST_VL_STRING", "std::list<std::string>", 2, "SQLite", "v1.3", 1],' +
    '[52, "LIST_BLOB", "std::list<cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[53, "VL_LIST_BLOB", "std::list<cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[54, "LIST_UUID", "std::list<boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[55, "VL_LIST_UUID", "std::list<boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[56, "PAIR_INT_BOOL", "std::pair<int, bool>", 0, "SQLite", "v1.3", 0],' +
    '[57, "PAIR_INT_INT", "std::pair<int, int>", 0, "SQLite", "v1.3", 0],' +
    '[58, "PAIR_INT_FLOAT", "std::pair<int, float>", 0, "SQLite", "v1.3", 0],' +
    '[59, "PAIR_INT_DOUBLE", "std::pair<int, float>", 0, "SQLite", "v1.3", 0],' +
    '[60, "PAIR_INT_STRING", "std::pair<int, std::string>", 1, "SQLite", "v1.3", 0],' +
    '[61, "PAIR_INT_VL_STRING", "std::pair<int, std::string>", 1, "SQLite", "v1.3", 0],' +
    '[62, "PAIR_INT_BLOB", "std::pair<int, cyclus::Blob>", 0, "SQLite", "v1.3", 0],' +
    '[63, "PAIR_INT_UUID", "std::pair<int, boost::uuids::uuid>", 0, "SQLite", "v1.3", 0],' +
    '[64, "PAIR_STRING_BOOL", "std::pair<std::string, bool>", 1, "SQLite", "v1.3", 0],' +
    '[65, "PAIR_STRING_INT", "std::pair<std::string, int>", 1, "SQLite", "v1.3", 0],' +
    '[66, "PAIR_STRING_FLOAT", "std::pair<std::string, float>", 1, "SQLite", "v1.3", 0],' +
    '[67, "PAIR_STRING_DOUBLE", "std::pair<std::string, double>", 1, "SQLite", "v1.3", 0],' +
    '[68, "PAIR_STRING_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.3", 0],' +
    '[69, "PAIR_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.3", 0],' +
    '[70, "PAIR_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[71, "PAIR_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[72, "PAIR_VL_STRING_BOOL", "std::pair<std::string, bool>", 1, "SQLite", "v1.3", 0],' +
    '[73, "PAIR_VL_STRING_INT", "std::pair<std::string, int>", 1, "SQLite", "v1.3", 0],' +
    '[74, "PAIR_VL_STRING_FLOAT", "std::pair<std::string, float>", 1, "SQLite", "v1.3", 0],' +
    '[75, "PAIR_VL_STRING_DOUBLE", "std::pair<std::string, double>", 1, "SQLite", "v1.3", 0],' +
    '[76, "PAIR_VL_STRING_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.3", 0],' +
    '[77, "PAIR_VL_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "SQLite", "v1.3", 0],' +
    '[78, "PAIR_VL_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[79, "PAIR_VL_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[80, "MAP_INT_BOOL", "std::map<int, bool>", 1, "SQLite", "v1.3", 0],' +
    '[81, "VL_MAP_INT_BOOL", "std::map<int, bool>", 1, "SQLite", "v1.3", 0],' +
    '[82, "MAP_INT_INT", "std::map<int, int>", 1, "SQLite", "v1.3", 1],' +
    '[83, "VL_MAP_INT_INT", "std::map<int, int>", 1, "SQLite", "v1.3", 1],' +
    '[84, "MAP_INT_FLOAT", "std::map<int, float>", 1, "SQLite", "v1.3", 0],' +
    '[85, "VL_MAP_INT_FLOAT", "std::map<int, float>", 1, "SQLite", "v1.3", 0],' +
    '[86, "MAP_INT_DOUBLE", "std::map<int, double>", 1, "SQLite", "v1.3", 1],' +
    '[87, "VL_MAP_INT_DOUBLE", "std::map<int, double>", 1, "SQLite", "v1.3", 1],' +
    '[88, "MAP_INT_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.3", 1],' +
    '[89, "VL_MAP_INT_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.3", 1],' +
    '[90, "MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.3", 1],' +
    '[91, "VL_MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "SQLite", "v1.3", 1],' +
    '[92, "MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[93, "VL_MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "SQLite", "v1.3", 0],' +
    '[94, "MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[95, "VL_MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "SQLite", "v1.3", 0],' +
    '[96, "MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.3", 0],' +
    '[97, "VL_MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.3", 0],' +
    '[98, "MAP_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.3", 1],' +
    '[99, "VL_MAP_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.3", 1],' +
    '[100, "MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.3", 0],' +
    '[101, "VL_MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.3", 0],' +
    '[102, "MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.3", 1],' +
    '[103, "VL_MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.3", 1],' +
    '[104, "MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.3", 1],' +
    '[105, "VL_MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.3", 1],' +
    '[106, "MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.3", 1],' +
    '[107, "VL_MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.3", 1],' +
    '[108, "MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.3", 0],' +
    '[109, "VL_MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.3", 0],' +
    '[110, "MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.3", 0],' +
    '[111, "VL_MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.3", 0],' +
    '[112, "MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.3", 0],' +
    '[113, "VL_MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "SQLite", "v1.3", 0],' +
    '[114, "MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.3", 1],' +
    '[115, "VL_MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "SQLite", "v1.3", 1],' +
    '[116, "MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.3", 0],' +
    '[117, "VL_MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "SQLite", "v1.3", 0],' +
    '[118, "MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.3", 1],' +
    '[119, "VL_MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "SQLite", "v1.3", 1],' +
    '[120, "MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.3", 1],' +
    '[121, "VL_MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.3", 1],' +
    '[122, "MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.3", 1],' +
    '[123, "VL_MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "SQLite", "v1.3", 1],' +
    '[124, "MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.3", 0],' +
    '[125, "VL_MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "SQLite", "v1.3", 0],' +
    '[126, "MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.3", 0],' +
    '[127, "VL_MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "SQLite", "v1.3", 0],' +
    '[1, "INT", "int", 0, "HDF5", "v1.3", 1],' +
    '[2, "FLOAT", "float", 0, "HDF5", "v1.3", 1],' +
    '[3, "DOUBLE", "double", 0, "HDF5", "v1.3", 1],' +
    '[4, "STRING", "std::string", 1, "HDF5", "v1.3", 1],' +
    '[5, "VL_STRING", "std::string", 1, "HDF5", "v1.3", 1],' +
    '[6, "BLOB", "cyclus::Blob", 0, "HDF5", "v1.3", 1],' +
    '[7, "UUID", "boost::uuids::uuid", 0, "HDF5", "v1.3", 1],' +
    '[8, "VECTOR_BOOL", "std::vector<bool>", 1, "HDF5", "v1.3", 0],' +
    '[9, "VL_VECTOR_BOOL", "std::vector<bool>", 1, "HDF5", "v1.3", 0],' +
    '[10, "VECTOR_INT", "std::vector<int>", 1, "HDF5", "v1.3", 1],' +
    '[11, "VL_VECTOR_INT", "std::vector<int>", 1, "HDF5", "v1.3", 1],' +
    '[12, "VECTOR_FLOAT", "std::vector<float>", 1, "HDF5", "v1.3", 1],' +
    '[13, "VL_VECTOR_FLOAT", "std::vector<float>", 1, "HDF5", "v1.3", 1],' +
    '[14, "VECTOR_DOUBLE", "std::vector<double>", 1, "HDF5", "v1.3", 1],' +
    '[15, "VL_VECTOR_DOUBLE", "std::vector<double>", 1, "HDF5", "v1.3", 1],' +
    '[16, "VECTOR_STRING", "std::vector<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[17, "VL_VECTOR_STRING", "std::vector<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[18, "VECTOR_VL_STRING", "std::vector<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[19, "VL_VECTOR_VL_STRING", "std::vector<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[20, "VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[21, "VL_VECTOR_BLOB", "std::vector<cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[22, "VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[23, "VL_VECTOR_UUID", "std::vector<boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[24, "SET_BOOL", "std::set<bool>", 1, "HDF5", "v1.3", 0],' +
    '[25, "VL_SET_BOOL", "std::set<bool>", 1, "HDF5", "v1.3", 0],' +
    '[26, "SET_INT", "std::set<int>", 1, "HDF5", "v1.3", 1],' +
    '[27, "VL_SET_INT", "std::set<int>", 1, "HDF5", "v1.3", 1],' +
    '[28, "SET_FLOAT", "std::set<float>", 1, "HDF5", "v1.3", 0],' +
    '[29, "VL_SET_FLOAT", "std::set<float>", 1, "HDF5", "v1.3", 0],' +
    '[30, "SET_DOUBLE", "std::set<double>", 1, "HDF5", "v1.3", 0],' +
    '[31, "VL_SET_DOUBLE", "std::set<double>", 1, "HDF5", "v1.3", 0],' +
    '[32, "SET_STRING", "std::set<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[33, "VL_SET_STRING", "std::set<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[34, "SET_VL_STRING", "std::set<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[35, "VL_SET_VL_STRING", "std::set<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[36, "SET_BLOB", "std::set<cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[37, "VL_SET_BLOB", "std::set<cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[38, "SET_UUID", "std::set<boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[39, "VL_SET_UUID", "std::set<boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[40, "LIST_BOOL", "std::list<bool>", 1, "HDF5", "v1.3", 0],' +
    '[41, "VL_LIST_BOOL", "std::list<bool>", 1, "HDF5", "v1.3", 0],' +
    '[42, "LIST_INT", "std::list<int>", 1, "HDF5", "v1.3", 1],' +
    '[43, "VL_LIST_INT", "std::list<int>", 1, "HDF5", "v1.3", 1],' +
    '[44, "LIST_FLOAT", "std::list<float>", 1, "HDF5", "v1.3", 0],' +
    '[45, "VL_LIST_FLOAT", "std::list<float>", 1, "HDF5", "v1.3", 0],' +
    '[46, "LIST_DOUBLE", "std::list<double>", 1, "HDF5", "v1.3", 0],' +
    '[47, "VL_LIST_DOUBLE", "std::list<double>", 1, "HDF5", "v1.3", 0],' +
    '[48, "LIST_STRING", "std::list<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[49, "VL_LIST_STRING", "std::list<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[50, "LIST_VL_STRING", "std::list<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[51, "VL_LIST_VL_STRING", "std::list<std::string>", 2, "HDF5", "v1.3", 1],' +
    '[52, "LIST_BLOB", "std::list<cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[53, "VL_LIST_BLOB", "std::list<cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[54, "LIST_UUID", "std::list<boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[55, "VL_LIST_UUID", "std::list<boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[56, "PAIR_INT_BOOL", "std::pair<int, bool>", 0, "HDF5", "v1.3", 0],' +
    '[57, "PAIR_INT_INT", "std::pair<int, int>", 0, "HDF5", "v1.3", 1],' +
    '[58, "PAIR_INT_FLOAT", "std::pair<int, float>", 0, "HDF5", "v1.3", 0],' +
    '[59, "PAIR_INT_DOUBLE", "std::pair<int, float>", 0, "HDF5", "v1.3", 0],' +
    '[60, "PAIR_INT_STRING", "std::pair<int, std::string>", 1, "HDF5", "v1.3", 0],' +
    '[61, "PAIR_INT_VL_STRING", "std::pair<int, std::string>", 1, "HDF5", "v1.3", 0],' +
    '[62, "PAIR_INT_BLOB", "std::pair<int, cyclus::Blob>", 0, "HDF5", "v1.3", 0],' +
    '[63, "PAIR_INT_UUID", "std::pair<int, boost::uuids::uuid>", 0, "HDF5", "v1.3", 0],' +
    '[64, "PAIR_STRING_BOOL", "std::pair<std::string, bool>", 1, "HDF5", "v1.3", 0],' +
    '[65, "PAIR_STRING_INT", "std::pair<std::string, int>", 1, "HDF5", "v1.3", 0],' +
    '[66, "PAIR_STRING_FLOAT", "std::pair<std::string, float>", 1, "HDF5", "v1.3", 0],' +
    '[67, "PAIR_STRING_DOUBLE", "std::pair<std::string, double>", 1, "HDF5", "v1.3", 0],' +
    '[68, "PAIR_STRING_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.3", 0],' +
    '[69, "PAIR_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.3", 0],' +
    '[70, "PAIR_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[71, "PAIR_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[72, "PAIR_VL_STRING_BOOL", "std::pair<std::string, bool>", 1, "HDF5", "v1.3", 0],' +
    '[73, "PAIR_VL_STRING_INT", "std::pair<std::string, int>", 1, "HDF5", "v1.3", 0],' +
    '[74, "PAIR_VL_STRING_FLOAT", "std::pair<std::string, float>", 1, "HDF5", "v1.3", 0],' +
    '[75, "PAIR_VL_STRING_DOUBLE", "std::pair<std::string, double>", 1, "HDF5", "v1.3", 0],' +
    '[76, "PAIR_VL_STRING_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.3", 0],' +
    '[77, "PAIR_VL_STRING_VL_STRING", "std::pair<std::string, std::string>", 2, "HDF5", "v1.3", 0],' +
    '[78, "PAIR_VL_STRING_BLOB", "std::pair<std::string, cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[79, "PAIR_VL_STRING_UUID", "std::pair<std::string, boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[80, "MAP_INT_BOOL", "std::map<int, bool>", 1, "HDF5", "v1.3", 0],' +
    '[81, "VL_MAP_INT_BOOL", "std::map<int, bool>", 1, "HDF5", "v1.3", 0],' +
    '[82, "MAP_INT_INT", "std::map<int, int>", 1, "HDF5", "v1.3", 1],' +
    '[83, "VL_MAP_INT_INT", "std::map<int, int>", 1, "HDF5", "v1.3", 1],' +
    '[84, "MAP_INT_FLOAT", "std::map<int, float>", 1, "HDF5", "v1.3", 0],' +
    '[85, "VL_MAP_INT_FLOAT", "std::map<int, float>", 1, "HDF5", "v1.3", 0],' +
    '[86, "MAP_INT_DOUBLE", "std::map<int, double>", 1, "HDF5", "v1.3", 1],' +
    '[87, "VL_MAP_INT_DOUBLE", "std::map<int, double>", 1, "HDF5", "v1.3", 1],' +
    '[88, "MAP_INT_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.3", 1],' +
    '[89, "VL_MAP_INT_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.3", 1],' +
    '[90, "MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.3", 1],' +
    '[91, "VL_MAP_INT_VL_STRING", "std::map<int, std::string>", 2, "HDF5", "v1.3", 1],' +
    '[92, "MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[93, "VL_MAP_INT_BLOB", "std::map<int, cyclus::Blob>", 1, "HDF5", "v1.3", 0],' +
    '[94, "MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[95, "VL_MAP_INT_UUID", "std::map<int, boost::uuids::uuid>", 1, "HDF5", "v1.3", 0],' +
    '[96, "MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.3", 0],' +
    '[97, "VL_MAP_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.3", 0],' +
    '[98, "MAP_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.3", 1],' +
    '[99, "VL_MAP_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.3", 1],' +
    '[100, "MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.3", 0],' +
    '[101, "VL_MAP_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.3", 0],' +
    '[102, "MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.3", 1],' +
    '[103, "VL_MAP_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.3", 1],' +
    '[104, "MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.3", 1],' +
    '[105, "VL_MAP_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.3", 1],' +
    '[106, "MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.3", 1],' +
    '[107, "VL_MAP_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.3", 1],' +
    '[108, "MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.3", 0],' +
    '[109, "VL_MAP_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.3", 0],' +
    '[110, "MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.3", 0],' +
    '[111, "VL_MAP_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.3", 0],' +
    '[112, "MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.3", 0],' +
    '[113, "VL_MAP_VL_STRING_BOOL", "std::map<std::string, bool>", 2, "HDF5", "v1.3", 0],' +
    '[114, "MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.3", 1],' +
    '[115, "VL_MAP_VL_STRING_INT", "std::map<std::string, int>", 2, "HDF5", "v1.3", 1],' +
    '[116, "MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.3", 0],' +
    '[117, "VL_MAP_VL_STRING_FLOAT", "std::map<std::string, float>", 2, "HDF5", "v1.3", 0],' +
    '[118, "MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.3", 1],' +
    '[119, "VL_MAP_VL_STRING_DOUBLE", "std::map<std::string, double>", 2, "HDF5", "v1.3", 1],' +
    '[120, "MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.3", 1],' +
    '[121, "VL_MAP_VL_STRING_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.3", 1],' +
    '[122, "MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.3", 1],' +
    '[123, "VL_MAP_VL_STRING_VL_STRING", "std::map<std::string, std::string>", 3, "HDF5", "v1.3", 1],' +
    '[124, "MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.3", 0],' +
    '[125, "VL_MAP_VL_STRING_BLOB", "std::map<std::string, cyclus::Blob>", 2, "HDF5", "v1.3", 0],' +
    '[126, "MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.3", 0],' +
    '[127, "VL_MAP_VL_STRING_UUID", "std::map<std::string, boost::uuids::uuid>", 2, "HDF5", "v1.3", 0],' +
    '[128, "MAP_STRING_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "SQLite", "v1.3", 1],' +
    '[129, "MAP_STRING_VL_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "SQLite", "v1.3", 1],' +
    '[130, "VL_MAP_STRING_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "SQLite", "v1.3", 1],' +
    '[131, "MAP_VL_STRING_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "SQLite", "v1.3", 1],' +
    '[132, "MAP_VL_STRING_VL_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "SQLite", "v1.3", 1],' +
    '[133, "VL_MAP_STRING_VL_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "SQLite", "v1.3", 1],' +
    '[134, "VL_MAP_VL_STRING_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "SQLite", "v1.3", 1],' +
    '[135, "VL_MAP_VL_STRING_VL_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "SQLite", "v1.3", 1],' +
    '[136, "MAP_STRING_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "SQLite", "v1.3", 1],' +
    '[137, "MAP_STRING_VL_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "SQLite", "v1.3", 1],' +
    '[138, "VL_MAP_STRING_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "SQLite", "v1.3", 1],' +
    '[139, "MAP_VL_STRING_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "SQLite", "v1.3", 1],' +
    '[140, "MAP_VL_STRING_VL_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "SQLite", "v1.3", 1],' +
    '[141, "VL_MAP_STRING_VL_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "SQLite", "v1.3", 1],' +
    '[142, "VL_MAP_VL_STRING_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "SQLite", "v1.3", 1],' +
    '[143, "VL_MAP_VL_STRING_VL_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "SQLite", "v1.3", 1],' +
    '[128, "MAP_STRING_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "HDF5", "v1.3", 0],' +
    '[129, "MAP_STRING_VL_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "HDF5", "v1.3", 0],' +
    '[130, "VL_MAP_STRING_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "HDF5", "v1.3", 0],' +
    '[131, "MAP_VL_STRING_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "HDF5", "v1.3", 0],' +
    '[132, "MAP_VL_STRING_VL_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "HDF5", "v1.3", 0],' +
    '[133, "VL_MAP_STRING_VL_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "HDF5", "v1.3", 0],' +
    '[134, "VL_MAP_VL_STRING_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "HDF5", "v1.3", 0],' +
    '[135, "VL_MAP_VL_STRING_VL_VECTOR_DOUBLE", "std::map<std::string, std::vector<double> >", 3, "HDF5", "v1.3", 0],' +
    '[136, "MAP_STRING_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "HDF5", "v1.3", 0],' +
    '[137, "MAP_STRING_VL_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "HDF5", "v1.3", 0],' +
    '[138, "VL_MAP_STRING_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "HDF5", "v1.3", 0],' +
    '[139, "MAP_VL_STRING_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "HDF5", "v1.3", 0],' +
    '[140, "MAP_VL_STRING_VL_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "HDF5", "v1.3", 0],' +
    '[141, "VL_MAP_STRING_VL_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "HDF5", "v1.3", 0],' +
    '[142, "VL_MAP_VL_STRING_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "HDF5", "v1.3", 0],' +
    '[143, "VL_MAP_VL_STRING_VL_MAP_INT_DOUBLE", "std::map<std::string, std::map<int, double> >", 3, "HDF5", "v1.3", 0],' +
    '[0, "BOOL", "bool", 0, "HDF5", "v1.3", 1]]';
dbdata = dbdata.replace(new RegExp('<', 'g'), '&lt;')
               .replace(new RegExp('>', 'g'), '&gt;');
