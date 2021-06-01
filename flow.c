#include <stdio.h>
#include <stdlib.h>

#include "raid.h"
#include "debug.h"

// unsigned int get_zone_number(struct rdev *dev)
// {
//         return ;
// }

#define READ_LAT                10
#define READ_ERR                20
#define WRITE_LAT               30
#define WRITE_ERR               40

unsigned long generic_make_request_smr(struct rdev *dev, int rw)
{
        unsigned long start_zone = dev->start_lba / dev->zone_size;
        unsigned long end_zone = dev->end_lba / dev->zone_size;

        unsigned int start_page = dev->start_lba / PAGE_SIZE;
        unsigned int end_page = dev->end_lba / PAGE_SIZE;

        if (rw == IO_READ) {
                if (start_zone == end_zone &&
                    dev->end_lba < dev->zones[start_zone].wp) {
                        return READ_LAT;
                } else {
                        bug("[err] SMR read error. start_zone: %lu, end_zone: %lu,
                            end_lba: %lu, write_pointer: %lu\n",
                            start_zone, end_zone, dev->end_lba, dev->zones[start_zone].wp);
                        
                        return READ_ERR;
                }
        } else {
                if (start_zone == end_zone &&
                    start_page == dev->zones[start_zone].wp) {
                        dev->zones[start_zone].wp++;
                        return WRITE_LAT;
                } else {
                        bug("[err] SMR write error. start_zone: %lu, end_zone: %lu,
                            end_lba: %lu, write_pointer: %lu\n",
                            start_zone, end_zone, dev->end_lba, dev->zones[start_zone].wp);
                        
                        WRITE_ERR;
                }
        }
}

unsigned long test_single_disk(struct mddev *mddev, struct io *io, short rw)
{       
        // int i;
        static unsigned long requst_times;
        
        struct rdev *dev = &mddev->rdev[0];

        // dev->disk_head.cylinder = 0;
        // dev->disk_head.sector = 0;

        unsigned long resp_time = 0;

        unsigned long logical_sector = io->logical_sector / SECTOR_SIZE;
        unsigned long last_sector = logical_sector + io->length /SECTOR_SIZE;
        
        // for (i = 0; i < mddev->parity_disks + mddev->data_disks; i++)
        //         pr_debug_sh("disk[%d]: %d\n", i, *(mddev->rdev[i].buf_ptr));

        for (; logical_sector < last_sector; logical_sector += STRIPE_SECTORS) {
                dev->sector = logical_sector;
                resp_time += generic_make_request(dev, rw);
                // printf("now rt: %lu\n", resp_time);
                // printf("now sec: %lu\n", logical_sector);
        } 
        resp_time += transfer_time() + controller_time();
        // printf("final now rt: %lu\n", resp_time);
        printf("[Request %lu] Logical Offset: %lu, Write Size: %lu, Response Time: %lu\n\n",
                requst_times++, io->logical_sector, io->length, resp_time);

        // print_handle_list(mddev);
        // sleep(5);
        // sleep(15);
        // dev->disk_head.sector -= 1;
        // printf("before: %u\n", dev->disk_head.sector);
        dev->disk_head.sector = (dev->disk_head.sector + dev->heads) % dev->nr_sectors;
        // printf("after: %u\n", dev->disk_head.sector);
        return resp_time;
}