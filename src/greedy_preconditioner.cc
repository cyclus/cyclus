
  std::vector<RequestGroup::Ptr>& groups =
      const_cast<std::vector<RequestGroup::Ptr>&>(graph_->request_groups());
  std::vector<RequestGroup::Ptr> sorted;

  if (sort_) {
    sorted = std::vector<RequestGroup::Ptr>(groups);
    groups = sorted;
  }  
