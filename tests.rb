#!/usr/bin/env ruby 
require "test/unit"

class Tests < Test::Unit::TestCase
  def test_simple_array
    dex_test "li.dex", "yelp.html", "li.yelp.json"
  end
  
  def dex_test(dex, input, output)
    root = File.dirname(__FILE__) 
    fixtures = "#{root}/fixtures"
    assert_equal `./dexter #{fixtures}/#{dex} #{fixtures}/#{input}`, File.read("#{fixtures}/#{output}")
  end
end