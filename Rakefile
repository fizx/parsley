require "rubygems"
require "rakeutil"

def relpath(path)
  File.expand_path(path).sub(File.expand_path(".") + "/", '')
end

def gcc(out, *srcs)
  srcs.flatten!
  file(relpath(SRC + out) => srcs.map{|s| relpath(SRC + s) }) do
    mkdir_p(File.dirname(relpath(SRC + out)))
    system "cd #{SRC} && gcc #{CFLAGS} -o #{out} #{srcs.join(" ")}"
  end
end

def gcc_main(name, *srcs)
  gcc "../bin/#{name}", srcs + ["#{name}_main.c"]
end

task :clean do
  File.open(".gitignore").each do |line|
    rm_rf Dir[line.strip]
  end
end

# ---------------------------------------------------------------------------------

const :CFLAGS, "-std=c99 -lfl -L/opt/local/lib -I/opt/local/include -ljson -largp"
const :SRC, "src/"
const :SRCS, %w[obstack.c printbuf.c kstring.c y.tab.c scanner.yy.c dexter.c]

task :default => "bin/dexterc"

gcc_main "dexterc", SRCS

file "src/y.tab.c" => ["src/parser.y"] do
  system "cd src && yacc -d -t parser.y"
end

file "src/scanner.yy.c" => ["src/y.tab.c", "src/scanner.l"] do
  system "cd src && lex -o scanner.yy.c scanner.l"
end