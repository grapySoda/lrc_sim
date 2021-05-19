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