cd ext/ &&
gcc  -I/opt/local/include -I/usr/local/include  -L/opt/local/lib -L/usr/local/lib   -I. -I. -I/System/Library/Frameworks/Ruby.framework/Versions/1.8/usr/lib/ruby/1.8/universal-darwin9.0 -I. -I/usr/include/libxml2 -L/usr/lib -lxslt -lxml2 -lz -lpthread -licucore -lm  -fno-common -arch i386 -Os -pipe -fno-common  -c dexterous.c &&
make &&
cd .. &&
gem build dexterous.gemspec  &&
sudo gem install dexterous &&
irb -rdexterous