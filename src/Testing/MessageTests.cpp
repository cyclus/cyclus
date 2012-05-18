#include <gtest/gtest.h>

#include "Communicator.h"
#include "Message.h"
#include "Resource.h"
#include "GenericResource.h"
#include "CycException.h"

#include <string>
#include <vector>

using namespace std;

class TrackerMessage : public Message {
  public:
    TrackerMessage(Communicator* originator) : Message(originator) { }

    vector<string> dest_list_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestCommunicator : public Communicator {
  public:

    TestCommunicator(string name) {
      msg_ = msg_ptr(new TrackerMessage(this));

      name_ = name;
      stop_at_return_ = true;
      flip_at_receive_ = false;
      flip_down_to_up_ = false;
      forget_set_dest_ = false;
      keep_ = false;
      kill_ = false;
      down_up_count_ = 0;
    }

    virtual ~TestCommunicator() { }

    Communicator* parent_;
    msg_ptr msg_;

    string name_;
    bool stop_at_return_, flip_at_receive_, forget_set_dest_;
    bool flip_down_to_up_;
    bool keep_;
    int down_up_count_;
    bool kill_;

    void startMessage() {
      dynamic_cast<TrackerMessage*>(msg_.get())->dest_list_.push_back(name_);
      msg_->setNextDest(parent_);
      msg_->sendOn();
    }

    void returnMessage() {
      if (!keep_) {
        return;
      }
      msg_->sendOn();
    }

  private:

    void receiveMessage(msg_ptr msg) {
      boost::intrusive_ptr<TrackerMessage> ptr;
      ptr = boost::intrusive_ptr<TrackerMessage>(dynamic_cast<TrackerMessage*>(msg.get()));
      ptr->dest_list_.push_back(name_);
      if (keep_) {
        msg_ = msg;
        return;
      }
      if (kill_) {
        msg->kill();
      }
      if (stop_at_return_ && this == msg->sender()) {
        return;
      } else if (flip_at_receive_) {
        msg->setDir(DOWN_MSG);
      } else if (flip_down_to_up_) {
        int max_num_flips = 2;
        if (msg->dir() == DOWN_MSG && down_up_count_ < max_num_flips) {
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
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - Message Passing Tests - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
class MessagePassingTest : public ::testing::Test {
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

    }
};

TEST_F(MessagePassingTest, CleanThrough) {
  ASSERT_NO_THROW(comm1->startMessage());

  vector<string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
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
TEST_F(MessagePassingTest, PassBeyondOrigin) {
  comm1->stop_at_return_ = false;

  ASSERT_THROW(comm1->startMessage(), CycException);

  vector<string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
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
TEST_F(MessagePassingTest, ForgetToSetDest) {
  comm3->forget_set_dest_ = true;

  ASSERT_THROW(comm1->startMessage(), CycException);

  vector<string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 3;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessagePassingTest, SendToSelf) {
  comm3->parent_ = comm3;

  ASSERT_THROW(comm1->startMessage(), CycException);

  vector<string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 3;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessagePassingTest, YoYo) {
  comm2->flip_down_to_up_ = true;

  ASSERT_NO_THROW(comm1->startMessage());

  vector<string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessagePassingTest, KillByDeletingSender) {
  msg_ptr msg = comm1->msg_;
  comm3->keep_ = true;

  ASSERT_NO_THROW(comm1->startMessage());
  ASSERT_FALSE(msg->isDead());
  delete comm1;
  ASSERT_TRUE(msg->isDead());
  ASSERT_NO_THROW(comm3->returnMessage());

  vector<string> stops = dynamic_cast<TrackerMessage*>(msg.get())->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 3;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessagePassingTest, KillSendOn) {
  comm3->kill_ = true;
  ASSERT_NO_THROW(comm1->startMessage());

  vector<string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 3;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST_F(MessagePassingTest, KillApproveTransfer) {
  // if this doesn't segfault, the test passes
  comm1->msg_->kill();
  comm1->msg_->trans().approveTransfer();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - -Message Public Interface Testing - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MessagePublicInterfaceTest : public ::testing::Test {
  protected:

    rsrc_ptr resource;
    double quantity1, quantity2;

    TestCommunicator* comm1;
    TestCommunicator* comm2;
    msg_ptr msg1;

    virtual void SetUp(){
      quantity1 = 1.0;
      quantity2 = 2.0;
      resource = gen_rsrc_ptr(new GenericResource("kg", "bananas", quantity1));

      Model* foo;
      Transaction* trans = new Transaction(foo, OFFER);
      comm1 = new TestCommunicator("comm1");
      comm2 = new TestCommunicator("comm2");
      msg1 = msg_ptr(new Message(comm1, comm2, *trans));
    };

    virtual void TearDown() {
      delete comm1;
      delete comm2;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - -Constructors and Cloning - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MessagePublicInterfaceTest, DISABLED_ConstructorOne) {
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MessagePublicInterfaceTest, DISABLED_ConstructorTwo) {
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MessagePublicInterfaceTest, DISABLED_ConstructorThree) {
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MessagePublicInterfaceTest, Cloning) {
  msg1->trans().setResource(resource);
  msg_ptr msg2 = msg1->clone();

  // check proper cloning of message members
  EXPECT_EQ(msg1->sender(), msg2->sender());

  // check proper cloning of message's resource
  rsrc_ptr resource2 = msg2->trans().resource();
  resource2->setQuantity(quantity2);

  ASSERT_DOUBLE_EQ(msg2->trans().resource()->quantity(), quantity2);
  ASSERT_DOUBLE_EQ(msg2->trans().resource()->quantity(), quantity2);
  ASSERT_NE(resource, msg1->trans().resource());
  ASSERT_NE(resource, resource2);

  EXPECT_DOUBLE_EQ(resource->quantity(), quantity1);
  EXPECT_DOUBLE_EQ(resource2->quantity(), quantity2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - Getters and Setters - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TEST_F(MessagePublicInterfaceTest, GetSetResource) {
  ASSERT_DOUBLE_EQ(resource->quantity(), quantity1);

  msg1->trans().setResource(resource);

  ASSERT_NE(resource, msg1->trans().resource());

  msg1->trans().resource()->setQuantity(quantity2);

  ASSERT_DOUBLE_EQ(resource->quantity(), quantity1);
  ASSERT_DOUBLE_EQ(msg1->trans().resource()->quantity(), quantity2);
}

