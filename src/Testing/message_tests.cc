#include <gtest/gtest.h>

#include "communicator.h"
#include "message.h"
#include "resource.h"
#include "generic_resource.h"
#include "error.h"

#include <string>
#include <vector>

class TrackerMessage : public cyclus::Message {
  public:
    TrackerMessage(cyclus::Communicator* originator) : cyclus::Message(originator) { }

    std::vector<std::string> dest_list_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class TestCommunicator : public cyclus::Communicator {
  public:

    TestCommunicator(std::string name) {
      msg_ = cyclus::msg_ptr(new TrackerMessage(this));

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

    cyclus::Communicator* parent_;
    cyclus::msg_ptr msg_;

    std::string name_;
    bool stop_at_return_, flip_at_receive_, forget_set_dest_;
    bool flip_down_to_up_;
    bool keep_;
    int down_up_count_;
    bool kill_;

    void StartMessage() {
      dynamic_cast<TrackerMessage*>(msg_.get())->dest_list_.push_back(name_);
      msg_->SetNextDest(parent_);
      msg_->SendOn();
    }

    void ReturnMessage() {
      if (!keep_) {
        return;
      }
      msg_->SendOn();
    }

  private:

    void ReceiveMessage(cyclus::msg_ptr msg) {
      using cyclus::UP_MSG;
      using cyclus::DOWN_MSG;
      boost::intrusive_ptr<TrackerMessage> ptr;
      ptr = boost::intrusive_ptr<TrackerMessage>(dynamic_cast<TrackerMessage*>(msg.get()));
      ptr->dest_list_.push_back(name_);
      if (keep_) {
        msg_ = msg;
        return;
      }
      if (kill_) {
        msg->Kill();
      }
      if (stop_at_return_ && this == msg->sender()) {
        return;
      } else if (flip_at_receive_) {
        msg->SetDir(DOWN_MSG);
      } else if (flip_down_to_up_) {
        int max_num_flips = 2;
        if (msg->Dir() == DOWN_MSG && down_up_count_ < max_num_flips) {
          msg->SetDir(UP_MSG);
          down_up_count_++;
        }
      }

      if ( !forget_set_dest_ ) {
        msg->SetNextDest(parent_);
      }

      msg->SendOn();
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
  ASSERT_NO_THROW(comm1->StartMessage());

  std::vector<std::string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
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

  ASSERT_THROW(comm1->StartMessage(), cyclus::Error);

  std::vector<std::string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
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

  ASSERT_THROW(comm1->StartMessage(), cyclus::Error);

  std::vector<std::string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
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

  ASSERT_THROW(comm1->StartMessage(), cyclus::Error);

  std::vector<std::string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
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

  ASSERT_NO_THROW(comm1->StartMessage());

  std::vector<std::string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
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
  using cyclus::msg_ptr;
  msg_ptr msg = comm1->msg_;
  comm3->keep_ = true;

  ASSERT_NO_THROW(comm1->StartMessage());
  ASSERT_FALSE(msg->IsDead());
  delete comm1;
  ASSERT_TRUE(msg->IsDead());
  ASSERT_NO_THROW(comm3->ReturnMessage());

  std::vector<std::string> stops = dynamic_cast<TrackerMessage*>(msg.get())->dest_list_;
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
  ASSERT_NO_THROW(comm1->StartMessage());

  std::vector<std::string> stops = dynamic_cast<TrackerMessage*>(comm1->msg_.get())->dest_list_;
  int num_stops = stops.size();
  int expected_num_stops = 3;

  ASSERT_EQ(num_stops, expected_num_stops);

  EXPECT_EQ(stops[0], "comm1");
  EXPECT_EQ(stops[1], "comm2");
  EXPECT_EQ(stops[2], "comm3");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - -Message Public Interface Testing - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class MessagePublicInterfaceTest : public ::testing::Test {
  protected:

    cyclus::rsrc_ptr resource;
    double quantity1, quantity2;

    TestCommunicator* comm1;
    TestCommunicator* comm2;
    cyclus::msg_ptr msg1;
    cyclus::Model* foo;

    virtual void SetUp(){
      quantity1 = 1.0;
      quantity2 = 2.0;
      resource = cyclus::gen_rsrc_ptr(new cyclus::GenericResource("kg", "bananas", quantity1));

      cyclus::Transaction* trans = new cyclus::Transaction(foo, cyclus::OFFER, NULL);
      comm1 = new TestCommunicator("comm1");
      comm2 = new TestCommunicator("comm2");
      msg1 = cyclus::msg_ptr(new cyclus::Message(comm1, comm2, *trans));
    };

    virtual void TearDown() {
      delete comm1;
      delete comm2;
    }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - -Constructors and Cloning - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MessagePublicInterfaceTest, FullConstructor) {
  using cyclus::Transaction;
  using cyclus::OFFER;
  using cyclus::REQUEST;
  double price = 1.2;
  double minfrac = 0.2;
  Transaction* trans;
  Transaction* trans_no_min;
  Transaction* trans_no_price;
  EXPECT_NO_THROW(trans = new Transaction(foo, OFFER, resource, price, minfrac)); 
  ASSERT_FLOAT_EQ(price, trans->price());
  ASSERT_FLOAT_EQ(minfrac, trans->Minfrac());
  ASSERT_EQ(true, trans->IsOffer());
  EXPECT_NO_THROW(trans_no_min = new Transaction(foo, REQUEST, resource, price)); 
  ASSERT_FLOAT_EQ(price, trans_no_min->price());
  ASSERT_FLOAT_EQ(0.0, trans_no_min->Minfrac());
  ASSERT_EQ(false, trans_no_min->IsOffer());
  EXPECT_NO_THROW(trans_no_price = new Transaction(foo, REQUEST, resource)); 
  ASSERT_FLOAT_EQ(0,trans_no_price->price());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MessagePublicInterfaceTest, DISABLED_ConstructorTwo) {
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MessagePublicInterfaceTest, DISABLED_ConstructorThree) {
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(MessagePublicInterfaceTest, Cloning) {
  using cyclus::msg_ptr;
  using cyclus::rsrc_ptr;
  msg1->trans().SetResource(resource);
  msg_ptr msg2 = msg1->clone();

  // check proper cloning of message members
  EXPECT_EQ(msg1->sender(), msg2->sender());

  // check proper cloning of message's resource
  rsrc_ptr resource2 = msg2->trans().Resource();
  resource2->SetQuantity(quantity2);

  ASSERT_DOUBLE_EQ(msg2->trans().Resource()->quantity(), quantity2);
  ASSERT_DOUBLE_EQ(msg2->trans().Resource()->quantity(), quantity2);
  ASSERT_NE(resource, msg1->trans().Resource());
  ASSERT_NE(resource, resource2);

  EXPECT_DOUBLE_EQ(resource->quantity(), quantity1);
  EXPECT_DOUBLE_EQ(resource2->quantity(), quantity2);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//- - - - - - - - - Getters and Setters - - - - - - - - - - - - - - - - - -
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

TEST_F(MessagePublicInterfaceTest, GetSetResource) {
  ASSERT_DOUBLE_EQ(resource->quantity(), quantity1);

  msg1->trans().SetResource(resource);

  ASSERT_NE(resource, msg1->trans().Resource());

  msg1->trans().Resource()->SetQuantity(quantity2);

  ASSERT_DOUBLE_EQ(resource->quantity(), quantity1);
  ASSERT_DOUBLE_EQ(msg1->trans().Resource()->quantity(), quantity2);
}

