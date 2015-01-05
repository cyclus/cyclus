#include <gtest/gtest.h>

#include "rec_backend.h"
#include "recorder.h"

class TestBack : public cyclus::RecBackend {
 public:
  TestBack() {
    flush_count = 0;
    notify_count = 0;
    flushed = false;
  }

  virtual void Notify(cyclus::DatumList data) {
    flush_count = data.size();
    this->data = data;
    notify_count++;
  }

  virtual std::string Name() {
    return "TestBack";
  }

  virtual void Flush() {
    flushed = true;
  }

  int flush_count;  // # Datum objects in last notify
  int notify_count;  // # times notify called
  bool flushed;
  cyclus::DatumList data;  // last receive list
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RecorderTest, Manager_NewDatum) {
  cyclus::Recorder m;
  cyclus::Datum* d = m.NewDatum("DumbTitle");
  EXPECT_EQ(d->title(), "DumbTitle");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RecorderTest, Manager_CreateDefault) {
  using cyclus::Recorder;
  Recorder m;
  EXPECT_EQ(m.dump_count(), cyclus::kDefaultDumpCount);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RecorderTest, Manager_GetSetDumpFreq) {
  using cyclus::Recorder;
  Recorder m;
  m.set_dump_count(1);
  EXPECT_EQ(m.dump_count(), 1);

  m.set_dump_count(cyclus::kDefaultDumpCount);
  EXPECT_EQ(m.dump_count(), cyclus::kDefaultDumpCount);
}

