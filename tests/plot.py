#!/usr/bin/python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.pyplot import MultipleLocator

def plot_data2(data):
        # x_major_locator=MultipleLocator(5)
        # y_major_locator=MultipleLocator(0.005)
        plt.figure(3)
        # lower_bound = 0
        # upper_bound = 100
        # lower_bound = 0
        # upper_bound = len(lat_nor)

        x = [int(i) for i in range(0, len(data))]
        y = data / 1000000
        print(y)
        # xmin = 0
        # xmax = 300
        # ymin = 0
        # ymax = 50
        axes = plt.gca()
        # axes.xaxis.set_major_locator(x_major_locator)
        # axes.yaxis.set_major_locator(y_major_locator)
        # axes.set_xlim([xmin,xmax])
        # axes.set_ylim([ymin,ymax])

        plt.grid()
        plt.xlabel("Requests")
        plt.ylabel("Latency (ms)")
        # plt.title("Latency")
        plots = plt.plot(x, y, linewidth=0.5)
        # plt.legend(plots, ('Latency'), loc='best', framealpha=0.5, prop={'size': 'large', 'family': 'monospace'})
        plt.savefig("Latency_data2")

def plot_cdf(data, data2):
        x_major_locator=MultipleLocator(5)
        y_major_locator=MultipleLocator(0.005)
        plt.figure(2)

        # xmin = 0
        # xmax = 56.75
        # ymin = 0.85
        # ymax = 1.05

        axes = plt.gca()
        # axes.xaxis.set_major_locator(x_major_locator)
        # axes.yaxis.set_major_locator(y_major_locator)

        # axes.set_xlim([xmin,xmax])
        # axes.set_ylim([ymin,ymax])

        plt.xlabel("Latency (ms)")
        plt.ylabel("Fraction of Requests")
        # plt.title("CDF")
        plt.grid()
        x = np.sort(data) / 1000000
        y = 1. * np.arange(len(data)) / (len(data) - 1)

        # axes = plt.gca()
        # axes.set_ylim([0,1])

        if (data2 == []):
                plots = plt.plot(x, y, linewidth=2.5)
        else:
                x2 = np.sort(data2) / 1000000
                y2 = 1. * np.arange(len(data2)) / (len(data2) - 1)
                plots = plt.plot(x, y, 'r-', x2, y2, 'g--')
                plt.legend(plots, ('Measured', 'Simulated'), loc='best', framealpha=0.5, prop={'size': 'large', 'family': 'monospace'})

        
        plt.savefig("CDF")
        # plt.show()

def mean_nor(data):
        nor = []
        max = np.max(data)
        min = np.min(data)
        mean = np.mean(data)

        for i in range(len(data)):
                nor.append((data[i] - min) / (max - min))

        return nor
# data_df = pd.read_csv('/home/ccs/test/lrc_sim/paper_result/sim_cmr/rand_write/rand-write_lat.csv')
data_df = pd.read_csv('rand-write_lat.csv')
data_np = data_df.to_numpy()
data_np_t = data_np.T

# data2_df = pd.read_csv('/home/ccs/test/lrc_sim/paper_result/sim_cmr/rand_write/sim-rand-write_lat.csv')
data2_df = pd.read_csv('sim-rand-write_lat.csv')
data2_np = data2_df.to_numpy()
data2_np_t = data2_np.T

pre_offset = 0
bytes_per_track = 1925120
# bytes_per_track = 39917296

lat_np = data_np_t[1]
offset_np = data_np_t[4]
difference = []

lat2_np = data2_np_t[1]
offset2_np = data2_np_t[4]
difference2 = []

case = 2
fixed = 0

# print("offset_max: ", np.max(offset_np))
# print("[Mesured]   Latency mean:", format(round(np.mean(lat_np), 2), ','))
# print("[Simulated] Latency mean:", format(round(np.mean(lat2_np), 2), ','))
print("[Mesured]   Latency mean:", round(np.mean(lat_np) / 1000000, 2), "(ms)")
print("[Simulated] Latency mean:", round(np.mean(lat2_np) / 1000000, 2), "(ms)")

if (fixed):
        for i in range(len(offset_np)):
                offset_np[i] %= bytes_per_track

for i in range(len(offset_np)):
        if (fixed):
                if (offset_np[i] - pre_offset >= 0):
                        distance = offset_np[i] - pre_offset
                else:
                        distance = bytes_per_track - (pre_offset - offset_np[i])
        else:
                distance = abs(offset_np[i] - pre_offset)
        # distance = abs(offset_np[i] - pre_offset)
        difference.append(distance)
        pre_offset = offset_np[i]

difference_np = np.array(difference)
lat_nor = mean_nor(lat_np)
offset_nor = mean_nor(offset_np)
difference_nor = mean_nor(difference_np)
# case = 1

# plt.rcParams["font.family"] = "Times New Roman"

