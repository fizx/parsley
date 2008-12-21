#!/usr/bin/env ruby 
require "test/unit"

class Tests < Test::Unit::TestCase
  def self.dex_test(dex, input, output)
    define_method "test_#{[dex,input,output].join(' ').gsub(/\W/, '_')}" do
      root = File.dirname(__FILE__) 
      fixtures = "#{root}/fixtures"
      assert_equal `./dexter #{fixtures}/#{dex} #{fixtures}/#{input}`, File.read("#{fixtures}/#{output}")
    end
  end
  
  dex_test "li.dex", "yelp.html", "li.yelp.json"
  dex_test "obj.dex", "yelp.html", "obj.yelp.json"
  dex_test "yelp.dex", "yelp.html", "yelp.full.json"
  
end