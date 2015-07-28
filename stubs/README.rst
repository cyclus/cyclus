.. _hello_world:

Hello, Cyclus!
==============
This pages walks you through a very simple hello world example using
|cyclus| agents.  First make sure that you have the dependencies installed,
namely |Cyclus|, CMake, and a recent version of Python (2.7 or 3.3+).

First, you need to get the ``cycstub`` code.  Cycstub is a skeleton code base
that you can use to quick-start new |cyclus| module development projects.
You can grab cycstub either by using git to
`clone the repository <https://github.com/cyclus/cycstub.git>`_ or by
`downloading the zip file <https://github.com/cyclus/cycstub/archive/develop.zip>`_.
Let's put this code in a ``tutorial`` directory and go into it.

**Getting cycstub via git:**

.. code-block:: bash

    $ git clone https://github.com/cyclus/cycstub.git tutorial
    $ cd tutorial

**Getting cycstub via zip:**

.. code-block:: bash

    $ curl -L https://api.github.com/repos/cyclus/cycstub/zipball > tutorial.zip
    $ unzip tutorial.zip
    $ mv cyclus-cycstub-* tutorial
    $ cd tutorial

------------

Since cycstub is a template project everything is named ``stub``. We need to
change this to reflect the name we want our new project to be called -
``tutorial`` here.  Cycstub comes with a renaming tool to do just this! From
the command line, run Python in the following way:

.. code-block:: bash

    tutorial $ python rename.py tutorial

------------

Let's now change the behavior of the TutorialFacility's ``Tick()`` &
``Tock()`` member functions to print "Hello" and "World" respectively.  To do
this, please open up the :file:`src/tutorial_facility.cc` file in your
favorite text editor (vim, emacs, gedit, `notepad++ <http://exofrills.org>`_).
Change the original functions to look like:

**Original Tick() and Tock() in src/tutorial_facility.cc:**

.. code-block:: c++

    void TutorialFacility::Tick() {}

    void TutorialFacility::Tock() {}

**New Tick() and Tock() in src/tutorial_facility.cc:**

.. code-block:: c++

    void TutorialFacility::Tick() {std::cout << "Hello, ";}

    void TutorialFacility::Tock() {std::cout << "World!\n";}

------------

Now that we have altered the behavior of the TutorialFacility, let's compile and
install the ``tutorial`` project.  This done with the install.py script.
The install script puts the project into your cyclus userspace,
``${HOME}/.local/lib/cyclus``.

.. code-block:: bash

    tutorial $ python install.py

------------

Let's run |cyclus| with the TutorialFacility! In the input directory there is
an :file:`example.xml`. Running |cyclus| on this file with the command
``cyclus input/example.xml`` should produce the following output.

.. code-block:: bash

    tutorial $ cyclus input/example.xml
                  :
              .CL:CC CC             _Q     _Q  _Q_Q    _Q    _Q              _Q
            CC;CCCCCCCC:C;         /_\)   /_\)/_/\\)  /_\)  /_\)            /_\)
            CCCCCCCCCCCCCl       __O|/O___O|/O_OO|/O__O|/O__O|/O____________O|/O__
         CCCCCCf     iCCCLCC     /////////////////////////////////////////////////
         iCCCt  ;;;;;.  CCCC
        CCCC  ;;;;;;;;;. CClL.                          c
       CCCC ,;;       ;;: CCCC  ;                   : CCCCi
        CCC ;;         ;;  CC   ;;:                CCC`   `C;
      lCCC ;;              CCCC  ;;;:             :CC .;;. C;   ;    :   ;  :;;
      CCCC ;.              CCCC    ;;;,           CC ;    ; Ci  ;    :   ;  :  ;
       iCC :;               CC       ;;;,        ;C ;       CC  ;    :   ; .
      CCCi ;;               CCC        ;;;.      .C ;       tf  ;    :   ;  ;.
      CCC  ;;               CCC          ;;;;;;; fC :       lC  ;    :   ;    ;:
       iCf ;;               CC         :;;:      tC ;       CC  ;    :   ;     ;
      fCCC :;              LCCf      ;;;:         LC :.  ,: C   ;    ;   ; ;   ;
      CCCC  ;;             CCCC    ;;;:           CCi `;;` CC.  ;;;; :;.;.  ; ,;
        CCl ;;             CC    ;;;;              CCC    CCL
       tCCC  ;;        ;; CCCL  ;;;                  tCCCCC.
        CCCC  ;;     :;; CCCCf  ;                     ,L
         lCCC   ;;;;;;  CCCL
         CCCCCC  :;;  fCCCCC
          . CCCC     CCCC .
           .CCCCCCCCCCCCCi
              iCCCCCLCf
               .  C. ,
                  :
    Hello, World!
    Hello, World!
    Hello, World!
    Hello, World!
    Hello, World!
    Hello, World!
    Hello, World!
    Hello, World!
    Hello, World!
    Hello, World!

    Status: Cyclus run successful!
    Output location: cyclus.sqlite
    Simulation ID: 0ae730e0-a9a8-4576-afaa-d1db6399d5a2

If you look in the input file you'll see that the simulation duration was set
to 10.  This is why "Hello, World!" is printed ten times.