TEST(RecorderTest, InjectSimId) {
  using cyclus::Datum;
  using cyclus::Recorder;
  Recorder m;
  // with injection
  EXPECT_TRUE(m.inject_sim_id());
  Datum* d = m.NewDatum("DumbTitle");
  d->AddVal("animal", std::string("monkey"))
   ->Record();
  ASSERT_EQ(d->vals().size(), 2);
  cyclus::Datum::Vals::const_iterator it = d->vals().begin();
  EXPECT_STREQ(it->first, "SimId");
  EXPECT_EQ(it->second.cast<boost::uuids::uuid>(), m.sim_id());
  ++it;
  EXPECT_STREQ(it->first, "animal");
  EXPECT_EQ(it->second.cast<std::string>(), "monkey");

  // without injection
  m.inject_sim_id(false);
  EXPECT_FALSE(m.inject_sim_id());
  d = m.NewDatum("OtherTitle");
  d->AddVal("music", std::string("funkey"))
   ->Record();
  ASSERT_EQ(d->vals().size(), 1);
  it = d->vals().begin();
  EXPECT_STREQ(it->first, "music");
  EXPECT_EQ(it->second.cast<std::string>(), "funkey");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RecorderTest, Manager_Closing) {
  using cyclus::Recorder;
  Recorder m;
  TestBack back1;
  TestBack back2;
  m.RegisterBackend(&back1);
  m.RegisterBackend(&back2);

  ASSERT_FALSE(back1.flushed);
  ASSERT_FALSE(back2.flushed);
  m.Close();
  ASSERT_FALSE(back1.flushed);
  ASSERT_FALSE(back2.flushed);

  Recorder n;
  TestBack back3;
  TestBack back4;
  n.RegisterBackend(&back3);
  n.RegisterBackend(&back4);
  ASSERT_FALSE(back3.flushed);
  ASSERT_FALSE(back4.flushed);
  n.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("monkey"))
      ->Record();
  n.Close();
  EXPECT_TRUE(back3.flushed);
  EXPECT_TRUE(back4.flushed);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RecorderTest, Manager_Buffering) {
  using cyclus::Recorder;
  TestBack back1;

  Recorder m;
  m.set_dump_count(2);
  m.RegisterBackend(&back1);

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("monkey"))
      ->Record();

  EXPECT_EQ(back1.flush_count, 0);
  EXPECT_EQ(back1.notify_count, 0);

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("elephant"))
      ->Record();

  EXPECT_EQ(back1.flush_count, 2);
  EXPECT_EQ(back1.notify_count, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RecorderTest, Manager_CloseFlushing) {
  using cyclus::Recorder;
  TestBack back1;

  Recorder m;
  m.set_dump_count(2);
  m.RegisterBackend(&back1);

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("monkey"))
      ->Record();

  EXPECT_EQ(back1.flush_count, 0);
  EXPECT_EQ(back1.notify_count, 0);

  m.Close();

  EXPECT_EQ(back1.flush_count, 1);
  EXPECT_EQ(back1.notify_count, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RecorderTest, Datum_record) {
  using cyclus::Datum;
  using cyclus::Recorder;
  TestBack back;
  Recorder m;
  m.set_dump_count(1);
  m.RegisterBackend(&back);

  EXPECT_EQ(back.flush_count, 0);

  Datum* d = m.NewDatum("DumbTitle");
  d->AddVal("animal", std::string("monkey"))
      ->Record();

  EXPECT_EQ(back.flush_count, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RecorderTest, Datum_addVal) {
  using cyclus::Datum;
  using cyclus::Recorder;
  TestBack back;
  Recorder m;
  m.RegisterBackend(&back);

  cyclus::Datum* d = m.NewDatum("DumbTitle");
  d->AddVal("animal", std::string("monkey"));
  d->AddVal("weight", 10);
  d->AddVal("height", 5.5);
  d->Record();

  ASSERT_EQ(d->vals().size(), 4);

  cyclus::Datum::Vals::const_iterator it = d->vals().begin();
  EXPECT_STREQ(it->first, "SimId");
  EXPECT_EQ(it->second.cast<boost::uuids::uuid>(), m.sim_id());
  ++it;
  EXPECT_STREQ(it->first, "animal");
  EXPECT_EQ(it->second.cast<std::string>(), "monkey");
  ++it;
  EXPECT_STREQ(it->first, "weight");
  EXPECT_EQ(it->second.cast<int>(), 10);
  ++it;
  EXPECT_STREQ(it->first, "height");
  EXPECT_DOUBLE_EQ(it->second.cast<double>(), 5.5);

  m.Close();

  cyclus::Datum::Vals vals = back.data.back()->vals();
  EXPECT_EQ(d, back.data.back());
}


//
// Raw Recorder Test
//


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RawRecorderTest, Manager_NewDatum) {
  using cyclus::Recorder;
  Recorder m (false);
  cyclus::Datum* d = m.NewDatum("DumbTitle");
  EXPECT_EQ(d->title(), "DumbTitle");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RawRecorderTest, Manager_CreateDefault) {
  using cyclus::Recorder;
  Recorder m (false);
  EXPECT_EQ(m.dump_count(), cyclus::kDefaultDumpCount);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RawRecorderTest, Manager_GetSetDumpFreq) {
  using cyclus::Recorder;
  Recorder m (false);
  m.set_dump_count(1);
  EXPECT_EQ(m.dump_count(), 1);

  m.set_dump_count(cyclus::kDefaultDumpCount);
  EXPECT_EQ(m.dump_count(), cyclus::kDefaultDumpCount);
}

