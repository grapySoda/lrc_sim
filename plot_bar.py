#!/usr/bin/python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.pyplot import MultipleLocator

width=0.25

# values = ['10%', '20%', '30%', '40%', '50%']
# x1=[5, 6, 7, 8, 9]
# y1=(4310*8, 7277*8, 8923*8, 9891*8, 10062*8)
# x2=[p + width for p in x1]
# y2=(3427*8, 5264*8, 5692*8, 5570*8, 5264*8)
# plt.bar(x1, y1, label='Appended Update', width=0.25)
# plt.bar(x2, y2, label='Appended Update & Reversed Update', width=0.25, hatch='/'*3)
# plt.xticks(x1,values)
# plt.legend()
# plt.xlabel('The Percentage of Identical Update Data')
# plt.ylabel('Number of Invalid Sectors')



# values = ['10%', '20%', '30%', '40%', '50%', '60%']
# x1=[5, 6, 7, 8, 9, 10]
# y1=(768*8, 918*8, 1074*8, 1224*8, 1380*8, 1536*8)
# x2=[p + width for p in x1]
# y2=(730*8, 472*8, 452*8, 514*8, 506*8, 570*8)
# plt.bar(x1, y1, label='Appended Update', width=0.25)
# plt.bar(x2, y2, label='Appended Update & Reversed Update', width=0.25, hatch='/'*3)
# plt.xticks(x1,values)
# plt.legend()
# plt.xlabel('The Percentage of Identical Update Data')
# plt.ylabel('Number of Invalid Sectors')



# values = ['10%', '20%', '30%', '40%', '50%']
# x1=[4, 8, 12, 16, 20]
# y1=(11.97, 12.05, 12.04, 12.03, 12.03)
# plot = plt.bar(x1, y1, width=3)
# plt.xticks(x1,values)
# for value in plot:
#     height = value.get_height()
#     plt.text(value.get_x() + value.get_width()/2.,
#              1.002*height,'%.2f' %height , ha='center', va='bottom')



values = ['10%', '20%', '30%', '40%', '50%']
x1=[2, 4, 6, 8, 10]
y1=(7307*8, 7185*8, 7121*8, 6923*8, 6810*8)
x2=[p + (width+0.15) for p in x1]
y2=(4172*8, 4918*8, 5587*8, 5705*8, 5807*8)
plot = plt.bar(x1, y1, label='Appended Update', width=0.4)
for value in plot:
    height = value.get_height()
    plt.text(value.get_x() + value.get_width()/2.,
             1.002*height,'%.2f' %height , ha='center', va='bottom')
plot = plt.bar(x2, y2, label='Appended Update & Reversed Update', width=0.4, hatch='/'*3)
for value in plot:
    height = value.get_height()
    plt.text(value.get_x() + value.get_width()/2.,
             1.002*height,'%.2f' %height , ha='center', va='bottom')
# plt.legend()
plt.xticks(x1,values)
plt.xlabel('The Percentage of Identical Update Data')
plt.ylabel('Number of Invalid Sectors')




# plt.title('ETF Dividend Yield')
plt.xlabel('Update Ratio')
# plt.ylabel('Latency (ms)')
plt.ylabel('Invalid Sectors')
plt.savefig("bar")