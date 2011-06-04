#!/usr/bin/env python

import re
import string
import sys
import operator
import os

# Function uses name variable to determine the atomic number size then the
# atomic number is parsed to determine number of zeroes to fit XML schema
# used in cyclus input files for isotope. Returns isotope string, Iso
def A_zero(name, Z):
  A = filter(lambda name: name.isdigit(), name)
  A_num = long(A)
  if A_num < 10:
    A = ''.join('00' + A)
  elif A_num < 100:
    A = ''.join('0' + A)      
  Iso = ''.join(Z + A)
  return Iso

# Filename and directory info
current_dir=os.getcwd()+'/'
input='Pre_2010_PWR_Fresh_UO2.out'
#input='Post_2010_PWR_Fresh_v02.out'
mypath=current_dir+input
myfile=open(mypath,'rb')

entry = open('test.txt', 'w')

lines = myfile.readlines()

# -------- Remove Extra Information in Original Origen Text Output --------
isoLoop = "no"

names=[]
values=[]
for line in lines:
  if re.search('basis = 1 MTU',line):
    isoLoop = "yes"
  elif re.search('basis =1 MTU',line):
    isoLoop = "yes"
    if isoLoop == "yes":
      entry.write(line[:-1])      
  elif re.search('Case',line, re.I):
    isoLoop = "no"     
  elif re.search('total',line, re.I):
    isoLoop = "no"
  elif re.search('charge',line, re.I):
    if isoLoop == "yes":
      isoLoop = "yes"
      line_array = line.lstrip().split(" ")
      names.append(line_array[0])
      values.append(line_array[-2])
  else:
    if isoLoop == "yes":
      line_array = line.lstrip().split(" ")
      if len(line_array[0])>2:
        name = line_array[0]
      else:
        if len(line_array[1])>0:
          name =line_array[0]+line_array[1]
        else:
          name =line_array[0]+line_array[2]
      names.append(name)
      values.append(line_array[-1])    
      entry.write(line[:-1])

# -------- XML Format Conversion for Element and Isotope ---------
lines=[]
i = 0
for name in names:
# First in loop look to see if line is break in section denoted with charge and
# length of the irradiation or decay.
  if re.search('charge',name, re.I):
    # Combine string name charge with number of days of charge
    lines.append(name + " " + str(values[i]))
    lines.append("\n")
