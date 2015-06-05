###########
Cyclus Core
###########

The core of the Cyclus nuclear fuel cycle simulator from the
University of Wisconsin - Madison is intended to be a simulation
framework upon which to develop innovative fuel cycle simulations.

To see user and developer documentation for this code, please visit
the `Cyclus Homepage`_.

*******************
Quick Start
*******************

If you want to get up and running as quickly as possible:

#. Install `VirtualBox <https://www.virtualbox.org/>`_
#. Download the `environment
   <http://cnergdata.engr.wisc.edu/cyclus/virtual-box/current/cyclus-conda-devs.ova>`_
#. Open the .ova file using VirtualBox (File->Import Appliance).
#. Login with the username ``ubuntu`` and pw ``reverse``
#. Skip to "Installing Cyclus"

Note that the Cyclus source code exists already on the virtual machine. You can
update it with

.. code-block:: console

    $ cd cyclus
	$ git pull origin cyclus

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

On some platforms, such as Ubuntu 14.04, the following are also necessary:

====================   ==================
Package                Minimum Version
====================   ==================
`g++`                  4.8.2
`libblas-dev`          1.2
`liblapack-dev`        3.5.0
====================   ==================


And a few optional dependencies:

====================   ==================
Package                Minimum Version
====================   ==================
doxygen (for docs)     1.7.6.1
tcmalloc (for speed)   any?
====================   ==================

*Note that the Debian/Ubuntu package `libtcmalloc` is NOT discovered correctly
by our build system.  Instead use `libgoogle-perftools-dev`.*

Installing Dependencies
=========================

Cyclus dependencies can either be installed via an operating system's package
manager or via Conda.

Via Conda
---------

*Note, at present, Conda dependency downloading is only supported for Linux*

A Conda installation may be the most straightforward for a new developer. These
instructions will be similar to those shown for Cyclus users `conda
installation <http://fuelcycle.org/user/install.html>`_.

#. Download the appropriate `miniconda installer <http://conda.pydata.org/miniconda.html>`_

#. Install miniconda

    .. code-block:: console

        $ bash Miniconda-3.5.2-Linux-x86_64.sh -b -p ~/miniconda

#. Add the following line to your ``.bashrc`` (``.bash_profile`` for Mac users):

    ``export PATH-"${HOME}/miniconda/bin:${PATH}"``

#. Install the dependencies

    .. code-block:: console

        $ conda install cyclus-deps

Installing Dependencies (Linux and Unix)
----------------------------------------

This guide assumes that the user has root access (to issue sudo commands) and
access to a package manager or has some other suitable method of automatically
installing established libraries. This process was tested using a fresh install
of Ubuntu 12.10 using apt-get as the package manager (scroll down further for
Mac OSX instructions).

The command to install a dependency takes the form of:

.. code-block:: bash

  sudo apt-get install package

where "package" is replaced by the correct package name. The minimal list of
required library package names is:

#. make
#. cmake
#. libboost-all-dev (see note below)
#. libxml2-dev
#. libxml++2.6-dev
#. libsqlite3-dev
#. libhdf5-serial-dev
#. libbz2-dev
#. coinor-libcbc-dev
#. coinor-libcoinutils-dev
#. coinor-libosi-dev
#. coinor-libclp-dev
#. coinor-libcgl-dev

and (optionally):

#. doxygen
#. g++
#. libblas-dev
#. liblapack-dev
#. libgoogle-perftools-dev

For example, in order to install libxml++ (and libxml2) on your system, type:

.. code-block:: bash

  sudo apt-get install libxml++2.6-dev

If you'd prefer to copy/paste, the following line will install all *Cyclus*
dependencies:

.. code-block:: bash

   sudo apt-get install -y cmake make libboost-all-dev libxml2-dev libxml++2.6-dev libsqlite3-dev libhdf5-serial-dev libbz2-dev coinor-libcbc-dev coinor-libcoinutils-dev coinor-libosi-dev coinor-libclp-dev coinor-libcgl-dev libblas-dev liblapack-dev g++ libgoogle-perftools-dev

Boost Note
^^^^^^^^^^

The `libboost-all-dev` used above will install the entire Boost library, which
is not strictly needed. We currently depend on a small subset of the Boost
libraries:

#. libboost-program-options-dev
#. libboost-system-dev
#. libboost-filesystem-dev

However, it is possible (likely) that additional Boost libraries will be used
because they are an industry standard. Accordingly, we suggest simply installing
`libboost-all-dev` to limit any headaches due to possible dependency additions
in the future.

