
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
*************

The data held by a material object is straightforward. The interface allows 
access to the mass or atomic composition of the material ::

    m_p->mass();   // returns the total mass of the material in kg
    m_p->atoms();  // returns the total atoms in the material in moles

While the default is to sum over all contained isotopes, the data can be 
retrieved for a single isotope, ::

    m_p->mass(92235, KG);   // returns the mass of 235U in kg
    m_p->atoms(92235);      // returns the mass of 235U in moles

It is capable of returning masses in kg or g, and capable of returning atoms in moles. ::

    m_p->mass(KG);  // returns the total mass of the material in kg
    m_p->mass(G);   // returns the total mass of the material in g
    m_p->atoms();   // returns the total atoms in the material in moles


Any time the data is queried, the Material object should be decayed, so that the 
most up-to-date material is returned. 


Material Methods  
****************

The methods that can be performed on a Material object in order to mainupulated 
are a small subset of mass conserving functions. These functions allow the user 
to combine two materials, extract a material from another, and decay materials 
on demand. 

**Material absorption** can be used when two materials should be combined physically
and when their histories should be shared in the future. To use this, the material 
to absorb must already exist. This helps with mass conservation ::

   mat_rsrc_ptr other_mat = mat_rsrc_ptr(new Material(c_p));
   m_p->absorb(other_mat);

**Material extraction** can be used when a subpart of a material is being separated 
physically from the original and when that subpart will have its own history (rather 
than a shared history) in the rest of the simulation. 


There are three ways to extract a material. The first method should be used when the 
material to extract already exists. This helps with mass conservation as the function
throws an error if the old material does not contain sufficient masses of each isotope 
in the new_mat ::

   mat_rsrc_ptr other_mat = mat_rsrc_ptr(new Material(c_p));
   m_p->extract(other_mat); 

The second way extracts a specified mass of material from the original. It internally
creates a new material that has the same normalized composition as the original but has 
the specified mass. Then, this function internally calls extract(new_mat) and returns 
the new material. ::

   double mass = 1000;
   MassUnit units = KG;
   mat_rsrc_ptr new_mat = extract(mass, units); 

An analgous function exists for extracting based on number of atoms. ::

   extract(atoms, units) 


 
Material Expectations 
*********************

The material class makes two primary promises. 

First, it will always decay material sufficiently that any retrieved data is 
current.  Second, it will always conserves mass, never creating material out 
of nothing.



RecipeLogger
------------

Predefined recipes are managed by the RecipeLogger in *Cyclus*. A CompMapPtr to a specific
recipe can be acquired through its static Recipe() method.
