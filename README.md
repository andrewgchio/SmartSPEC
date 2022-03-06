# SmartSPEC 

## Project Description<a id="description"></a>

*SmartSPEC* is a smart space simulator and data generator that creates customizable smart space datasets using semantic models of spaces, people, events and sensors. We employ ML-based approaches to characterize and learn attributes of the embedded people and events in a sensorized space and apply an event-driven simulation strategy to generate realistic simulated data about the space (events, trajectories, sensor datasets, etc.).

The *SmartSPEC* architecture consists of two main components:

* *Scenario Learning*, which uses input seed connectivity data and a priori knowledge of the underlying space and sensors to learn high-order concepts of events, people and trajectories, which we refer to as "metaevents", "metapeople" and "metatrajectories", respectively; and

* *Scenario Generation*, which takes *SmartSPEC* data to generate a synthetic dataset from which a smart space dataset (e.g., trajectory dataset, sensor observation dataset, etc.) can be derived. We use the variations of the [data models](#generation-data-models) described below to define various scenarios, which drives the generation of new observable phenomena in the smart space. 

<p align="center">
    <img src="https://user-images.githubusercontent.com/16398500/152928317-698204c2-6e37-44c3-ad1b-75fa8c486f01.png" alt="SmartSPEC Architecture">
</p>


## Requirements<a id="requirements"></a>

The Scenario Learning component of *SmartSPEC* requires the following: 

* [Python](https://www.python.org) (version >= 3.6.8)
* [numpy](https://numpy.org)(version >= 1.19.2)
* [pandas](https://pandas.pydata.org) (version >= 1.1.0)
* [matplotlib](https://matplotlib.org) (version >= 3.2.2)
* [scikit-learn](https://scikit-learn.org/stable) (version >= 0.24.2)
* [python-rapidjson](https://github.com/python-rapidjson/python-rapidjson) (version >= 1.5)
* [tqdm](https://github.com/tqdm/tqdm) (version >= 4.62.3)
* [ruptures](https://ctruong.perso.math.cnrs.fr/ruptures-docs/build/html/index.html) (for break point detection) (version >= 1.1.5)
* [MySQL Python Connector](https://dev.mysql.com/doc/connector-python/en) (version >= 8.0.13)

Note that most of these requirements can be fulfilled by using an [Anaconda](https://www.anaconda.com) environment. After installing Anaconda, open the Anaconda command line and execute: 
```
conda create --name smartspec
conda activate smartspec
conda install python-rapidjson numpy pandas matplotlib tqdm scikit-learn 
pip install ruptures mysql-connector-python
``` 

Note that there may be some secondary dependencies to install; please refer to the installation guides for the appropriate dependency if there are issues. 

A local MySQL database is used as the data source for learning metaevents and metapeople. To set up the local data source: 

* Start your local MySQL service 
* Create a database named `simulation_seed` with a table `connectivity`. This can be accomplished with the following SQL script. 
```sql
CREATE TABLE simulation_seed.connectivity 
(
    wifi_ap VARCHAR(32) NULL,
    cnx_time DATETIME NULL,
    client_id VARCHAR(64) NULL
);
```

* Populate table by importing data. This data should be in the following form:
```
wifi_ap,cnx_time,client_id
1,2017-01-01 07:30:31,81
9,2017-01-01 10:39:13,72
8,2017-01-01 10:40:08,72
...
```

Please note that you may need to modify the default security settings of MySQL in order to create the MySQL database (e.g., configure database user to authenticate via `mysql_native_password`. 

* Note, the local data queries can be optimized by setting indices as follows: 
```sql
CREATE INDEX cnx_date ON simulation_seed.connectivity (cnx_time);
CREATE INDEX idx_ap ON simulation_seed.connectivity (wifi_ap);
CREATE INDEX time_and_ap ON simulation_seed.connectivity (cnx_time, wifi_ap);
```

The Scenario Generation component of *SmartSPEC* requires

* C++ (version >= 17)
* [boost](https://www.boost.org/) (version >= 1.68.0)
* [date](https://github.com/HowardHinnant/date) (included in project)
* [rapidjson](https://github.com/Tencent/rapidjson) (included in project)

## Using SmartSPEC

SmartSPEC provides three modes of operation to generate synthetic data varying in the level of user involvement/automation (see figure below). The steps to use our system are as follows:

* Define the simulated space and its embedded sensors (1).
* Define MetaPeople and MetaEvents manually (2a) or automatically (2b).
* Define specific people and events based on the previous metamodels manually (3a) or automatically (3b).
* Configure the simulation and automatically generate the synthetic dataset (4).

<p align="center">
    <img src="https://user-images.githubusercontent.com/16398500/152927616-787abd34-42ba-4776-be5d-b44792c12a34.svg" alt="SmartSPEC Workflows")
</p>

## Scenario Learning: Configurations and Data Models <a id="learning-data-models"></a>

### Config File

`Config.txt` is a configuration file used to run SmartSPEC in the scenario learning phase. It lists many learning parameters, as shown below: 

```
[learners]
start       = DateStr
end         = DateStr
unit        = int
validity    = int
smooth      = str (one of "EMA", "SMA")
window      = int
time-thresh = int
occ-thresh  = int

[filepaths]
spaces     = Path
metaevents = Path
metapeople = Path
people     = Path
events     = Path
plots      = Path
```

In the `learners` section, `start` and `end` refer to the start and end dates (expressed as `'YYYY-MM-DD'`) for learning. `unit` denotes the number of minutes to group connection events. `validity` refers to the number of minutes for which the client will stay around the sensor. `smooth` and `window` are used to indicate the type of smoothening function to apply to the occupancy graphs; use `smooth=SMA` to apply a simple moving average and `smooth=EMA` to apply an exponential moving average. The `time-thresh` determines a minimum duration (minutes) required to realize an event. `occ-thresh` determines the minimum number of attendees required to realize an event. 

The relative paths to files used as input / produced as output should be specified in the `filepaths` section. `plots` is a directory where plots of learned events will be saved. 

Example:
```
[learners]
start       = 2017-04-01
end         = 2017-05-01
unit        = 5
validity    = 10
smooth      = EMA
window      = 10
time-thresh = 30
occ-thresh  = 1

[filepaths]
spaces     = data/demo/Spaces.json
metaevents = data/demo/MetaEvents.json
metapeople = data/demo/MetaPeople.json
people     = data/demo/People.json
events     = data/demo/Events.json
plots      = data/demo/plots

```

### Input Seed Data

Please read the requirements for Scenario Learning mentioned [here](#requirements). 

## Scenario Generation: Configurations and Data Models <a id="generation-data-models"></a>

### Config File
Another `Config.txt` is used as a configuration file used to run SmartSPEC in the scenario generation phase. It lists many simulation parameters, as shown below: 

```
[people]
number = int
generation = str (one of "none", "diff", "all")

[events]
number = int
generation = str (one of "none", "diff", "all")

[synthetic-data-generator]
start = DateStr
end   = DateStr

[filepaths]
metapeople          = Path
metaevents          = Path
people              = Path
events              = Path
spaces              = Path
sensors             = Path
output              = Path
generated-files     = Path
shortest-path-cache = Path
```

In the `people` section, `number` refers to the number of people to simulate and `generation` refers to the manner in which new people (if any) should be added. If `generation=none`, then `number` is ignored and the people specified in `filepaths/people` will be used. If `generation=diff`, then one of each metaperson will first be generated (up to `number`), then additional people will be added (up to `number`). If `generation=all`, then `number` people will be generated using metapeople. The options `number` and `generation` work similarly in the `events` section.

In the `synthetic-data-generator` section, `start` and `end` refer to strings of the form `'YYYY-MM-DD'` that denote the start and end date of the simulation. 

The relative paths to files used as input / produced as output should be specified in the `filepaths` section. Note that `shortest-path-cache` is a cache file used to store shortest paths between spaces (a default for determining trajectories between spaces).

Example: 
```
[people]
number = 500
generation = all

[events]
number = 5000
generation = diff

[synthetic-data-generator]
start = 2018-01-08
end   = 2018-01-29

[filepaths]
metapeople          = data/demo/MetaPeople.json
metaevents          = data/demo/MetaEvents.json
people              = data/demo/People.json
events              = data/demo/Events.json
spaces              = data/demo/Spaces.json
sensors             = data/demo/Sensors.json
output              = data/demo/output/
generated-files     = data/demo/
shortest-path-cache = data/demo/output/shortest-path-cache.csv
```

### Spaces File
`Spaces.json` describes a logical space in SmartSPEC. It is encoded by the following JSON formatted file: 

```
[
    {
        "id" : int,
        "description" : str (default=""),
        "coordinates" : [int, int, int],
        "capacity" : int (default=-1),
        "neighbors" : [int]
    }, 
    ...
]
```

The `id` property should uniquely identify a space. 

The `description` property is optional and provides a user-friendly name for the space. 

The `coordinates` property denotes x,y,z coordinates. The coordinate system is arbitrarily defined by the user. 

The `capacity` property denotes the maximum number of people that can be in the space at the same time. `-1` is a special value denoting that a space has infinite capacity. 

Example: 
```
[
    {
        "id" : 1,
        "description" : "first floor lobby",
        "coordinates" : [30, 50, 10],
        "capacity" : 30,
        "neighbors" : [2, 3]
    },
    ...
]
```

Special note: You must also define an extra space entry with `id=0` denoting the outside space (conceptually representing when a person is outside of the simulated space). 

### People File
`People.json` describes a person in SmartSPEC. It is encoded by the following JSON formatted file: 

```
[
    {
        "id" : int,
        "metaperson-id" : int,
        "description" : str (default=""),
        "profile-index" : int
    },
    ...
]
``` 

The `id` property should uniquely identify a person.

The `metaperson-id` property determines the metaperson group to which the person belongs. 

The `description` property is optional and provides a user-friendly name for the person. 

The `profile-index` property denotes the time profile index to use in the associated metaperson entry. 

Example: 
```
[
    {
        "id" : 14,
        "metaperson-id" : 2,
        "description" : "Alice",
        "profile-index" : 0
    },
    ...
]
```

### Events File
`Events.json` describes a semantic event in SmartSPEC. It is encoded by the following JSON formatted file: 

```
[
    {
        "id" : int,
        "metaevent-id" : int,
        "description" : str (default=""),
        "profile-index" : int,
        "space-ids" : [int],
        "capacity" : [
            {
                "metaperson-id" : int, 
                "range" : [int, int]
            }, 
            ...
        ]
    },
    ...
]
```

The `id` property should uniquely identify an event.

The `metaevent-id` property determines the metaevent group to which the event belongs. 

The `description` property is optional and provides a user-friendly name for the event. 

The `profile-index` property denotes the time profile index to use in the associated metaevent entry. 

The `space-ids" property denotes the spaces in which the event can take place. 

The `capacity` property denotes the number of people of each type of metaperson. The `range` property specifies bounds (`[lo, high]`) on the number of people in attendance. 

Example:
```
[
    {
        "id" : 20,
        "metaevent-id" : 12,
        "description" : "Computer Lab #5",
        "profile-index" : 1,
        "space-ids" : [183, 189, 192],
        "capacity" : [
            {
                "metaperson-id" : 1,
                "range" : [2, 8]
            },
            {
                "metaperson-id" : 2,
                "range" : [0, 1]
            },
            ...
        ]
    },
    ...
]
```

Special Note 1: A special event with `id=0` denoting the "leisure" event should be defined. The leisure event serves as a default event that a person will attend if they are unable to attend any other event. 

Special Note 2: Additionally, a special event with `id=-1` denoting an "out-of-simulation" event will be also defined automatically; it serves as a placeholder event to indicate that a person is arriving / leaving the simulated space at the start/end of the day, respectively. 

### Sensors File
`Sensors.json` describes both in-situ and mobile sensors in SmartSPEC. It is encoded by the following JSON formatted file: 

```
[
    {
        "id" : int,
        "description" : str (default="")
        "mobility" : str (either "static" or "mobile"),
        "coverage" : [int] or int,
        "interval" : int
    },
    ...
]
```

The `id` property should uniquely identify a sensor.

The `description` property is optional and provides a user-friendly name for the sensor. 

The `mobility` property is a string taking on one of the values `"static"` or `"mobile"`. An in-situ sensor is modeled with `mobility="static"` and a list of covered spaces `coverage=[int]`.

The `interval` property determines the periodic interval in minutes for which the sensor produces observations. 

Example:
```
[
    {
        "id" : 3,
        "description" : "hallway"
        "mobility" : "static",
        "coverage" : [40, 41, 43],
        "interval" : 60
    },
    ...
]
```

### MetaPeople File
`MetaPeople.json` describes a characterization of a group of similar people. It is encoded by the following JSON formatted file: 

```
[
    {
        "id" : int,
        "description" : str (default=""),
        "probability" : int or double,
        "time-profiles" : [
            {
                "probability" : int or double,
                "profile" : TimeProfile
            }, 
            ...
        ],
        "event-affinity" : [
            {
                "metaevent-id" : int,
                "probability" : int or double
            },
            ...
        ]
    },
    ...
]
```

The `id` property should uniquely identify a metaperson.

The `description` property is optional and provides a user-friendly name for the metaperson. 

The `probability` property denotes the proportion of the population that the metaperson represents. It is used to generate additional people. This number is normalized if the sum of probabilities across metapeople is greater than 1. 

The `time-profiles` property denotes a list of time profiles `profile`, each with a given probability `probability` of selection. See [below](#time-profile) for a description of a time profile. The probabilty is normalized if the sum across time profiles is greater than 1. The time profiles denote the days and times for which a person enters/exits the simulated space. 

The `event-affinity` property denotes the probabilities with which people in this group of metapeople will attend events. The probabilities specified here should be relative to each other. 

Example:
```
[
    {
        "id" : 22,
        "description" : "student",
        "probability" : 15,
        "time-profiles" : [
            {
                "probability" : 1,
                "profile" : [
                    {
                        "pattern": {
                            "start-date" : "2018-01-11",
                            "end-date" : "2018-02-05",
                            "period" : "week",
                            "period-details" : {
                                "days-of-week" : [4]
                            }
                        },
                        "duration" : {
                            "start-time" : ["09:35:00", "00:10:00"],
                            "end-time" : ["12:20:00", "00:10:00"],
                            "required" : ["02:00:00", "00:15:00"]
                        }
                    }
                ]
            }, 
            ...
        ],
        "event-affinity" : [
            {
                "metaevent-id" : 5,
                "probability" : 10
            },
            {
                "metaevent-id" : 7,
                "probability" : 20
            },
            ...
        ]
    },
    ...
]
```

### MetaEvents File
`MetaEvents.json` describes a characterization of a group of similar events. It is encoded by the following JSON formatted file: 

```
[
    {
        "id" : int,
        "description" : str (default=""),
        "probability" : int or double,
        "spaces" : {
            "space-ids" : [int],
            "number" : int
        },
        "time-profiles" : [
            {
                "probability" : int or double,
                "profile" : TimeProfile
            }, 
            ...
        ],
        "capacity" : [
            {
                "metaperson-id" : int,
                "lo" : [int, int],
                "hi" : [int, int]
            },
            ...
        ]
    },
    ...
]
```

The `id` property should uniquely identify a metaevent.

The `description` property is optional and provides a user-friendly name for the metaevent. 

The `probability` property denotes the number of events in the metaevent group. It is used to generate additional events. The number is normalized to become a proportion. 

The `spaces` property denotes the potential spaces for which events in this metaevent group take place. In the generation process, `number` spaces are selected at random from the `space-ids` list. 

The `time-profiles` property denotes a list of time profiles `profile`, each with a given probability `probability` of selection. See [below](#time-profile) for a description of a time profile. The probabilty is normalized if the sum across time profiles is greater than 1. The time profiles denote the days and times for which an event occurs. 

The `capacity` property denotes the attendance ranges for different types of metapeople. The `lo` and `hi` properties denote mean and standard deviation parameters for a Normal distribution. 

Example:
```
[
    {
        "id" : 8,
        "description" : "Operating Systems Lecture"
        "probability" : 10,
        "spaces" : {
            "space-ids" : [1000],
            "number" : 1
        },
        "time-profiles" : [
            {
                "probability" : 1.0,
                "profile" : [
                    {
                        "pattern": {
                            "start-date" : "2018-01-08",
                            "end-date" : "2018-03-06",
                            "period" : "week",
                            "period-details" : {
                                "days-of-week" : [1,3,5]
                            }
                        },
                        "duration" : {
                            "start-time" : ["09:00:00", "00:02:00"],
                            "end-time" : ["09:50:00", "00:02:00"],
                            "required" : ["00:45:00", "00:05:00"]
                        }
                    }
                ]
            },
            ...
        ],
        "capacity" : [
            {
                "metaperson-id" : 7,
                "lo" : [30, 5],
                "hi" : [200, 30]
            },
            ...
        ]
    },
    ...
]
```

### TimeProfile<a id="time-profile"></a>
`TimeProfile`: Used for both MetaPeople and MetaEvents; it denotes an "active" time period for an "active" day. The semantics of an "active" day and time depends on the object it describes: an active day/time for an event denotes when the event occurs, and an active day/time for a person denotes the expected time of entry/exit from the simulated space. It is encoded as follows: 

```
[
    {
        "pattern" : {
            "start-date" : DateStr,
            "end-date" : DateStr,
            "period" : See below,
            "period-details" : See below
        },
        "duration" : {
            "start-time" : [TimeStr, TimeStr],
            "end-time" : [TimeStr, TimeStr],
            "required" : [TimeStr, TimeStr]
        }
    },
    ...
]
```

The `start-date` and `end-date` properties are strings in the form of `'YYYY-MM-DD'` and denote the start/end dates of the time profile pattern. The `period` and `period-details` properties are explained below. 

The `start-time` and `end-time` properties are strings in the form `'HH:MM:SS'` and denote the start/end times for a given active day. The 2-list represents a mean time and standard deviation time as parameters to a Normal distribution.

The `required` property denotes the minimum duration of an active period -- for people, this denotes the minimum amount of time that they must be able to attend an event before committing, and for events, this denotes the minimum amount of time that the event spans. The `required` property is defined by a mean time and standard deviation time, also as parameters to a Normal distribution. 

There are several different values for the attributes `period` and `period-details` as follows:

1. Daily Period: Denotes an active day that "repeats every <repeat-every> days" (e.g., repeats every 3 days)
```
{
    ...
    "period" : "day",
    "period-details" : {
        "repeat-every" : int (default=1)
    }
    ...
}
```

2. Weekly Period: Denotes an active day that "repeats on <days-of-week> of every <repeat-every> week" (e.g., repeats every MWF of every 2 weeks). Note, that the <days-of-week> are integers corresponding to the ISO Week (Mon=1, Sun=7). 
```
{
    ...
    "period" : "week",
    "period-detail" : {
        "repeat-every" : int (default=1),
        "days-of-week" : [int]
    }
    ...
}
```

3. Monthly Period, Pattern A: Denotes an active day that "repeats on <days-of-month> of every <repeat-every> month" (e.g., repeats every 3rd, 5th, and 10th day of every 2 months). 
```
{
    ...
    "period" : "month",
    "period-detail" : {
        "repeat-every" : int (default=1),
        "days-of-month" : [int]
    }
    ...
}
```

4. Monthly Period, Pattern B: Denotes an active day that "repeats on <days-of-week>, in <week-of-month>, every <repeat-every> month" (e.g., repeats on MWF in week 1 of every month). Note, that the <days-of-week> are integers corresponding to the ISO week (Mon=1, Sun=7).
```
{
    ...
    "period" : "month",
    "period-detail" : {
        "repeat-every" : int (default=1),
        "weeks-of-month" : [int]
        "days-of-week" : [int]
    }
    ...
}
```

5. Yearly Period, Pattern A: Denotes an active day that "repeats on <days-of-year>, every <repeat-every> year" (e.g., repeat every 1st day of each year)
```
{
    ...
    "period" : "year",
    "period-detail" : {
        "repeat-every" : int (default=1),
        "days-of-year" : [int]
    }
    ...
}
```

6. Yearly Period, Pattern B: Denotes an active day that "repeats on <days-of-week> in <weeks-of-year>, every <repeat-every> year" (e.g., repeat every MWF in 5th and 10th weeks of every 2nd year)
```
{
    ...
    "period" : "year",
    "period-detail" : {
        "repeat-every" : int (default=1),
        "weeks-of-year" : [int],
        "days-of-week" : [int]
    }
    ...
}
```

7. Yearly Period, Pattern C: Denotes an active day that "repeats on <days-of-month> of every <month-of-year>, every <repeat-every> year" (e.g., repeat on 1st day of every 4nd month of every year)
```
{
    ...
    "period" : "year",
    "period-detail" : {
        "repeat-every" : int (default=1),
        "months-of-year" : [int],
        "days-of-month" : [int]
    }
    ...
}
```

8. Yearly Period, Pattern D: Denotes an active day that "repeats on <days-of-week> in each <weeks-of-month> of <months-of-year>, every <repeat-every> year" (e.g., repeat MWF in each 2nd week of the 5th month, every 4th year)
```
{
    ...
    "period" : "year",
    "period-detail" : {
        "repeat-every" : int (default=1),
        "months-of-year" : [int],
        "weeks-of-month" : [int],
        "days-of-week" : [int]
    }
    ...
}
```

## Running SmartSPEC

To run the Scenario Learning Component: 

Compile: None

Setup: (Recommended) Open the anaconda command prompt and run `conda activate smartspec`

Run: `python main.py <config-file>` in the `scenario-learning` directory


To run the Scenario Generation Component:

Compile (Entity Generator): `g++ -std=c++17 entitygen.cpp -o entitygen` or `make entityGenCompile`

Run (Entity Generator): `entitygen <config-file>`

Compile (Synthetic Data Generator): `g++ -std=c++17 datagen.cpp -o datagen` or `make dataGenCompile`

Run (Synthetic Data Generator): `datagen <config-file>`

## Citations: <a id="citations"></a>

If you use this project, please cite the following paper: 

Andrew Chio, Daokun Jiang, Peeyush Gupta, Georgios Bouloukakis, Roberto Yus, Sharad Mehrotra, Nalini Venkatasubramanian (2022). SmartSPEC: Customizable Smart Space Datasets via Event-driven Simulations. *In International Conference on Pervasive Computing and Communication.*

## License: <a id="license"></a>

```
The MIT License (MIT)

Copyright (c) 2022 Andrew Chio

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```
