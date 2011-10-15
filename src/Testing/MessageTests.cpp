#include <gtest/gtest.h>

#include "Communicator.h"
#include "Message.h"
#include "GenException.h"

#include <string>
#include <map>

using namespace std;

class TrackerMessage : public Message {
  public:
    TrackerMessage(MessageDir dir, Communicator* originator) : 
                                       Message(dir, originator) { }

    vector<string> dest_list_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestCommunicator : public Communicator {
  public:

    TestCommunicator(string name) {
      msg_ = new TrackerMessage(UP_MSG, this);
      name_ = name;
      stop_at_return_ = true;
      flip_at_receive_ = false;
      forget_set_dest_ = false;
    }
    Communicator* parent_;
    string name_;
    bool stop_at_return_, flip_at_receive_, forget_set_dest_;
    TrackerMessage* msg_;

    void startMessage() {
      msg_->dest_list_.push_back(name_);
      msg_->setNextDest(parent_);
      msg_->sendOn();
    }

    ~TestCommunicator() {
      delete msg_;
    }

  private:

    void receiveMessage(Message* msg) {
      (dynamic_cast<TrackerMessage*>(msg))->dest_list_.push_back(name_);
      if (stop_at_return_ && this == msg->getSender()) {
        return;
      } else if (flip_at_receive_) {
        msg->setDir(DOWN_MSG);
      }

      if ( !forget_set_dest_ ) {
        msg->setNextDest(parent_);
      }

      msg->sendOn();
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MessageTest : public ::testing::Test {
  protected:
    TestCommunicator* comm1;
    TestCommunicator* comm2;
    TestCommunicator* comm3;
    TestCommunicator* comm4;


    virtual void SetUp(){
      comm1 = new TestCommunicator("comm1");
      comm2 = new TestCommunicator("comm2");
      comm3 = new TestCommunicator("comm3");
      comm4 = new TestCommunicator("comm4");
    };

    virtual void TearDown() {
      delete comm1;
      delete comm2;
      delete comm3;
      delete comm4;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessageTest, CleanThrough) {
  bool passed = true;

  comm1->parent_ = comm2;
  comm2->parent_ = comm3;
  comm3->parent_ = comm4;

  comm4->flip_at_receive_ = true;

  try {
    comm1->startMessage();
  } catch (GenException error) {
    passed = false;
  }

  vector<string> stops = comm1->msg_->dest_list_;

  EXPECT_TRUE(stops.size() == 7);

  if (passed) {
    EXPECT_TRUE(stops[0] == "comm1");
    EXPECT_TRUE(stops[1] == "comm2");
    EXPECT_TRUE(stops[2] == "comm3");
    EXPECT_TRUE(stops[3] == "comm4");
    EXPECT_TRUE(stops[4] == "comm3");
    EXPECT_TRUE(stops[5] == "comm2");
    EXPECT_TRUE(stops[6] == "comm1");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessageTest, PassBeyondOrigin) {
  bool passed = true;

  comm1->parent_ = comm2;
  comm2->parent_ = comm3;
  comm3->parent_ = comm4;

  comm1->stop_at_return_ = false;
  comm4->flip_at_receive_ = true;

  try {
    comm1->startMessage();
  } catch (GenException error) {
    passed = false;
  }

  vector<string> stops = comm1->msg_->dest_list_;

  EXPECT_FALSE(passed);
  EXPECT_TRUE(stops.size() == 7);

  if (passed) {
    EXPECT_TRUE(stops[0] == "comm1");
    EXPECT_TRUE(stops[1] == "comm2");
    EXPECT_TRUE(stops[2] == "comm3");
    EXPECT_TRUE(stops[3] == "comm4");
    EXPECT_TRUE(stops[4] == "comm3");
    EXPECT_TRUE(stops[5] == "comm2");
    EXPECT_TRUE(stops[6] == "comm1");
  }
}