Installing Dependencies (Mac OSX)
----------------------------------------

Cyclus archetype development is not fully supported on Mac.  Nonetheless,
because there are some use cases which require installation from source,
we have compiled a list of instructions that should be successful. (Note that
the HDF5 interface is not working on Yosemite as of 1-Apr-2015. Ignore related
warnings when building cyclus). Use a Mac platform at your own risk, we strongly
recommend sticking to Linux for development.

This guide assumes that the user has root access (to issue sudo commands) and
access to a package manager or has some other suitable method of automatically
installing established libraries. This process was tested using a fresh install
of Yosemite 10.10.2 using macports as the package manager.  Macports installs
packages in /opt/local.  If installing to a different location,
(i.e. /usr/local) change paths in the following instructions accordingly. If
you use homebrew, try the following instructions with brew commands in place of
the port commands.

The command to install a dependency takes the form of:

.. code-block:: bash

  sudo port install package

where "package" is replaced by the correct package name. The minimal list of
required library package names is:

#. cmake
#. boost
#. libxml2
#. libxmlxx2
#. sqlite3
#. doxygen
#. glibmm

Then install Coin-Cbc and HDF5 from source. They can be downloaded to any
directory on your computer:
   
**Coin-Cbc**: Download and build using the svn command in the terminal:
   
.. code-block:: bash

  svn co https://projects.coin-or.org/svn/Cbc/stable/2.8 Coin-Cbc
  cd Coin-Cbc/
  mkdir build
  cd build/
  ../configure --prefix=/opt/local
  make
  sudo make install

