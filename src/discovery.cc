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
}

std::set<std::string> DiscoverSpecsInDir(std::string d) {
  using std::string;
  using std::set;
  namespace fs = boost::filesystem;
  set<string> specs;
  set<string> libspecs;
  fs::recursive_directory_iterator it(d);
  fs::recursive_directory_iterator last;
  BOOST_FOREACH(fs::path const & pth, std::make_pair(it, last)) {
    string pthstr = pth.string();
    if (!boost::algorithm::ends_with(pthstr, SUFFIX)) {
      continue;
    } else if (is_directory(pth)) {
      continue;
    }
    string p = pth.parent_path().string();
    string lib = pth.filename().string();
    p = p.substr(d.length(), string::npos);
    lib = lib.substr(3, lib.rfind("."));  // remove 'lib' prefix and suffix
    libspecs = DiscoverSpecs(p, lib); 
    for (set<string>::iterator ls = libspecs.begin(); ls != libspecs.end(); ++ls)
      specs.insert(*ls);
  }
  return specs;
}

std::set<std::string> DiscoverSpecsInCyclusPath() {
  using std::string;
  using std::set;
  using std::vector;
  set<string> specs;
  set<string> dirspecs;
  vector<string> cycpath = Env::cyclus_path();
  for (vector<string>::iterator it = cycpath.begin(); it != cycpath.end(); ++it) {
    dirspecs = DiscoverSpecsInDir(*it);
    for (set<string>::iterator ds = dirspecs.begin(); ds != dirspecs.end(); ++ds)
      specs.insert(*ds);
  }
  return specs;
}

}  // namespace cyclus
