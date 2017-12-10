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
        print("=== Start AttrToaster ===\n{")
        print('"id": {0},'.format(self.id))
        print('"prototype": "{0}"'.format(self.prototype))
        print("}\n=== End AttrToaster === ")
