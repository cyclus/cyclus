Domain Model Document Outline
=============================

Within the Domain Model
~~~~~~~~~~~~~~~~~~~~~~~~

#. Introduction
#. History/Motivation/Future
#. Simulation Environment Overview

   a. introduce concepts like current environment online doc....

   b. common mental model of constructed simulation

#. (Sample) use cases & ecosystem needs
#. Core Component Concepts

   a. Markets

      i. Commodities

      ii. Defining Objectives and Constrains

   b. Agency of Facilities, Regions, and Institutions

      i. Use Cases (inherited from Use Cases above)

   c. Resources

      i. Materials

   d. Messages

   e. Transactions


   f. Agent properties

      i. What are separabel properties of agents ??? 
      
      ii. Communicators

      iii. Time agents

      iv. prototypes

      v. dynamic loading

      vi. Specializations

         1. Facilities

         2. Institutions

         3. Regions

#. Utility Component Concepts

   a. File Loading

      i. XML

      ii. Exceptions & error handling

      iii. Logging

#. Simulation Wide concepts

   a. Introduction: Simulation phases

   b. Initialization

   c. Simulation construction

   d. Facility deployment from prototypes

   e. Scope of agent knowledge of simulation

      i. what does each agent know

      ii. what info can each agent get from another

   f. Communication and Material Flow 

      i. Tick : Offer/request transmission, etc.

      ii. Market resolution

      iii. Tock : Material transactions, etc.

      iv. How are ticks/etc. passed through the tree

   g. Module internals

      i. When can you perform internal behavior

      ii. What can you expect from the state of the system

   h. Output & persistence


Outside the domain model
~~~~~~~~~~~~~~~~~~~~~~~~

#. Software Design expectations

   a. code Style

   b. documentation style

   c. git workflows

   d. Tests

#. Ecosystem

   a. Requirements and expectations

   b. Finding and Using Modules

   c. cycamore

   d. cycstub

   e. Toaster

   f. blah....
