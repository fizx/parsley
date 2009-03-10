#!/bin/sh
libtoolize || glibtoolize
aclocal
autoconf
automake -a
bison -yd parser.y
