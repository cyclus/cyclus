"""The event handling module in cyclus."""
from __future__ import print_function, unicode_literals

cdef public void eventloophook():
    from cyclus.events import loop
    loop()
