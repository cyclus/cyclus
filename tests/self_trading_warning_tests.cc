#include <gtest/gtest.h>
#include <sstream>

// For testing self-trading warnings
#include "context.h"
#include "material.h"
#include "trade.h"
#include "trade_executor.h"
#include "test_context.h"
#include "resource_helpers.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Simple test facility for self-trading tests
class SelfTradingTestFacility : public cyclus::Facility {
 public:
  SelfTradingTestFacility(cyclus::Context* ctx) : cyclus::Facility(ctx) {}
  virtual ~SelfTradingTestFacility() {}

  virtual cyclus::Agent* Clone() { 
    return new SelfTradingTestFacility(context()); 
  }
  
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { 
    return cyclus::Inventories(); 
  }

  void Tick() {}
  void Tock() {}

  // Simple implementation to support trading
  virtual std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr>
      GetMatlRequests() {
    std::set<cyclus::RequestPortfolio<cyclus::Material>::Ptr> ports;
    cyclus::RequestPortfolio<cyclus::Material>::Ptr port(
        new cyclus::RequestPortfolio<cyclus::Material>());
    
    cyclus::Material::Ptr mat = cyclus::NewBlankMaterial(100);
    port->AddRequest(mat, this, "NaturalUranium");
    ports.insert(port);
    return ports;
  }

  virtual std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr>
      GetMatlBids(cyclus::CommodMap<cyclus::Material>::type& commod_requests) {
    std::set<cyclus::BidPortfolio<cyclus::Material>::Ptr> ports;
    
    if (commod_requests.count("NaturalUranium") > 0) {
      cyclus::BidPortfolio<cyclus::Material>::Ptr port(
          new cyclus::BidPortfolio<cyclus::Material>());
      
      std::vector<cyclus::Request<cyclus::Material>*>& requests = 
          commod_requests.at("NaturalUranium");
      
      for (std::vector<cyclus::Request<cyclus::Material>*>::iterator it = 
               requests.begin(); it != requests.end(); ++it) {
        cyclus::Request<cyclus::Material>* req = *it;
        cyclus::Material::Ptr offer = cyclus::NewBlankMaterial(100);
        port->AddBid(req, offer, this);
      }
      ports.insert(port);
    }
    return ports;
  }

  virtual void GetMatlTrades(
      const std::vector<cyclus::Trade<cyclus::Material>>& trades,
      std::vector<std::pair<cyclus::Trade<cyclus::Material>, 
                           cyclus::Material::Ptr>>& responses) {
    for (std::vector<cyclus::Trade<cyclus::Material>>::const_iterator it = 
             trades.begin(); it != trades.end(); ++it) {
      cyclus::Material::Ptr response = cyclus::NewBlankMaterial(it->amt);
      responses.push_back(std::make_pair(*it, response));
    }
  }

  virtual void AcceptMatlTrades(
      const std::vector<std::pair<cyclus::Trade<cyclus::Material>,
                                 cyclus::Material::Ptr>>& responses) {
    // Simple acceptance - just ignore the materials
  }
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Test for self-trading warning functionality
class SelfTradingWarningTest : public ::testing::Test {
 public:
  virtual void SetUp() {
    // Create a test context
    tc_ = std::make_unique<cyclus::TestContext>();
    
    // Create a test facility that can trade with itself
    facility_ = new SelfTradingTestFacility(tc_->get());
    
    // Set up the facility
    facility_->Build(nullptr);
    facility_->EnterNotify();
  }

  virtual void TearDown() {
    delete facility_;
  }

