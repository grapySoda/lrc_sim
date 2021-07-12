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

random.seed()

# case = 0                        # [Sequential]  offset = 0 M -> 10 M, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 1                        # [Reverse]     offset = 10 M -> 0 M, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 2                        # [Inplace]     offset = 4 K, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 3                        # [Inner]       offset = 900 G, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 4                        # [Random]      offset = 0 M -> 10 M, bs = 4 KiB, size = 10 MiB, step = 4 K
# case = 5                        # [N*step]      offset = 0 M -> 20 M, bs = 4 KiB, size = 10 MiB, step = 8 K
# case = 6                        # [N*step]      offset = 0 M -> 20 M, bs = 4 KiB, size = 10 MiB, step = 8 K
# case = 7                        # [N*step]      offset = 0 M -> 20 M, bs = 4 KiB, size = 10 MiB, step = 8 K
# case = 8                        # [N*step]      offset = 0 M -> 20 M, bs = 4 KiB, size = 10 MiB, step = 8 K
case = 9                        # [N*step]      offset = 0 M -> 20 M, bs = 4 KiB, size = 10 MiB, step = 8 K
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

elif (case == 6):
        # offset = 536870912000
        # offset = 912680550400
        update_ratio = 0.5
        offset = 0
        size = 10*1024*1024
        # size = 912680550400
        step = 4*1024
        # step = 2097152
        # step = 3145728
        # nr_ops = 2560
        nr_ops = size / step
         
        size = int(size * (1 - update_ratio))
        update_ops = int(nr_ops * update_ratio)
        # print("size", size)
        # print("update_ops", update_ops)
        offsets = range(offset, offset + size, step)
        # random.shuffle(offsets)

        offsets_append = []
        for i in range(update_ops):
                # print("update_ops", update_ops, "len(offsets)", len(offsets))
                if (update_ops > len(offsets)):
                        upper = len(offsets)
                else:
                        upper = update_ops
                k = random.randint(0, upper)
                offsets_append.append(offsets[k])

        # print("len(offsets)", len(offsets))

        for i in range(update_ops):
                offsets.append(offsets_append[i])
        # print("len(offsets)", len(offsets))
        random.shuffle(offsets)
        # offsets = offsets[:nr_ops]

elif (case == 7):
        # offset = 536870912000
        # offset = 912680550400
        update_ratio = 0.4
        offset = 0
        size = 10*1024*1024*768
        # size = 912680550400
        # step = 4*1024
        # step = 2097152
        step = 3145728
        # nr_ops = 2560
        nr_ops = size / step
         
        size = int(size * (1 - update_ratio))
        update_ops = int(nr_ops * update_ratio)
        # print("size", size)
        # print("update_ops", update_ops)
        offsets = range(offset, offset + size, step)
        # random.shuffle(offsets)

        offsets_append = []
        for i in range(update_ops):
                # print("update_ops", update_ops, "len(offsets)", len(offsets))
                if (update_ops > len(offsets)):
                        upper = len(offsets)
                else:
                        upper = update_ops
                k = random.randint(0, upper)
                offsets_append.append(offsets[k])

        # print("len(offsets)", len(offsets))

        for i in range(update_ops):
                offsets.append(offsets_append[i])
        # print("len(offsets)", len(offsets))
        random.shuffle(offsets)
        # offsets = offsets[:nr_ops]

elif (case == 8):
        # offset = 536870912000
        # offset = 912680550400
        update_ratio = 0.05
        offset = 0
        size = 10*1024*1024*768
        # size = 912680550400
        # step = 4*1024
        # step = 2097152
        step = 3145728
        # nr_ops = 2560
        nr_ops = size / step
         
        size = int(size * (1 - update_ratio))
        update_ops = int(nr_ops * update_ratio)
        # print("size", size)
        # print("update_ops", update_ops)
        offsets = range(offset, offset + size, step)
        # random.shuffle(offsets)

        offsets_append = []
        if (update_ops > len(offsets)):
                upper = len(offsets)
        else:
                upper = update_ops
        k = random.randint(0, upper)
        for i in range(update_ops):
                # print("update_ops", update_ops, "len(offsets)", len(offsets))
                offsets_append.append(offsets[k])
        print("offsets[k]:", offsets[k])
        # print("len(offsets)", len(offsets))

        for i in range(update_ops):
                offsets.append(offsets_append[i])
        # print("len(offsets)", len(offsets))
        random.shuffle(offsets)
        # offsets = offsets[:nr_ops]

