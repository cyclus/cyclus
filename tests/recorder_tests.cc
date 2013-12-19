#include <gtest/gtest.h>

#include "recorder.h"
#include "rec_backend.h"

class TestBack : public cyclus::RecBackend {
  public:
    TestBack() {
      flush_count = 0;
      notify_count = 0;
      closed = false;
    };

    virtual void Notify(cyclus::DatumList data) {
      flush_count = data.size();
      this->data = data;
      notify_count++;
    };

    virtual std::string Name() {
      return "TestBack";
    };

    virtual void Close() {
      closed = true;
    };

    int flush_count; // # Datum objects in last notify
    int notify_count; // # times notify called
    bool closed;
    cyclus::DatumList data; // last receive list
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(RecorderTest, Manager_NewDatum) {
  cyclus::Recorder m;
  cyclus::Datum* d = m.NewDatum("DumbTitle");
  EXPECT_EQ(d->title(), "DumbTitle");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(RecorderTest, Manager_CreateDefault) {
  using cyclus::Recorder;
  Recorder m;
  EXPECT_EQ(m.dump_count(), cyclus::kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(RecorderTest, Manager_GetSetDumpFreq) {
  using cyclus::Recorder;
  Recorder m;
  m.set_dump_count(1);
  EXPECT_EQ(m.dump_count(), 1);

  m.set_dump_count(cyclus::kDefaultDumpCount);
  EXPECT_EQ(m.dump_count(), cyclus::kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(RecorderTest, Manager_Closing) {
  using cyclus::Recorder;
  Recorder m;
  TestBack back1;
  TestBack back2;
  m.RegisterBackend(&back1);
  m.RegisterBackend(&back2);

  ASSERT_FALSE(back1.closed);
  ASSERT_FALSE(back2.closed);

  m.close();

  EXPECT_TRUE(back1.closed);
  EXPECT_TRUE(back2.closed);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
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

  m.close();

  EXPECT_EQ(back1.flush_count, 1);
  EXPECT_EQ(back1.notify_count, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(RecorderTest, Datum_record) {
  using cyclus::Datum;
  using cyclus::Recorder;
  TestBack back;
  Recorder m;
  m.set_dump_count(1);
  m.RegisterBackend(&back);

  Datum* d = m.NewDatum("DumbTitle");
  d->AddVal("animal", std::string("monkey"));

  EXPECT_EQ(back.flush_count, 0);

  d->Record();

  EXPECT_EQ(back.flush_count, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
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
  EXPECT_STREQ(it->first, "SimID");
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

  m.close();

  cyclus::Datum::Vals vals = back.data.back()->vals();
  ASSERT_EQ(vals.size(), 4);
  EXPECT_STREQ(vals.front().first, "SimID");
  EXPECT_EQ(vals.front().second.cast<boost::uuids::uuid>(), m.sim_id());
}

