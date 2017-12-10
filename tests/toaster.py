import json

from cyclus.agents import Facility
from cyclus import lib
import cyclus.typesystem as ts


class DefaultToaster(Facility):
    """Meant for testing default values"""

    bread = ts.String(default='rye')
    level = ts.Double(default=4.2)

    def tick(self):
        print('Bread is ' + self.bread)
        print('Toast level is {0}'.format(self.level))


class AttrToaster(Facility):
    """Meant for testing attributes values"""

    def tick(self):
        info = {
            'id': self.id,
            'kind': self.kind,
            'spec': self.spec,
            'version': self.version,
            'prototype': self.prototype,
            }
        s = json.dumps(info)
        print("=== Start AttrToaster ===\n")
        print(s)
        print("\n=== End AttrToaster ===")
