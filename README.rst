_______________________________________________________________________
Cyclus Core
_______________________________________________________________________

**Last Updated: 2.28.2012**

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
`libxml2`              2                 
`boost`                1.34.1            
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

As you do your development, push primarily only to your own fork. Push to
the blessed repository (usually the "develop" branch) only after:

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

  * In general, **every commit** (notice this is not 'every push') to the
    "develop" and "master" branches should compile and pass tests. This
    means that when you are ready to move changes from one of your topic
    branches into the "develop" branch, you should use a NON-fast-forward
    merge.  For example::
    
      git checkout develop
      git merge --no-ff [your topic branch]
    
    Possible exceptions to this 'no fast-forward' merge
    include:

     - your topic branch consists of only one (compileable and passes
       tests) commit.

     - every commit in your topic branch is compileable and passes tests.


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


Executive Summary
=================

This example assumes you have just finished working at computer1 and will move to 
computer2 at some later time. Additionally, you have correctly updated your origin's 
branches at the time of leaving.

Workflow: Beginning
-------------------

Assuming you have just sat down at computer2, the following commands will fully update 
your local branches: ::
    .../cyclus_dir/$ git checkout develop
    .../cyclus_dir/$ git pull origin develop 
    .../cyclus_dir/$ git pull upstream develop
    .../cyclus_dir/$ git push origin develop
    .../cyclus_dir/$ git checkout work
    .../cyclus_dir/$ git pull origin work
    .../cyclus_dir/$ git rebase develop
    .../cyclus_dir/$ git push origin work

Workflow: End
-------------

Assuming you have commited some changes to the local work branch, finishing your project
(i.e., your work branch *compiles*, *runs input files*, and *passes all tests*), you 
will want to update your local develop branch and remote (origin) work and develop branches.
The following commands will perform those actions: ::
    .../cyclus_dir/$ git checkout develop
    .../cyclus_dir/$ git pull upstream develop
    .../cyclus_dir/$ git merge --no-ff work 
    .../cyclus_dir/$ git push origin develop
    .../cyclus_dir/$ git checkout work
    .../cyclus_dir/$ git rebase develop
    .../cyclus_dir/$ git push origin work


The Gory Details
================

We begin with a fork of the main ("blessed") Cyclus repository. After initially forking
the repo, we will have two branches in our fork: "Master" and "Develop".

Acquiring a Fork of the Cyclus Repository
-----------------------------------------

But hark! One may ask: What's a fork? How do I set up my fork?

Lo, an easy solution exists. A fork is *your* copy of Cyclus. Github offers an excelent 
`tutorial <http://help.github.com/fork-a-repo/>`_ on how to set one up. The rest of this
example assumes you have set up the "upstream" repository as cyclus/core. Note that git
refers to your fork as "origin".

We now have a copy of Cyclus in our fork. Let us create that "Work" branch:
    .../cyclus_dir/$ git branch work
    .../cyclus_dir/$ git push origin work

We now have the following situation: there exists the "blessed" copy of the Master and
Develop branches, there exists your fork's copy of the Master, Develop, and Work branches,
*AND* there exists your *local* copy of the Master, Develop, and Work branches. It is 
important now to note that you may wish to work from home or the office. If you keep your 
fork's branches up to date (i.e., "push" your changes before you leave), only your *local*
copies of your branches may be different when you next sit down at the other location.

Rebasing
--------

It is important now to discuss rebasing. The reason why rebasing exists is because it 
allows developers to keep a clean flow of commits. In short, rebasing allows you to 
perform all of your changes (that you have committed on your local branch copies) 
on the *most recent version* of a remote (origin or upstream) branch copy, *regardless* 
of when it was initially pulled. In other words, let us say you pull from Origin's 
Develop branch, commit some changes on your local branch, and then want to push those 
changes back. Instead of simply pushing those changes, you should pull with the --rebase 
tag. That will suspend all of your commits, apply any intermitent changes that have occured, 
and replace all of your commits *on top* of the new *HEAD* of your local branch (in git 
terminology). You will have to merge any conflicts, but you would have had to do that anyway 
if you decided to simply push originally.

Workflow: The Beginning
-----------------------

Now, for the workflow! This is by no means the only way to perform this type of workflow, 
but I assume that you wish to handle conflicts as often as possible (so as to keep their total 
number small). Let us imagine that you have been at work, finished, and successfully pushed 
your changes to your *Origin* directory. You are now at home, perhaps after dinner (let's just 
say some time has passed), and want to continue working a bit (you're industrious, I suppose... 
or a grad student). To begin, let us update our *home's local branches*: ::
    .../cyclus_dir/$ git checkout develop
    .../cyclus_dir/$ git pull origin develop 
    .../cyclus_dir/$ git pull upstream develop
    .../cyclus_dir/$ git push origin develop
    .../cyclus_dir/$ git checkout work
    .../cyclus_dir/$ git pull origin work
    .../cyclus_dir/$ git rebase develop
    .../cyclus_dir/$ git push origin work

Perhaps a little explanation is required. We first update the develop branch. Think of this process 
like adding train cars on to a train. We always want the front (the engine) to be the upstream or
"blessed" branch. When you sit down at your local copy, it is the only train car and is the "engine"
by default. We rebase on the origin branch, making it the "engine" and our local branch the "caboose",
making our train look like "origin - local". We then rebase on the upstream branch, making our train 
look like "upstream - origin - local". At this point, our *local develop branch* is fully up-to-date, 
so we push it to origin.

We then want to update our *local work branch*, knowing that the local develop branch is fully 
up-to-date. First we switch over to work, so our train looks like "workLocal". Rebasing on the origin
branch makes our train look like "workOrigin - workLocal". Finally we rebase on the local develop branch,
because it is fully up-to-date, making our train look like "developLocal - workOrigin - workLocal."  At 
this point, our *local develop branch* is fully up-to-date, so we push it to origin.

Workflow: The End
-----------------

As time passes, you make some changes to files, and you commit those changes (to your *local work
branch*). Eventually (hopefully) you come to a stopping point where you have finished your project 
on your work branch *AND* it compiles *AND* it runs input files correctly *AND* it passes all tests!
Perhaps you have found Nirvana. In any case, you've performed the final commit to your work branch,
so it's time to merge those changes with the local develop branch and push them to origin's develop
branch: ::
    .../cyclus_dir/$ git checkout develop
    .../cyclus_dir/$ git pull upstream develop
    .../cyclus_dir/$ git merge --no-ff work 
    .../cyclus_dir/$ git push origin develop
    .../cyclus_dir/$ git checkout work
    .../cyclus_dir/$ git rebase develop
    .../cyclus_dir/$ git push origin work

Here again we checkout the develop branch and rebase on the upstream branch, making our develop train 
look like "upstream - local". We then merge the working branch using the no-fast-foward flag. Much like 
rebasing preserves the lineage of commits when pulling, no-fast-forwarding preserves the lineage of 
commits when merging. Additionally, in our train analogy, whereas pulling with rebase adds a car to 
the front of the line, merging with the --no-ff flag adds a car to the back of the line. Accordingly,
our develop branch's train now looks like "upstream - local - workLocal", exactly what we want it to look
like! So, we push those changes back to our origin repository, and clean up by re-updating work (because 
we might have gotten some changes with that upstream pull). Done and done!
