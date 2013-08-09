
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, mat_diff_same) {
  using cyclus::KG;
  using cyclus::Iso;
  using cyclus::Material;
  Material::Ptr same_as_orig = Material::Ptr(new Material(test_comp_));
  same_as_orig->SetQuantity(test_size_);
  std::map<Iso, double> remainder;
  EXPECT_NO_THROW(remainder = test_mat_->diff(same_as_orig));
  std::map<Iso, double>::iterator it;
  for(it=remainder.begin(); it!=remainder.end(); ++it){
    EXPECTLOAT_EQ(0, (*it).second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, mat_diff) {
  using cyclus::KG;
  using cyclus::Iso;
  using cyclus::Material;
  Material::Ptr two_orig = Material::Ptr(new Material(test_comp_));
  two_orig->SetQuantity(2*test_size_);
  std::map<Iso, double> remainder;
  EXPECT_NO_THROW(remainder = two_test_mat_->diff(test_mat_));
  double expected;
  std::map<Iso, double>::iterator it;
  for(it=remainder.begin(); it!=remainder.end(); ++it){
    expected = test_size_*(*two_orig->isoVector().comp())[(*it).first];
    EXPECTLOAT_EQ( expected, (*it).second);
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, diff_same) {
  using cyclus::KG;
  using cyclus::Iso;
  std::map<Iso, double> remainder;
  EXPECT_NO_THROW(remainder = test_mat_->diff(test_comp_, test_size_, KG));
  std::map<Iso, double>::iterator it;
  for(it=remainder.begin(); it!=remainder.end(); ++it){
    EXPECTLOAT_EQ(0, (*it).second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, diff_half) {
  using cyclus::KG;
  using cyclus::Iso;
  std::map<Iso, double> remainder;
  EXPECT_NO_THROW(remainder = test_mat_->diff(test_comp_, 0.5*test_size_, KG));
  double expected;
  std::map<Iso, double>::iterator it;
  for(it=remainder.begin(); it!=remainder.end(); ++it){
    expected = 0.5*test_size_*((*test_comp_)[(*it).first]);
    EXPECTLOAT_EQ( expected, (*it).second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, diff_close_size) {
  using cyclus::Material;
  using cyclus::CompMapPtr;
  using cyclus::CompMap;
  using cyclus::KG;
  using cyclus::Iso;
  std::map<Iso, double> remainder;
  EXPECT_NO_THROW(remainder = test_mat_->diff(test_comp_, test_size_-cyclus::eps_rsrc(), KG));
  double expected;
  std::map<Iso, double>::iterator it;
  for(it=remainder.begin(); it!=remainder.end(); ++it){
    expected = cyclus::eps_rsrc()*((*test_comp_)[(*it).first]);
    EXPECTLOAT_EQ( expected, (*it).second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, diff_close_comp) {
  using cyclus::Material;
  using cyclus::CompMapPtr;
  using cyclus::CompMap;
  using cyclus::KG;
  using cyclus::Iso;
  std::map<Iso, double> remainder;
  CompMapPtr close_comp = CompMapPtr(new CompMap(*diff_comp_));
  (*close_comp)[am241_]*=(1-cyclus::eps_rsrc()/test_size_);
  diff_mat_->SetQuantity(test_size_, KG);
  EXPECT_NO_THROW(remainder = diff_mat_->diff(close_comp, test_size_, KG));
  double expected;
  std::map<Iso, double>::iterator it;
  for(it=remainder.begin(); it!=remainder.end(); ++it){
    if((*it).first == am241_) {
      expected= (*diff_comp_)[am241_]*test_size_ - (*close_comp)[am241_]*test_size_;
    } else {
      expected =0;
    }
    EXPECTLOAT_EQ( expected, (*it).second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, Apply_threshold_zero){
  std::map<Iso, double> test_vec_;
  test_vec_[1]=1.0;
  test_vec_[2]=2.0;
  test_vec_[3]=3.0;

  // if the threshold is 0, applying the threshold should do nothing
  using cyclus::Iso;
  std::map<Iso, double> result_vec;
  EXPECT_NO_THROW( result_vec = test_mat_->ApplyThreshold(test_vec_, 0));
  std::map<Iso, double>::iterator it;
  for(it=result_vec.begin(); it != result_vec.end(); ++it){
    EXPECTLOAT_EQ(test_vec_[(*it).first], (*it).second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, Apply_threshold_inf){
  using cyclus::Iso;
  std::map<Iso, double> test_vec_;
  test_vec_[1]=1.0;
  test_vec_[2]=2.0;
  test_vec_[3]=3.0;

  // if the threshold is infinit, applying it should zero any vector
  std::map<Iso, double> result_vec;
  double infty = std::numeric_limits<double>::infinity();
  EXPECT_NO_THROW( result_vec = test_mat_->ApplyThreshold(test_vec_, infty));
  std::map<Iso, double>::iterator it;
  for(it=result_vec.begin(); it != result_vec.end(); ++it){
    EXPECTLOAT_EQ(0, (*it).second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, Apply_threshold_negative){
  using cyclus::Iso;
  std::map<Iso, double> test_vec_;
  test_vec_[1]=1.0;
  test_vec_[2]=2.0;
  test_vec_[3]=3.0;

  // if the threshold is negative, the function should throw
  std::map<Iso, double> result_vec;
  double infty = std::numeric_limits<double>::infinity();
  EXPECT_THROW( result_vec = test_mat_->ApplyThreshold(test_vec_, -1), cyclus::ValueError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
TEST(MaterialTest, Apply_threshold_medium){
  using cyclus::Iso;
  std::map<Iso, double> test_vec_;
  test_vec_[1]=1.0;
  test_vec_[2]=2.0;
  test_vec_[3]=3.0;

  // if the threshold is in a reasonable range, it should zero small vals
  std::map<Iso, double> result_vec;
  double infty = std::numeric_limits<double>::infinity();
  std::map<Iso, double>::iterator it;
  for(it=test_vec_.begin(); it != test_vec_.end(); ++it){
    EXPECT_NO_THROW( result_vec = test_mat_->ApplyThreshold(test_vec_, (*it).second));
    EXPECTLOAT_EQ(0, result_vec[(*it).first]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class CompMapTests : public ::testing::Test {
 protected:
  TestCompMapPtr comp_;
  cyclus::Map map_, atomified_, massified_;
  cyclus::Basis basis_;
  double ratio_;
  int n_species_;
  std::vector<int> isotopes_, masses_;

  boost::shared_ptr<TestCompMap> child, parent;
  cyclus::CompMapPtr root;
  int t1, t2, root_decay_time;

 public:
  virtual void SetUp() { 
    basis_ = cyclus::MASS;
    map_ = cyclus::Map();
    comp_ = TestCompMapPtr(new TestCompMap(basis_));
    isotopes_.push_back(1001),isotopes_.push_back(2004);
    masses_.push_back(10),masses_.push_back(20);
  }

  virtual void TearDown() {}

  void LoadMap() {
    n_species_ = isotopes_.size();
    double mass_sum, atom_sum;
    for (int i = 0; i < n_species_; i++) {
      int iso = isotopes_.at(i);
      double mass_value = (double)masses_.at(i);
      double atom_value = mass_value / cyclus::MT->GramsPerMol(iso);
      map_[iso] = mass_value;
      massified_[iso] = mass_value;
      atomified_[iso] = atom_value;
      mass_sum += mass_value;
      atom_sum += atom_value;
    }
    ratio_ = mass_sum/atom_sum;
    for (cyclus::Map::iterator it = atomified_.begin(); it != atomified_.end(); it++) {
      massified_[it->first] /= mass_sum;
      atomified_[it->first] /= atom_sum;
    }
  }

  void LoadLineage() {
    t1 = 3;
    t2 = 1001;
    // root
    root_decay_time = t1+t2;
    root = cyclus::CompMapPtr(new cyclus::CompMap(basis_));
    // parent/
    parent = TestCompMapPtr(new TestCompMap(basis_));
    parent->SetParent(root);
    parent->SetDecayTime(t1);
    // child
    child = TestCompMapPtr(new TestCompMap(basis_));
    child->SetParent(parent->me());
    child->SetDecayTime(t2);
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(CompMapTest, almostEquality) {
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  comp_->normalize();
  CompMap copy = CompMap(*comp_);
  CompMap::Iterator it;
  for(it=copy.begin(); it!=copy.end(); ++it){
    (*it).second *= 1.1;
  }
  EXPECTALSE(copy == *comp_);
  EXPECT_TRUE(copy.AlmostEqual(*comp_, 1.1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(CompMapTest, almostEqualZeroEntry) {
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  comp_->normalize();
  CompMap copy = CompMap(*comp_);
  CompMap::Iterator it;
  double the_max = 0.0;
  for(it=copy.begin(); it!=copy.end(); ++it){
    (*it).second = 0;
  }
  EXPECTALSE(copy == *comp_);
  EXPECT_TRUE(copy.AlmostEqual(*comp_, 1.1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(CompMapTest, almostEqualNegThresh) {
  using cyclus::CompMap;
  LoadMap();
  comp_->SetMap(map_);
  comp_->normalize();
  CompMap copy = CompMap(*comp_);
  EXPECT_THROW(copy.AlmostEqual(*comp_, -1.0), cyclus::ValueError);
}

class IsoVectorTests : public ::testing::Test {
protected:
  cyclus::CompMapPtr comp;
  cyclus::IsoVector vec;
  cyclus::IsoVector zero_vec;

  std::vector<int> isotopes;
  double ratio;
  cyclus::CompMapPtr add_result, mix_result;
  cyclus::IsoVector add_to_vec, to_add_vec;
  double efficiency;
  cyclus::CompMapPtr subtract_result, separate_result;
  cyclus::IsoVector subtract_from_vec, to_subtract_vec;
  

  // this sets up the fixtures
  virtual void SetUp() {
    comp = cyclus::CompMapPtr(new cyclus::CompMap(cyclus::MASS));
    vec = cyclus::IsoVector(comp);
  };
  
  // this tears down the fixtures
  virtual void TearDown() {
  }

  void LoadMaps() {
    using cyclus::IsoVector;
    using cyclus::CompMap;
    using cyclus::CompMapPtr;
    using cyclus::MASS;

    isotopes.push_back(1001),isotopes.push_back(2004);
    
    // add, mix terms
    ratio = 2;
    CompMapPtr to_add = CompMapPtr(new CompMap(MASS));
    (*to_add)[isotopes.at(isotopes.size()-1)] = 1;
    to_add_vec = IsoVector(to_add);
    CompMapPtr add_to = CompMapPtr(new CompMap(MASS));
    (*add_to)[isotopes.at(0)] = 2;
    add_to_vec = IsoVector(add_to);
    // add, mix results
    add_result = CompMapPtr(new CompMap(MASS));
    (*add_result)[isotopes.at(0)] = 1;
    (*add_result)[isotopes.at(isotopes.size()-1)] = 1;
    add_result->normalize();
    mix_result = CompMapPtr(new CompMap(MASS));
    (*mix_result)[isotopes.at(0)] = 1;
    (*mix_result)[isotopes.at(isotopes.size()-1)] = 1*ratio;
    mix_result->normalize();    

    // subtract, separate terms
    efficiency = 0.9;
    subtract_from_vec = IsoVector(add_result);
    to_subtract_vec = IsoVector(to_add);
    // subtract, separate results
    subtract_result = add_to;
    separate_result = CompMapPtr(new CompMap(MASS));
    (*separate_result)[isotopes.at(0)] = 1;
    (*separate_result)[isotopes.at(isotopes.size()-1)] = 1-efficiency;
    separate_result->normalize();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,addition) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  EXPECT_NO_THROW(add_to_vec += to_add_vec);
  EXPECT_TRUE(add_to_vec.CompEquals(add_result));
  for (CompMap::Iterator it = add_to_vec.comp()->begin(); it != add_to_vec.comp()->end(); it++) {
    EXPECT_DOUBLE_EQ((*add_result)[it->first],it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,simple_mixing) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  IsoVector v1 = IsoVector(subtract_result);
  IsoVector v2 = IsoVector(subtract_result);
  EXPECT_EQ(v1,v2);
  v1.Mix(v2,1);
  EXPECT_TRUE(v1.CompEquals(subtract_result));
  EXPECT_EQ(v1,v2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,mixing) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  EXPECT_NO_THROW(add_to_vec.Mix(to_add_vec,ratio));
  EXPECT_TRUE(add_to_vec.CompEquals(mix_result));
  EXPECT_THROW(add_to_vec.Mix(to_add_vec,-1), cyclus::ValueError);
  EXPECT_NO_THROW(add_to_vec.Mix(to_add_vec,0)); // corner
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,mixing_zero_vector) { 
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  EXPECT_NO_THROW(zero_vec.Mix(to_add_vec, .1)); // should just ignore ratio
  EXPECT_TRUE(zero_vec.CompEquals(to_add_vec)); // the resulting vector is the added vector
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,intersection_fraction) {
  LoadMaps();
  add_to_vec += to_add_vec;
  double amt;
  EXPECT_NO_THROW(amt = add_to_vec.IntersectionFraction(to_add_vec));
  EXPECT_DOUBLE_EQ(amt,0.5);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,subtraction) {
  using cyclus::IsoVector;
  using cyclus::CompMap;
  using cyclus::CompMapPtr;
  using cyclus::MASS;
  LoadMaps();
  EXPECT_NO_THROW(subtract_from_vec -= to_subtract_vec);
  EXPECT_TRUE(subtract_from_vec.CompEquals(subtract_result));
  for (CompMap::Iterator it = subtract_from_vec.comp()->begin(); it != subtract_from_vec.comp()->end(); it++) {
    EXPECT_DOUBLE_EQ((*subtract_result)[it->first],it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(IsoVectorTests,separation) {
  LoadMaps();
  EXPECT_NO_THROW(subtract_from_vec.Separate(to_subtract_vec,efficiency));
  EXPECT_TRUE(subtract_from_vec.CompEquals(separate_result));
  EXPECT_THROW(subtract_from_vec.Separate(to_subtract_vec,-1),cyclus::ValueError);
  EXPECT_THROW(subtract_from_vec.Separate(to_subtract_vec,2),cyclus::ValueError);
  EXPECT_NO_THROW(subtract_from_vec.Separate(to_subtract_vec,0)); // corner
}

