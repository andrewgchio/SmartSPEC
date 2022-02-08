from collections import defaultdict

from itertools import groupby

import numpy as np

from sklearn.metrics import pairwise_distances
from sklearn.cluster import AgglomerativeClustering

from model.Person import Person
from model.MetaPerson import MetaPerson

from utils.timeprofile import TimeProfile as TP
from utils.timeprofile import TimeProfileEntry as TPE
from utils.timeprofile import PatDur as PD
from utils.utils import jaccard_index 

class PeopleLearner:
    def __init__(self):
        pass # Nothing to initialize

    def find_attendees(self, events):
        attendees = defaultdict(set)
        for e in events:
            for cid in e.cids:
                attendees[cid].add(e.id)
        return attendees

    def gen_people(self, events_by_id, metaevents):
        '''
        Learn about people from looking at partitions of events
        '''
        people = []

        attendees = self.find_attendees(events_by_id.values())
        for cid, eids in attendees.items():
            pd = PD.merge(pd for eid in eids for pd in events_by_id[eid].tp)
            capacity = defaultdict(int)
            for eid in eids:
                capacity[events_by_id[eid].meid] += 1

            people.append(Person(cid, TP(pd), capacity, eids))

            # print(people[-1])

        return people

    
    def gen_metapeople(self, events_by_id, metaevents):
        people = self.gen_people(events_by_id, metaevents)

        metapeople = []
        for plist in self._group_people(people):
            metapeople.append(MetaPerson.parse_from_list(plist))

        return people, metapeople

    def _group_people(self, people, time_diff_ratio=0.8, event_diff_ratio=0.2,
                        min_group=3):
        '''
        The main logic for grouping people together when generating metapeople.
        We call two people similar if they have similar time profiles, and 
        attend similar events. 

        :param people: The set of people to group
        :param time_diff_ratio: The difference in people's start/end times. 
        :param event_diff_ratio: The difference in event attendance
        :param min_group: The minimum group size
        '''
        if not people: # no people to group
            return []
        if len(people) == 1:
            return [people[0]]

        # The current set of people
        people_by_id = {p.id : p for p in people}

        def people_dist(pidlist1, pidlist2):
            '''
            Determine the distance between people
            :param pidlist1, pidlist2: pids of people to compare
            '''
            p1, p2 = people_by_id[pidlist1[0]], people_by_id[pidlist2[0]]
            eids_dist = jaccard_index(p1.eids, p2.eids)
            return eids_dist

        pids = np.array(list(people_by_id)) # all ids
        pdists = pairwise_distances(pids.reshape(-1,1), metric=people_dist,
                                    force_all_finite=False)
        ac = AgglomerativeClustering(distance_threshold=0.3, n_clusters=None,
                                     affinity='precomputed', linkage='average')
        ac.fit(pdists)
        for _,gp in groupby(sorted(zip(ac.labels_,pids)), key=lambda x : x[0]):
            yield list(map(lambda x : people_by_id[x[1]], gp))

        '''
        # A simple heuristic to consider if Agglomerative Clustering is slow
        # Attempt to group people based on the events they attend
        small = []
        count = 0
        while people:
            group = [people.pop()]
            unmatched_people = []

            for p in people:
                if jaccard_index(group[0].eids, p.eids) >= event_diff_ratio:
                    group.append(p)
                else:
                    unmatched_people.append(p)
            people = unmatched_people
            yield group
            # Ignore groups that are too small
            # if len(group) >= min_group: 
            #     yield group
            # else:
            #     count += 1
            # else:
                # small.extend(group)
        print(f'Number of people removed: {count}', flush=True)
        '''

