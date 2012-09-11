
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

CompMaps are constructed as either an atom-based or mass-based. An example of such a construction
is provided: ::

   CompMapPtr c_p = CompMapPtr(new CompMap(MASS));
   (*c_p)[92235] = 10.0; // 10 g_235/ 100 g_comp
   (*c_p)[92238] = 90.0; // 90 g_238/ 100 g_comp
   c_p.normalize();
 
IsoVectors
----------

The IsoVector class is effectively a wrapper and manager of CompMapPtrs. Addition and 
subtraction operators are defined here, using specific cases of the mix() and separate()
functions.

Materials
---------

Materials are the main objects of information in *Cyclus* simulations. A Material has a quantity
and an IsoVector, comprising its composition. Materials are also passed around *Cyclus* functions
in boost smart pointers called "mat_rsrc_ptr"s. 

A Material is effectively fully initialized through construction via CompMapPtr or IsoVector and
setting its quantity. For example: ::

    mat_rsrc_ptr m_p = mat_rsrc_ptr(new Material(c_p));
    m_p->setQuantity(10); // 10kg of c_p


Material Data 
~~~~~~~~~~~~~~

mass(isos, time, units) 
should return data about an appropriately decayed material
allow kg, g (amu?)
allow summing
just a single isotope 
a set of isotopes
a single element
a set of elements,
return data for any time, in any allowed unit.
atoms(isos, time, units) 
should return data about an appropriately decayed material
always moles (?)
allow summing
just a single isotope 
a set of isotopes
a single element
a set of elements,
return data for any time, in any allowed unit.


Material Methods  
~~~~~~~~~~~~~~~~~~~

absorb(other_mat)
To use this, the material to absorb has to already exist. This helps with mass conservation
extract(new_mat)
To use this, the material to extract has to already exist. This helps with mass conservation
returns the new_mat
throws an error if the old material does not contain sufficient masses of each isotope in the new_mat
extract(mass, units)  
This function should create a new material made of this material's composition, but the mass specified, then call extract(new_mat)
returns the new_mat
To use this, the material to extract is created within the material class and returned as part of this function call. This helps with mass conservation
throws an error if the old material does not contain sufficient mass 
extract(atoms, units) 
This function should create a new material made of this material's composition, but the atoms specified, then call extract(new_mat)
returns the new_mat
To use this, the material to extract is created within the material class and returned as part of this function call. This helps with mass conservation
throws an error if the old material does not contain sufficient atoms 


Material Expectations 
~~~~~~~~~~~~~~~~~~~~~~~~

Always decays material sufficiently so that the retrieved data is current.
Always conserves mass, never creating material out of nothing.




RecipeLogger
------------

Predefined recipes are managed by the RecipeLogger in *Cyclus*. A CompMapPtr to a specific
recipe can be acquired through its static Recipe() method.
