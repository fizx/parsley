require File.dirname(__FILE__) + "/../ext/cparsley"
require "rubygems"
require "json"
require "thread"

class Parsley
  def initialize(parsley, incl = "")
    if(parsley.is_a?(Hash))
      parsley = parsley.to_json 
    end
    @@mutex ||= Mutex.new
    @@mutex.synchronize do
      @parsley = CParsley.new(parsley, incl)
    end
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
      @parsley.parse_file options[:file], options[:input], options[:output]
    else
      @parsley.parse_string options[:string], options[:input], options[:output]
    end
  end
end