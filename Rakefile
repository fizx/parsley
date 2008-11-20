require "rubygems"
require "rakeutil"

task :yelp do
  system %[time xsltproc --repeat --html -o examples/yelp.output.xml examples/yelp.xml.xsl examples/yelp.html]
  system %[time xsltproc --repeat --html -o examples/yelp.output.json examples/yelp.json.xsl examples/yelp.html]
end

task :parser => "src/scanner.yy.c"

file "src/parser.tab.c" => ["src/parser.y"] do
  system "cd src && yacc -d -t parser.y"
end

file "src/scanner.yy.c" => ["src/parser.tab.c", "src/scanner.l"] do
  system "cd src && lex -o scanner.yy.c scanner.l"
end