
.. summary Some developers notes on how materials and isotopic vectors work

Materials and Isotopics in Cyclus
=================================

Introduction
------------
The following section will discuss the three classes that comprise materials and
isotopics in *Cyclus*:

  * Material

  * IsoVector

  * CompMap

Materials are the primary Resource that is transacted in a *Cyclus* simulation. A 
Material has two constitutents: an amount (quantity) and an isotopic composition.
Isotopic compositions can be expensive to copy and to check equality, and are therefore
decomposed into two containers. CompMaps are the most basic isotopic container, and
can be thought of as a simple map of isotopic integer values (the keys) to values.
An IsoVector manages a smart pointer to a CompMap in order to minimize unneccessary
operations.

CompMaps
--------

CompMaps are immutable objects: once constructed, they are guaranteed not to change. This
design decision provides confidence to developers and thread safety.

*Cyclus* provides public API for CompMaps only through the use of boost smart pointers, 
typedefed as "CompMapPtr"s. 

IsoVectors
----------

The IsoVector class is effectively a wrapper and manager of CompMapPtrs. Addition and 
subtraction operators are defined here, using specific cases of the mix() and separate()
functions.

Materials
---------

Materials are the main constituents in *Cyclus* simulations. A Material has a quantity
and an IsoVector, comprising its composition.

RecipeLogger
------------

Predefined recipes are managed by the RecipeLogger in *Cyclus*. A CompMapPtr to a specific
recipe can be acquired through its static Recipe() method.
