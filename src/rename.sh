
function capFirst {
  echo $1 | sed "s/^\(.\)/\u\1/"
}

function rename {
  newname=`capFirst $1`
  ag -l --ignore=CMake*, --ignore=GoogleTest* --ignore=doc $1 $2 | xargs sed -i "s/\([^a-zA-Z_0-9]\)$1(/\1$newname(/g"
}

while read funcname; do
  rename $funcname $2
done < $1

