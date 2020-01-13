This repository holds scripts to analyze Cyclus' output and to convert it into
nuclear fuel cycle metrics.

Installing Cymetric
----------------

Cymetric is available on the same platforms as Cyclus: Ubuntu and Max OS.

Dependencies
~~~~~~~~~~~~
First, please check to make sure you have all of the dependencies.

Required dependencies:

* Cyclus and its dependencies
* `Jinja2 <http://jinja.pocoo.org/docs/dev/>`_
* `pandas <http://pandas.pydata.org/>`_
* `NumPy <http://www.numpy.org/>`_
* `matplotlib <http://matplotlib.org/index.html>`_ and matplotlib.pyplot

Optional dependencies:

* `PyNE`_ and its dependencies

PyNE is what the Cyclus project uses for its nuclear data, and many metrics
in cymetric depend on it.

Building from Source
~~~~~~~~~~~~~~~~~~~~

Cymetric can be built from source by first downloading the code by cloning the
GitHub repository:

.. code-block:: bash

    $ git clone https://github.com/cyclus/cymetric

Then build and install to the same location Cyclus is installed:

.. code-block:: bash

    $ cd cymetric
    $ python setup.py install --prefix $(cyclus --install-path)

Next, run the tests to ensure everything is working properly:

.. code-block:: bash

    $ nosetests -w tests/

Installation via Binary
~~~~~~~~~~~~~~~~~~~~~~~

The latest released version of Cymetric is available via Conda. After obtaining
miniconda by following steps 1 and 2 `here`_, Cymetric can be installed by the
following command:

.. code-block:: bash

    $ conda install cymetric --yes

Using Cymetric
--------------

Tutorial
~~~~~~~~

The tutorial and the API doucmentation can be found on `fuelcycle.org`_ in the
`User Guide`_.

Use-case Examples
~~~~~~~~~~~~~~~~~

See the examples directory within the main cymetric directory to review an
example of how an analysis may be carried out using cymetric's capabilities.



.. _`PyNE`: http://github.com/pyne/pyne
.. _`fuelcycle.org`: http://fuelcycle.org
.. _`User Guide`: http://fuelcycle.org/user/index.html
.. _`here`: http://fuelcycle.org/user/install.html
