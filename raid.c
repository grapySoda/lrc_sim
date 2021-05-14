#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "raid.h"
#include "bitops.h"
#include "debug.h"

void free_handle_list(struct mddev *mddev)
{
        int i;

        struct stripe_head *sh;
        struct stripe_head **handle_list = mddev->handle_list;
        
        for (i = 0; handle_list[i]; i++) {
                sh = handle_list[i];
                free(sh->dev);
                free(sh);
        }
}

void print_stripe_head(struct stripe_head *sh, unsigned long *stripe_num, int disks)
{
        int i;
        pr_debug_sh("sh(%lu): stripe number %lu, stripe offset %lu dev(w): ",
                     (*stripe_num)++, sh->stripe_number, sh->stripe_offset);
        for (i = 0; i < disks; i++)
                if (i == sh->lpd1_idx || i == sh->lpd2_idx ||
                        i == sh->gqd1_idx || i == sh->gqd2_idx)
                        pr_debug_sh("2");
                else
                        if (test_bit(R_Wantwrite, &sh->dev[i].flags))
                                pr_debug_sh("1");
                        else
                                pr_debug_sh("0");

        pr_debug_sh(" dev(r): ");
        for (i = 0; i < disks; i++)
                if (test_bit(R_Wantread, &sh->dev[i].flags))
                        pr_debug_sh("1");
                else
                        pr_debug_sh("0");

        pr_debug_sh("\n");
}

void print_handle_list(struct mddev *mddev)
{
        int i, j;
        int stripe_num = 0;
        int disks = mddev->data_disks + mddev->parity_disks;
        
        static int count;

        struct stripe_head *sh;
        struct stripe_head **handle_list = mddev->handle_list;

        printf("[%d times]\n", count);
        for (i = 0; handle_list[i]; i++) {
                sh = handle_list[i];
                printf("sh(%d): stripe number %lu, stripe offset %lu dev(w): ",
                        stripe_num++, sh->stripe_number, sh->stripe_offset);
                for (j = 0; j < disks; j++)
                        if (j == sh->lpd1_idx || j == sh->lpd2_idx ||
                            j == sh->gqd1_idx || j == sh->gqd2_idx)
                                        printf("2");
                        else
                                if (test_bit(R_Wantwrite, &sh->dev[j].flags))
                                        printf("1");
                                else
                                        printf("0");

                printf(" dev(r): ");
                for (j = 0; j < disks; j++)
                        if (test_bit(R_Wantread, &sh->dev[j].flags))
                                printf("1");
                        else
                                printf("0");

                // puts("");
                // for (j = 0; j < disks; j++)
                //         printf("d[%d].sector: %lu ", j, sh->dev[j].sector);

                puts("");

        }
        puts("");

        count++;
}

