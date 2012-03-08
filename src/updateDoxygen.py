#! /usr/bin/env python

import os

# Utility functions
def addnewlines(lines):
    lines=[line+'\n' for line in lines]
    return lines

def makewritelines(lines):
    myline=''
    for line in lines:
        myline+=line
    return myline

def iseven(number):
    if number%2==0:
        return True
    else:
        return False

# Filename and directory info
file_name="4dknap"

current_dir=os.getcwd()+'/'
input=file_name+'.dat'
mypath=current_dir+input
fin=open(mypath,'r')

# Get values and weights from .dat file
# All RHS values set to 1000.0
values,weights=fin.read().split('-')
fin.close()
values=values.split('[')[1].split(']')[0].split()
weights=[weight.split() for weight in weights.split('[')[1].split(']')[0].split('\n')[1:]]
rhs_vals=['1000.0' for value in values]

# Create lines under Rows header
rows=['ROWS']
rows.append(" N R100")
for i in range(len(weights)):
    index=i+1
    row_index="R"+str(index+100)
    rows.append(" L "+row_index)

# Create lines under Columns header
columns=['COLUMNS']
for i in range(len(values)):
    index1=i+1
    col_index="C"+str(index1+100)
    for j in range(len(weights)+1):
        index2=j
        row_index="R"+str(index2+100)
        if iseven(j):
            if j==0:
                line=' %6s %10s %16s'%(col_index,row_index,values[i])
            else:
                line=' %6s %10s %16s'%(col_index,row_index,weights[j-1][i])
                if j==len(weights):
                    columns.append(line)
        else:
            line+=' %6s %16s'%(row_index,weights[j-1][i])
            columns.append(line)

# Create lines under RHS header
rhs=['RHS']
for i in range(len(weights)):
    index=i+1
    row_index='R'+str(index+100)
    if iseven(i):
        line=' %6s %10s %16s'%('RHS',row_index,rhs_vals[i])
        if i==len(weights):
            rhs.append(line)
    else:
        line+=' %6s %16s'%(row_index,rhs_vals[i])
        rhs.append(line)

# Create lines under Bounds header
bounds=['BOUNDS']
for i in range(len(values)):
    index=i+1
    col_index="C"+str(100+index)
    line=' %2s %3s %9s'%('BV','BIN',col_index)
    bounds.append(line)

# Get input heading info
input=file_name+'.info'
mypath=current_dir+input
infofile=open(mypath,'r')
infolines=infofile.read().split('\n')
infofile.close()

# Add lines together and create .mps input file
mylines=infolines[:-1]+rows+columns+rhs+bounds+['ENDATA']
mylines=makewritelines(addnewlines(mylines))
output=file_name+'.mps'
mypath=current_dir+output
outfile=open(mypath,'w')
outfile.write(mylines)
outfile.close()

