# This script replaces the cyclus and cyclus_unit_tests commands with simple
# wrappers that will modify the user's environment as needed to point  
# cyclus-sepcific envrionment variables to the conda install location $PREFIX.
# Conda packaging has three phases which come to a head here.
#
#   1. builing the package on a builder's computer
#   2. installing the package on the user's machine, where this script is run
#   3. runtime, when the wrapper script(s) execute. 
#
# At install time (2), the conda post-link phase will define some extra 
# environment variables, such as $PREFIX, that are not available elsewhere.
# These variables are descriped at http://conda.pydata.org/docs/building/build-scripts.html
# Otherwise envrionment variables in the wrapper script (eg $CYCLUS_PATH)
# must be escaped here so that they are evaluated at run time (3) rather
# than at build (1) or install (2).
echo "post-link.sh, PREFIX: $PREFIX"

mv $PREFIX/bin/cyclus $PREFIX/bin/cyclus_base
echo "#!/bin/bash
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

