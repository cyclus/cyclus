#include "mixer.h"

#include "agent_tests.h"
#include "context.h"
#include "cyclus.h"
#include "equality_helpers.h"
#include "facility_tests.h"

#include <gtest/gtest.h>

using cyclus::QueryResult;

namespace cycamore {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef std::vector<
    std::pair<std::pair<double, double>, std::map<std::string, double> > >
    t_instream;

cyclus::Composition::Ptr c_pustream() {
  using pyne::nucname::id;

  cyclus::CompMap m;
  m[id("pu239")] = 100;
  m[id("pu240")] = 10;
  m[id("pu241")] = 1;
  m[id("pu242")] = 1;
  return cyclus::Composition::CreateFromMass(m);
};

cyclus::Composition::Ptr c_natu() {
  using pyne::nucname::id;

  cyclus::CompMap m;
  m[id("u235")] = 0.007;
  m[id("u238")] = 0.993;
  return cyclus::Composition::CreateFromMass(m);
};

cyclus::Composition::Ptr c_uox() {
  using pyne::nucname::id;

  cyclus::CompMap m;
  m[id("u235")] = 0.04;
  m[id("u238")] = 0.96;
  return cyclus::Composition::CreateFromMass(m);
};

class MixerTest : public ::testing::Test {
 public:
  typedef cyclus::toolkit::ResBuf<cyclus::Material> InvBuffer;

  cyclus::TestContext tc_;
  Mixer* mf_facility_;

  virtual void SetUp() {
    mf_facility_ = new Mixer(tc_.get());

    std::vector<std::map<std::string, double> > in_commods;
    {
      std::map<std::string, double> in_com;
      in_com.insert(std::pair<std::string, double>("in_c1", 1));
      in_commods.push_back(in_com);
    }
    {
      std::map<std::string, double> in_com;
      in_com.insert(std::pair<std::string, double>("in_c2", 1));
      in_commods.push_back(in_com);
    }
    {
      std::map<std::string, double> in_com;
      in_com.insert(std::pair<std::string, double>("in_c3", 1));
      in_commods.push_back(in_com);
    }

    std::vector<double> in_ratios = {1, 1, 1};
    std::vector<double> in_caps = {30, 20, 10};
    SetIn_stream(in_commods, in_ratios,  in_caps);

    SetOutStream_comds("out_com");
  }
  virtual void TearDown() { delete mf_facility_; }

  std::vector<std::map<std::string, double> > in_coms;
  std::vector<double> in_frac;
  std::vector<double> in_cap;

  std::string out_com;
  double out_cap;

  double throughput;

  void SetThroughput(double thpt) {
    throughput = thpt;
    mf_facility_->throughput = thpt;
  }

  void SetIn_stream(t_instream streams) {
    mf_facility_->streams_ = streams;

    in_frac.clear();
    in_cap.clear();
    for (int i = 0; i < streams.size(); i++) {
      in_frac.push_back(streams[i].first.first);
      in_cap.push_back(streams[i].first.second);
    }
  }

  void SetIn_stream(std::vector<std::map<std::string, double> > in_stream,
                    std::vector<double> ratios, std::vector<double> caps) {
    t_instream instream_tmp;
    for (int i = 0; i < in_stream.size(); i++) {
      std::pair<double, double> info_mtp =
          std::pair<double, double>(ratios[i], caps[i]);
      instream_tmp.push_back(
          std::pair<std::pair<double, double>, std::map<std::string, double> >(
              info_mtp, in_stream[i]));
    }
    SetIn_stream(instream_tmp);
  }

  void SetStream_ratio(std::vector<double> new_ratios) {
    for (int i = 0; i < new_ratios.size(); i++) {
      mf_facility_->streams_[i].first.first = new_ratios[i];
    }

    mf_facility_->mixing_ratios = new_ratios;
  }

