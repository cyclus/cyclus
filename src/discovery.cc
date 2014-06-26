#include "discovery.h"

namespace cyclus {

std::set<std::string> DiscoverArchetypes(const std::string s) {
  // Note that 9 is the length of the word "Construct"
  using std::string;
  std::set<string> archs;
  size_t offset = 0;
  size_t end_offset = 0;
  const string words = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
                                 "abcdefghijklmnopqrstuvwxyz";
  string construct = "Construct";
  if (SUFFIX == ".dylib")
    construct = "_Construct";
  size_t lenconstruct = construct.length();
  while ((offset = s.find(construct, offset)) != string::npos) {
    end_offset = s.find_first_not_of(words, offset+lenconstruct);
    if (words.find(s[offset-1]) != string::npos || offset+lenconstruct == end_offset) {
      // make sure construct starts the word
      offset += lenconstruct;
      continue;
    }
    archs.insert(string(s, offset+lenconstruct, end_offset - offset - lenconstruct));
    offset = end_offset;
  }
  return archs;
}

std::set<std::string> DiscoverSpecs(std::string p, std::string lib) {
  using std::string;
  using std::set;
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
  set<string> archs = DiscoverArchetypes(s);
  set<string> specs;
  string spec;
  AgentSpec agentspec;
  for (set<string>::iterator it = archs.begin(); it != archs.end(); ++it) {
    spec = p + ":" + lib + ":" + (*it);
    agentspec = AgentSpec(spec);
    if (DynamicModule::Exists(agentspec))
      specs.insert(spec);
  }
  return specs;
};

}  // namespace cyclus
