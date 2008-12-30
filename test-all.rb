#!/usr/bin/env ruby
require "fileutils"

log = File.dirname(__FILE__) + "/test.log"
code = 0
dexter = File.dirname(__FILE__) + "/dexter"

FileUtils.rm_f(log)

Dir[File.dirname(__FILE__) + "/test/*.dex"].each do |f|
  print "testing #{f} ..."
  STDOUT.flush
  begin
    html = f.sub(/dex$/, "html")
    expected = File.read(f.sub(/dex$/, "json")).strip
  
    output = `#{dexter} #{f} #{html}`.strip
  
    if expected == output
      puts "success"
    else
      puts "failed, see test.log"
      code += 1
      File.open(log, "a") do |lf|
        lf.puts "Failure: #{f}"
        lf.puts "=" * 78
        lf.puts "expected: #{expected}"
        lf.puts "got:      #{output}"
        lf.puts
      end
    end
  rescue Exception => e
    code += 1
    puts "exception, see test.log"
    File.open(log, "a") do |lf|
      lf.puts e.message 
      lf.puts e.backtrace.join("\n")
      lf.puts
    end
  end
end

exit code