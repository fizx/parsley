require "rubygems"
require "rakeutil"

task :yelp do
  system %[time xsltproc --repeat --html -o examples/yelp.output.xml examples/yelp.xml.xsl examples/yelp.html]
end

task :clean do
  File.open(".gitignore").each do |line|
    rm_rf Dir[line.strip]
  end
end

task :default => :parser

task :parser => ["bin/parser", "bin/scanner"]

file "bin/parser" => Dir["src/*.c"] + ["src/y.tab.c", "src/scanner.yy.c"] do
  mkdir_p "bin"
  system "cd src && gcc -std=c99 -lfl -o ../bin/parser kstring.c y.tab.c scanner.yy.c parser_main.c"
end

file "bin/scanner" => Dir["src/*.c"] + ["src/y.tab.c", "src/scanner.yy.c"] do
  mkdir_p "bin"
  system "cd src && gcc -std=c99 -lfl -o ../bin/scanner kstring.c y.tab.c scanner.yy.c scanner_main.c"
end

file "src/y.tab.c" => ["src/parser.y"] do
  system "cd src && yacc -d -t parser.y"
end

file "src/scanner.yy.c" => ["src/y.tab.c", "src/scanner.l"] do
  system "cd src && lex -d -o scanner.yy.c scanner.l"
end