#!/bin/sh
folder=$1
added_news_file=$((`git diff origin/master --name-only $folder |wc -l`))
if [ $added_news_file -eq 0 ]; then
    exit 1
fi
