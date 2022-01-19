import statistics as stats
from itertools import tee
import rapidjson
import numpy as np
from configparser import ConfigParser
from datetime import datetime as dt

def read_config(fname):
    cf = ConfigParser()
    cf.read(fname)

    config = {section : {} for section in cf.sections()}

    for section in cf.sections():
        options = cf.options(section)
        for option in options:
            try:
                config[section][option] = cf.get(section, option)
            except:
                config[section][option] = None
    return config

def dump_json(fname, data, **kwargs):
    with open(fname, 'w') as f:
        rapidjson.dump(data, f, **kwargs)

def str_mins(n):
    '''
    Converts a integer number of minutes to a string time. 
    :param n: Integer. Number of minutes.
    :return: a string representing a time
    '''
    mins, secs = divmod(round(60*n), 60)
    hrs, mins = divmod(mins, 60)
    return f'{int(hrs):02d}:{int(mins):02d}:{int(secs):02d}'

def to_mins(t):
    return t.hour*60 + t.minute

def to_date(datestr):
    return dt.strptime(datestr, '%Y-%m-%d')

def timediff(t1, t2):
    return abs(to_mins(t1) - to_mins(t2))

def mean_std(iterable, n=0):
    '''
    Compute the mean and stdev of some iterable
    :param iterable: an iterable containing numbers
    :return: mean, stdev of the iterable
    '''
    m,s  = tee(iterable)
    mean = stats.mean(m)
    std  = stats.pstdev(s, mean)
    return np.round((mean, std), n)

def min_max(iterable):
    '''
    Compute the min and max of some iterable
    :param iterable: an iterable containing numbers
    :return: min, max element of iterable
    '''
    lo, hi = float('-inf'), float('inf')
    for i in iterable:
        if lo > i:
            lo = i
        if hi < i:
            hi = i
    return lo, hi

def jaccard_index(A, B):
    '''
    Return the Jaccard Index measure of similarity between two sets A, B

    :param A, B: the sets to compare
    '''
    sim = len(A & B)
    return sim / (len(A) + len(B) - sim) if sim else 0

def get_all_wifi_aps(conn, start_date, end_date):
    '''
    Return a list of all wifi aps between the given dates
    :param start_date: String. e.g. '2017-11-28'
    :param end_date: String. e.g. '2018-01-01'
    :return: A list of string. (wifi aps)
    '''
    cursor = conn.lcnx.cursor()

    cursor.execute(f"""\
SELECT DISTINCT(wifi_ap) FROM wifi_connection
WHERE cnx_time BETWEEN '{start_date}' AND '{end_date} 23:59:59.99'""")

    return [wifi_ap[0] for wifi_ap in cursor]

def read_wifi_aps_to_sids():
    wifi_aps = {}
    with open('./data/metadata/wifi-ap-to-sid.csv') as f:
        for line in f:
            wap,sid = line.strip().split(',')
            wifi_aps[wap] = sid
    return wifi_aps
