import os
import re


def get_path() :
    path = argv[1]
    return path

def get_xml_files() :
    for root, dirs, files in os.walk(path, followlinks=True):
        if '.git' in dirs:
            dirs.remove('.git')
        for name in files: 
            if re.search("\.xml",name):
                os.system("./cyclus " + os.path.join(root, name))
