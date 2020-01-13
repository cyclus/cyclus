#! /usr/bin/env python

import cymetric as cym
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

db = cym.dbopen('recycle.sqlite')
evaler = cym.Evaluator(db)
comm = evaler.eval('CommissioningSeries')
decomm = evaler.eval('DecommissioningSeries')

# make exit counts negative for plotting purposes
neg = -decomm['Count']
decomm = decomm.drop('Count', axis=1)
decomm = pd.concat([decomm, neg], axis=1)

# for later merge, rename time columns
comm.rename(columns={'EnterTime' : 'Time'}, inplace=True)
decomm.rename(columns={'ExitTime' : 'Time'}, inplace =True)

# pivot tables for plotting purposes, and merge tables
c = comm.pivot('Time', 'Prototype')['Count'].reset_index()
d = decomm.pivot('Time', 'Prototype')['Count'].reset_index()
cd = pd.merge(c, d, left_on='Time', right_on='Time', how='outer', sort=True,
    suffixes=('_enter', '_exit')).fillna(0)

# pandas always changes everything to floats in the presence of NaNs, so change
# the TimeStep column  back to integer
cd.Time = cd.Time.astype(int)

# only plot facilities of interest: lwr and frx
colors = ['green', 'purple']
ax = cd[['Time', 'lwr_enter', 'frx_enter']].plot.bar(x = 'Time', figsize=(30, 10), stacked=True, width=1, legend=False, color=colors)
l = plt.legend()
l.get_texts()[0].set_text('LWR')
l.get_texts()[1].set_text('FR')
l.set_title('Prototype')
cd[['Time', 'lwr_exit', 'frx_exit']].plot.bar(x = 'Time', ax = ax, figsize=(30, 10), stacked=True, width=1, legend=False, color=colors)

plt.xlabel(r'TimeStep', fontsize=18); plt.ylabel(r'Count', fontsize=18)
plt.savefig('ex.png')
