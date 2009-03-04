require "test/unit"
require File.dirname(__FILE__) + "/../lib/parsley"

class TestParsley < Test::Unit::TestCase
  def setup
    @file = File.dirname(__FILE__) + "/../../test/yelp.html"
  end
  
  def test_yelp
    @parsley = Parsley.new(File.read(File.dirname(__FILE__) + "/../../test/yelp-home.let"))
    out = @parsley.parse(:file => File.dirname(__FILE__) + "/../../test/yelp-home.html")
    assert_equal "/c/sf/shopping", out["categories"][0]["href"]
  end
  
  def test_yelp_xml
    @parsley = Parsley.new(File.read(File.dirname(__FILE__) + "/../../test/yelp-home.let"))
    out = @parsley.parse(:file => File.dirname(__FILE__) + "/../../test/yelp-home.html", :output => :xml)
  end
  
  def test_simple
    @parsley = Parsley.new("hi" => "h1")
    assert_equal({"hi" => "Nick's Crispy Tacos"}, @parsley.parse(:file => @file))
  end
  
  def test_simple_string
    @parsley = Parsley.new("hi" => "h1")
    assert_equal({"hi" => "Nick's Crispy Tacos"}, @parsley.parse(:string => "<html><body><h1>Nick's Crispy Tacos</h1></body></html>"))
  end  
  
  def test_xml
    @parsley = Parsley.new("hi" => "h1")
    xml = "<?xml version=\"1.0\"?>\n<parsley:root xmlns:parsley=\"http://parslets.com/json\"><hi>Nick's Crispy Tacos</hi></parsley:root>\n"
    assert_equal(xml, @parsley.parse(:file => @file, :output => :xml))
  end
  
  def test_json
    @parsley = Parsley.new("hi" => "h1")
    assert_equal('{ "hi": "Nick\'s Crispy Tacos" }', @parsley.parse(:file => @file, :output => :json))
  end
  
  def test_rescuable_file_error
    @parsley = Parsley.new("hi" => "h1")
    @nonexistant_file = File.dirname(__FILE__) + "/../fixtures/yelp.html"
    assert_equal({"hi" => "Nick's Crispy Tacos"}, @parsley.parse(:file => @nonexistant_file)) rescue nil
  end
  
  def test_array_string
    @parsley = Parsley.new({"foo" => ["li"]})
    out = @parsley.parse(:file => @file)
    assert_kind_of Hash, out
    assert_kind_of Array, out["foo"], out.inspect
    assert out["foo"].length > 1
  end
end