void init_disk(struct mddev *mddev)
{
        int i;
        unsigned long j;

        int data_disks   = mddev->data_disks;
        int raid_disks   = mddev->data_disks + mddev->parity_disks;
        
        unsigned int nr_data_cylinders   = mddev->data_disk_info->nr_cylinders;
        unsigned int nr_parity_cylinders = mddev->parity_disk_info->nr_cylinders;

        unsigned int nr_data_sectors     = mddev->data_disk_info->nr_sectors;
        unsigned int nr_parity_sectors   = mddev->parity_disk_info->nr_sectors;

        unsigned long nr_data_bitmap     = mddev->data_disk_info->disk_capacity >> 16;
        unsigned long nr_parity_bitmap   = mddev->parity_disk_info->disk_capacity >> 16;
        
        unsigned long nr_data_buf        = mddev->data_disk_info->buffer_size >> PAGE_SHIFT;
        unsigned long nr_parity_buf      = mddev->parity_disk_info->buffer_size >> PAGE_SHIFT;

        mddev->rdev = malloc(sizeof(struct rdev) * raid_disks);
        
        /* init data_disk's bitmap */
        for (i = 0; i < data_disks; i++) {
                mddev->rdev[i].nr_buf = nr_data_buf;
                mddev->rdev[i].flags = 0;

                mddev->rdev[i].buf_write_ptr = 0;
                mddev->rdev[i].buf_usage = nr_data_buf;

                mddev->rdev[i].disk_head.sector = 0;
                mddev->rdev[i].disk_head.cylinder = 0;

                mddev->rdev[i].nr_cylinders = nr_data_cylinders;
                mddev->rdev[i].nr_sectors   = nr_data_sectors;

                mddev->rdev[i].heads = mddev->data_disk_info->heads;

                mddev->rdev[i].sb = malloc(sizeof(struct superblock));

                /* init bitmap */
                mddev->rdev[i].sb->bitmap  = malloc(sizeof(unsigned long) * nr_data_bitmap);
                if (mddev->rdev[i].sb->bitmap)
                        pr_debug("init_disk, dev[%d] bitmap allocate success.\n", i);
                else 
                        pr_debug("init_disk, dev[%d] bitmap allocate failed.\n", i);

                for (j = 0; j < nr_data_bitmap; j++)
                        mddev->rdev[i].sb->bitmap[j] = 0;

                /* init buffer */
                mddev->rdev[i].buf = malloc(sizeof(struct buf) * nr_data_buf);
                if (mddev->rdev[i].buf)
                        pr_debug("init_disk, dev[%d] buffer allocate success.\n", i);
                else
                        pr_debug("init_disk, dev[%d] buffer allocate failed.\n", i);

                for (j = 0; j < nr_data_buf; j++) {
                        mddev->rdev[i].buf[j].number = -1;
                        mddev->rdev[i].buf[j].offset = -1;
                }
        }

        /* init parity_disk's bitmap */
        for (i = data_disks; i < raid_disks; i++) {
                mddev->rdev[i].flags = 0;
                mddev->rdev[i].nr_buf = nr_parity_buf;

                mddev->rdev[i].buf_write_ptr = 0;
                mddev->rdev[i].buf_usage = nr_parity_buf;

                mddev->rdev[i].disk_head.sector = 0;
                mddev->rdev[i].disk_head.cylinder = 0;

                mddev->rdev[i].nr_cylinders = nr_parity_cylinders;
                mddev->rdev[i].nr_sectors   = nr_parity_sectors;

                mddev->rdev[i].heads = mddev->parity_disk_info->heads;

                mddev->rdev[i].sb = malloc(sizeof(struct superblock));

                /* init bitmap */
                mddev->rdev[i].sb->bitmap = malloc(sizeof(unsigned long) * nr_parity_bitmap);
                if (mddev->rdev[i].sb->bitmap)
                        pr_debug("init_disk, dev[%d] bitmap allocate success.\n", i);
                else
                        pr_debug("init_disk, dev[%d] bitmap allocate failed.\n", i);

                for (j = 0; j < nr_parity_bitmap; j++)
                        mddev->rdev[i].sb->bitmap[j] = 0;

                /* init buffer */
                mddev->rdev[i].buf = malloc(sizeof(struct buf) * nr_parity_buf);
                if (mddev->rdev[i].buf)
                        pr_debug("init_disk, dev[%d] buffer allocate success.\n", i);
                else
                        pr_debug("init_disk, dev[%d] buffer allocate failed.\n", i);

                for (j = 0; j < nr_parity_buf; j++) {
                        mddev->rdev[i].buf[j].number = 0;
                        mddev->rdev[i].buf[j].offset = 0;
                }
        }

        // printf("size of data_disk's bitmap: %lu\n", mddev->sb[0].bitmap[nr_data_bitmap-1]);
        // printf("size of parity_disk's bitmap: %lu\n", mddev->sb[3].bitmap[nr_parity_bitmap-1]);
}

void init_handle_list(struct mddev *mddev)
{
        int i;
        for (i = 0; i < HANDLE_LIST_SIZE; i++)
                mddev->handle_list[i] = NULL;
}

struct stripe_head* init_stripe_head(struct mddev *mddev)
{
        int i;
        int raid_disks = mddev->data_disks + mddev->parity_disks;

        struct stripe_head *sh;

        sh      = malloc(sizeof(struct stripe_head));
        sh->dev = malloc(sizeof(struct rdev) * raid_disks);

        for (i = 0; i < raid_disks; i++) {
                sh->dev[i] = mddev->rdev[i];
                sh->dev[i].flags = 0;
        }

        return sh;
}
// void raid_run_ops(struct stripe_head *sh)
// {
//         ;
// }

