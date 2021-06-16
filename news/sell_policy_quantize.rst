**Added:** None

**Changed:** 
- now default quantize are zero which corresponds to no quantize policy.

**Deprecated:** None

**Removed:** None

**Fixed:**
- fix sell_policy that was offering bids when capacity was inbetween 0 and the
  quantize, bids that one was not able to fullfill and caused cyclus to crash.

**Security:** None