elif (case == 9):
        # offset = 536870912000
        # offset = 912680550400
        update_ratio = 0.2

        cen = 0.8
        dev = 0.2

        big_data_update_ratio = 0.8
        small_data_update_ratio = 0.2

        big_data_raio = 0.2
        small_data_ratio = 0.8

        big_data_cen_ratio = cen * big_data_update_ratio
        small_data_cen_ratio = dev * big_data_update_ratio
        big_data_dec_ratio = cen * (1 - big_data_update_ratio)
        small_data_dec_ratio = dev * (1 - big_data_update_ratio)

        offset = 0
        # size = 10*1024*1024*1000
        # size = 10*1024*1024*2000
        size = 10*1024*1024*3000

        # size = 3*1024*1024*2560

        # size = 912680550400
        # step = 4*1024
        # step = 2097152

        # step = 1048576
        # step = 2097152
        step = 3145728

        nr_ops = 10000
        
        big_data_ops = int(nr_ops * big_data_raio)
        small_data_ops = int(nr_ops * small_data_ratio)

        update_ops = int(nr_ops * update_ratio)

        big_data_cen_ops = int(update_ops * big_data_cen_ratio)
        big_data_dec_ops = int(update_ops * big_data_dec_ratio)
        small_data_cen_ops = int(update_ops * small_data_cen_ratio)
        small_data_dec_ops = int(update_ops * small_data_dec_ratio)
         
        size = int(size * (1 - update_ratio))
        
        # print("size", size)
        # print("update_ops", update_ops)
        offsets = range(offset, offset + size, step)
        # random.shuffle(offsets)

        offsets_final = []
        offsets_update = []
        # offsets_update_big_data_cen = []
        # offsets_update_big_data_dec = []
        # offsets_update_small_data_cen = []
        # offsets_update_small_data_dec = []
        
        distributtion = 0.6

        offsets_big_data = offsets[:big_data_ops]
        print(len(offsets_big_data))
        random.shuffle(offsets_big_data)
        a = int(distributtion * len(offsets_big_data))
        offsets_update_big_data_cen = offsets_big_data[:a]
        print(big_data_cen_ops)
        print(len(offsets_update_big_data_cen))
        offsets_update_big_data_dec = offsets_big_data[a:]
        for i in range(big_data_cen_ops):
                k = random.randint(0, len(offsets_update_big_data_cen)-1)
                offsets_big_data.append(offsets_update_big_data_cen[k])
        for i in range(big_data_dec_ops):
                k = random.randint(0, len(offsets_update_big_data_dec)-1)
                offsets_big_data.append(offsets_update_big_data_dec[k])

        offsets_small_data = offsets[big_data_ops:]
        print(len(offsets_small_data))
        random.shuffle(offsets_small_data)
        a = int(distributtion * len(offsets_small_data))
        offsets_update_small_data_cen = offsets_small_data[:a]
        offsets_update_small_data_dec = offsets_small_data[a:]
        for i in range(small_data_cen_ops):
                k = random.randint(0, len(offsets_update_small_data_cen)-1)
                offsets_small_data.append(offsets_update_small_data_cen[k])
        for i in range(small_data_dec_ops):
                k = random.randint(0, len(offsets_update_small_data_dec)-1)
                offsets_small_data.append(offsets_update_small_data_dec[k])

        final = []
        final_size = []
        for i in range(len(offsets_big_data)):
                final.append(offsets_big_data[i])
                # final_size.append(4096)
                final_size.append(3145728)
        for i in range(len(offsets_small_data)):
                final.append(offsets_small_data[i])
                final_size.append(4096)

        r_list = []
        for i in range(len(final)):
                r_list.append(i)

        f_final = []
        f_final_size = []
        random.shuffle(r_list)
        for i in range(len(final)):
                k = r_list[i]
                f_final.append(final[k])
                f_final_size.append(final_size[k])

        print(len(f_final))
        print(len(f_final_size))
        # if (update_ops > len(offsets)):
        #         upper = len(offsets)
        # else:
        #         upper = update_ops

        # for i in range(len(offsets)):
        #         k = random.randint(0, update_ops)
        #         # print("update_ops", update_ops, "len(offsets)", len(offsets))
        #         if (i < update_ops):
        #                 offsets_update.append(offsets[i])
        #         else:
        #                 offsets_new_write.append(offsets[i])

        # print("offsets[k]:", offsets[k])
        # print("len(offsets)", len(offsets))

        # for i in range(update_ops):
        #         offsets.append(offsets_append[i])
        # print("len(offsets)", len(offsets))
        # random.shuffle(offsets)
        # offsets = offsets[:nr_ops]
else:
        print("ERROR")

# offsets = range(offset, offset + size, step)
# offsets = offsets[:nr_ops]

# gen_trace('read', args.device, offsets)
if (case != 9):
        gen_trace('write', args.device, offsets, step)
else:
        f = open('write.trace', 'w')
        for i in range(len(offsets)):
                f.write('%d, %d, %d, %d, %d\n' % (0, 0, 0, f_final_size[i], f_final[i]))
                # f.write('\n'.join(["%d, %d, %d, %d, %d" % (0, 0, 0, final_size[i], final[i])]))
        f.close()

