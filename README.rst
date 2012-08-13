_______________________________________________________________________
Cyclus Core
_______________________________________________________________________

The core of the Cyclus nuclear fuel cycle simulator from the University of 
Wisconsin - Madison is intended to be a simulation framework upon which to 
develop innovative fuel cycle simulations. 

To see user and developer documentation for this code, please visit the `Cyclus Homepage`_.


-----------------------------------------------------------------------
LISCENSE
-----------------------------------------------------------------------

::

    Copyright (c) 2010-2012, University of Wisconsin Computational Nuclear Engineering Research Group
     All rights reserved.
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    
      - Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.
      
      - Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      
      - Neither the name of the University of Wisconsin Computational
        Nuclear Engineering Research Group nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

------------------------------------------------------------------
Building Cyclus
------------------------------------------------------------------

The `Cyclus Homepage`_ has much more detailed guides and information.
This Readme is intended to be a quick reference for building cyclus for the
first time.

The Cyclus code requires the following software and libraries.

====================   ==================
Package                Minimum Version   
====================   ==================
`CMake`                2.8            
`boost`                1.34.1
`libxml2`              2                 
`sqlite3`              3.7.10            
`Cyclopts`             0.1            
`coin-Cbc`             2.7            
====================   ==================

~~~~~~~~~~~~~~~~~~~~~~~~~~~
Building and Running Cyclus
~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to facilitate future compatibility with multiple platforms, Cyclus is
built using  `Cmake <http://www.cmake.org>`_. This relies on CMake version
2.8 or higher and the CMakeLists.txt file in `src/`. It is
recommended that you use CMake to build the Cyclus executable external to the
source code. To do this, execute the following steps::

    .../core/$ mkdir build
    .../core/$ cd build
    .../core/build$ cmake ../src

You should see output like this::

    ...
    ...
    >> -- Configuring done
    >> -- Generating done
    >> -- Build files have been written to: .../core/build
    /core/build$ make cyclus
    >> Scanning dependencies of target cyclus
    ...
    ...
    >> [100%] Building CXX object CMakeFiles/cyclus.dir/SourceFac.cpp.o
    >> Linking CXX executable cyclus
    >> [100%] Built target cyclus

Now, you can make cyclus, and run it with some input file, for this example, call it `input.xml`::

    .../core/build$ make
    .../core/build$ ./cyclus input.xml

The `Cyclus Homepage`_ has much more detailed guides and information.  If
you intend to develop for *Cyclus*, please visit it to learn more.


.. _`Cyclus Homepage`: http://cyclus.github.com


--------------------------------------------------------------------------
The Developer Workflow
--------------------------------------------------------------------------

*Note that "blessed" repository refers to the primary `cyclus/core` repository.*

As you do your development, push primarily only to your own fork. Make a pull 
request to the blessed repository (usually the "develop" branch) only after:

  * You have pulled the latest changes from the blessed repository.
  * You have completed a logical set of changes.
  * Cyclus compiles with no errors.
  * All tests pass.
  * Cyclus input files run as expected.
  * (recommended) your code has been reviewed by another developer.

Code from the "develop" branch generally must pass even more rigorous checks
before being integrated into the "master" branch. Hotfixes would be a
possible exception to this.

~~~~~~~~~~~~~~~~~~~
Workflow Notes
~~~~~~~~~~~~~~~~~~~

  * Use a branching workflow similar to the one described at
    http://progit.org/book/ch3-4.html.

  * The "develop" branch is how core developers will share (generally compilable) progress
    when we are not yet ready for the code to become 'production'.

  * Keep your own "master" and "develop" branches in sync with the blessed repository's
    "master" and "develop" branches. The master branch should always be the 'stable'
    or 'production' release of cyclus.
    
     - Pull the most recent history from the blessed repository "master"
       and/or "develop" branches before you merge changes into your
       corresponding local branch. Consider doing a rebase pull instead of
       a regular pull or 'fetch and merge'.  For example::

         git checkout develop
         git pull --rebase blessed develop

     - Only merge changes into your "master" or "develop" branch when you
       are ready for those changes to be integrated into the blessed
       repository's corresponding branch. 

  * As you do development on topic branches in your own fork, consider rebasing
    the topic branch onto the "master" and/or "develop"  branches after *pulls* from the blessed
    repository rather than merging the pulled changes into your branch.  This
    will help maintain a more linear (and clean) history.
    *Please see caution about rebasing below*.  For example::

      git checkout [your topic branch]
      git rebase develop

  * **Passing Tests**

      - To check that your branch passes the tests, you must build and install your topic 
        branch and then run the CyclusUnitTestDriver (at the moment, ```make 
        test``` is insufficient). For example ::
      
          mkdir build
          mkdir install
          cd build
          cmake ../src -DCMAKE_INSTALL_PREFIX=../install
          make
          make install
          ../install/cyclus/bin/CyclusUnitTestDriver

      - If your changes to the core repository have an effect on any module 
        repositories (such as `cyamore <https://github.com/cyclus/cycamore/>`_ 
        ), please install those modules and test them appropriately as well.  

  * **Making a Pull Request** 
    
      - When you are ready to move changes from one of your topic branches into the 
        "develop" branch, it must be reviewed and accepted by another 
        developer. 

      - You may want to review this `tutorial <https://help.github.com/articles/using-pull-requests/>`_ 
        before you make a pull request to the develop branch.
        
  * **Reviewing a Pull Request** 

     - Build, install, and test it. If you have added the remmote repository as 
       a remote you can check it out and merge it with the current develop 
       branch thusly, ::
       
         git checkout -b remote_name/branch_name
         git merge develop

     - Look over the code. 

        - Check that it meets `our style guidelines <http://cyclus.github.com/devdoc/style_guide.html>`_.

        - Make inline review comments concerning improvements. 
      
     - Accept the Pull Request    

        - In general, **every commit** (notice this is not 'every push') to the
          "develop" and "master" branches should compile and pass tests. This
          is guaranteed by using a NON-fast-forward merge during the pull request 
          acceptance process. 
    
        - The green "Merge Pull Request" button does a non-fast-forward merge by 
          default. However, if that button is unavailable, you've made minor 
          local changes to the pulled branch, or you just want to do it from the 
          command line, make sure your merge is a non-fast-forward merge. For example::
          
            git checkout develop
            git merge --no-ff remote_name/branch_name -m "A message""


