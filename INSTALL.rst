###########
Cyclus Installation Guide
###########

Cyclus uses the CMake cross platformbuild-generator as a build system. TO
simplify the usage of cmake to build Cyclus from source, a python script is
provided with the source.

.. contents:: Table of Contents

******************************
Dependencies
******************************

In order to facilitate future compatibility with multiple platforms,
Cyclus is built using `CMake`_. A full list of the Cyclus package
dependencies is shown below:

====================   ==================
Package                Minimum Version
====================   ==================
`CMake`                2.8
`boost`                1.46.1
`libxml2`              2
`libxml++`             2.36
`python`               2.7 or 3.3+
`sqlite3`              3.7.10
`HDF5`                 1.8.4
`Coin-Cbc`             2.5
====================   ==================

You can find `here <DEPENDENCIES.rst>`_  help to install those dependencies for
the major supported system.

******************************
Installation
******************************

Before going further on the installation proccedure be sure you have installied
all the required dependencies listed above. You can also find `there
<DEPENDENCIES.rst>`_ instruction to install those dependencies depending of the
system you are working on.

Installing Cyclus from source:
=============================================
 
Once you have installed all the dependency you can follow those step the
complete the default installation of Cyclus:

#. Create a folder to host the Cyclus environment: ``mkdir cyclusENV && cd
   cyclusENV``
#. Download the source from there
   or from the Git-repository: ``git clone https://github.com/cyclus/cyclus . &&
   git fetch && git checkout master``
#. move into the new Cyclus directory:  ``cd cyclus``
#. run the install script: ``python instal.py``







Assuming you have the dependencies installed correctly, installing Cyclus is
fairly straightforward.

We make the following assumptions in this guide:

#. there is some master directory in which you're placing all
   Cyclus-related files called .../cyclus
#. you want to install cyclus **locally** (in ``~/.local``)
#. you have acquired the Cyclus source code from the `Cyclus repo`_
#. you have placed the Cyclus repository in .../cyclus/cyclus

Under these assumptions **and** if you used a package manager to
install coin-Cbc (i.e. it's installed in a standard location), the
Cyclus building and installation process will look like:

.. code-block:: bash

    .../cyclus/cyclus$ python install.py

If you have installed coin-Cbc from source or otherwise have it
installed in a non-standard location, you should make use of the
coinRoot installation flag. The otherwise identical process would look
like:

.. code-block:: bash

    .../cyclus/cyclus$  python install.py --coin_root=path/to/coin

Additionally, if you have installed Boost in a non-standard location
you should make use of the boostRoot installation flag.

.. code-block:: bash

    .../cyclus/cyclus$ python install.py --coin_root=/path/to/coin --boost_root=/path/to/boost

There are additional options which can be inspected via `install.py`'s help:

.. code-block:: bash

    .../cyclus/cyclus$ python install.py -h

Finally, add the following line to the **bottom** your ``~/.bashrc`` file
(``~/.bash_profile`` on Macs):

.. code-block:: bash

    export PATH="$HOME/.local/bin:$PATH"

Then update your environment

.. code-block:: bash

    $ source ~/.bashrc

Running Tests
=============

Installing Cyclus will also install a test driver (i.e., an executable of all of
our tests). You can run the tests yourself via:

.. code-block:: bash

    $ cyclus_unit_tests

Running Cyclus
==============

You can find instructions for writing an input file for cyclus from `Cyclus User
Guide`_ or use sample input files from `Cycamore Repo`_. Assuming you have some
file `input.xml`, you can run Cyclus via:

.. code-block:: bash

    $ cyclus path/to/input.xml

For a more detailed explanation, checkout the user guide.

.. _`Cyclus Homepage`: http://fuelcycle.org/
.. _`Cyclus User Guide`: http://fuelcycle.org/user/index.html
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cycamore Repo`: https://github.com/cyclus/cycamore

