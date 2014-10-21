#ifndef CYCLUS_SRC_VERSION_H_
#define CYCLUS_SRC_VERSION_H_

#define CYCLUS_VERSION_MAJOR 1
#define CYCLUS_VERSION_MINOR 1
#define CYCLUS_VERSION_MICRO 1

namespace cyclus {
namespace version {

const char* describe();

const char* core();

const char* boost();

const char* sqlite3();

const char* hdf5();

const char* xml2();

const char* coincbc();

const char* coinclp();

}  // namespace version
}  // namespace cyclus

#endif  // CYCLUS_SRC_VERSION_H_
