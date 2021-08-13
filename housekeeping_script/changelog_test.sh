#!/bin/sh

OWNER=cyclus
REPO=cyclus
CHANGELOG_FILE=CHANGELOG.rst

# default main repo setup
PR_BASE_BRANCH=master
echo "Testing changelog against $PR_BASE_BRANCH branch"

master_repo="https://github.com/${OWNER}/${REPO}.git"
default_branch=$PR_BASE_BRANCH

# setup temp remote 
git_remote_name=ci_changelog_`git log --pretty=format:'%h' -n 1`
git remote add ${git_remote_name} ${master_repo}
git fetch ${git_remote_name}

# diff against temp remote
added_changelog_entry=$((`git diff ${git_remote_name}/${default_branch} -- ${CHANGELOG_FILE} |wc -l`))

# cleaning temp remote
git remote remove ${git_remote_name}

# analysing the diff and returning accordingly
if [ $added_changelog_entry -eq 0 ]; then
    echo "No new changelog entry detected, please update the ${CHANGELOG_FILE} according to your submited changes!"
    exit 1
fi