if (case == 0):
        lower_bound = 0
        upper_bound = 100
        # lower_bound = 0
        # upper_bound = len(lat_nor)

        x = [int(i) for i in range(0, len(lat_nor))]
        y1 = lat_nor
        y2 = offset_nor
        # y2 = difference_nor[lower_bound:upper_bound]

        # x = x[lower_bound:upper_bound]

        plt.xlabel("Requests")
        plt.ylabel("Latency (ms)")
        # plt.title("Relative of Latency and Offset")
        plots = plt.plot(x, y1, 'r-', x, y2, 'g-')
        plt.legend(plots, ('Latency (Normalized)', 'Offset(Normalized)'), loc='best', framealpha=0.5, prop={'size': 'large', 'family': 'monospace'})
        plt.savefig("Relative_of_Latency_and_Offset1")
        # plt.show()
elif (case == 1):
        plt.figure(1)

        # xmin = 0
        # xmax = 60
        ymin = 0
        ymax = 12

        x = lat_np / 1000000
        # x = lat_np / 1000000
        # y = offset_np / 1000000
        # y = difference_nor
        y = difference_np / 1000000

        slope_x1 = []
        slope_y1 = []

        slope_x2 = []
        slope_y2 = []

        for i in range(len(x)):
                if (0.99 < y[i] < 1.01):
                        if (x[i] < 10):
                                slope_x1.append(x[i])
                                slope_y1.append(y[i])

                if (1.49 < y[i] < 1.51):
                        if (x[i] < 10):
                                slope_x2.append(x[i])
                                slope_y2.append(y[i])

        slope = (np.mean(slope_x2) - np.mean(slope_x1)) / (np.mean(slope_y2) - np.mean(slope_y1))

        print("slope:", slope)

        plt.grid()
        axes = plt.gca()
        # axes.set_xlim([xmin,xmax])
        axes.set_ylim([ymin,ymax])
        plt.xlabel("Latency (ms)")
        plt.ylabel("Moving Distance (MB)")
        # plt.ylabel("Offset (MB)")
        # plt.title("scatter")
        plt.scatter(x, y, s=1)
        # plt.legend(plots, ('Latency'), loc='best', framealpha=0.5, prop={'size': 'large', 'family': 'monospace'})
        plt.savefig("scatter")

        plot_cdf(lat_np, lat2_np)
        # plt.show()
elif (case == 2):
        lower_bound = 0
        upper_bound = 100
        # lower_bound = 0
        # upper_bound = len(lat_nor)

        x = [int(i) for i in range(0, len(lat_nor))]
        y = lat_np / 1000000
        
        xmin = 0
        xmax = 300
        ymin = 0
        ymax = 50
        axes = plt.gca()
        # axes.xaxis.set_major_locator(x_major_locator)
        # axes.yaxis.set_major_locator(y_major_locator)
        # axes.set_xlim([xmin,xmax])
        axes.set_ylim([ymin,ymax])

        plt.figure(1)
        plt.grid()
        plt.xlabel("Requests")
        plt.ylabel("Latency (ms)")
        # plt.title("Latency")
        plots = plt.plot(x, y, linewidth=0.5)
        # plt.legend(plots, ('Latency'), loc='best', framealpha=0.5, prop={'size': 'large', 'family': 'monospace'})
        plt.savefig("Latency")
        # plt.show()
        aaa=[]
        plot_cdf(lat_np, lat2_np)
        # plot_cdf(lat2_np, aaa)
        plot_data2(lat2_np)
        # plot_cdf(lat_np, aaa)
# elif (case == 3):
#         lower_bound = 0
#         upper_bound = 100
#         # lower_bound = 0
#         # upper_bound = len(lat_nor)

#         x = [int(i) for i in range(0, len(lat_nor))]
#         y = lat2_np / 1000000
        
#         xmin = 0
#         xmax = 300
#         ymin = 0
#         ymax = 50
#         axes = plt.gca()
#         # axes.xaxis.set_major_locator(x_major_locator)
#         # axes.yaxis.set_major_locator(y_major_locator)
#         # axes.set_xlim([xmin,xmax])
#         axes.set_ylim([ymin,ymax])

#         plt.figure(1)
#         plt.grid()
#         plt.xlabel("Requests")
#         plt.ylabel("Latency (ms)")
#         # plt.title("Latency")
#         plots = plt.plot(x, y, linewidth=0.5)
#         # plt.legend(plots, ('Latency'), loc='best', framealpha=0.5, prop={'size': 'large', 'family': 'monospace'})
#         plt.savefig("Latency")
#         # plt.show()
#         aaa=[]
#         plot_cdf(lat2_np, aaa)
#         # plot_data2(lat2_np)
#         # plot_cdf(lat_np, aaa)
else:
        print("ERROR")
print(round(np.min(lat_np), 2))
print(round(np.mean(offset_np), 2))
