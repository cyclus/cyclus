wget https://github.com/gidden/ciclus/archive/travis.zip -O ciclus.zip
unzip -j ciclus.zip "*/cyclus/*" -d conda-recipe
conda build --no-test conda-recipe
conda install --use-local cyclus=0.0
