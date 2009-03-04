task :default => :test

task :test => :build do
  Dir["test/*test*.rb"].each {|f| load f }
end

task :build => :configure do 
  system "cd ext && make"
end

task :clean => :configure do
  system "cd ext && make clean"
end

task :configure do
  system "cd ext && ruby extconf.rb" unless File.exists?("ext/Makefile")
end

task :install do 
  system "gem build parsley-ruby.gemspec"
  system "gem install parsley-ruby"
end