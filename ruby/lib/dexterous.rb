require File.dirname(__FILE__) + "/../ext/cdexter"
require "rubygems"
require "json"

class Dexterous
  def initialize(dex, incl = "")
    if(dex.is_a?(Hash))
      dex = dex.to_json 
    end
    @dex = CDexter.new(dex, incl)
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
    options[:file] || options[:string] || throw("must specify what to parse")
    options[:input] ||= :html
    options[:output]||= :ruby
    if options[:file]
      @dex.parse_file options[:file], options[:input], options[:output]
    else
      @dex.parse_string options[:string], options[:input], options[:output]
    end
  end
end