# With regular expression character 
  elif re.search('he',name):
    Z = '02' # element converted to integer based on if statement
    # Function call to Iso to return string of numbers to fit XML format.
    Iso = A_zero(name, Z)
    # Combine Iso name charge with number of days of charge.
    lines.append(Iso + " " + str(values[i]))
  elif re.search('li',name):
    Z = '03' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('be',name):
    Z = '04' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ne',name):
    Z = '10' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('na',name):
    Z = '11' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('mg',name):
    Z = '12' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('al',name):
    Z = '13' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('si',name):
    Z = '14' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('cl',name):
    Z = '17' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ar',name):
    Z = '18' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ca',name):
    Z = '20' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('sc',name):
    Z = '21' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ti',name):
    Z = '22' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('cr',name):
    Z = '24' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('mn',name):
    Z = '25' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('fe',name):
    Z = '26' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('co',name):
    Z = '27' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ni',name):
    Z = '28' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('cu',name):
    Z = '29' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('zn',name):
    Z = '30' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ga',name):
    Z = '31' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ge',name):
    Z = '32' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('as',name):
    Z = '33' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('se',name):
    Z = '34' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('br',name):
    Z = '35' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('kr',name):
    Z = '36' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('rb',name):
    Z = '37' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('sr',name):
    Z = '38' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('zr',name):
    Z = '40' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('nb',name):
    Z = '41' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('mo',name):
    Z = '42' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('tc',name):
    Z = '43' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ru',name):
    Z = '44' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('rh',name):
    Z = '45' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('pd',name):
    Z = '46' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ag',name):
    Z = '47' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('cd',name):
    Z = '48' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('in',name):
    Z = '49' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('sn',name):
    Z = '50' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('sb',name):
    Z = '51' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('te',name):
    Z = '52' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('xe',name):
    Z = '54' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('cs',name):
    Z = '55' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ba',name):
    Z = '56' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('la',name):
    Z = '57' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ce',name):
    Z = '58' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('pr',name):
    Z = '59' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('nd',name):
    Z = '60' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('pm',name):
    Z = '61' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('sm',name):
    Z = '62' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('eu',name):
    Z = '63' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('gd',name):
    Z = '64' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('tb',name):
    Z = '65' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('dy',name):
    Z = '66' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ho',name):
    Z = '67' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('er',name):
    Z = '68' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('tm',name):
    Z = '69' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('yb',name):
    Z = '70' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('lu',name):
    Z = '71' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('hf',name):
    Z = '72' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ta',name):
    Z = '73' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('re',name):
    Z = '75' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('os',name):
    Z = '76' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ir',name):
    Z = '77' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('pt',name):
    Z = '78' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('au',name):
    Z = '79' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('hg',name):
    Z = '80' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('tl',name):
    Z = '81' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('pb',name):
    Z = '82' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('bi',name):
    Z = '83' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('po',name):
    Z = '84' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('at',name):
    Z = '85' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('rn',name):
    Z = '86' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('fr',name):
    Z = '87' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ra',name):
    Z = '88' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('ac',name):
    Z = '89' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('th',name):
    Z = '90' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('pa',name):
    Z = '91' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('np',name):
    Z = '93' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('pu',name):
    Z = '94' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('am',name):
    Z = '95' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('cm',name):
    Z = '96' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('bk',name):
    Z = '97' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('cf',name):
    Z = '98' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('es',name):
    Z = '99' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('fm',name):
    Z = '100' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('md',name):
    Z = '101' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('no',name):
    Z = '102' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('lr',name):
    Z = '103' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('rf',name):
    Z = '104' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('db',name):
    Z = '105' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('sg',name):
    Z = '106' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('bh',name):
    Z = '107' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('hs',name):
    Z = '108' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('mt',name):
    Z = '109' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('h',name):
    Z = '01' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('b',name):
    Z = '05' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('c',name):
    Z = '06' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('n',name):
    Z = '07' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('o',name):
    Z = '08' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('f',name):
    Z = '09' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('p',name):
    Z = '15' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('s',name):
    Z = '16' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('k',name):
    Z = '19' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('v',name):
    Z = '23' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('y',name):
    Z = '39' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('i',name):
    Z = '53' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('w',name):
    Z = '74' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  elif re.search('u',name):
    Z = '92' # element converted to integer based on if statement
    Iso = A_zero(name, Z)
    lines.append(Iso + " " + str(values[i]))
  else:
    lines.append(name + " " + str(values[i]))
  i+=1

output = open("output.txt","w")
output.writelines(lines)
output.close()
myfile.close()
entry.close()


# Deals with creating Text File without Windows generated '^M' character
# and replaces the '^M' character with '\n'. This is because ORIGEN is run in
# Windows and the Windows generated text file uses the '^M' return character.
text = open("output.txt", 'rb').read().replace('\r\n', '\n')
open("test.txt", 'wb').write(text)

output.close()
myfile.close()
entry.close()

# ------ Convert Text File Output to  XML Formatting for Cyclus -------

output = open("test.txt", 'rb')
olines = output.readlines()

XMLout = open('pwr.xml', 'wb')

xlines=[]
i = 0

# Initial information to be copied into XML recipe file.
XMLout.write("  " + "<recipe>" + "\n")
XMLout.write("    " + "<name>" + "spent_PWR_uo2_Post2010"
             + "</name>" + "\n")
XMLout.write("    " + "<basis>" + "mass" + "</basis>" + "\n")
XMLout.write("    " + "<unit>" + "assembly" + "</unit>" + "\n")
# Used basis of 1E+3 since per MTU for Origen.
# 1000 kg = 1 MTU.
XMLout.write("    " + "<total>" + "1000" + "</total>" + "\n")

for line in olines:
  oline_array = line.lstrip().split(" ")
  ID = oline_array[0]
  if ID == "charge":
    chargeTime = oline_array[1]
  else:
    if re.search("1620.0", chargeTime):
      XMLout.write("    " + "<isotope>" + "\n")
      XMLout.write("       " + "<id>" + ID + "</id>")
      # Divide comp in grams by 1E+6 to get the mass fraction because
      # that is what is used in cyclus for isotope composition.
      compFloat = float(oline_array[1].rstrip())/(1.1345E+6)
      # Have to use compFloat to divide composition into mass fraction.
      # Then with comp you convert back to string.
      comp = '%1.3E'%(compFloat)
      XMLout.write("<comp>" + comp + "</comp>" + "\n")
      XMLout.write("    " + "</isotope>" + "\n")

XMLout.write("  " + "</recipe>" + "\n")

XMLout.close()
