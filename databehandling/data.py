import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
from scipy import stats
from datetime import timedelta

paths = {
    "with_com": "data/intelligent/",
    "no_com": "data/no_comm/",
    "greedy": "data/greedy/"
}


def load_experiment_data(data_path):
    data = {}
    for path in os.listdir(data_path):
        joined_csv = []
        for file in os.listdir(data_path + path):
            joined_csv.append(
                pd.read_csv(os.path.join(data_path, path, file),
                            delimiter=";"))
        data[path] = pd.concat(joined_csv)
    return data


com_data = load_experiment_data(paths["with_com"])
nocom_data = load_experiment_data(paths["no_com"])
greedy_data = load_experiment_data(paths["greedy"])
print(com_data)


def tct(test):
    return np.max(test["completion time"])


def average_order_completion_time(experiment):
    return pd.concat(experiment.values())["completion duration"].mean()


def median_order_completion_time(experiment):
    return pd.concat(experiment.values())["completion duration"].median()


def do_ttest(expA, expB, col):
    return stats.ttest_ind(expA[col], expB[col])

def get_acts(exp_data):
    seeds = []
    acts = []
    for seed, data in exp_data.items():
        seeds.append(int(seed))
        acts.append(np.mean(data["completion duration"]))
    return seeds, acts


def get_tcts(exp_data):
    seeds = []
    tcts = []
    for seed, data in exp_data.items():
        seeds.append(int(seed))
        tcts.append(np.max(data["completion time"]))
    return seeds, tcts

print(average_order_completion_time(com_data))
print(average_order_completion_time(nocom_data))
print(average_order_completion_time(greedy_data))

print(median_order_completion_time(com_data))
print(median_order_completion_time(nocom_data))
print(median_order_completion_time(greedy_data))

print(
    do_ttest(pd.concat(com_data.values()), pd.concat(greedy_data.values()),
             "completion duration"))

print(get_tcts(greedy_data))

# plt.boxplot([
#     pd.concat(com_data.values())["completion duration"],
#     pd.concat(nocom_data.values())["completion duration"],
#     pd.concat(greedy_data.values())["completion duration"]
# ],
#             labels=["intelligent", "no comms", "greedy"])
#plt.show()

N = 10
xrange = np.arange(N)
width = 0.25
fig, ax = plt.subplots()
for i, dataset in enumerate([com_data, nocom_data, greedy_data]):
    seeds, tcts = get_tcts(dataset)
    inds = map(lambda n: n - 1336, seeds)
    ax.bar(xrange + i*width, tcts, width)
ax.set_xticks(xrange + width)
ax.set_xticklabels([])
ax.set_title("Total completion times by order group")
plt.legend(["Communication", "No communication", "Greedy"], loc="lower right")

plt.savefig("tct_bars.pdf", format="pdf", bbox_inches='tight')

N = 10
xrange = np.arange(N)
width = 0.25
fig, ax = plt.subplots()
for i, dataset in enumerate([com_data, nocom_data, greedy_data]):
    seeds, acts = get_acts(dataset)
    inds = map(lambda n: n - 1336, seeds)
    ax.bar(xrange + i*width, acts, width)
ax.set_xticks(xrange + width)
ax.set_xticklabels([])
ax.set_title("average completion times by order group")
plt.legend(["Communication", "No communication", "Greedy"], loc="lower right")

import tikzplotlib
plt.savefig("act_bars.pdf",format="pdf")

