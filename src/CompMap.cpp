//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMap::CompMap(Basis b) {
  basis_ = b;
  init();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMap::CompMap(Basis b, Map m) {
  basis_ = b;
  init();
  map_ = m;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMap::~CompMap() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double& operator[](int tope) {
  return map_.operator[](tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool IsoVector::operator<(const IsoVector& rhs) const {
  return (ID_ < rhs.ID());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int CompMap::count(Iso tope) {
  return map_.count(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::erase(Iso tope) {
  map_.erase(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool IsoVector::logged() const {
  return (ID_ > 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int IsoVector::ID() const {
  return ID_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::massFraction(const Iso& tope) const {
  validateIsotopeNumber(tope);
  if (composition_->count(tope) == 0) {
    throw CycIndexException("This composition has no Iso: " + tope);
  }
  return (*composition_)[tope];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::atomFraction(const Iso& tope) const {
  validateIsotopeNumber(tope);
  if (composition_->count(tope) == 0) {
    throw CycIndexException("This composition has no Iso: " + tope);
  }
  return (*composition_)[tope] * MT->gramsPerMol(tope) / mass_to_atoms_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr IsoVector::parent() const {
  return parent_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::decay_time() const {
  return decay_time_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::mass_to_atoms() const {
  return mass_to_atoms_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr IsoVector::me() {
  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVectorPtr IsoVector::root_comp(IsoVectorPtr comp) {
  IsoVectorPtr child = comp;
  while (child->parent()) {
    child = child->parent();
  }
  return child;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::root_decay_time(IsoVectorPtr comp) {
  IsoVectorPtr child = comp;
  double time = comp->decay_time();
  while (child->parent()) {
    child = child->parent();
    time += child->decay_time();
  }
  return time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::massify(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    ci->second *= MT->gramsPerMol(ci->first);
    sum+= ci->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::atomify(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    validateEntry(ci->first,ci->second);
    ci->second /= MT->gramsPerMol(ci->first);
    sum += ci->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::normalize(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator it = comp.begin(); 
       it != comp.end(); it++) {
    validateEntry(it->first,it->second);
    sum += it->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::normalize(CompMap& comp, double sum) {
  if ( sum != 1) { // only normalize if needed
    for (CompMap::iterator it = comp.begin(); 
         it != comp.end(); it++) {
      it->second /= sum;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::checkCompMap() {
  normalize(*composition_);
  validateIsoVector(composition_);
  mass_to_atoms_ = calculateMassAtomRatio(*composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::calculateMassAtomRatio(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    sum+= ci->second;
  }
  return sum;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int IsoVector::getAtomicNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope / 1000; // integer division;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int IsoVector::getMassNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::validate() {
  for (Map::iterator it = map_.begin(); it != map_.end(); it ++) {
    validateEntry(it->first,it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::validateEntry(const Iso& tope, const double& value) {
  validateIsotopeNumber(tope);
  validateValue(value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::validateIsotopeNumber(const Iso& tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;  
  if (tope < lower_limit || tope > upper_limit) {
    stringstream st;
    st << tope;
    string isotope = st.str();
    throw CycRangeException("Isotope identifier '" + isotope + "' is not valid.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::validateValue(const double& value) {
  if (value < 0.0) {
    string err_msg = "IsoVector has negative quantity for an isotope.";
    throw CycRangeException(err_msg);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::print() {
  CLOG(LEV_INFO3) << detail(*this->comp());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string IsoVector::detail(const CompMap& c) {
  stringstream ss;
  vector<string> entries = compStrings(c);
  for (vector<string>::iterator entry = entries.begin(); 
       entry != entries.end(); entry++) {
    CLOG(LEV_INFO3) << *entry;
  }
  return "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> IsoVector::compStrings(const CompMap& c) {
  stringstream ss;
  vector<string> comp_strings;
  for (CompMap::const_iterator entry = c.begin(); 
       entry != c.end(); entry++) {
    ss.str("");
    ss << entry->first << ": " << entry->second << " % / kg";
    comp_strings.push_back(ss.str());
  }
  return comp_strings;
}
