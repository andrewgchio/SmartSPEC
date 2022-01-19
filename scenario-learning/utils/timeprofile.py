import statistics as stats
import json
from collections import defaultdict

from .utils import str_mins, mean_std

class TimeProfile:
    def __init__(self, *entries):
        self.tps = list(entries)
        self.prs = [1.0 for i in range(len(self.tps))]

    def add(self, tpe, pr=1):
        self.tps.append(tpe)
        self.prs.append(pr)

    def as_dict(self):
        return [{'probability' : pr, 'profile' : tpe.as_dict()} 
                for pr, tpe in zip(self.prs, self.tps)]

    def __getitem__(self, idx):
        return self.tps[idx]

    def __str__(self):
        return f'TP {json.dumps(self.as_dict(), sort_keys=True, indent=4)}'

    

class TimeProfileEntry:
    def __init__(self, *patdur):
        self.tpe = list(patdur)

    @staticmethod
    def basic(date, start_time, end_time, Nrequired, recurring):
        return TimeProfileEntry(
                PatDur.basic(date, start_time, end_time, Nrequired, recurring))

    def add(date, start_time, end_time, Nrequired, recurring):
        self.tpe.append(
            PatDur.basic(date, start_time, end_time, Nrequired, recurring))

    def add_patdur(self, patdur):
        self.tpe.append(patdur)

    def mean_start_time(self):
        return stats.mean(x.start_time[0] for x in self.tpe)

    def mean_end_time(self):
        return stats.mean(x.end_time[0] for x in self.tpe)

    def as_dict(self):
        return [x.as_dict() for x in self.tpe]

    def __getitem__(self, idx):
        return self.tpe[idx]

    def __iter__(self):
        return iter(self.tpe)

    def __str__(self):
        return f'TPE {json.dumps(self.as_dict(), sort_keys=True, indent=4)}'

    @staticmethod
    def diff(tp1, tp2):
        '''
        Return a measure of similarity in time
        0 = not similar, 1 = similar
        
        :param tp1, tp2: two time-profiles to compare
        '''
        t1      = tp1.mean_start_time(), tp1.mean_end_time()
        t2      = tp2.mean_start_time(), tp2.mean_end_time()
        total   = min(t1[0], t2[0]), max(t1[1], t2[1])
        overlap = max(t1[0], t2[0]), min(t1[1], t2[1])
        return max(overlap[1] - overlap[0], 0) / (total[1] - total[0])

class PatDur:
    def __init__(self, start_date, end_date, period, period_details,
                Nstart_time, Nend_time, Nrequired, recurring):
        self.start_date, self.end_date   = start_date, end_date
        self.period, self.period_details = period, period_details
        self.start_time, self.end_time   = Nstart_time, Nend_time
        self.required                    = Nrequired
        self.recurring                   = recurring

    @staticmethod
    def basic(date, start_time, end_time, Nrequired, recurring):
        return PatDur(date, date, 
                    'week', {'days-of-week' : [date.isoweekday()]},
                    (start_time, 0), (end_time, 0), Nrequired, recurring)

    @staticmethod
    def merge(PDlist):
        start_times = defaultdict(list)
        end_times   = defaultdict(list)
        req         = defaultdict(list)
        days        = set()

        for pd in PDlist:
            start_times[pd.start_date].append(pd.start_time[0])
            end_times[pd.end_date].append(pd.end_time[0])
            req[pd.end_date].append(pd.end_time[0]-pd.start_time[0])
            days.add(pd.start_date.isoweekday())
            days.add(pd.end_date.isoweekday())

        sdate  = min(start_times)
        edate  = max(end_times)
        Nstime = mean_std(min(st) for st in start_times.values())
        Netime = mean_std(max(et) for et in end_times.values())
        Nreq   = mean_std(min(rt) for rt in req.values())

        return PatDur(sdate, edate, 'week', {'days-of-week' : list(days)},
                        Nstime, Netime, Nreq, 0)

    def as_dict(self):
       return \
       {
            'pattern' : {
                'start-date'      : str(self.start_date),
                'end-date'        : str(self.end_date),
                'period'          : self.period,
                'period-details'  : self.period_details
            }, 
            'duration' : {
                'start-time'      : [str_mins(self.start_time[0]), 
                                     str_mins(self.start_time[1])],
                'end-time'        : [str_mins(self.end_time[0]), 
                                     str_mins(self.end_time[1])],
                'required'        : [str_mins(self.required[0]), 
                                     str_mins(self.required[1])],
                'recurring'       : self.recurring
            }
        }

    def __str__(self):
        return f'PD {json.dumps(self.as_dict(), sort_keys=True, indent=4)}'