// void schedule_reconstruction(struct stripe_head *sh)
// {
//         ;
// }

// if ((*ptr) < (dev->nr_buf * BUF_FLUSH_THRESHOLD)) {
//         buf_time = buffer_write(dev, bm_number, bm_offset);
//         pr_debug_rt("buffer write time: %d\n", buf_time);
// } else {
//         buf_time = buffer_write(dev, bm_number, bm_offset) + write_back(dev);
//         pr_debug_rt("buffer write time: %d (write back)\n", buf_time);
// }


// int buffer_flush(struct rdev *dev)
// {
//         // unsigned int bwt = dev->buf_write_ptr;  /* buf write ptr */

//         static int wb_times;                    /* write buf times */
//         static int bfr;                         /* buffer flush ratio */

//         bfr = (bfr == 0) ? BUF_FLUSH_RATIO + rand() % BUF_FLUSH_DEVIATION
//                          : bfr;
                         
//         if (dev->buf_usage > (dev->nr_buf * (1 - BUF_FLUSH_THRESHOLD))) {
//                 if (wb_times++ < BUF_FLUSH_RATIO) {
//                         return 0;
//                 } else {
//                         wb_times = 0;
//                         bfr = BUF_FLUSH_RATIO + rand() % BUF_FLUSH_DEVIATION;

//                         return 1000000;         /* (under construction!!) small flush */
//                         // return rotate_disk(dev);
//                 }
//         } else {
//                 return 20000000;                /* (under construction!!) big flush */
//         }
// }

// int buffer_write(struct rdev *dev, unsigned long bm_number, unsigned long bm_offset)
// {
//         unsigned int *ptr = &(dev->buf_write_ptr);
//         int buf_time = 0;

//         buf_time += buffer_flush(dev);
//         pr_debug("buffer_write, ptr = %d\n", (*ptr));

//         dev->buf[*ptr].number = bm_number;
//         dev->buf[*ptr].offset = bm_offset; 
//         (*ptr)++;
//         dev->buf_usage--;

//         return buf_time + BUF_WRITE_TIME + rand() % BUF_DEVIATION;
// }

// int write_back(struct rdev *dev)
// {
//         dev->buf_write_ptr = 0;
//         pr_debug("\n\n\n\n\n\n\n\n\nbuf full!!!!\n\n\n\n\n\n\n\n\n");
        
//         return DISK_WRITE_TIME * BUF_SIZE + rand() % BUF_DEVIATION;
// }

unsigned long rotate_time(struct rdev *dev, unsigned long sector, int platters)
{
        // printf("logic sector: %lu\n", sector);
        unsigned long current_sector = (sector / platters / 8) % dev->nr_sectors;
        printf("current sector: %lu\n", dev->disk_head.sector);
        printf("new sector: %lu\n", current_sector);
        // unsigned long current_sector = sector % dev->nr_sectors;
        unsigned long rotate_sectors = (current_sector >= (dev->disk_head.sector * platters))
                                        ? current_sector - dev->disk_head.sector
                                        : dev->nr_sectors - (dev->disk_head.sector - current_sector);
        
        dev->disk_head.sector = current_sector;
        printf("rotate sectors: %lu\n", rotate_sectors);
        // printf("rotate time: %lu\n", rotate_sectors * ROTATE_PER_SECTOR);

        return rotate_sectors * ROTATE_PER_SECTOR;
}

unsigned long seek_time(struct rdev *dev, unsigned long sector, int platters)
{
        unsigned long current_cylinder = (sector / platters / 8) / dev->nr_sectors;
        // printf("seek time: %lu\n", current_cylinder);
        // unsigned long current_cylinder = sector / dev->nr_sectors;
        unsigned long seek_cylinders = (current_cylinder >= dev->disk_head.cylinder)
                                        ? current_cylinder - dev->disk_head.cylinder
                                        : dev->disk_head.cylinder - current_cylinder;

        dev->disk_head.cylinder = current_cylinder;

        // printf("seek time: %lu\n", current_cylinder);
        // printf("seek time: %lu\n", seek_cylinders * SEEK_BETWEEN_TRACK);

        return seek_cylinders * SEEK_BETWEEN_TRACK;
}

unsigned long transfer_time(void)
{
        return TRANSFER_TIME;
}