  void SetStream_capacity(std::vector<double> new_caps) {
    for (int i = 0; i < new_caps.size(); i++) {
      mf_facility_->streams_[i].first.second = new_caps[i];
    }

    mf_facility_->in_buf_sizes = new_caps;
  }

  void SetOutStream_comds(std::string com) {
    out_com = com;
    mf_facility_->out_commod = com;
  }

  void SetOutStream_capacity(double cap) {
    out_cap = cap;
    mf_facility_->out_buf_size = cap;
  }

  void SetInputInv(std::vector<cyclus::Material::Ptr> mat) {
    for (int i = 0; i < mat.size(); i++) {
      std::string name = "in_stream_" + std::to_string(i);
      mf_facility_->streambufs[name].Push(mat[i]);
    }
  }

  std::vector<double> GetStream_ratio() { return mf_facility_->mixing_ratios; }

  std::vector<double> GetStream_capacity() {
    return mf_facility_->in_buf_sizes;
  }

  std::string GetOutStream_comds() { return mf_facility_->out_commod; }

  double GetOutStream_capacity() { return mf_facility_->out_buf_size; }

  double GetThroughput() { return mf_facility_->throughput; }

  InvBuffer* GetOutPutBuffer() { return &mf_facility_->output; }

  std::map<std::string, InvBuffer> GetStreamBuffer() {
    return mf_facility_->streambufs;
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Checking that ratios correctly default to 1/N.
TEST_F(MixerTest, StreamDefaultRatio) {
  SetOutStream_capacity(50);
  SetThroughput(1e200);
  mf_facility_->EnterNotify();

  double ext_val = 1.0 / 3.0;
  std::vector<double> strm_ratio_ = GetStream_ratio();

  //
  for (int i = 0; i < in_coms.size(); i++) {
    EXPECT_DOUBLE_EQ(ext_val, strm_ratio_[i]);
  }
}

// Test things about the mixing ratio normalisation.
TEST_F(MixerTest, StreamRatio) {
  // Checking renormalisation when sum of ratio is grester tham 1.
  std::vector<double> in_cap_ = {30, 20, 10};
  std::vector<double> in_frac_ = {2, 1, 5};
  in_cap_ = {30, 20, 10};

  SetStream_ratio(in_frac_);
  SetStream_capacity(in_cap_);
  SetOutStream_capacity(50);
  SetThroughput(1e200);
  mf_facility_->EnterNotify();

  std::vector<double> strm_ratio_ = GetStream_ratio();
  double sum = 0.0;
  for (int i = 0; i < strm_ratio_.size(); i++) {
    sum += strm_ratio_[i];
  }

  EXPECT_DOUBLE_EQ(sum, 1) << "Ratios normalized incorrectly: want 1, got "
                           << sum;

  // Checking renormalisation when sum of ratio is smaller tham 1.
  in_frac_ = {0.1, 0.2, 0.5};
  SetOutStream_capacity(50);
  SetThroughput(1e200);

  SetStream_ratio(in_frac_);
  mf_facility_->EnterNotify();
  strm_ratio_ = GetStream_ratio();

  sum = 0;
  for (int i = 0; i < strm_ratio_.size(); i++) {
    sum += strm_ratio_[i];
  }

  EXPECT_DOUBLE_EQ(sum, 1) << "Ratios normalized incorrectly: want 1, got "
                           << sum;
}

// Check the correct mixing cyclus::Composition
TEST_F(MixerTest, MixingComposition) {
  using cyclus::Material;

  std::vector<double> in_frac_ = {0.80, 0.15, 0.05};
  SetStream_ratio(in_frac_);

  SetOutStream_capacity(50);

  SetThroughput(1e200);

  std::vector<Material::Ptr> mat;
  mat.push_back(Material::CreateUntracked(in_cap[0], c_natu()));
  mat.push_back(Material::CreateUntracked(in_cap[1], c_pustream()));
  mat.push_back(Material::CreateUntracked(in_cap[2], c_uox()));

  SetInputInv(mat);
  mf_facility_->Tick();

  cyclus::CompMap v_0 = c_natu()->mass();
  cyclus::compmath::Normalize(&v_0, in_frac_[0]);
  cyclus::CompMap v_1 = c_pustream()->mass();
  cyclus::compmath::Normalize(&v_1, in_frac_[1]);
  cyclus::CompMap v_2 = c_uox()->mass();
  cyclus::compmath::Normalize(&v_2, in_frac_[2]);
  cyclus::CompMap v = v_0;
  v = cyclus::compmath::Add(v, v_1);
  v = cyclus::compmath::Add(v, v_2);

  InvBuffer* buffer = GetOutPutBuffer();
  Material::Ptr final_mat = cyclus::ResCast<Material>(buffer->PopBack());
  cyclus::CompMap final_comp = final_mat->comp()->mass();


  cyclus::compmath::Normalize(&v, 1);
  cyclus::compmath::Normalize(&final_comp, 1);

  cyclus::CompMap sum_v = cyclus::compmath::Add(v, final_comp);

  cyclus::CompMap::iterator it;
  for (it = sum_v.begin(); it != sum_v.end(); it++) {
    EXPECT_DOUBLE_EQ(final_comp[it->first], v[it->first])
        << "Unexpected difference on nuclide " << it->first << ".";
  }
}

// Check the throughput constrain
TEST_F(MixerTest, Throughput) {
  using cyclus::Material;

  std::vector<double> in_frac_ = {0.80, 0.15, 0.05};

  SetStream_ratio(in_frac_);

  SetOutStream_capacity(50);

  SetThroughput(0.5);

  std::vector<Material::Ptr> mat;
  mat.push_back(Material::CreateUntracked(in_cap[0], c_natu()));
  mat.push_back(Material::CreateUntracked(in_cap[1], c_pustream()));
  mat.push_back(Material::CreateUntracked(in_cap[2], c_uox()));
  SetInputInv(mat);

  mf_facility_->Tick();

  std::vector<double> cap;
  for (int i = 0; i < in_coms.size(); i++) {
    cap.push_back(in_cap[i] - 0.5 * in_frac[i]);
  }

  std::map<std::string, InvBuffer> streambuf = GetStreamBuffer();

  for (int i = 0; i < in_coms.size(); i++) {
    std::string name = "in_stream_" + std::to_string(i);
    double buf_size = in_cap[i];
    double buf_ratio = in_frac[i];
    double buf_inv = streambuf[name].quantity();

    // checking that each input buf was reduce of the correct amount
    // (constrained by the throughput"
    EXPECT_EQ(buf_size - 0.5 * buf_ratio, buf_inv)
        << " one (or more) input "
           "buffer inventory was not drawn from in the correct ratio.";
  }

  // output buffer size should be equal to the throuput size
  EXPECT_EQ(throughput, GetOutPutBuffer()->quantity())
      << " mixing was not "
         "correctly constrained by throughput.";
}

// multiple input streams can be correctly requested and used as
//  material inventory.
TEST(MixerTests, MultipleFissStreams) {
  std::string config =
      "<in_streams>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.8</mixing_ratio>"
            "<buf_size>2.5</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream1</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.15</mixing_ratio>"
            "<buf_size>3</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream2</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.05</mixing_ratio>"
            "<buf_size>5</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream3</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
      "</in_streams>"
      "<out_commod>mixedstream</out_commod>"
      "<outputbuf_size>0</outputbuf_size>"
      "<throughput>0</throughput>";
  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Mixer"), config, simdur);
  sim.AddSource("stream1").recipe("unatstream").capacity(1).Finalize();
  sim.AddSource("stream2").recipe("uoxstream").capacity(1).Finalize();
  sim.AddSource("stream3").recipe("pustream").capacity(1).Finalize();
  sim.AddRecipe("unatstream", c_natu());
  sim.AddRecipe("uoxstream", c_pustream());
  sim.AddRecipe("pustream", c_uox());
  int id = sim.Run();

  // Checking the number of transaction is as expected 3.
  cyclus::QueryResult qr = sim.db().Query("Transactions", NULL);
  EXPECT_EQ(3, qr.rows.size());

  // Checking that all input stream get one transaction each.
  std::vector<cyclus::Cond> conds;
  conds.push_back(cyclus::Cond("Commodity", "==", std::string("stream1")));
  qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(1, qr.rows.size());

  conds[0] = cyclus::Cond("Commodity", "==", std::string("stream2"));
  qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(1, qr.rows.size());

  conds[0] = cyclus::Cond("Commodity", "==", std::string("stream3"));
  qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(1, qr.rows.size());
}

// multiple input streams can be correctly requested and used as
//  material inventory.
TEST(MixerTests, CompleteMixingProcess) {
  std::string config =
      "<in_streams>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.8</mixing_ratio>"
            "<buf_size>2.5</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream1</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.15</mixing_ratio>"
            "<buf_size>3</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream2</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.05</mixing_ratio>"
            "<buf_size>5</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream3</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
      "</in_streams>"
      "<out_commod>mixedstream</out_commod>"
      "<outputbuf_size>10</outputbuf_size>"
      "<throughput>1</throughput>";
  int simdur = 2;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Mixer"), config, simdur);
  sim.AddSource("stream1").recipe("unatstream").capacity(1).Finalize();
  sim.AddSource("stream2").recipe("uoxstream").capacity(1).Finalize();
  sim.AddSource("stream3").recipe("pustream").capacity(1).Finalize();
  sim.AddRecipe("unatstream", c_natu());
  sim.AddRecipe("uoxstream", c_pustream());
  sim.AddRecipe("pustream", c_uox());

  sim.AddSink("mixedstream").capacity(10).Finalize();
  int id = sim.Run();

  // Checking that all input stream get one transaction each.
  std::vector<cyclus::Cond> conds;
  conds.push_back(cyclus::Cond("Commodity", "==", std::string("mixedstream")));
  cyclus::QueryResult qr = sim.db().Query("Transactions", &conds);
  EXPECT_EQ(1, qr.rows.size());

  cyclus::Material::Ptr m = sim.GetMaterial(qr.GetVal<int>("ResourceId"));
  EXPECT_DOUBLE_EQ(1., m->quantity());
}

TEST(MixerTests, PositionInitialize) {
  std::string config =
      "<in_streams>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.8</mixing_ratio>"
            "<buf_size>2.5</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream1</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.15</mixing_ratio>"
            "<buf_size>3</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream2</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
        "<stream>"
          "<info>"
            "<mixing_ratio>0.05</mixing_ratio>"
            "<buf_size>5</buf_size>"
          "</info>"
          "<commodities>"
            "<item>"
              "<commodity>stream3</commodity>"
              "<pref>1</pref>"
            "</item>"
          "</commodities>"
        "</stream>"
      "</in_streams>"
      "<out_commod>mixedstream</out_commod>"
      "<outputbuf_size>0</outputbuf_size>"
      "<throughput>0</throughput>";
  int simdur = 1;
  cyclus::MockSim sim(cyclus::AgentSpec(":cycamore:Mixer"), config, simdur);
  sim.AddSource("stream1").recipe("unatstream").capacity(1).Finalize();
  sim.AddSource("stream2").recipe("uoxstream").capacity(1).Finalize();
  sim.AddSource("stream3").recipe("pustream").capacity(1).Finalize();
  sim.AddRecipe("unatstream", c_natu());
  sim.AddRecipe("uoxstream", c_pustream());
  sim.AddRecipe("pustream", c_uox());
  int id = sim.Run();

  QueryResult qr = sim.db().Query("AgentPosition", NULL);
  EXPECT_EQ(qr.GetVal<double>("Latitude"), 0.0);
  EXPECT_EQ(qr.GetVal<double>("Longitude"), 0.0);
}

}  // namespace cycamore
