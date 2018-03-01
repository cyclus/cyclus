from rever.activity import dockeractivity

$PROJECT = $GITHUB_ORG = $GITHUB_REPO = 'cyclus'

$CHANGELOG_FILENAME = 'CHANGELOG.rst'
$CHANGELOG_TEMPLATE = 'TEMPLATE.rst'

$DOCKER_APT_DEPS = ['libc6', 'libc6-i386', 'libc6-dev', 'libc-dev', 'gcc']
$DOCKER_CONDA_DEPS = ['cyclus-build-deps', 'make']
$DOCKER_INSTALL_COMMAND = (
    'git clean -fdx && '
    './install.py --build_type=Release '
    '             -DBLAS_LIBRARIES="-L/opt/conda/lib -lopenblas" '
    '             -DLAPACK_LIBRARIES="-L/opt/conda/lib -lopenblas"'
    )


with! dockeractivity(name='cyclus-tests', lang='sh'):
    cd tests && export PATH=${HOME}/.local/bin:${PATH} && cyclus_unit_tests && nosetests

$ACTIVITIES = ['cyclus-tests', 'changelog', 'tag',
               'push_tag', 'conda_forge', 'ghrelease',
               ]
