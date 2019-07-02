#!/bin/sh
folder=$1

added_news_file=$((`git diff master $folder --name-only |wc -l`))
echo $added_news_file


if [ $added_news_file -eq 0 ]; then
    echo "OUT"
    exit 1
fi
