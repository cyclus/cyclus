$PROJECT = $GITHUB_REPO = 'cymetric'
$GITHUB_ORG = 'cyclus'

$ACTIVITIES = ['version_bump', 'changelog', 'nose', 'tag', 'push_tag',
               'pypi', 'conda_forge', 'ghrelease']
$CHANGELOG_FILENAME = 'CHANGELOG.rst'
$CHANGELOG_TEMPLATE = 'TEMPLATE.rst'

$DOCKER_CONDA_DEPS = ['cyclus', 'cycamore', 'nose', 'pytables', 'pandas',
                      'python-graphviz', 'pyne', 'numpy']
$DOCKER_INSTALL_COMMAND = 'git clean -fdx && ./setup.py install --user'

$VERSION_BUMP_PATTERNS = [
    ('cymetric/__init__.py', '__version__\s*=.*', "__version__ = '$VERSION'"),
    ('setup.py', 'VERSION\s*=.*', "VERSION = '$VERSION'"),
]

$NOSE_COMMAND = 'cd tests/ && rm test_filters.py && nosetests'
