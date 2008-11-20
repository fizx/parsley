require "rubygems"
require "rakeutil"

task :default => :yelp 

task :yelp do
  system %[time xsltproc --repeat --html -o examples/yelp.output.xml examples/yelp.xml.xsl examples/yelp.html]
end