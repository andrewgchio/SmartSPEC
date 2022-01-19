from datetime import datetime, timedelta
from collections import defaultdict
from itertools import tee, chain

import pandas as pd
import math
import matplotlib.pyplot as plt
from tqdm import tqdm
import json

import ruptures as rpt

from model.Event import Event
from model.MetaEvent import MetaEvent

from utils.smooth import simple_moving_avg, exponential_moving_avg
from utils.timeprofile import TimeProfile as TP
from utils.timeprofile import TimeProfileEntry as TPE
from utils.timeprofile import PatDur as PD
from utils.utils import mean_std, str_mins, jaccard_index 

class EventsLearner:
    def __init__(self, conn, start, end, 
                unit=5, validity=10, window=10, smooth='EMA', 
                time_thresh=30, occ_thresh=20, plots=None, data=None):
        '''
        Learn events between the dates start and end

        :param conn: A connection to a MySQL database
        :param start, end: date range from which to learn
        :param unit: Integer. Number of minutes to group connection events 
        :param validity: Integer. Number of minutes for which the client will 
                stay around the WiFi AP.
        :param window: An integer representing window/span to perform SMA/EMA
        :param smooth: A smoothening technique to round spikes in occupancy plot
        :param time_thresh: Integer Minutes. Time threshold to generate event
        :param occ_thresh: Occupancy threshold to generate event
        '''
        self.conn        = conn
        self.start       = start
        self.end         = end
        self.unit        = unit
        self.validity    = validity
        self.window      = window
        self.smooth      = smooth
        self.time_thresh = time_thresh
        self.occ_thresh  = occ_thresh
        self.plots       = plots
        self.data        = data

        if self.data is not None: # A string (filename) was entered
            self.data = pd.read_csv(self.data)
            self.data['time'] = self.data['time'].apply(
                        lambda x : datetime.strptime(x, '%Y-%m-%d %H:%M:%S'))
            self.data['day'] = self.data['time'].apply(lambda x : x.date())

    def get_obs_on_date(self, date):
        '''
        Get Connection observations for a given date

        :param date: String. e.g., '2017-11-28'. Requested date. 
        :return: a dict of wifi-ap to list of observations [(cnx_time, cid)]
        '''
        if self.data is not None:
            X = self.data[self.data['day'] == date]

            cursor = list((str(r['SpaceID']),r['time'],r['user']) 
                        for _,r in X.iterrows())
        else:
            cursor = self.conn.lcnx.cursor()

            cursor.execute(f"""\
SELECT * FROM wifi_connection 
WHERE cnx_time BETWEEN '{date}' AND '{date} 23:59:59.99'""")

        obs_groups = defaultdict(list)
        for wifi_ap, cnx_time, cid in cursor:
            obs_groups[wifi_ap].append((cnx_time, cid))
        return obs_groups

    def obs_to_time_series(self, obs):
        '''
        Convert the observations for a WiFi AP to a time series with validity 
        of observations applied.

        :param obs: A list of observations (cnx_time, client_id) from a WiFi AP.
        :return: series. A pandas data frame with two columns:
                'time' is the starting time
                'count_validity_#' is the number of distinct connecting clients 
                    with validity applied.
        :return: col. A column name.
        '''
        df = pd.DataFrame(dtype='int8', 
                          index=range(math.ceil(24*60 / self.unit)))
        for cnx_time, cid in obs:
            mins = 60*cnx_time.hour + cnx_time.minute
            lo = math.floor(mins / self.unit)
            hi = math.floor((mins + self.validity) / self.unit)
            df.loc[lo:hi, cid] = 1

        col = f'count_validity_{self.validity}'
        return pd.DataFrame(data={ \
                'time': range(0, 24*60, self.unit), 
                col : df.sum(axis=1)}), \
                col


    def make_smooth_obs(self, df, col):
        '''
        Apply a smoothening function on dataframe[col]

        :param df: The dataframe to work with
        :param col: The column containing the data
        '''
        if self.smooth == 'EMA':
            return exponential_moving_avg(df, self.window, col)
        elif self.smooth == 'SMA':
            return simple_moving_avg(df, self.window, col)
        else:
            return df, col

    def change_detection(self, series, show=False, save=False, 
            fname=None):
        '''
        Execute change point detection algorithm (ruptures) on the given series

        :param series: Numpy array to do execute change point detection on
        :param show: Boolean. Plot the detected breakpoints.
        :param save: Boolean. Save the plot in {fname}
        :param fname: String. Filename to save plot in; will save in plots dir
        :return: breakpoints List of breakpoints (e.g. [155, 185, ..., 480]). 
                Represents the points where a change happens.
        ''' 
        if all(series == 0):
            return [288] # end of breakpoints
        model = rpt.Pelt(model="rbf", min_size=self.time_thresh // self.unit)
        bkpts = model.fit_predict(series, pen=0.3)
        if show or save:
            fig, axarr = rpt.display(series, bkpts)
            ax = axarr[0]
            ticks = range(0, series.shape[0], 60//self.unit)
            ticklabels = [f'{x*self.unit//60}:00' for x in ticks]
            ax.set_xticks(ticks)
            ax.set_xticklabels(ticklabels, rotation=45)
            ax.set_xlabel('Time of day')
            ax.set_ylabel('Number of clients')
            ax.set_title(f'Learned Events')
            if fname:
                plt.savefig(f'{self.plots}/{fname}.png', bbox_inches='tight')
            if show:
                plt.show()
            plt.close()
        return bkpts

    def split_bkpts(self, series, bkpts):
        '''
        Generator that interprets the set of breakpoints created from the change
        point detection algorithm and yields the appropriate start/end time in 
        a number of intervals for the day

        :param series: A series of occupancy data
        :param bkpts: A list of breakpoints as produced by the change point 
                detection algorithm
        '''
        start, end = tee(bkpts)
        for s,e in zip(chain((0,), start), end):
            yield (s*self.unit, e*self.unit-1), series[s:e]

    def _gen_events(self, obs, date, wifi_ap):
        '''
        Generate events based on a time series for a given date, wifi-ap. 

        Our pipeline is as follows: 
            1. Raw Connections
            2. Connections for the given wifi_ap
            3. Event detection with specified smoothing function
            4. Events List generated

        :param obs: Set of observations
        :param date: String representing a date (e.g., '2017-11-28')
        :param wifi_ap: String representing a wifi-ap
        :return: elist, a list of event instances
        '''
        # Smoothen observations
        df, col = self.obs_to_time_series(obs)
        df, newcol = self.make_smooth_obs(df, col)

        # Generate list of breakpoints
        series = df.loc[:, col].to_numpy()
        bkpts = self.change_detection(series, save=True,
                                      fname=f'date_{date}_wifi-ap_{wifi_ap}')
        # Add events
        elist = []
        for (s,e),X in self.split_bkpts(series, bkpts):
            size = list(map(lambda x : int(math.ceil(x)), mean_std(X, n=1)))
            if size[0] > self.occ_thresh:
                Nreq = 0.6 * (e-s), 0.1 * (e-s) # TODO: change? 
                sdt, edt = f'{date} {str_mins(s)}', f'{date} {str_mins(e)}'
                elist.append(
                    Event(cids={cid for dt,cid in obs if sdt <= str(dt) <= edt}, 
                          tp=TPE.basic(date, s,e, Nreq, 0), 
                          space=wifi_ap)
                )
 
                print(elist[-1]) # Print most recent event

        return elist

    def gen_all_events(self, wifi_aps):
        '''
        Loop over all dates and given wifi_aps and produce a list of all events

        :param wifi_aps: The set of wifi-aps for which we generate events
        :return: all_events. The list of all events found
        '''
        print('Learning Events')
        curr_dt = self.start.date()
        end_dt  = self.end.date()
        day = timedelta(days=1)
        all_events = defaultdict(list)
        while curr_dt <= end_dt:
            print(f'... current date: {curr_dt}', flush=True)
            all_obs = self.get_obs_on_date(curr_dt)
            for i, wifi_ap in enumerate(tqdm(wifi_aps)):
                #print(f'Events({curr_dt}, {wifi_ap}): ({i}/{len(wifi_aps)})')
                all_events[wifi_ap].extend(
                        self._gen_events(all_obs[wifi_ap], curr_dt, wifi_ap))
            curr_dt += day
            print('AllEvents len: ', sum(map(len, all_events.values())), flush=True)
        return all_events

    def gen_metaevents(self, fspaces):
        '''
        Given a set of wifi-aps, learn events, and group them into metaevents

        :param wifi_aps: The set of wifi-aps for which we generate events
        :return: metaevents. The list of all metaevents learned. 
        '''
        with open(fspaces) as f:
            wifi_aps = [x.strip() for x in f]
        events_by_wifi_aps = self.gen_all_events(wifi_aps)
        events_by_id = {}
        metaevents = []
        for wifi_ap, events in events_by_wifi_aps.items():
            for e in events:
                events_by_id[e.id] = e
            for elist in self._group_events(events):
                metaevents.append(MetaEvent.parse_from_list(elist))
        return events_by_id, metaevents

    def _group_events(self, events, time_diff_ratio=0.8, occ_diff_ratio=0.2,
                        min_group=2):
        '''
        The main logic for grouping events together when generating metaevents. 
        We call two events similar if they occur in the same space, are similar
        in time, and have similar attendees

        :param events: The set of events to group
        :param time_diff_ratio: The difference in event times. Jaccard Index.
        :param occ_diff_ratio: The difference in attendees. Jaccard index.
        '''
        small = []
        count = 0
        while events:
            grouped = [events.pop()]
            unmatched_events = []
            for e in events:
                if TPE.diff(grouped[0].tp, e.tp) >= time_diff_ratio and \
                    jaccard_index(grouped[0].cids, e.cids) >= occ_diff_ratio:
                    grouped.append(e)
                else:
                    unmatched_events.append(e)
            events = unmatched_events
            # Ignore groups of events that are too small 
            # if len(grouped) >= min_group:
            yield grouped
            # else:
                # count += 1
            # else:
                # small.extend(grouped)
        '''
        # Forcibly group events that are in smaller groups
        while small:
            group = [small.pop()]
            H = Heap(small, key=lambda x : 
                        (jaccard_index(group[0].cids, x.cids), x.id))
            while len(group) <= min_group:
                group.append(H.pop())
            yield group

            if len(small) <= min_group:
                yield group
                break
        '''

    def update_event_capacity(self, events_by_id, metaevents, people):
        # TODO: perhaps this can be obtained in obs_to_time_series
        event_capacities = defaultdict(lambda : defaultdict(int))
        for a in people:
            for eid in a.eids:
                event_capacities[eid][a.mid] += 1
        for eid, events in events_by_id.items():
            ma_map = event_capacities[eid]
            events.capacity = [{'metapeople-id' : mid, 'range' : [0, ct]}
                                for mid, ct in ma_map.items() 
                                if mid is not None]

        for me in metaevents:
            me_cap = defaultdict(list)
            for eid, mid_capacities in event_capacities.items():
                for mid, ct in mid_capacities.items():
                    me_cap[mid].append(ct)
            me.capacity = [{'metapeople-id' : mid, 
                            'lo' : [0, 0], 
                            'hi' : list(map(lambda x : int(math.ceil(x)), mean_std(cts)))}
                            for mid, cts in me_cap.items()
                            if mid is not None]