 protected:
  std::unique_ptr<cyclus::TestContext> tc_;
  SelfTradingTestFacility* facility_;
};

TEST_F(SelfTradingWarningTest, SelfTradingWarningIssued) {
  // Capture stderr to check for warning messages
  std::stringstream captured_stderr;
  std::streambuf* original_stderr = std::cerr.rdbuf();
  std::cerr.rdbuf(captured_stderr.rdbuf());
  
  // Create a material for testing
  cyclus::CompMap v;
  v[922350000] = 1;  // U-235
  v[922380000] = 2;  // U-238
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromAtom(v);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(100, comp);
  
  // Create a trade where the same facility is both supplier and requester
  cyclus::Request<cyclus::Material>* req = 
      cyclus::Request<cyclus::Material>::Create(mat, facility_, "NaturalUranium");
  cyclus::Bid<cyclus::Material>* bid = 
      cyclus::Bid<cyclus::Material>::Create(req, mat, facility_);
  
  cyclus::Trade<cyclus::Material> trade(req, bid, 50.0);
  std::vector<cyclus::Trade<cyclus::Material>> trades;
  trades.push_back(trade);
  
  // Execute the trade - this should trigger the warning
  cyclus::TradeExecutor<cyclus::Material> executor(trades);
  executor.ExecuteTrades(tc_->get());
  
  // Restore stderr
  std::cerr.rdbuf(original_stderr);
  
  // Check that the warning was issued
  std::string stderr_output = captured_stderr.str();
  EXPECT_TRUE(stderr_output.find("State Warning") != std::string::npos);
  EXPECT_TRUE(stderr_output.find("is trading with itself") != std::string::npos);
  EXPECT_TRUE(stderr_output.find("NaturalUranium") != std::string::npos);
  
  // Clean up
  delete bid;
  delete req;
}

TEST_F(SelfTradingWarningTest, NoWarningForDifferentAgents) {
  // Create a second test facility
  SelfTradingTestFacility* facility2 = new SelfTradingTestFacility(tc_->get());
  facility2->Build(nullptr);
  facility2->EnterNotify();
  
  // Capture stderr
  std::stringstream captured_stderr;
  std::streambuf* original_stderr = std::cerr.rdbuf();
  std::cerr.rdbuf(captured_stderr.rdbuf());
  
  // Create a trade between different facilities
  cyclus::CompMap v;
  v[922350000] = 1;
  v[922380000] = 2;
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromAtom(v);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(100, comp);
  
  cyclus::Request<cyclus::Material>* req = 
      cyclus::Request<cyclus::Material>::Create(mat, facility2, "NaturalUranium");
  cyclus::Bid<cyclus::Material>* bid = 
      cyclus::Bid<cyclus::Material>::Create(req, mat, facility_);
  
  cyclus::Trade<cyclus::Material> trade(req, bid, 50.0);
  std::vector<cyclus::Trade<cyclus::Material>> trades;
  trades.push_back(trade);
  
  // Execute the trade - this should NOT trigger the warning
  cyclus::TradeExecutor<cyclus::Material> executor(trades);
  executor.ExecuteTrades(tc_->get());
  
  // Restore stderr
  std::cerr.rdbuf(original_stderr);
  
  // Check that no self-trading warning was issued
  std::string stderr_output = captured_stderr.str();
  EXPECT_TRUE(stderr_output.find("is trading with itself") == std::string::npos);
  
  // Clean up
  delete bid;
  delete req;
  delete facility2;
}

TEST_F(SelfTradingWarningTest, WarningIncludesCorrectAgentId) {
  // Capture stderr
  std::stringstream captured_stderr;
  std::streambuf* original_stderr = std::cerr.rdbuf();
  std::cerr.rdbuf(captured_stderr.rdbuf());
  
  // Create a material for testing
  cyclus::CompMap v;
  v[922350000] = 1;
  v[922380000] = 2;
  cyclus::Composition::Ptr comp = cyclus::Composition::CreateFromAtom(v);
  cyclus::Material::Ptr mat = cyclus::Material::CreateUntracked(100, comp);
  
  // Create a trade where the same facility is both supplier and requester
  cyclus::Request<cyclus::Material>* req = 
      cyclus::Request<cyclus::Material>::Create(mat, facility_, "NaturalUranium");
  cyclus::Bid<cyclus::Material>* bid = 
      cyclus::Bid<cyclus::Material>::Create(req, mat, facility_);
  
  cyclus::Trade<cyclus::Material> trade(req, bid, 50.0);
  std::vector<cyclus::Trade<cyclus::Material>> trades;
  trades.push_back(trade);
  
  // Execute the trade
  cyclus::TradeExecutor<cyclus::Material> executor(trades);
  executor.ExecuteTrades(tc_->get());
  
  // Restore stderr
  std::cerr.rdbuf(original_stderr);
  
  // Check that the warning includes the correct agent ID
  std::string stderr_output = captured_stderr.str();
  std::string expected_id = std::to_string(facility_->id());
  EXPECT_TRUE(stderr_output.find(expected_id) != std::string::npos);
  
  // Clean up
  delete bid;
  delete req;
} 