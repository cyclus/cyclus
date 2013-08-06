#include <gtest/gtest.h>

#include "event_manager.h"
#include "event_backend.h"

class TestBack : public cyclus::EventBackend {
  public:
    TestBack() {
      flush_count = 0;
      notify_count = 0;
      closed = false;
    };

    virtual void Notify(cyclus::EventList evs) {
      flush_count = evs.size();
      events = evs;
      notify_count++;
    };

    virtual std::string Name() {
      return "TestBack";
    };

    virtual void Close() {
      closed = true;
    };

    int flush_count; // # events in last notify
    int notify_count; // # times notify called
    bool closed;
    cyclus::EventList events; // last receive list
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_NewEvent) {
  cyclus::EventManager m;
  cyclus::Event* ev = m.NewEvent("DumbTitle");
  EXPECT_EQ(ev->title(), "DumbTitle");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_CreateDefault) {
  using cyclus::EventManager;
  EventManager m;
  EXPECT_EQ(m.dump_count(), cyclus::kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_GetSetDumpFreq) {
  using cyclus::EventManager;
  EventManager m;
  m.set_dump_count(1);
  EXPECT_EQ(m.dump_count(), 1);

  m.set_dump_count(cyclus::kDefaultDumpCount);
  EXPECT_EQ(m.dump_count(), cyclus::kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_Closing) {
  using cyclus::EventManager;
  EventManager m;
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
TEST(EventManagerTest, Manager_Buffering) {
  using cyclus::EventManager;
  TestBack back1;

  EventManager m;
  m.set_dump_count(2);
  m.RegisterBackend(&back1);

  m.NewEvent("DumbTitle")
   ->AddVal("animal", std::string("monkey"))
   ->Record();

  EXPECT_EQ(back1.flush_count, 0);
  EXPECT_EQ(back1.notify_count, 0);

  m.NewEvent("DumbTitle")
   ->AddVal("animal", std::string("elephant"))
   ->Record();

  EXPECT_EQ(back1.flush_count, 2);
  EXPECT_EQ(back1.notify_count, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_CloseFlushing) {
  using cyclus::EventManager;
  TestBack back1;

  EventManager m;
  m.set_dump_count(2);
  m.RegisterBackend(&back1);

  m.NewEvent("DumbTitle")
   ->AddVal("animal", std::string("monkey"))
   ->Record();

  EXPECT_EQ(back1.flush_count, 0);
  EXPECT_EQ(back1.notify_count, 0);

  m.close();

  EXPECT_EQ(back1.flush_count, 1);
  EXPECT_EQ(back1.notify_count, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Event_record) {
  using cyclus::Event;
  using cyclus::EventManager;
  TestBack back;
  EventManager m;
  m.set_dump_count(1);
  m.RegisterBackend(&back);

  Event* ev = m.NewEvent("DumbTitle");
  ev->AddVal("animal", std::string("monkey"));

  EXPECT_EQ(back.flush_count, 0);

  ev->Record();

  EXPECT_EQ(back.flush_count, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Event_addVal) {
  using cyclus::Event;
  using cyclus::EventManager;
  TestBack back;
  EventManager m;
  m.RegisterBackend(&back);

  cyclus::Event* ev = m.NewEvent("DumbTitle");
  ev->AddVal("animal", std::string("monkey"));
  ev->AddVal("weight", 10);
  ev->AddVal("height", 5.5);
  ev->Record();

  ASSERT_EQ(ev->vals().size(), 4);

  cyclus::Event::Vals::const_iterator it = ev->vals().begin();
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

  cyclus::Event::Vals vals = back.events.back()->vals();
  ASSERT_EQ(vals.size(), 4);
  EXPECT_STREQ(vals.front().first, "SimID");
  EXPECT_EQ(vals.front().second.cast<boost::uuids::uuid>(), m.sim_id());
}

