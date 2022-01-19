# Event.py

from utils.utils import dump_json

class Event:

    eid = 1 

    def __init__(self, cids, tp, space):
        '''
        An event consists of a set of attendees (cids), a time profile
        detailing when it occurs (tp), and a space in which it takes place
        (space)

        Upon initialization, we assign a unique id to the event

        :param cids: A set of event attendees
        :param tp: A time profile detailing when the event occurs
        :param space: The space in which the event took place
        '''
        self.id       = Event.eid
        Event.eid     += 1

        self.cids     = cids
        self.tp       = tp
        self.space    = space

        # To be initialized later
        self.meid     = None
        self.capacity = None


    def __str__(self):
        return f'Event {self.id} {self.tp}; size={len(self.cids)}'

    def as_dict(self):
        return {
            'id'            : self.id,
            'metaevent-id' : self.meid,
            'profile-index' : 0, # Only one profile learned for each event
            'space-ids'     : [self.space],
            'capacity'      : self.capacity
        }

    @staticmethod
    def dump(events, fname):
        dump_json(fname, [e.as_dict() for e in events if e.meid is not None])

