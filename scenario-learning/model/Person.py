# Person.py

from utils.utils import dump_json

class Person:

    pid = 1

    def __init__(self, cid, tp, capacity, eids):
        self.id       = Person.pid
        Person.pid     += 1
        self.cid      = cid
        self.tp       = tp
        self.capacity = capacity
        self.eids     = eids

        self.mid     = None

    def __str__(self):
        return f'Person {self.id} {self.tp}; events={self.eids}'

    def as_dict(self):
        return {
            'id'            : self.id,
            'metaperson-id' : self.mid,
            'profile-index' : 0
        }

    @staticmethod
    def dump(people, fname):
        dump_json(fname, [p.as_dict() for p in people if p.mid is not None])

