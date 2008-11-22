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

task :default => "bin/dexter"

file "bin/dexter" => Dir["src/*.c"] + ["src/y.tab.c", "src/scanner.yy.c"] do
  mkdir_p "bin"
  system "cd src && gcc -std=c99 -largp -ljson -lfl -L/opt/local/lib -I/opt/local/include -o ../bin/dexter obstack.c kstring.c y.tab.c scanner.yy.c dexter.c"
end

file "src/y.tab.c" => ["src/parser.y"] do
  system "cd src && yacc -d -t parser.y"
end

file "src/scanner.yy.c" => ["src/y.tab.c", "src/scanner.l"] do
  system "cd src && lex -d -o scanner.yy.c scanner.l"
end