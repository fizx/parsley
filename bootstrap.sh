#!/bin/sh
libtoolize --force || glibtoolize --force
aclocal
autoconf
automake -a
