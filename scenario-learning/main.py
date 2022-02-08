from pathlib import Path
import builtins
import sys

from model.Event import Event
from model.Person import Person
from model.MetaEvent import MetaEvent
from model.MetaPerson import MetaPerson

from learner.eventslearner import EventsLearner
from learner.peoplelearner import PeopleLearner

from utils.utils import read_config, to_date
from utils.connection import Connection

# from utils.utils import get_all_wifi_aps
# from parameters import *

if __name__ == '__main__':

    # Ensure printing by flushing
    print = lambda *args, **kwargs : builtins.print(*args, flush=True)

    config = read_config(sys.argv[1])

    print(f'Learn: {config["learners"]["start"]}, {config["learners"]["end"]}')

    print(sys.argv)

    if len(sys.argv) > 2 : # file name to data
        el = EventsLearner(
                None, 
                start=to_date(config['learners']['start']), 
                end=to_date(config['learners']['end']), 
                unit=int(config['learners']['unit']), 
                validity=int(config['learners']['validity']), 
                smooth=config['learners']['smooth'],
                window=int(config['learners']['window']),
                time_thresh=int(config['learners']['time-thresh']), 
                occ_thresh=int(config['learners']['occ-thresh']), 
                plots=config['filepaths']['plots'],
                data=config['filepaths']['data'])
        # Do Event Learning
        print('Event Learner setup')
        events_by_id, metaevents = el.gen_metaevents(config['filepaths']['spaces'])
        print('Event Learning done')
        
    else:
        with Connection() as conn:
            print('Connection setup')
            el = EventsLearner(
                    conn=conn, 
                    start=to_date(config['learners']['start']), 
                    end=to_date(config['learners']['end']), 
                    unit=int(config['learners']['unit']), 
                    validity=int(config['learners']['validity']), 
                    smooth=config['learners']['smooth'],
                    window=int(config['learners']['window']),
                    time_thresh=int(config['learners']['time-thresh']), 
                    occ_thresh=int(config['learners']['occ-thresh']),
                    plots=config['filepaths']['plots'])

            # Do Event Learning
            print('Event Learner setup')
            events_by_id, metaevents = el.gen_metaevents(config['filepaths']['spaces'])
            print('Event Learning done')

    print('People Learner setup')
    pl = PeopleLearner()
    people, metapeople = pl.gen_metapeople(events_by_id, metaevents)
    print('People Learner done')

    el.update_event_capacity(events_by_id, metaevents, people)

    print('Done')

    print(f'''\
Summary: 
    start       = {config["learners"]["start"]}
    end         = {config["learners"]["end"]}
    #MetaPeople = {len(metapeople)}
    #MetaEvents = {len(metaevents)}
    #People     = {len(people)}
    #Events     = {len(events_by_id)}

''')

    # Save learned models
    Event.dump(events_by_id.values(), config['filepaths']['events'])
    Person.dump(people,               config['filepaths']['people'])
    MetaPerson.dump(metapeople,       config['filepaths']['metapeople'])
    MetaEvent.dump(metaevents,        config['filepaths']['metaevents'])


