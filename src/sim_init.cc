
SimInit::SimInit() {
  se_ = new SimEngine();
  se_->ti = new Timer();
}


SimEngine* SimInit::Init(QueryBackend* b, boost::uuids::uuid simid) {
}

SimEngine* SimInit::Restart(QueryBackend* b, boost::uuids::uuid simid, int t) {
}

SimEngine* SimInit::InitBase() {
  se_->ctx = new Context(se_->ti, se_->rec);

  LoadControlParams();
  LoadRecipes()
  LoadSolverInfo();
  LoadPrototypes();
  LoadInitialAgents();
  LoadInventories();

  // use rec.set_dump_count to reset all buffered datums that we don't want
  // to be eventually sent to backends that are to-be-added.
  rec_->set_dump_count(kDefaultDumpCount);

  return se_;
}

void SimInit::LoadControlParams() {
  q_->Query("info")
}

void SimInit::LoadRecipes() {
}

void SimInit::LoadSolverInfo() {
}

void SimInit::LoadPrototypes() {
}

void SimInit::LoadInitialAgents() {
}

void SimInit::LoadInventories() {
}
