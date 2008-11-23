require "rubygems"
require "rakeutil"

task :test => "bin/dexter-path-parser" do
  
end

task :clean do
  File.open(".gitignore").each do |line|
    rm_rf Dir[line.strip]
  end
end

task :default => "bin/dexter-path-parser"

file "bin/dexter-path-parser" => %w[bin/dexter src/dexter-path-parser.c] do
  system "cd src && gcc -std=c99 -lfl -L/opt/local/lib -I/opt/local/include -o ../bin/dexter-path-parser obstack.c kstring.c y.tab.c scanner.yy.c dexter-path-parser.c"
end

file "bin/dexter" => %w[src/y.tab.c src/scanner.yy.c src/dexter.c src/dexter.c] do
  mkdir_p "bin"
  system "cd src && gcc -std=c99 -largp -ljson -lfl -L/opt/local/lib -I/opt/local/include -o ../bin/dexter obstack.c kstring.c y.tab.c scanner.yy.c dexter.c"
end

file "src/y.tab.c" => ["src/parser.y"] do
  system "cd src && yacc -d -t parser.y"
end

file "src/scanner.yy.c" => ["src/y.tab.c", "src/scanner.l"] do
  system "cd src && lex -o scanner.yy.c scanner.l"
end