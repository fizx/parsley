#!/bin/sh
automake --add-missing
autoreconf
bison -yd