**HDF5**: The 1.8.13 version appears to work better than 1.8.14.  Do not use
the macports distribution, it is definitely broken.
Download and build using the gzip Linux/Unix distribution of
`HDF5. <http://www.hdfgroup.org/ftp/HDF5/releases/hdf5-1.8.13/src/hdf5-1.8.13.tar.gz>`_   (For Safari users - the file will be automatically unzipped so change
the *mv* command in the the following codeblock to *mv hdf5-1.8.13.tar hdf5/* ).

.. code-block:: bash

  mkdir hdf5/
  mv hdf5-1.8.13.tar.gz hdf5/
  cd hdf5/
  mkdir build
  cd build/
  ../configure --prefix=/opt/local
  make
  sudo make install

Finally, update your path and the following environment variables in your
~/.profile (or ~/.bashrc ) file:

.. code-block:: bash

  export DYLD_FALLBACK_LIBRARY_PATH=/opt/local/lib:/opt/local:$DYLD_FALLBACK_LIBRARY_PATH

  export CMAKE_MODULE_PATH=/opt/local/include:$CMAKE_MODULE_PATH
  export CMAKE_PREFIX_PATH=/opt/local:$CMAKE_PREFIX_PATH

  export HDF5_DIR=/opt/local/hdf5/lib
  export HDF5_ROOT=/opt/local/hdf5

  # add to PATH:
  export PATH=${HDF5_DIR}:/opt/local/bin:${HOME}/.local/bin:$PATH


Installing Cyclus (Linux, Unix, and Mac OSX)
=============================================

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

**********************
The Developer Workflow
**********************

General Notes
=============

* The terminology we use is based on the `Integrator Workflow
  <http://en.wikipedia.org/wiki/Integrator_workflow>`_

* Use a branching workflow similar to the one described at
  http://progit.org/book/ch3-4.html.

* Keep your own "master" and "develop" branches in sync with the blessed
  repository's "master" and "develop" branches. Specifically, do not push your
  own commits directly to your "master" and "develop" branches.

* Any commit should *pass all tests* (see `Running Tests`_).

* See the `An Example`_ section below for a full walk through

Issuing a Pull Request
======================

* When you are ready to move changes from one of your topic branches into the
  "develop" branch, it must be reviewed and accepted by another developer.

* You may want to review this `tutorial
  <https://help.github.com/articles/using-pull-requests/>`_ before you make a
  pull request to the develop branch.

Reviewing a Pull Request
========================

* Look over the code.

  * Check that it meets `our style guidelines
    <http://fuelcycle.org/kernel/pr_review.html>`_.

  * Make inline review comments concerning improvements.

* Wait for the Continuous Integration service to show full test passage

* Click the green "Merge Pull Request" button

  * Note: if the button is not available, the requester needs to merge or rebase
    from the current HEAD of the blessed's "develop" (or "master") branch

Cautions
========

* **NEVER** merge the "master" branch into the "develop" branch. Changes should
  only flow *to* the "master" branch *from* the "develop" branch.

* **DO NOT** rebase any commits that have been pulled/pushed anywhere else other
  than your own fork (especially if those commits have been integrated into the
  blessed repository.  You should NEVER rebase commits that are a part of the
  'master' branch. *If you do, we will never, ever accept your pull request*.

An Example
==========

Introduction
------------

As this type of workflow can be complicated to converts from SVN and very complicated
for brand new programmers, an example is provided.

For the sake of simplicity, let us assume that we want a single "sandbox" branch
in which we would like to work, i.e. where we can store all of our work that may not
yet pass tests or even compile, but where we also want to save our progress. Let us
call this branch "Work". So, when all is said and done, in our fork there will be
three branches: "Master", "Develop", and "Work".

Acquiring Cyclus and Workflow
-----------------------------

We begin with a fork of the main ("blessed") Cyclus repository. After initially forking
the repo, we will have two branches in our fork: "Master" and "Develop".

Acquiring a Fork of the Cyclus Repository
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A fork is *your* copy of Cyclus. Github offers an excellent
`tutorial <http://help.github.com/fork-a-repo/>`_ on how to set one up. The rest of this
example assumes you have set up the "upstream" repository as cyclus/core. Note that git
refers to your fork as "origin".

First, let's make our "work" branch:
::
    .../cyclus_dir/$ git branch work
    .../cyclus_dir/$ git push origin work

We now have the following situation: there exists the "blessed" copy of the Master and
Develop branches, there exists your fork's copy of the Master, Develop, and Work branches,
*AND* there exists your *local* copy of the Master, Develop, and Work branches. It is
important now to note that you may wish to work from home or the office. If you keep your
fork's branches up to date (i.e., "push" your changes before you leave), only your *local*
copies of your branches may be different when you next sit down at the other location.

Workflow: The Beginning
^^^^^^^^^^^^^^^^^^^^^^^

Now, for the workflow! This is by no means the only way to perform this type of
workflow, but I assume that you wish to handle conflicts as often as possible
(so as to keep their total number small). Let us imagine that you have been at
work, finished, and successfully pushed your changes to your *Origin*
repository. You are now at home and want to continue working a bit. To begin,
let's update our *home's local branches*.  ::

    .../cyclus_dir/$ git checkout develop
    .../cyclus_dir/$ git pull upstream develop
    .../cyclus_dir/$ git push origin develop

    .../cyclus_dir/$ git checkout work
    .../cyclus_dir/$ git pull origin work
    .../cyclus_dir/$ git rebase develop
    .../cyclus_dir/$ git push origin work

Perhaps a little explanation is required. We first want to make sure that this new local copy of
the develop branch is up-to-date with respect to the remote origin's branch and remote upstream's
branch. If there was a change from the remote upstream's branch, we want to push that to origin.
We then follow the same process to update the work branch, except:

#. we don't need to worry about the *upstream* repo because it doesn't have a work branch, and
#. we want to incorporate any changes which may have been introduced in the develop branch update.

Workflow: The End
^^^^^^^^^^^^^^^^^

As time passes, you make some changes to files, and you commit those changes (to your *local work
branch*). Eventually (hopefully) you come to a stopping point where you have finished your project
on your work branch *AND* it compiles *AND* it runs input files correctly *AND* it passes all tests!
Perhaps you have found Nirvana. In any case, you've performed the final commit to your work branch,
so it's time to make a pull request online and wait for our developer friends to
review and accept it.

Sometimes, your pull request will be closed by the reviewer until further
changes are made to appease the reviewer's concerns. This may be frustrating,
but please act rationally, discuss the issues on the GitHub space made for your
pull request, consult the `style guide <http://cyclus.github.com/devdoc/style_guide.html>`_,
email the developer listhost for further advice, and make changes to your topic branch
accordingly. The pull request will be updated with those changes when you push them
to your fork.  When you think your request is ready for another review, you can
reopen the review yourself with the button made available to you.

See also
--------

A good description of a git workflow with good graphics is available at
http://nvie.com/posts/a-successful-git-branching-model/

Releases
========

If you are going through a release of Cyclus and Cycamore, check out the release
procedure notes `here
<https://github.com/cyclus/cyclus/blob/develop/doc/release_procedure.rst>`_ and
on the `website <http://fuelcycle.org/cep/cep3.html>`_.