unsigned long controller_time(void)
{
        return CONTROLLER_TIME;
}

unsigned long generic_make_request(struct rdev *dev, int rw)
{
        int i;
        static int platter_num;
        // unsigned int *ptr = &(dev->buf_write_ptr);

        unsigned long sector = dev->sector;
        // pr_debug("pg_number: %lu\n", pg_number);

        /* search buffer */
        // for (i = 0; i < (*ptr); i++) {
        //         if (dev->buf[i].number == bm_number &&
        //             dev->buf[i].offset == bm_offset) {
        //                 if (rw == IO_READ) {
        //                         buf_time = BUF_READ_TIME + rand() % BUF_DEVIATION;
        //                         pr_debug_rt("buffer read hit time: %u\n", buf_time);
        //                         return buf_time;
        //                 } else {
        //                         buf_time = BUF_WRITE_TIME + rand() % BUF_DEVIATION;
        //                         pr_debug_rt("buffer write hit time: %u\n", buf_time);
        //                         return buf_time;
        //                 }
        //         }
        // }
        // pr_debug("buffer miss\n");

        /* write to buf or flush the buf before write to buf */
        // buf_time = buffer_write(dev, bm_number, bm_offset);
        // if ((*ptr) < (dev->nr_buf * BUF_FLUSH_RATIO)) {
        //         buf_time = buffer_write(dev, bm_number, bm_offset);
        //         pr_debug_rt("buffer write time: %d\n", buf_time);
        // } else {
        //         buf_time = buffer_write(dev, bm_number, bm_offset) + write_back(dev);
        //         pr_debug_rt("buffer write time: %d (write back)\n", buf_time);
        // }

        // if (platter_num <= 2 * dev->platters) {
        //         platter_num++;
        //         return 0;
        // } else {
        //         ;
        // }

        return rotate_time(dev, sector, dev->heads) + seek_time(dev, sector, dev->heads);

        // if (!test_bit(bm_offset, &dev->sb->bitmap[bm_number]) && rw != IO_READ) {
        //         pr_debug("disk miss\n");
        //         set_bit(bm_offset, &dev->sb->bitmap[bm_number]);
        //         buf_time += DISK_WRITE_TIME;
        // } else pr_debug("disk hit\n");

        // return DISK_READ_TIME + buf_time;
}

unsigned long ops_run_io(struct stripe_head *sh, int disks, int rw)
{
        int i;
        int dev_num = -1;

        unsigned long resp_time;
        unsigned long max_resp_time = 0;
        
        struct rdev *dev;

        for (i = disks; i--; ) {
                dev = &sh->dev[i];
                if (test_bit(R_Wantread, &dev->flags) && rw == IO_READ) {
                        clear_bit(R_Wantread, &dev->flags);

                        resp_time = generic_make_request(dev, rw);
                        dev_num = (resp_time >= max_resp_time) ? i : dev_num;
                        max_resp_time = (resp_time >= max_resp_time) ? resp_time : max_resp_time;
                        
                        pr_debug_rt("ops_run_io, read, disk[%d], spend time: %lu\n", i, resp_time);
                }
                
                if (test_bit(R_Wantwrite, &dev->flags) && rw == IO_WRITE) {
                        clear_bit(R_Wantwrite, &dev->flags);

                        resp_time = generic_make_request(dev, rw);
                        dev_num = (resp_time >= max_resp_time) ? i : dev_num;
                        max_resp_time = (resp_time >= max_resp_time) ? resp_time : max_resp_time;

                        pr_debug_rt("ops_run_io, write, disk[%d], spend time: %lu\n", i, resp_time);
                }           
        }
        if (dev_num > -1) pr_debug("ops_run_io, request done, disk[%d], max spend time: %lu\n\n", dev_num, max_resp_time);
        
        return max_resp_time;
}

