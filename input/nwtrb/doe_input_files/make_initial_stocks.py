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

# Filename and directory info
current_dir=os.getcwd()+'/'
input='nwtrb_plant_input_raw'
mypath=current_dir+input
fin=open(mypath,'r')

# Read the input file
in_lines=fin.readlines()
fin.close()

# These words start lines that we're not interested in
fou_words = ['Wet','Core','Site']

# These are the first word of compound state names
state_words = ['New','North','South']

# This is the container for each reactor site
site_info = []

# Read each line. Determine if the line is a new reactor site,
# a reactor entry, or neither. Act appropriately.
first_entry = True
for line in in_lines:
    line_arr = line.split()
    first_word = line_arr[0]
    # Make sure this is a line we care about
    if (first_word in fou_words) == False:
        # See if this is a site or reactor entry
        # Note: a reactor entry starts with a one-digit integer
        if len(first_word) > 1:
            # This is a site entry
            # Check to make sure this isn't the first entry
            if first_entry:
                first_entry = False
            else:
                # this is a new entry. record the data for the last entry.
                site = [name,dry_storage,reactor_info]
                site_info.append(site)
            # Get the site name
            name = line_arr[0:-4]
            if name[-1] in state_words:
                name = name[0:-1]
            # Get the amount of fuel in dry storage
            dry_storage = float(line_arr[-1])
            # Instantiate the reactor_info list
            reactor_info = []
        else:
            # This is a reactor entry
            number = line_arr[0]
            type = line_arr[1]
            start_year = int(line_arr[4])
            wet_storage = float(line_arr[9])
            reactor = [number, type, start_year, wet_storage]
            reactor_info.append(reactor)

# Now construct the initial stocks container
initial_stocks = []

# Fill initial stocks
reference_year = 2010
for site in site_info:
    # Get all the site information
    name,dry_storage,reactor_info = site
    site_name = "_".join(name)
    n_reactors = len(reactor_info)
    for reactor in reactor_info:
        # Get all the reactor information
        number, type, start_year, wet_storage = reactor
        
        # Format the initial_stocks entries and append
        fac_name = 'nwtrb:' + site_name + '_' + number
        in_commodity = 'spent_' + type + '_uo2'
        recipe = 'nwtrb:' + in_commodity
        
        # I assume that each reactor in a site contributed
        # to dry storage EQUALLY
        amount = wet_storage + (dry_storage / n_reactors)
        # Convert the amount from MTU to kg UO2
        # Note: conversion factor based on NWTRB-type assemblies
        amount = 1.1344 * amount * 1E+03
        
        # I assume all material is of the same age
        # equal to the reactor's median age (in months)
        age = (reference_year - start_year) * 12
        
        # Add the entry to the initial_stocks
        entry = [fac_name, in_commodity, recipe, amount, age]
        initial_stocks.append(entry)

# Make the write lines
write_lines = []
write_lines.append('        <initialstocks>')
for entry in initial_stocks:
    # Get entry information
    fac_name, in_commodity, recipe, amount, age = entry
    # Get each entry line
    fac_line = '            <facility>' + fac_name + '</facility>'
    commodity_line = '            <incommodity>' + in_commodity + '</incommodity>'
    recipe_line = '            <recipe>' +  recipe + '</recipe>'
    amount_line ='            <amount>' + '%1.3E' % (amount) + '</amount>'
    age_line = '	    <age>' + str(age) +'</age>'
    # Add each entry line
    write_lines.append('          <entry>')
    write_lines.append(fac_line)
    write_lines.append(commodity_line)
    write_lines.append(recipe_line)
    write_lines.append(amount_line)
    write_lines.append(age_line)
    write_lines.append('          </entry>')
write_lines.append('        </initialstocks>')

# Write lines to the output file        
output = 'initialstocks'
mypath=current_dir+output
outfile=open(mypath,'w')
write_lines=makewritelines(addnewlines(write_lines))
outfile.write(write_lines)
outfile.close()
