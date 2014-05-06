rm prey; rm pred;
metric -db=cyclus.sqlite deployseries Predator > pred
metric -db=cyclus.sqlite deployseries Prey > prey
./plot.p
