echo "===シナリオ１==="
./bufcache <<EOS
init
free
getblk 10
free
EOS

echo "\n===シナリオ２==="
./bufcache <<EOS
init
hash
getblk 18
hash
EOS

echo "\n===シナリオ３==="
./bufcache <<EOS
init
set 3 stat K
hash
getblk 18
hash
EOS
