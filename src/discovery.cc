#include "discovery.h"

#include <fstream>
#include <iostream>
#include <streambuf>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "agent.h"
#include "context.h"
#include "dynamic_module.h"
#include "env.h"
#include "recorder.h"
#include "suffix.h"
#include "timer.h"

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
  std::cerr << "libpath: " << libpath << "\n";
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
    std::cerr << "spec name: " << spec << "\n";
    agentspec = AgentSpec(spec);
    if (DynamicModule::Exists(agentspec))
      std::cerr << "found spec: " << spec << "\n";
      specs.insert(spec);
  }
  return specs;
}

std::set<std::string> DiscoverSpecsInDir(std::string d) {
  using std::string;
  using std::set;
  namespace fs = boost::filesystem;
  //std::cout << "Discovering dir pre " << d << "\n";
  set<string> specs;
  set<string> libspecs;
  fs::path pth;
  boost::system::error_code errc;
  boost::system::error_code no_err;
  //std::cout << "setting pth \n";
  pth = d;
  //std::cout << "pth is : " << pth.string() << "\n";
  //std::cout << "Discovering dir " << d << "\n";
  //fs::recursive_directory_iterator it(d, errc);
  fs::recursive_directory_iterator it(pth, errc);
  //std::cout << "got recur iterator\n";
  fs::recursive_directory_iterator last;
  //std::cout << "got recur iterator last\n";
  //std::cout << "initial path: " << it->path().string() << "\n";
  //if (errc != no_err || !fs::is_directory(it->path(), errc)) {
    //std::cout << "initial error, returning\n";
  //  return specs;
  //}
  //std::cout << "no initial error\n";
  for (; it != last; it.increment(errc)) {
    //std::cout << "path: " << it->path().string() << "\n";
    //std::cout << "errc: " << errc << "\n";
    if (errc != no_err) {
      std::cout << "level " << it.level() << "\n";
      //it.no_push();
      std::cout << "no pushed\n";
      if (it.level() > 0) {
        it.pop();
        //std::cout << "popped\n";
      }
      //std::cout << "continuing\n";
      continue;
    }
    pth = it->path();
    string pthstr = pth.string();
    //std::cout << "new pth: " << pth << "\n";
    //std::cout << "new pthstr: " << pthstr << "\n";
    bool irf = fs::is_regular_file(pth, errc);
    //std::cout << "new pth regfile: " << irf << "\n";
    //std::cout << "new pth regfile 0 errc: " << (errc == no_err) << "\n";
    //std::cout << "  errc: " << errc << "\n";
    if (errc != no_err || !irf) {
      //std::cout << "not regfile, continuing\n";
      it.no_push();
      //std::cout << "no pushed\n";
      if (it.level() > 0) {
        it.pop();
      //  std::cout << "popped\n";
      }
      continue;
    } else if (fs::is_directory(pth, errc)) {
      //std::cout << "is dir, continuing\n";
      //it.pop();
      continue;
    } else if (!boost::algorithm::ends_with(pthstr, SUFFIX)) {
      //std::cout << "does not end in suffix, continuing\n";
      //if (it.level() > 0) {
      //  it.pop();
      //  std::cout << "popped\n";
      //}
      continue;
    }
    string p = pth.parent_path().string();
    //std::cout << "parent path: " << p << "\n";
    string lib = pth.filename().string();
    //std::cout << "path lib: " << lib << "\n";
    if (d.length() < p.length())
      p = p.substr(d.length()+1, string::npos);
    else
      p = "";
    //std::cout << "parent path revised: " << p << "\n";
    lib = lib.substr(3, lib.rfind(".") - 3);  // remove 'lib' prefix and suffix
    //std::cout << "path lib revised: " << lib << "\n";
    try {
      //std::cout << "discovering specs\n";
      libspecs = DiscoverSpecs(p, lib);
      //std::cout << "discovered specs\n";
    } catch (cyclus::IOError& e) {
      //std::cout << "failed discovering specs\n";
    }
    for (set<string>::iterator ls = libspecs.begin(); ls != libspecs.end(); ++ls) {
      //std::cout << "  adding spec" << (*ls) << "\n";
      specs.insert(*ls);
      //std::cout << "  added spec" << (*ls) << "\n";
    }
    //std::cout << " found all in dir\n";
  }
  //std::cout << "returning specs\n";
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
    //std::cout << "dir on path: " << (*it) << "\n";
    //std::cout << "dir on length: " << (*it).length() << "\n";
    //std::cout << "dir on result: " << ((*it).length() == 0 ? "." : (*it)) << "\n";
    dirspecs = DiscoverSpecsInDir((*it).length() == 0 ? "." : (*it));
    //std::cout << "found specs on " << (*it) << ":\n";
    for (set<string>::iterator ds = dirspecs.begin(); ds != dirspecs.end(); ++ds) {
      //std::cout << "  spec in dir: " << (*ds) << "\n";
      specs.insert(*ds);
    }
  }
  return specs;
}

Json::Value DiscoverMetadataInCyclusPath() {
  std::set<std::string> specs = cyclus::DiscoverSpecsInCyclusPath();
  Json::Value root(Json::objectValue);
  Json::Value spec(Json::arrayValue);
  Json::Value anno(Json::objectValue);
  Json::Value schm(Json::objectValue);
  Recorder rec;
  Timer ti;
  Context* ctx = new Context(&ti, &rec);
  std::string s;
  std::set<std::string>::iterator it;

  for (it = specs.begin(); it != specs.end(); ++it) {
    s = *it;
    Agent* m = DynamicModule::Make(ctx, s);
    spec.append(s);
    anno[s] = m->annotations();
    schm[s] = m->schema();
    ctx->DelAgent(m);
  }
  delete ctx;

  root["specs"] = spec;
  root["annotations"] = anno;
  root["schema"] = schm;

  return root;
}

}  // namespace cyclus
