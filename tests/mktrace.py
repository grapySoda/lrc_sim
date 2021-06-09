#!/usr/bin/python

import argparse
import random
import sys

sys.path.append('.')

from lib import gen_trace

parser = argparse.ArgumentParser()
parser.add_argument("device")

args = parser.parse_args()

# offset = 0
offset = 900000000000           # 900G
size = 1024*1024*1024
step = 4*1024
nr_ops = 2560

case = 0                        # [Sequential]  offset = 0 M -> 10 M, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 1                        # [Reverse]     offset = 10 M -> 0 M, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 2                        # [Inplace]     offset = 4 K, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 3                        # [Inner]       offset = 900 G, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 4                        # [Random]      offset = 0 M -> 10 M, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 5                        # [N*step]      offset = 0 M -> 20 M, bs = 4 KiB, size = 10 MiB, step = 8 K

if (case == 0):
        offset = 0                      
        size = 10*1024*1024#*256
        step = 4*1024#*256
        # nr_ops = 2560
        nr_ops = size / step
        offsets = range(offset, offset + size, step)

elif (case == 1):
        offset = 0
        size = 10*1024*1024
        step = 4*1024
        # nr_ops = 2560
        nr_ops = size / step
        offsets = range(offset + size - step, offset  - step, -step)

elif (case == 2):
        offset = 4*1024                 
        size = 10*1024*1024
        step = 4*1024
        # nr_ops = 2560
        nr_ops = size / step
        offsets = []
        for i in range(nr_ops):
                offsets.append(step)

elif (case == 3):
        offset = 900000000000           # 900G
        size = 10*1024*1024
        step = 4*1024
        # nr_ops = 2560
        nr_ops = size / step
        offsets = range(offset, offset + size, step)

elif (case == 4):
        # offset = 536870912000
        # offset = 912680550400
        offset = 0
        size = 10*1024*1024
        # size = 912680550400
        step = 4*1024
        nr_ops = 2560
        # nr_ops = size / step
        offsets = range(offset, offset + size, step)
        random.shuffle(offsets)
        offsets = offsets[:nr_ops]

elif (case == 5):
        n = 3
        offset = 52428800
        size = n * 10*1024*1024
        step = 4*1024
        # nr_ops = 2560
        nr_ops = size / step
        offsets = range(offset, offset + size, n * step)

else:
        print("ERROR")

# offsets = range(offset, offset + size, step)
# offsets = offsets[:nr_ops]

# gen_trace('read', args.device, offsets)
gen_trace('write', args.device, offsets)

