# MetaPerson.py

from collections import defaultdict

from utils.utils import dump_json
from utils.timeprofile import TimeProfile as TP
from utils.timeprofile import TimeProfileEntry as TPE
from utils.timeprofile import PatDur as PD

class MetaPerson:

    mid = 1

    def __init__(self, tps, capacity, probability=1.0, people=None):
        self.id          = MetaPerson.mid
        MetaPerson.mid   += 1

        self.tps         = tps
        self.capacity    = capacity
        self.probability = probability
        self.people      = people
        for p in self.people:
            p.mid = self.id

    @staticmethod
    def parse_from_list(plist):
        tp = TP(TPE(PD.merge(x for person in plist for x in person.tp)))

        # TODO: is this the right thing to do? 
        capacity = defaultdict(int)
        for person in plist:
            for meid, ct in person.capacity.items():
                capacity[meid] += ct

        capacity_list = [{'metaevent-id' : meid, 'probability' : ct} 
                            for meid, ct in capacity.items()
                            if meid is not None]

        return MetaPerson(tps=tp, capacity=capacity_list, 
                        probability=len(plist), people=plist)


    def __str__(self):
        return f'''\
MetaPerson {self.id}; 
on {self.tps}'''

    def as_dict(self):
        return {
            'id'             : self.id,
            'probability'    : self.probability,
            'time-profiles'  : self.tps.as_dict(),
            'event-affinity' : self.capacity
        }

    @staticmethod
    def dump(metapeople, fname):
        dump_json(fname, [mp.as_dict() for mp in metapeople])

