# Cyclus-Dakota Sensitivity Analysis 

This folder contains a test example of Cyclus sensitivity analysis studies conducted using Dakota. Cyclus interfaces with Dakota through a Python interface.

To run the test case, run the following from this directory:
```
dakota -i dakota_test.in -o dakota_test.out
```

In the test case, dakota is running 3 Cyclus simulations in which power is varied from 1500 to 1600MW with 2 partitions. This results in 3 Cyclus simulations run for 1500MW, 1550MW, and 1600MW. 
Each Cyclus simulation is 10 months long. 
In this example, the output is not being processed using a sql query to return the total power generated in the simulation, which is 15000MW, 15500MW, and 16000MW for the respective simulations. 

## Dependencies 
Dakota & its dependencies: https://dakota.sandia.gov/download.html

### More Examples 
For more examples of more complex sensitivity analysis studies conducted using Cyclus and Dakota, go to: https://github.com/arfc/dcwrapper
