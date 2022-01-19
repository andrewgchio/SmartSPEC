# MetaEvent.py

from utils.utils import dump_json
from utils.timeprofile import TimeProfile as TP
from utils.timeprofile import TimeProfileEntry as TPE

class MetaEvent:

    meid = 1

    def __init__(self, slist, tps, probability=1.0, events=None):
        '''
        :param space_ids: A list of spaces in which the event could occur
        :param tps: TimeProfile detailing when the metaevent occurs
        :param probability: The probability of this metaevent occuring
        '''
        self.id          = MetaEvent.meid
        MetaEvent.meid   += 1

        self.space_ids   = {'space-ids' : list(slist), 'number' : 1}
        self.tps         = tps
        self.probability = probability
        self.events      = events
        for e in self.events:
            e.meid = self.id

        self.capacity = None


    @staticmethod
    def parse_from_list(elist):
        sids = set()
        tp_entry = TPE()
        capacity = []
        for event in elist:
            sids.add(event.space)
            for p in event.tp:
                tp_entry.add_patdur(p)
        time_profiles = TP(tp_entry)

        return MetaEvent(slist=sids, tps=time_profiles, events=elist)

    def __str__(self):
        return f'''\
MetaEvent in {self.space_ids['space-ids']}
on {self.tps}'''

    def as_dict(self):
        return {
            'id' : self.id, 
            'probability' : self.probability,
            'spaces' : self.space_ids,
            'time-profiles' : self.tps.as_dict(),
            'capacity' : self.capacity
        }

    @staticmethod
    def dump(metaevents, fname):
        dump_json(fname, [me.as_dict() for me in metaevents])

