echo "post-link.sh, PREFIX: $PREFIX"

mv $PREFIX/bin/cyclus $PREFIX/bin/cyclus_base
echo "#!/bin/bash
#export LD_LIBRARY_PATH=$PREFIX/lib:$PREFIX/lib/cyclus
#export DYLD_FALLBACK_LIBRARY_PATH=$PREFIX/lib/cyclus
#export DYLD_LIBRARY_PATH=
export CYCLUS_PATH=\"\$CYCLUS_PATH:\$HOME/.local/lib/cyclus:$PREFIX/lib/cyclus\"
if [ -z \"\$CYCLUS_NUC_DATA\" ]; then
  export CYCLUS_NUC_DATA=\"$PREFIX/share/cyclus/cyclus_nuc_data.h5\"
fi
if [ -z \"\$CYCLUS_RNG_SCHEMA\" ]; then
  export CYCLUS_RNG_SCHEMA=\"$PREFIX/share/cyclus/cyclus.rng.in\"
fi

$PREFIX/bin/cyclus_base \$*
" > $PREFIX/bin/cyclus
chmod 755 $PREFIX/bin/cyclus

mv $PREFIX/bin/cyclus_unit_tests $PREFIX/bin/cyclus_unit_tests_base
echo "#!/bin/bash
#export LD_LIBRARY_PATH=$PREFIX/lib:$PREFIX/lib/cyclus
#export DYLD_FALLBACK_LIBRARY_PATH=$PREFIX/lib/cyclus
#export DYLD_LIBRARY_PATH=
export CYCLUS_PATH=\"\$CYCLUS_PATH:\$HOME/.local/lib/cyclus:$PREFIX/lib/cyclus\"
if [ -z \"\$CYCLUS_NUC_DATA\" ]; then
  export CYCLUS_NUC_DATA=\"$PREFIX/share/cyclus/cyclus_nuc_data.h5\"
fi
if [ -z \"\$CYCLUS_RNG_SCHEMA\" ]; then
  export CYCLUS_RNG_SCHEMA=\"$PREFIX/share/cyclus/cyclus.rng.in\"
fi

$PREFIX/bin/cyclus_unit_tests_base \$*
" > $PREFIX/bin/cyclus_unit_tests
chmod 755 $PREFIX/bin/cyclus_unit_tests

