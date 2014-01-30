###########
Cyclus Core
###########

The core of the Cyclus nuclear fuel cycle simulator from the 
University of Wisconsin - Madison is intended to be a simulation 
framework upon which to develop innovative fuel cycle simulations. 

To see user and developer documentation for this code, please visit 
the `Cyclus Homepage`_.

******************************
Building and Installing Cyclus
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
`libxml++`             2.6
`python`               2.6
`sqlite3`              3.7.10            
`HDF5`                 1.8.0
`Coin-Cbc`             2.5
`Coin-Cgl`             0.58
`Coin-Clp`             1.90
`Coin-Osi`             0.99
====================   ==================

An optional dependency (to build documentation) is:

====================   ==================
Package                Minimum Version   
====================   ==================
doxygen                1.7.6.1
====================   ==================

Windows
=======

A native Windows installation is not currently explicitly supported. However, a 
Virtual Machine image can be downloaded which includes a pre-installation of all 
of these dependencies as well as a pre-installation of Cyclus. If you do not 
have access to a Linux or Unix (MacOS) machine, please follow these three steps:

#. Install `VirtualBox <https://www.virtualbox.org/>`_
#. Download `cyclus_user_environment.ova 
   <http://cnergdata.engr.wisc.edu/cyclus/virtual-box/current/cyclus_user_environment.ova>`_ 
#. Open the .ova file using VirtualBox (File->Import Appliance).

The user name is "cyclus-user" and the password is "cyclus". You now have the 
same user environment as someone who installed cyclus on an Ubuntu Linux 
machine. Congratulations. You may skip to `Running Tests`_ .


Installing Dependencies (Linux and Unix)
========================================

This guide assumes that the user has root access (to issue sudo commands) and
access to a package manager or has some other suitable method of automatically
installing established libraries. This process was tested using a fresh install
of Ubuntu 12.10 using apt-get as the package manager; if on a Mac system, a good
manager to use is macports. In that case, replace all of the following instances 
of "apt-get" with "port".

The command to install a dependency takes the form of:

.. code-block:: bash

  sudo apt-get install package

where "package" is replaced by the correct package name. The minimal list of
required library package names is:

#. cmake
#. libboost-all-dev (see note below)
#. libxml2-dev
#. libxml++2.6-dev
#. libsqlite3-dev
#. libhdf5-serial-dev
#. libbz2-dev
#. coinor-libcbc-dev
#. coinor-libcgl-dev
#. coinor-libclp-dev
#. coinor-libosi-dev

and (optionally):

#. doxygen

For example, in order to install libxml++ (and libxml2) on your system, type:

.. code-block:: bash

  sudo apt-get install libxml++2.6-dev

Boost Note
----------

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

Installing Cyclus
=================

Assuming you have the dependencies installed correctly, installing Cyclus is
fairly straightforward. 

We make the following assumptions in this guide:

#. there is some master directory in which you're placing all
   Cyclus-related files called .../cyclus
#. you have a directory named .../cyclus/install in which you plan
   to install all Cyclus-related files
#. you have acquired the Cyclus source code from the `Cyclus repo`_
#. you have placed the Cyclus repository in .../cyclus/cyclus

Under these assumptions **and** if you used a package manager to 
install coin-Cbc (i.e. it's installed in a standard location), the
Cyclus building and installation process will look like:

.. code-block:: bash

    .../cyclus/cyclus$ python install.py --prefix=../install

If you have installed coin-Cbc from source or otherwise have it 
installed in a non-standard location, you should make use of the 
coinRoot installation flag. The otherwise identical process would look 
like:

.. code-block:: bash

    .../cyclus/cyclus$  python install.py --prefix=../install --coin_root=path/to/coin

Additionally, if you have installed Boost in a non-standard location
you should make use of the boostRoot installation flag.

.. code-block:: bash

    .../cyclus/cyclus$ python install.py --prefix=../install --coin_root=/path/to/coin --boost_root=/path/to/boost

There are additional options which can be inspected via `install.py`'s help:

.. code-block:: bash

    .../cyclus/cyclus$ python install.py -h

Running Tests
=============

Installing Cyclus will also install a test driver (i.e., an executable of all of
our tests). You can run the tests yourself via:

.. code-block:: bash

    ...$ prefix/bin/cyclus_unit_tests

Running Cyclus
==============

You can find instructions for writng an input file for cyclus from `Cyclus User
Guide`_ or use sample input files from `Cycamore Repo`_. Assuming you have some
file `input.xml`, you can run Cyclus via:

.. code-block:: bash

    ...$ prefix/bin/cyclus path/to/input.xml

For a more detailed explanation, checkout the user guide.

.. _`Cyclus Homepage`: http://cyclus.github.com
.. _`CMake`: http://www.cmake.org
.. _`Cyclus repo`: https://github.com/cyclus/cyclus
.. _`Cyclus User Guide`: http://cyclus.github.io/usrdoc/main.html
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
  own commits directly to your "master" and "develop" branches (see `Updating
  Your Repositories`_ below).

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
    <http://cyclus.github.com/devdoc/style_guide.html>`_.

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
  'master' branch. *If you do, you will be flogged publicly*.

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

