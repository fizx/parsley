require "test/unit"
require File.dirname(__FILE__) + "/../lib/dexterous"

class TestDexterous < Test::Unit::TestCase
  def setup
    @file = File.dirname(__FILE__) + "/../../test/yelp.html"
  end
  
  def test_simple
    @dex = Dexterous.new("hi" => "h1")
    assert_equal({"hi" => "Nick's Crispy Tacos"}, @dex.parse(:file => @file))
  end
  
  def test_xml
    @dex = Dexterous.new("hi" => "h1")
    xml = "<?xml version=\"1.0\"?>\n<dexter:root xmlns:dexter=\"http://kylemaxwell.com/dexter\"><hi>Nick's Crispy Tacos</hi></dexter:root>\n"
    assert_equal(xml, @dex.parse(:file => @file, :output => :xml))
  end
  
  def test_json
    @dex = Dexterous.new("hi" => "h1")
    assert_equal('{ "hi": "Nick\'s Crispy Tacos" }', @dex.parse(:file => @file, :output => :json))
  end
  
  def test_rescuable_file_error
    @dex = Dexterous.new("hi" => "h1")
    @nonexistant_file = File.dirname(__FILE__) + "/../fixtures/yelp.html"
    assert_equal({"hi" => "Nick's Crispy Tacos"}, @dex.parse(:file => @nonexistant_file)) rescue nil
  end
  
  def test_array_string
    @dex = Dexterous.new({"foo" => ["li"]})
    out = @dex.parse(:file => @file)
    assert_kind_of Hash, out
    assert_kind_of Array, out["foo"], out.inspect
    assert out["foo"].length > 1
  end
end