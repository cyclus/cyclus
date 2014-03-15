#!/usr/bin/env python
"""The cyclus preprocessor.

cycpp is a 3-pass preprocessor which adds reflection-like semantics to cyclus 
agents. This is needed to provide a high-level, user-facing API to cyclus.
Code that uses cycpp is entirely valid C++ code and will compile normally even
without first running it through the cycpp. This is because cycpp relies on 
custom #pragmas to annotate code. These pragmas are skipped - by definition - 
by the C preprocessor and the C/C++ compiler.

The three passes of cycpp are:

1. run cpp normally to canonize all other preprocessor directives,
2. accumulate annotations for agents and state variables, and
3. generate code based on annotations.  

cycpp is implemented entirely in this file and with tools from the Python standard
library. It requires Python 2.7+ or Python 3.3+ to run.
"""


def main():
    pass

if __name__ == "__main__":
    main()

