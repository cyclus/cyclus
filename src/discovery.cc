#include "discovery.h"

namespace cyclus {

std::vector<std::string> DiscoverArchetypes(const std::string s) {
  // Note that 9 is the length of the word "Construct"
  using std::string;
  std::vector<string> archs;
  size_t offset = 0;
  size_t end_offset = 0;
  const string words = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz";
  while ((offset = s.find("Construct", offset)) != string::npos) {
    end_offset = s.find_first_not_of(words, offset+9);
    if (words.find(s[offset-1]) != string::npos || offset+9 == end_offset) {
      // make sure construct starts the word
      offset += 9;
      continue;
    }
    archs.push_back(string(s, offset+9, end_offset - offset - 9));
    offset = end_offset;
  }
  return archs;
}

std::set<std::string> DiscoverSpecs(std::string p, std::string lib) {
  using std::string;
  using std::vector;
  namespace fs = boost::filesystem;
  // find file
  string libpath = (fs::path(p) / fs::path("lib" + lib + SUFFIX)).string();
  libpath = Env::FindModule(libpath);

  // read in file, pre-allocates space
  std::ifstream f (libpath.c_str());
  std::string s;
  f.seekg(0, std::ios::end);   
  s.reserve(f.tellg());
  f.seekg(0, std::ios::beg);
  s.assign((std::istreambuf_iterator<char>(f)),
            std::istreambuf_iterator<char>());

  // find specs
  vector<string> archs = DiscoverArchetypes(s);
  std::set<string> specs;
  for (unsigned int i = 0; i < archs.size(); ++i)
    specs.insert(p + ":" + lib + ":" + archs[i]);
  return specs;
};

}  // namespace cyclus