TEST(RawRecorderTest, InjectSimId) {
  using cyclus::Datum;
  using cyclus::Recorder;
  Recorder m (false);
  // Without injection
  EXPECT_FALSE(m.inject_sim_id());
  Datum* d = m.NewDatum("DumbTitle");
  d->AddVal("music", std::string("funkey"))
   ->Record();
  ASSERT_EQ(d->vals().size(), 1);
  cyclus::Datum::Vals::const_iterator it = d->vals().begin();
  EXPECT_STREQ(it->first, "music");
  EXPECT_EQ(it->second.cast<std::string>(), "funkey");

  // with injections
  m.inject_sim_id(true);
  EXPECT_TRUE(m.inject_sim_id());
  d = m.NewDatum("OtherTitle");
  d->AddVal("animal", std::string("monkey"))
   ->Record();
  ASSERT_EQ(d->vals().size(), 2);
  it = d->vals().begin();
  EXPECT_STREQ(it->first, "SimId");
  EXPECT_EQ(it->second.cast<boost::uuids::uuid>(), m.sim_id());
  ++it;
  EXPECT_STREQ(it->first, "animal");
  EXPECT_EQ(it->second.cast<std::string>(), "monkey");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RawRecorderTest, Manager_Closing) {
  using cyclus::Recorder;
  Recorder m (false);
  TestBack back1;
  TestBack back2;
  m.RegisterBackend(&back1);
  m.RegisterBackend(&back2);

  ASSERT_FALSE(back1.flushed);
  ASSERT_FALSE(back2.flushed);
  m.Close();
  ASSERT_FALSE(back1.flushed);
  ASSERT_FALSE(back2.flushed);

  Recorder n (false);
  TestBack back3;
  TestBack back4;
  n.RegisterBackend(&back3);
  n.RegisterBackend(&back4);
  ASSERT_FALSE(back3.flushed);
  ASSERT_FALSE(back4.flushed);
  n.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("monkey"))
      ->Record();
  n.Close();
  EXPECT_TRUE(back3.flushed);
  EXPECT_TRUE(back4.flushed);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RawRecorderTest, Manager_Buffering) {
  using cyclus::Recorder;
  TestBack back1;

  Recorder m (false);
  m.set_dump_count(2);
  m.RegisterBackend(&back1);

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("monkey"))
      ->Record();

  EXPECT_EQ(back1.flush_count, 0);
  EXPECT_EQ(back1.notify_count, 0);

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("elephant"))
      ->Record();

  EXPECT_EQ(back1.flush_count, 2);
  EXPECT_EQ(back1.notify_count, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RawRecorderTest, Manager_CloseFlushing) {
  using cyclus::Recorder;
  TestBack back1;

  Recorder m (false);
  m.set_dump_count(2);
  m.RegisterBackend(&back1);

  m.NewDatum("DumbTitle")
      ->AddVal("animal", std::string("monkey"))
      ->Record();

  EXPECT_EQ(back1.flush_count, 0);
  EXPECT_EQ(back1.notify_count, 0);

  m.Close();

  EXPECT_EQ(back1.flush_count, 1);
  EXPECT_EQ(back1.notify_count, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RawRecorderTest, Datum_record) {
  using cyclus::Datum;
  using cyclus::Recorder;
  TestBack back;
  Recorder m (false);
  m.set_dump_count(1);
  m.RegisterBackend(&back);

  EXPECT_EQ(back.flush_count, 0);

  Datum* d = m.NewDatum("DumbTitle");
  d->AddVal("animal", std::string("monkey"))
      ->Record();

  EXPECT_EQ(back.flush_count, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(RawRecorderTest, Datum_addVal) {
  using cyclus::Datum;
  using cyclus::Recorder;
  TestBack back;
  Recorder m (false);
  m.RegisterBackend(&back);

  cyclus::Datum* d = m.NewDatum("DumbTitle");
  d->AddVal("animal", std::string("monkey"));
  d->AddVal("weight", 10);
  d->AddVal("height", 5.5);
  d->Record();

  ASSERT_EQ(d->vals().size(), 3);

  cyclus::Datum::Vals::const_iterator it = d->vals().begin();
  EXPECT_STREQ(it->first, "animal");
  EXPECT_EQ(it->second.cast<std::string>(), "monkey");
  ++it;
  EXPECT_STREQ(it->first, "weight");
  EXPECT_EQ(it->second.cast<int>(), 10);
  ++it;
  EXPECT_STREQ(it->first, "height");
  EXPECT_DOUBLE_EQ(it->second.cast<double>(), 5.5);

  m.Close();

  cyclus::Datum::Vals vals = back.data.back()->vals();
  EXPECT_EQ(d, back.data.back());
}
