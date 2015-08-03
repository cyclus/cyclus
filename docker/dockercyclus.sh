#!/bin/bash

docker run --rm -w /data -v $PWD:/data cyclus/cycamore cyclus $@

