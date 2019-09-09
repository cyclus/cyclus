#!/bin/sh

# If default branch skip the test
branch=`git branch | grep \* | cut -d ' ' -f2`
def_branch="master"
if [ "$branch" == "$def" ]; then
    return 0
fi

folder=$1
added_news_file=$((`git diff origin/master --name-only $folder |wc -l`))

if [ $added_news_file -eq 0 ]; then
    exit 1
fi
