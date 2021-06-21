#!/usr/bin/python3

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.pyplot import MultipleLocator

width=0.25

x1=[5, 6, 7, 8, 9, 10]
y1=(768*8, 918*8, 1074*8, 1224*8, 1380*8, 1536*8)
x2=[p + width for p in x1]
y2=(730*8, 472*8, 452*8, 514*8, 506*8, 570*8)
plt.bar(x1, y1, label='Appended Update', width=0.25)
plt.bar(x2, y2, label='Appended Update & Reversed Update', width=0.25, hatch='/'*3)
plt.legend()
plt.xlabel('The Percentage of Identical Update Data')
plt.ylabel('Number of Invalid Sectors')

# x1=[10, 20, 30, 40, 50]
# y1=(10.02, 9.99, 9.97, 10.0, 10.02)
# plt.bar(x1, y1, width=3)
# plt.xticks([p + width/2 for p in x1], x1)

# x1=[10, 20, 30, 40]
# y1=(1536*8, 3072*8, 4608*8, 6144*8)
# x2=[p + width for p in x1]
# y2=(1536*8, 3072*8, 4608*8, 6144*8)
# plt.bar(x1, y1, label='Appended Update')
# plt.bar(x2, y2, label='Appended Update & Reversed Update', width=0.25, hatch='/'*3)
# # plt.legend()
# plt.xlabel('The Percentage of Identical Update Data')
# plt.ylabel('Number of Invalid Sectors')

# plt.title('ETF Dividend Yield')
plt.xlabel('Update Ratio (%)')
plt.ylabel('Latency (ms)')
plt.savefig("bar")