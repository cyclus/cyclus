# Cyclus-Dakota Sensitivity Analysis 

This folder contains a test example of Cyclus sensitivity analysis studies conducted using Dakota. Cyclus interfaces with Dakota through a Python interface.

To run the test case, run the following from this directory:
```
dakota -i dakota_test.in -o dakota_test.out
```

In the test case, dakota is running 2 Cyclus simulations in which power is varied from 1500 to 1600MW with 2 partitions. This results in 3 Cyclus simulations run for 1500MW, 1550MW, and 1600MW. 

## Dependencies 
Dakota & its depenencies: https://dakota.sandia.gov/download.html

### More Examples 
For more examples of more complex sensitivity analysis studies conducted using Cyclus and Dakota, go to: https://github.com/arfc/dcwrapper
