require "rubygems"
require "nokogiri"
require "hpricot"
require "parsley"
require "benchmark"
require "pp"

YELP_HTML = File.dirname(__FILE__) + "/yelp.html"

def noko
  parse Nokogiri.Hpricot(File.open(YELP_HTML))
end

def hpri
  parse Hpricot(File.open(YELP_HTML))  
end

def parse(doc)
  out = {}
  out["name"] = (doc / "h1").first.inner_text
  out["phone"] = (doc / "#bizPhone").first.inner_text
  out["address"] = (doc / "address").first.inner_text
  out["reviews"] = (doc / ".nonfavoriteReview").map do |node|
    review = {}
    review["date"] = (node / ".ieSucks .smaller").first.inner_text
    review["user_name"] = (node / ".reviewer_info a").first.inner_text
    review["comment"] = (node / ".review_comment").first.inner_text
    review
  end
end

def pars
  parselet = Parsley.new({
    "name" => "h1",
    "phone" => "#bizPhone",
    "address" => "address",
    "reviews(.nonfavoriteReview)" => [
      {
        "date" => ".ieSucks .smaller",
        "user_name" => ".reviewer_info a",
        "comment" => ".review_comment"
      }
    ]
  })
  parselet.parse(:file => YELP_HTML)
end

Benchmark.bm do |x|
  x.report("nokogiri: ")  { 3.times { noko } }
  x.report("hpricot: ")   { 3.times { hpri } }
  x.report("parsley: ") { 3.times { pars } }
end

