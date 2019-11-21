import sys
import os
import input as inp
import output as oup
import dakota.interfacing as di
import subprocess
import cymetric as cym
from cymetric import fco_metrics
# ----------------------------
# Parse Dakota parameters file
# ----------------------------

params, results = di.read_parameters_file()

# -------------------------------
# Convert and send to Cyclus
# -------------------------------

# Edit Cyclus input file
cyclus_template = 'test.xml.in'
scenario_name = 'PW' + str(round(params['power']))
variable_dict = {'handle': scenario_name, 'power_cap': params['power']}
output_xml = 'test.xml'
inp.render_input(cyclus_template, variable_dict, output_xml)

# Run Cyclus with edited input file
output_sqlite = 'test'+scenario_name+'.sqlite'
os.system('cyclus -i ' + output_xml + ' -o ' + output_sqlite)

# ----------------------------
# Return the results to Dakota
# ----------------------------
# return total electricity generated to Dakota 
cursor = oup.cursor(output_sqlite)
power = cursor.execute('SELECT time, sum(value) FROM timeseriespower').fetchall()

for i, r in enumerate(results.responses()):
    if r.asv.function:
        r.function = power[0][1]

results.write()
