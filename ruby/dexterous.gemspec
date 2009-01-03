Gem::Specification.new do |s|
  s.name     = "dexterous"
  s.version  = "0.1.0"
  s.date     = "2008-08-10"
  s.summary  = "Ruby binding for dexter"
  s.email    = "kyle@kylemaxwell.com"
  s.homepage = "http://github.com/fizx/robots"
  s.description = "Ruby binding for dexter"
  s.has_rdoc = true
  s.require_paths = ["lib", "ext"]
  s.extensions = "ext/extconf.rb"
  s.authors  = ["Kyle Maxwell"]
  s.files    = Dir["**/*"]
  s.rdoc_options = ["--main", "README"]
  s.extra_rdoc_files = ["README"]
  s.add_dependency("json", ["> 0.0.0"])
end