~~~~~~~~~~~~~~~~~~~
Cautions
~~~~~~~~~~~~~~~~~~~

  * **NEVER** merge the "master" branch into the "develop"
    branch. Changes should only flow *to* the "master" branch *from* the
    "develop" branch.

  * **DO NOT** rebase any commits that have been pulled/pushed anywhere
    else other than your own fork (especially if those commits have been
    integrated into the blessed repository.  You should NEVER rebase
    commits that are a part of the 'master' branch.  *If you do, you will be
    flogged publicly*.

  * Make sure that you are pushing/pulling from/to the right branches.
    When in doubt, use the following syntax::

      git push [remote] [from-branch]:[to-branch]

    and (*note that pull always merges into the current checked out branch*)::

      git pull [remote] [from-branch]


~~~~~~~~~~~~~~~~~~~
An Example
~~~~~~~~~~~~~~~~~~~


Introduction
============

As this type of workflow can be complicated to converts from SVN and very complicated
for brand new programmers, an example is provided.

For the sake of simplicity, let us assume that we want a single "sandbox" branch
in which we would like to work, i.e. where we can store all of our work that may not
yet pass tests or even compile, but where we also want to save our progress. Let us 
call this branch "Work". So, when all is said and done, in our fork there will be 
three branches: "Master", "Develop", and "Work".

Acquiring Cyclus and Workflow
=============================

We begin with a fork of the main ("blessed") Cyclus repository. After initially forking
the repo, we will have two branches in our fork: "Master" and "Develop".

Acquiring a Fork of the Cyclus Repository
-----------------------------------------

A fork is *your* copy of Cyclus. Github offers an excelent 
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
-----------------------

Now, for the workflow! This is by no means the only way to perform this type of workflow, 
but I assume that you wish to handle conflicts as often as possible (so as to keep their total 
number small). Let us imagine that you have been at work, finished, and successfully pushed 
your changes to your *Origin* repository. You are now at home, perhaps after dinner (let's just 
say some time has passed), and want to continue working a bit (you're industrious, I suppose... 
or a grad student). To begin, let's update our *home's local branches*.
::

    .../cyclus_dir/$ git checkout develop
    .../cyclus_dir/$ git pull origin develop 
    .../cyclus_dir/$ git pull upstream develop
    .../cyclus_dir/$ git push origin develop

    .../cyclus_dir/$ git checkout work
    .../cyclus_dir/$ git pull origin work
    .../cyclus_dir/$ git merge develop
    .../cyclus_dir/$ git push origin work

Perhaps a little explanation is required. We first want to make sure that this new local copy of 
the develop branch is up-to-date with respect to the remote origin's branch and remote upstream's
branch. If there was a change from the remote upstream's branch, we want to push that to origin. 
We then follow the same process to update the work branch, except:

#. we don't need to worry about the *upstream* repo because it doesn't have a work branch, and
#. we want to incorporate any changes which may have been introduced in the develop branch update.

Workflow: The End
-----------------

As time passes, you make some changes to files, and you commit those changes (to your *local work
branch*). Eventually (hopefully) you come to a stopping point where you have finished your project 
on your work branch *AND* it compiles *AND* it runs input files correctly *AND* it passes all tests!
Perhaps you have found Nirvana. In any case, you've performed the final commit to your work branch,
so it's time to make a pull request online and wait for our developer friends to 
review and accept it.

Sometimes, your pull request will be closed by the reviewer until further 
changes are made to appease the reviewer's concerns. This may be frustrating, 
but please act rationally, discuss the issues on the github space made for your 
pull request, consult the `style guide <http://cyclus.github.com/devdoc/style_guide.html>`_, 
email the developer listhost for further advice, and make changes to your topic branch 
accordingly. The pull request will be updated with those changes when you push them 
to your fork.  When you think your request is ready for another review, you can 
reopen the review yourself with the button made available to you. 

See also
--------

A good description of a git workflow with good graphics is available at
http://nvie.com/posts/a-successful-git-branching-model/

