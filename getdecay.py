#! /usr/bin/env python
from __future__ import print_function, unicode_literals
import os
import sys
import io
import shutil
import tarfile
import argparse
if sys.version_info[0] < 3:
    from urllib import urlopen
else:
    from urllib.request import urlopen

DECAY_H = os.path.join('src', 'pyne_decay.h')
DECAY_CPP = os.path.join('src', 'pyne_decay.cc')
DECAY_H_REP = os.path.join('src', '_pyne_decay.h')
DECAY_CPP_REP = os.path.join('src', '_pyne_decay.cc')
DECAY_URL = 'http://data.pyne.io/decay.tar.gz'

def ensure_include():
    with io.open(DECAY_CPP, 'r') as f:
        cc = f.read()
    if cc.startswith('#include "pyne.h"'):
        return
    incs = ('#include "pyne.h"\n'
            '#include "pyne_decay.h"\n')
    with io.open(DECAY_CPP, 'w') as f:
        f.write(incs)
        f.write(cc)

def download():
    print('Downloading ' + DECAY_URL)
    try:
        durl = urlopen(DECAY_URL)
        d = durl.read()
        durl.close()
    except IOError:
        print('...failed!')
        return False
    f = io.BytesIO(d)
    tar = tarfile.open(fileobj=f, mode='r:gz')
    tar.extractall()
    tar.close()
    durl.close()
    shutil.move('decay.h', DECAY_H)
    shutil.move('decay.cpp', DECAY_CPP)
    return True


def ensure_decay():
    mb = 1024**2
    if os.path.isfile(DECAY_H) and os.path.isfile(DECAY_CPP) and \
       os.stat(DECAY_CPP).st_size > mb:
        return
    downloaded = download()
    if downloaded:
        ensure_include()
        return
    print('!'*42)
    print('Decay files could not be downloaded or generated, using surrogates instead.')
    print('!'*42 + '\n')
    shutil.copy(DECAY_H_REP, DECAY_H)
    shutil.copy(DECAY_CPP_REP, DECAY_CPP)
    ensure_include()


if __name__ == '__main__':
    desc = 'Downloads pre-generated decay source code for Cyclus.'
    parser = argparse.ArgumentParser(description=desc)
    desc = 'Root directory for Cyclus project code.'
    parser.add_argument('--root', help=desc, default='.')
    args = parser.parse_args()

    os.chdir(args.root)
    ensure_decay()
