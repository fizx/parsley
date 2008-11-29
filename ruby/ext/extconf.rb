#!/usr/bin/env ruby
ENV["LDFLAGS"] = "#{ENV["LDFLAGS"]} -I/opt/local/include -I/usr/local/include"
ENV["CPPFLAGS"] = "#{ENV["CPPFLAGS"]} -L/opt/local/lib -L/usr/local/lib"
ENV["ARCHFLAGS"] = "-arch #{`uname -p` =~ /powerpc/ ? 'ppc' : 'i386'}"
require "mkmf"

have_header('dexter.h')
have_library('json', 'json_object_new_string')
have_library('dexter', 'dex_compile')
create_makefile("dexterous")
