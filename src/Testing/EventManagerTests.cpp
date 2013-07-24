#include <gtest/gtest.h>

#include "EventManager.h"
#include "EventBackend.h"

class TestBack : public EventBackend {
  public:
    TestBack() {
      flush_count = 0;
      notify_count = 0;
      closed = false;
    };

    virtual void notify(EventList evs) {
      flush_count = evs.size();
      events = evs;
      notify_count++;
    };

    virtual std::string name() {
      return "TestBack";
    };

    virtual void close() {
      closed = true;
    };

    int flush_count; // # events in last notify
    int notify_count; // # times notify called
    bool closed;
    EventList events; // last receive list
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_NewEvent) {
  EventManager m;
  event_ptr ev = m.newEvent("DumbTitle");
  EXPECT_EQ(ev->title(), "DumbTitle");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_CreateDefault) {
  EventManager m;
  EXPECT_EQ(m.dump_count(), kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_GetSetDumpFreq) {
  EventManager m;
  m.set_dump_count(1);
  EXPECT_EQ(m.dump_count(), 1);

  m.set_dump_count(-1);
  EXPECT_EQ(m.dump_count(), -1);

  m.set_dump_count(kDefaultDumpCount);
  EXPECT_EQ(m.dump_count(), kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_Closing) {
  EventManager m;
  TestBack back1;
  TestBack back2;
  m.registerBackend(&back1);
  m.registerBackend(&back2);

  ASSERT_FALSE(back1.closed);
  ASSERT_FALSE(back2.closed);

  m.close();

  EXPECT_TRUE(back1.closed);
  EXPECT_TRUE(back2.closed);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_Buffering) {
  TestBack back1;

  EventManager m;
  m.set_dump_count(2);
  m.registerBackend(&back1);

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("monkey"))
   ->record();

  EXPECT_EQ(back1.flush_count, 0);
  EXPECT_EQ(back1.notify_count, 0);

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("elephant"))
   ->record();

  EXPECT_EQ(back1.flush_count, 2);
  EXPECT_EQ(back1.notify_count, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Manager_CloseFlushing) {
  TestBack back1;

  EventManager m;
  m.set_dump_count(2);
  m.registerBackend(&back1);

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("monkey"))
   ->record();

  EXPECT_EQ(back1.flush_count, 0);
  EXPECT_EQ(back1.notify_count, 0);

  m.close();

  EXPECT_EQ(back1.flush_count, 1);
  EXPECT_EQ(back1.notify_count, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Event_record) {
  TestBack back;
  EventManager m;
  m.set_dump_count(1);
  m.registerBackend(&back);

  event_ptr ev = m.newEvent("DumbTitle");
  ev->addVal("animal", std::string("monkey"));

  EXPECT_EQ(back.flush_count, 0);

  ev->record();

  EXPECT_EQ(back.flush_count, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(EventManagerTest, Event_addVal) {
  TestBack back;
  EventManager m;
  m.registerBackend(&back);

  event_ptr ev = m.newEvent("DumbTitle");
  ev->addVal("animal", std::string("monkey"));
  ev->addVal("weight", 10);
  ev->addVal("height", 5.5);
  ev->record();

  ASSERT_EQ(ev->vals().size(), 3);

  Event::Vals::const_iterator it = ev->vals().begin();
  EXPECT_EQ(it->first, "animal");
  EXPECT_EQ(boost::spirit::any_cast<std::string>(it->second), "monkey");
  ++it;
  EXPECT_EQ(it->first, "weight");
  EXPECT_EQ(boost::spirit::any_cast<int>(it->second), 10);
  ++it;
  EXPECT_EQ(it->first, "height");
  EXPECT_DOUBLE_EQ(boost::spirit::any_cast<double>(it->second), 5.5);

  m.close();

  Event::Vals vals = back.events.back()->vals();
  ASSERT_EQ(vals.size(), 3);
  EXPECT_EQ(vals.front().first, "animal");
  EXPECT_EQ(boost::spirit::any_cast<std::string>(vals.front().second), "monkey");
}