struct stripe_head* get_active_stripe(struct mddev *mddev, 
                                      unsigned long stripe_number, 
                                      unsigned long stripe_offset)
{
        int i, tail;

        struct stripe_head *sh;
        struct stripe_head **handle_list = mddev->handle_list;

        // int disks = mddev->data_disks + mddev->parity_disks;

        /* If do not have any stripe head in handle list */
        if (!handle_list[0]) {
                // pr_debug("Did not have any sh in handle list, create one.\n");
                sh = init_stripe_head(mddev);
                sh->stripe_number = stripe_number;
                sh->stripe_offset = stripe_offset;

                handle_list[0] = sh;

                return sh;
        }

        /* Search the handle list for the match stripe head */
        for (i = 0; handle_list[i]; i++) {
                if (handle_list[i]->stripe_number == stripe_number &&
                    handle_list[i]->stripe_offset == stripe_offset) {
                        // pr_debug("Find match one, return.\n");
                        sh = handle_list[i];

                        return sh;
                }
                if (!handle_list[i + 1])
                        tail = i + 1;
        }

        /* Did not find the match stripe head, create one */
        // pr_debug("Did not find any match , create one. stripe_number: %lu, stripe_offeset: %lu\n", 
        //        stripe_number, stripe_offset);
        sh = init_stripe_head(mddev);
        sh->stripe_number = stripe_number;
        sh->stripe_offset = stripe_offset;

        handle_list[tail] = sh;

        return sh;
}

int handle_stripe_dirtying(struct stripe_head *sh, int level, int disks)
{
        int i;
        int rcw = 0;
        int rmw = 0;
        struct rdev *dev;

        if (level <= 5) {
                for (i = disks; i--; ) {
                        dev = &sh->dev[i];

                        /* compute rcw */
                        if (!test_bit(R_Wantwrite, &dev->flags) && i != sh->lpd1_idx)
                                rcw++;
                        else
                                rmw++;
                }
        } else {
                rmw = 2;
                rcw = 1;
        }

        if (rmw < rcw && rmw > 0) {
                for (i = disks; i--; ) {
                        dev = &sh->dev[i];
                        if (test_bit(R_Wantwrite, &dev->flags) || i == sh->lpd1_idx) {
                                dev->sector = sh->sector;
                                set_bit(R_Wantread, &dev->flags);
                        }
                }
        } 
        if (rcw <= rmw && rcw > 0) {
                for (i = disks; i--; ) {
                        dev = &sh->dev[i];
                        if (i != sh->lpd1_idx && i != sh->lpd2_idx &&
                            i != sh->gqd1_idx && i != sh->gqd2_idx &&
                            !test_bit(R_Wantwrite, &dev->flags)) {
                                dev->sector = sh->sector;
                                set_bit(R_Wantread, &dev->flags);
                        }
                }
        }
        return (rcw <= rmw);
}

unsigned long handle_stripe(struct stripe_head *sh, struct mddev *mddev, unsigned long *stripe_num)
{
        int rcw;
        int disks = mddev->data_disks + mddev->parity_disks;
        unsigned long resp_time = 0;

        rcw = handle_stripe_dirtying(sh, mddev->level, disks);
        print_stripe_head(sh, stripe_num, disks);
        resp_time += ops_run_io(sh, disks, IO_READ);
        // schedule_reconstruction(sh, rcw);
        // raid_run_ops(sh);
        resp_time += ops_run_io(sh, disks, IO_WRITE);
        return resp_time;

}

unsigned long raid_compute_sector(struct mddev *mddev, unsigned long r_sector,
                                  int *dd_idx, struct stripe_head *sh,
                                  unsigned long *stripe_number, unsigned long *stripe_offset)
{
        unsigned long stripe, stripe2;
        unsigned long chunk_number, chunk_offset;
        unsigned long new_sector;

        int lpd1_idx, lpd2_idx, gpd1_idx, gpd2_idx;

        int sectors_per_chunk = mddev->chunk_sectors;
        int data_disks = mddev->data_disks;
        int raid_disks = data_disks + mddev->parity_disks;

        chunk_offset = sector_div(r_sector, sectors_per_chunk);
        *stripe_offset = chunk_offset;
	chunk_number = r_sector;

        stripe = chunk_number;
	*dd_idx = sector_div(stripe, data_disks);
	stripe2 = stripe;
        *stripe_number = stripe;

        lpd1_idx = lpd2_idx = gpd1_idx = gpd2_idx = -1;

        switch(mddev->level) {
                case 4:
                        lpd1_idx = data_disks;
                        break;
                case 5:
                        lpd1_idx = sector_div(stripe2, raid_disks);
			*dd_idx = (lpd1_idx + 1 + *dd_idx) % raid_disks;
			break;
                case 6:
                        lpd1_idx = raid_disks - 1 - sector_div(stripe2, raid_disks);
			gpd1_idx = lpd1_idx + 1;
			if (lpd1_idx == raid_disks-1) {
				(*dd_idx)++;	/* Q D D D P */
				gpd1_idx = 0;
			} else if (*dd_idx >= lpd1_idx)
				(*dd_idx) += 2; /* D D P Q D */
			break;
        }

        if (sh) {
                sh->gqd1_idx = gpd1_idx;
                sh->gqd2_idx = gpd2_idx;
                sh->lpd1_idx = lpd1_idx;
                sh->lpd2_idx = lpd2_idx;
        }

        new_sector = stripe * sectors_per_chunk + chunk_offset;
	return new_sector;
}

