require "rubygems"
require "rakeutil"

task :default => :yelp 

task :yelp do
  system %[time xsltproc --repeat --html -o examples/yelp.output.json examples/yelp.xsl examples/yelp.html]
end