
Cyclus Inventory Tool
======================

The inventory tool builds an inventory table for fast querying of agent
resource inventory.  The table has 5 columns:

* SimID: a cyclus simulation ID
* ResID: a resource ID
* AgentID: a cyclus agent ID
* StartTime: timestep the resource identified by ResID was first present inside the agent identified by AgentID
* EndTime: timestep the resource identified by ResID was last present inside the agent identified by AgentID

An agent X's inventory at time t can be determined by Querying for all ResID's
that have ``(AgentID == X) AND (StartTime <= t) AND (EndTime > t)``.  This
query can be joined with the Resources, Agents, and Compositions tables
to retrieve the desired inventory information.

To build the tool:

#. Install go1.1.2 or later (http://golang.org/doc/install).  Be sure
   to set the GOROOT env var as per install directions and add ``$GOROOT/bin``
   to your path.

   Alternatively, you can use apt-get or equivalent (if the package is a recent
   enough version of Go).  You will also need to set the GOPATH env var as the
   location to install the sqlite driver dependency.
  
#. There is one external dependency.  To install it run::

    go get code.google.com/p/go-sqlite/go1/sqlite3

#. Run ``go build`` in the inventory directory (this dir) to build the binary.

