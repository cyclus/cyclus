
**********************
The Developer Workflow
**********************

General Notes
=============

* We expect contributors to use a forking workflow `as described here
  <https://www.atlassian.com/git/tutorials/comparing-workflows/forking-workflow>`_.

* Keep your own "main" branch in sync with the mainline
  repository's "main" branch. Specifically, do not push your
  own commits directly to your "main" branch.

* Any pull request should *pass all tests* (see `Running Tests`_).

* See the `An Example`_ section below for a full walk through

* In addition to a review of the algorithmic and logical changes in your
  contribution, it will be reviewed on a variety of levels including such
  things as style, documentation, tests, etc.  While such review can appear
  tedious, these aspects are important for the long term success of the
  project.

Issuing a Pull Request
======================

* Please make sure you describe the changes you made to the code in the 
  `CHANGELOG <CHANGELOG.rst>`_.

* When you are ready to move changes from one of your topic branches into the
  "main" branch, it must be reviewed and accepted by another developer.

* You may want to review this `tutorial
  <https://help.github.com/articles/using-pull-requests/>`_ before you make a
  pull request to the main branch.

Reviewing a Pull Request
========================

* Look over the code.

  * Check that it meets `our style guidelines
    <http://fuelcycle.org/kernel/pr_review.html>`_.

  * Make inline review comments concerning improvements.

* Wait for the Continuous Integration service to show full test passage

* Click the green "Merge Pull Request" button

  * Note: if the button is not available, the requester needs to merge or rebase
    from the current HEAD of the mainline "main" branch

Running Tests
=============

You can run the tests yourself using:
  - for Cyclus:

    .. code-block:: console

      $ cyclus_unit_tests

  - for Cycamore:

    .. code-block:: console

      $ cycamore_unit_tests

Cautions
========

* **DO NOT** rebase any commits that have been pulled/pushed anywhere else other
  than your own fork (especially if those commits have been integrated into the
  blessed repository).  You should NEVER rebase commits that are a part of the
  'main' branch. *If you do, we will never, ever accept your pull request*.

An Example
==========

Introduction
------------

As this type of workflow can be complicated, an example is provided.

For the sake of simplicity, let us assume that we want a single "sandbox" branch
in which we would like to work, i.e. where we can store all of our work that may not
yet pass tests or even compile, but where we also want to save our progress. Let us
call this branch ``work``. So, when all is said and done, in our fork there will be
two branches: ``main`` and ``work``

Acquiring Cyclus and Workflow
-----------------------------

We begin with a fork of the mainline Cyclus repository. After initially forking
the repo, we will have the ``main`` branch in your fork.

Acquiring a Fork of the Cyclus Repository
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A fork is *your* copy of Cyclus. Github offers an excellent `tutorial
<https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/fork-a-repo>`_
on how to create a fork and then setup your local development evnironment with
remote connections to both the ``upstream`` repository at ``cyclus/cyclus`` and
your fork of this repository referred to as ``origin``.

First, let's make our ``work`` branch.  Assuming that you are in the ``cyclus`` directory:
::
    $ git branch work
    $ git push origin work

We now have the following situation: there exists the mainline copy of the ``main``
branch, there exists your fork's copy of the ``main`` and ``work`` branches,
*AND* there exists your *local* copy of the ``main`` and ``work`` branches. 

Workflow
^^^^^^^^

Now, for the workflow! This is by no means the only way to perform this type of
workflow, but we assume that you wish to handle conflicts as often as possible
(so as to keep their total number small). 

As time passes, you make some changes to files, and you commit those changes (to
your *local ``work`` branch*). Eventually (hopefully) you come to a stopping
point where you have finished your project on your ``work`` branch *AND* it
compiles *AND* it runs input files correctly *AND* it passes all tests! Perhaps
you have found Nirvana. 

Over this time, it is possible that the ``main`` branch into which you are
proposing your pull request has advanced with other changes. In order to make
sure your ``work`` branch remains up to date, you will want to periodically
rebase your ``work`` branch onto the ``upstream/main`` branch.  This will
process will reapply all of the changes you have made on top of the most
up-to-date version of the ``upstream/main`` branch.  Even if you have not been
doing this regularly, you'll want to do it before you initiate a pull request.
::

  $ git checkout main
  $ git pull upstream main
  $ git checkout work
  $ git rebase main
  $ git push origin work

Note: you may need to force the push of the rebased ``work`` branch to your fork.

In any case, you've performed the final commit to your ``work`` branch, so it's
time to make a pull request online and wait for our main friends to review and
accept it.  

Sometimes, your pull request will be held by the reviewer until
further changes are made to appease the reviewer's concerns. This may be
frustrating, but please act rationally, discuss the issues on the GitHub space
made for your pull request, consult the `style guide
<http://cyclus.github.com/devdoc/style_guide.html>`_, reach out on `slack
<https://cyclus-nuclear.slack.com>`_ for further advice, and make changes to
your ``work`` branch accordingly. The pull request will be updated with those
changes when you push them to your fork. When you think your request is ready
for another review, you can reopen the review yourself with the button made
available to you.

Synchronizing across multiple computers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
You may wish to work on different computers over time, sometime on your laptop
and other times on a desktop at the office. If you keep your fork's branches up
to date (i.e., "push" your changes before you leave), only your *local* copies
of your branches may be different when you next sit down at the other location.

Let us imagine that you have been at the office, finished, and successfully
pushed your changes to your ``origin`` repository. You are now at home and want
to continue working a bit on your laptop. To begin, let's update our *laptop's
local branches*
::
  $ git checkout work
  $ git pull origin work

This may also be a good time to ensure your ``work`` branch is up-to-date with the 
``upstream/main`` branch
::

  $ git chekout main
  $ git pull upstream main
  $ git checkout work
  $ git rebase main
  $ git push origin work


Releases
========

If you are going through a release of Cyclus and Cycamore, check out the release
procedure notes `here
<https://github.com/cyclus/cyclus/blob/main/doc/release_procedure.rst>`_ and
on the `website <http://fuelcycle.org/cep/cep3.html>`_.
