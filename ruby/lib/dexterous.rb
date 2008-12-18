require File.dirname(__FILE__) + "/../ext/cdexterous"
require "rubygems"
require "json"

class Dexterous
  def initialize(dex)
    if(dex.kind_of(Hash))
      dex = dex.to_json 
    end
    @dex = CDexterous.compile(dex, "")
  end
  
  # Valid options:
  #
  # Requires one of:
  # :file -- the input file path
  # :string -- the input string
  #
  # And optionally:
  # :input => [:xml, :html]
  # :output => [:json, :xml, :ruby]
  # :allow_empty -- If false, throws an exception if any value is empty.
  #
  # Defaults are :input => :html, :output => :ruby, :allow_empty => false
  def parse(options = {})
    if options[:file]
      D
    else
    end
  end
end