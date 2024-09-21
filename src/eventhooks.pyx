"""The event handling module in cyclus."""
from __future__ import print_function, unicode_literals

cdef public api void eventloophook "CyclusEventLoopHook" () except *:
    from cyclus.events import loop
    loop()


