#!/bin/sh
folder=$1
git_remote_name=ci_news_`git log --pretty=format:'%h' -n 1`
git remote add ${git_remote_name} https://github.com/cyclus/cyclus/
git fetch ${git_remote_name}
added_news_file=$((`git diff ${git_remote_name}/master --name-only $folder |wc -l`))
git remote remove ${git_remote_name}
if [ $added_news_file -eq 0 ]; then
    exit 1
fi
