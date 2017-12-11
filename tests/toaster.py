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
        # easily convertible to JSON
        info = {
            'id': self.id,
            'str': str(self),
            'hash': hash(self),
            'kind': self.kind,
            'spec': self.spec,
            'schema': self.schema,
            'version': self.version,
            'parent': self.parent_id,
            'prototype': self.prototype,
            'enter_time': self.enter_time,
            'lifetime': self.lifetime,
            'exit_time': self.exit_time,
            'childern': list(self.children),
            'annotations': str(self.annotations),
            }
        s = json.dumps(info)
        print("=== Start AttrToaster ===\n")
        print(s)
        print("\n=== End AttrToaster ===")
        # Can't easilty convert to JSON.
        # Make sure these don't segfault
        p = self.parent()
        self.children_str()
        self.tree_strs(p)
        self.in_family_tree(p),
        self.ancestor_of(p),
        self.decendent_of(p),
        self.decomission()

