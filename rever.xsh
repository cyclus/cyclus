from rever.activity import docker dockeractivity

$PROJECT = $GITHUB_ORG = $GITHUB_REPO = 'cyclus'
$ACTIVITIES = ['changelog',
               # 'tag',
               'cyclus-tests',
               #'push_tag', 'conda_forge', 'ghrelease',
               ]

$CHANGELOG_FILENAME = 'CHANGELOG.rst'
$CHANGELOG_TEMPLATE = 'TEMPLATE.rst'

$DOCKER_CONDA_DEPS = ['cyclus-build-deps']
$DOCKER_INSTALL_COMMAND = (
    'git clean -fdx && '
    './install.py --build_type=Release'
    )


with! dockeractivity(name='cyclus-tests'):
    cd tests
    cyclus_unit_tests
    nosetests
