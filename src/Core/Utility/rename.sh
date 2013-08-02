
function capFirst {
  echo $1 | sed "s/^\(.\)/\u\1/"
}

function rename {
  newname=`capFirst $1`
  ag -l $1 $2 | xargs sed -i "s/\(\.\|->\|::\| \)$1(/\1$newname(/"
}

while read funcname; do
  rename $funcname $2
done < $1

