
**********************
The Developer Workflow
**********************

General Notes
=============

* The terminology we use is based on the ``Integrator Workflow
  <http://en.wikipedia.org/wiki/Integrator_workflow>``_

* Use a branching workflow similar to the one described at
  http://progit.org/book/ch3-4.html.

* Keep your own "master" and "develop" branches in sync with the blessed
  repository's "master" and "develop" branches. Specifically, do not push your
  own commits directly to your "master" and "develop" branches.

* Any commit should *pass all tests* (see ``Running Tests``_).

* See the ``An Example``_ section below for a full walk through

* In addition to a review of the algorithmic and logical changes in your
  contribution, it will be reviewed on a variety of levels including such
  things as style, documentation, tests, etc.  While such review can appear
  tedious, these aspects are important for the long term success of the
  project.

Issuing a Pull Request
======================

* When you are ready to move changes from one of your topic branches into the
  "develop" branch, it must be reviewed and accepted by another developer.

* You may want to review this ``tutorial
  <https://help.github.com/articles/using-pull-requests/>``_ before you make a
  pull request to the develop branch.

Reviewing a Pull Request
========================

* Look over the code.

  * Check that it meets ``our style guidelines
    <http://fuelcycle.org/kernel/pr_review.html>``_.

  * Make inline review comments concerning improvements.

* Wait for the Continuous Integration service to show full test passage

* Click the green "Merge Pull Request" button

  * Note: if the button is not available, the requester needs to merge or rebase
    from the current HEAD of the blessed's "develop" (or "master") branch

Running Tests
=============

You can run the tests yourself using:
  - for Cyclus:

    .. code-block:: bash

      $ cyclus_unit_tests

  - for Cycamore:
    
    .. code-block:: bash

      $ cycamore_unit_tests

Cautions
========

* **NEVER** merge the "master" branch into the "develop" branch. Changes should
  only flow *to* the "master" branch *from* the "develop" branch.

* **DO NOT** rebase any commits that have been pulled/pushed anywhere else other
  than your own fork (especially if those commits have been integrated into the
  blessed repository).  You should NEVER rebase commits that are a part of the
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
``tutorial <http://help.github.com/fork-a-repo/>``_ on how to set one up. The rest of this
example assumes you have set up the "upstream" repository as ``cyclus/core``. Note that git
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

Sometimes, your pull request will be held by the reviewer until further changes
are made to appease the reviewer's concerns. This may be frustrating, but please
act rationally, discuss the issues on the GitHub space made for your pull
request, consult the ``style guide
<http://cyclus.github.com/devdoc/style_guide.html>``_, email the developer
listhost for further advice, and make changes to your topic branch accordingly.
The pull request will be updated with those changes when you push them to your
fork.  When you think your request is ready for another review, you can reopen
the review yourself with the button made available to you.

See also
--------

A good description of a git workflow with good graphics is available at
http://nvie.com/posts/a-successful-git-branching-model/

Releases
========

If you are going through a release of Cyclus and Cycamore, check out the release
procedure notes ``here
<https://github.com/cyclus/cyclus/blob/develop/doc/release_procedure.rst>``_ and
on the ``website <http://fuelcycle.org/cep/cep3.html>``_.
