import sys
import os
import input as inp
import dakota.interfacing as di
import subprocess
# ----------------------------
# Parse Dakota parameters file
# ----------------------------

params, results = di.read_parameters_file()

# -------------------------------
# Convert and send to Dymond
# -------------------------------

# Edit Dymond6 input file
cyclus_template = 'test.xml.in'
scenario_name = 'PW' + str(round(params['x1']))
variable_dict = {'handle': scenario_name, 'power_cap': params['x1']}
output_xml = 'test.xml'
inp.render_input(cyclus_template, variable_dict, output_xml)

# Run Cyclus with edited input file
output_sqlite = 'test'+scenario_name+'.sqlite'
os.system('cyclus -i ' + output_xml + ' -o ' + output_sqlite)

# ----------------------------
# Return the results to Dakota
# ----------------------------

for i, r in enumerate(results.responses()):
    if r.asv.function:
        r.function = 1

results.write()
