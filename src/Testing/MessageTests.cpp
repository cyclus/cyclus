#include <gtest/gtest.h>

#include "Communicator.h"
#include "Message.h"
#include "GenException.h"

#include <string>
#include <vector>

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
      flip_down_to_up_ = false;
      forget_set_dest_ = false;
      down_up_count_ = 0;
    }

    ~TestCommunicator() {
      delete msg_;
    }

    Communicator* parent_;
    TrackerMessage* msg_;

    string name_;
    bool stop_at_return_, flip_at_receive_, forget_set_dest_;
    bool flip_down_to_up_;
    int down_up_count_;

    void startMessage() {
      msg_->dest_list_.push_back(name_);
      msg_->setNextDest(parent_);
      msg_->sendOn();
    }

  private:

    void receiveMessage(Message* msg) {
      (dynamic_cast<TrackerMessage*>(msg))->dest_list_.push_back(name_);
      if (stop_at_return_ && this == msg->getSender()) {
        return;
      } else if (flip_at_receive_) {
        msg->setDir(DOWN_MSG);
      } else if (flip_down_to_up_) {
        int max_num_flips = 2;
        if (msg->getDir() == DOWN_MSG && down_up_count_ < max_num_flips) {
          msg->setDir(UP_MSG);
          down_up_count_++;
        }
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

      comm1->parent_ = comm2;
      comm2->parent_ = comm3;
      comm3->parent_ = comm4;
      comm4->flip_at_receive_ = true;
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
  ASSERT_NO_THROW(comm1->startMessage());

  vector<string> stops = comm1->msg_->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 7;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
  EXPECT_EQ(stops[3], "comm4");
  EXPECT_EQ(stops[4], "comm3");
  EXPECT_EQ(stops[5], "comm2");
  EXPECT_EQ(stops[6], "comm1");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessageTest, PassBeyondOrigin) {
  comm1->stop_at_return_ = false;

  ASSERT_THROW(comm1->startMessage(), GenException);

  vector<string> stops = comm1->msg_->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 7;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
  EXPECT_EQ(stops[3], "comm4");
  EXPECT_EQ(stops[4], "comm3");
  EXPECT_EQ(stops[5], "comm2");
  EXPECT_EQ(stops[6], "comm1");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessageTest, ForgetToSetDest) {
  comm3->forget_set_dest_ = true;

  ASSERT_THROW(comm1->startMessage(), GenException);

  vector<string> stops = comm1->msg_->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 3;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessageTest, SendToSelf) {
  comm3->parent_ = comm3;

  ASSERT_THROW(comm1->startMessage(), GenException);

  vector<string> stops = comm1->msg_->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 3;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessageTest, YoYo) {
  comm2->flip_down_to_up_ = true;

  ASSERT_NO_THROW(comm1->startMessage());

  vector<string> stops = comm1->msg_->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 15;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
  EXPECT_EQ(stops[3], "comm4");
  EXPECT_EQ(stops[4], "comm3");
  EXPECT_EQ(stops[5], "comm2");
  EXPECT_EQ(stops[6], "comm3");
  EXPECT_EQ(stops[7], "comm4");
  EXPECT_EQ(stops[8], "comm3");
  EXPECT_EQ(stops[9], "comm2");
  EXPECT_EQ(stops[10], "comm3");
  EXPECT_EQ(stops[11], "comm4");
  EXPECT_EQ(stops[12], "comm3");
  EXPECT_EQ(stops[13], "comm2");
  EXPECT_EQ(stops[14], "comm1");
}

