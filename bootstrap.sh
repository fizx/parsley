#!/bin/sh
libtoolize || glibtoolize
automake --add-missing
autoreconf
bison -yd parser.y