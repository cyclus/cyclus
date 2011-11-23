// Channel.h

#if !defined(_CHANNEL)
#define _CHANNEL

#include "Model.h"
#include "Resource.h"

class Channel {
  public:
    Channel();

    ~Channel() {};

    void approve();

    void setSupplier()

  private:

    Resource resource_;

    int approval_count_;

    Model* supplier_;

    Model* requester_;

    void transactResources();

};

#endif
