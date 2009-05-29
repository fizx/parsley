#!/bin/sh
cat /dev/null > /tmp/bisect.sh
chmod +x /tmp/bisect.sh
cp "test/$1".* /tmp || exit
echo "./bootstrap.sh && ./configure && make && ./parsley /tmp/$1.let /tmp/$1.html 2>&1 | diff /tmp/$1.json -" >> /tmp/bisect.sh
cat /tmp/bisect.sh
echo "-------------"
echo "git bisect run /tmp/bisect.sh"