// void print_sh(struct stripe_head *sh, struct mddev *mddev)
// {
//         int i;
//         for (i = 0; i < mddev->data_disks; i++)
//                 printf("sh->dev[%d] = %lu\n", i, sh->dev[i].sector);
// }

unsigned long make_request(struct mddev *mddev, struct io *io)
{       
        int i, dd_idx;
        static unsigned long requst_times;
        
        unsigned long resp_time = 0;

        unsigned long stripe_number;
        unsigned long stripe_offset;

        unsigned long logical_sector = io->logical_sector / SECTOR_SIZE;
        unsigned long last_sector = logical_sector + io->length / SECTOR_SIZE;
        unsigned long new_sector = 0;

        struct stripe_head *sh;
        
        init_handle_list(mddev);
        srand(time(NULL));
        // for (i = 0; i < mddev->parity_disks + mddev->data_disks; i++)
        //         pr_debug_sh("disk[%d]: %d\n", i, *(mddev->rdev[i].buf_ptr));

        for (; logical_sector < last_sector; logical_sector += STRIPE_SECTORS) {
                new_sector = raid_compute_sector(mddev, logical_sector, &dd_idx, NULL,
                                                 &stripe_number, &stripe_offset);

                sh = get_active_stripe(mddev, stripe_number, stripe_offset);
                sh->dev[dd_idx].sector = new_sector;
                sh->sector = new_sector;
                
                raid_compute_sector(mddev, logical_sector, &dd_idx, sh,
                                    &stripe_number, &stripe_offset);

                pr_debug("make_request, sector %llu logical %llu dd_idx %d pd_idx %d " 
                         "stripe_number %lu, stripe_offset %lu\n",
                        (unsigned long long)new_sector,
                        (unsigned long long)logical_sector,
                        dd_idx, sh->lpd1_idx, stripe_number, stripe_offset);

                set_bit(R_Wantwrite, &sh->dev[dd_idx].flags);
                
                /* set parity disk flags and sector */
                set_bit(R_Wantwrite, &sh->dev[sh->lpd1_idx].flags);
                sh->dev[sh->lpd1_idx].sector = sh->sector;


                if (mddev->level > 5)
                        set_bit(R_Wantwrite, &sh->dev[sh->gqd1_idx].flags);

                // print_handle_list(mddev);
        }
        
        unsigned long stripe_num = 0;

        for (i = 0; mddev->handle_list[i]; i++)
                resp_time += handle_stripe(mddev->handle_list[i], mddev, &stripe_num);

        printf("\n[Request %lu] Logical Offset: %lu, Write Size: %lu, Response Time: %lu\n",
                requst_times++, io->logical_sector, io->length, resp_time);

        // print_handle_list(mddev);
        // sleep(5);
        free_handle_list(mddev);
        // sleep(15);
        return resp_time;
}

unsigned long test_single_disk(struct mddev *mddev, struct io *io, short rw)
{       
        int i;
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
        printf("\n[Request %lu] Logical Offset: %lu, Write Size: %lu, Response Time: %lu\n",
                requst_times++, io->logical_sector, io->length, resp_time);

        // print_handle_list(mddev);
        // sleep(5);
        // sleep(15);
        // dev->disk_head.sector -= 1;
        printf("before: %lu\n", dev->disk_head.sector);
        dev->disk_head.sector = (dev->disk_head.sector + dev->heads) % dev->nr_sectors;
        printf("after: %lu\n", dev->disk_head.sector);
        return resp_time;
}