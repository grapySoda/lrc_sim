#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
                printf("sh(%d): stripe number %d, stripe offset %d dev(w): ",
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
        puts("init_disk");
        unsigned long i, j;

        int data_disks   = mddev->data_disks;
        int raid_disks   = mddev->data_disks + mddev->parity_disks;
        
        unsigned long nr_data_bitmap      = mddev->data_disk_info->disk_capacity >> 16;
        unsigned long nr_parity_bitmap    = mddev->parity_disk_info->disk_capacity >> 16;

        mddev->sb = malloc(sizeof(struct superblock) * raid_disks);

        /* init data_disk's bitmap */
        for (i = 0; i < data_disks; i++) {
                mddev->sb[i].bitmap = malloc(sizeof(unsigned long) * nr_data_bitmap);
                
                if (!mddev->sb[i].bitmap) {
                        puts("Allocat failed!, break");
                        break;
                } else puts("Success");

                for (j = 0; j < nr_data_bitmap; j++)
                        mddev->sb[i].bitmap[j] = 21;
        }

        /* init parity_disk's bitmap */
        printf("data_disks: %d, raid_disks: %d\n", data_disks, raid_disks);
        for (i = data_disks; i < raid_disks; i++) {
                mddev->sb[i].bitmap = malloc(sizeof(unsigned long) * nr_parity_bitmap);
                
                if (!mddev->sb[i].bitmap) {
                        puts("Allocate failed!, break");
                        break;
                } else puts("Success");

                for (j = 0; j < nr_parity_bitmap; j++)
                        mddev->sb[i].bitmap[j] = 22;
        }
        
        printf("size of data_disk's bitmap: %lu\n", mddev->sb[0].bitmap[nr_data_bitmap-1]);
        printf("size of parity_disk's bitmap: %lu\n", mddev->sb[3].bitmap[nr_parity_bitmap-1]);
}

void init_handle_list(struct mddev *mddev)
{
        int i;
        for (i = 0; i < HANDLE_LIST_SIZE; i++)
                mddev->handle_list[i] = NULL;
}

struct stripe_head* init_stripe_head(int raid_disks)
{
        int i;
        struct stripe_head *sh;

        sh      = malloc(sizeof(struct stripe_head));
        sh->dev = malloc(sizeof(struct rdev) * raid_disks); 
        
        for (i = 0; i < raid_disks; i++) {
                sh->dev[i].sector = 0;
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
int buffer_write(struct rdev *dev, unsigned long bm_number, unsigned long bm_offset)
{
        int *ptr = &dev->buf->ptr;

        dev->buf[*ptr].number = bm_number;
        dev->buf[*ptr].offset = bm_offset;
        (*ptr)++;

        return BUF_WRITE_TIME;
}

int write_back(struct rdev *dev)
{
        dev->buf->ptr = 0;

        return DISK_WRITE_TIME * BUF_SIZE;
}

int generic_make_request(struct rdev *dev, int rw)
{
        int i;
        int buf_time = 0;
        int *ptr = &dev->buf->ptr;

        unsigned long pg_number = dev->sector / STRIPE_SECTORS;

        unsigned long bm_number = pg_number / sizeof(unsigned long);
        unsigned long bm_offset = pg_number % sizeof(unsigned long);

        /* search buffer */
        for (i = 0; i < BUF_SIZE; i++) {
                if (dev->buf[i].number == bm_number &&
                    dev->buf[i].offset == bm_offset) {
                        if (rw == IO_READ)
                                return BUF_READ_TIME;
                        else
                                return BUF_WRITE_TIME;
                }
        }

        /* search disk */
        if ((*ptr) < BUF_SIZE)
                buf_time += buffer_write(dev, bm_number, bm_offset);
        else
                buf_time += buffer_write(dev, bm_number, bm_offset) + write_back(dev);
        
        if (!test_bit(bm_offset, &dev->sb->bitmap[bm_number]))
                set_bit(bm_offset, &dev->sb->bitmap[bm_number]);

        return DISK_READ_TIME + buf_time;
}

void ops_run_io(struct stripe_head *sh, int disks, int rw)
{
        int i;
        struct rdev *dev;

        for (i = disks; i--; ) {
                dev = &sh->dev[i];
                if (test_bit(R_Wantread, &dev->flags) && rw == IO_READ) {
                        clear_bit(R_Wantread, &dev->flags);
                        generic_make_request(dev, rw);
                }
                
                if (test_bit(R_Wantwrite, &dev->flags) && rw == IO_WRITE) {
                        clear_bit(R_Wantwrite, &dev->flags);
                        generic_make_request(dev, rw);
                }           
        }
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
                printf("Did not have any sh in handle list, create one.\n");
                sh = init_stripe_head(mddev->data_disks + mddev->parity_disks);
                sh->stripe_number = stripe_number;
                sh->stripe_offset = stripe_offset;

                handle_list[0] = sh;

                return sh;
        }

        /* Search the handle list for the match stripe head */
        for (i = 0; handle_list[i]; i++) {
                if (handle_list[i]->stripe_number == stripe_number &&
                    handle_list[i]->stripe_offset == stripe_offset) {
                        printf("Find match one, return.\n");
                        sh = handle_list[i];

                        return sh;
                }
                if (!handle_list[i + 1])
                        tail = i + 1;
        }

        /* Did not find the match stripe head, create one */
        printf("Did not find any match , create one. stripe_number: %lu, stripe_offeset: %lu\n", 
               stripe_number, stripe_offset);
        sh = init_stripe_head(mddev->data_disks + mddev->parity_disks);
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
                        if (test_bit(R_Wantwrite, &dev->flags) || i == sh->lpd1_idx)
                                set_bit(R_Wantread, &dev->flags);
                }
        } 
        if (rcw <= rmw && rcw > 0) {
                for (i = disks; i--; ) {
                        dev = &sh->dev[i];
                        if (i != sh->lpd1_idx && i != sh->lpd2_idx &&
                            i != sh->gqd1_idx && i != sh->gqd2_idx &&
                            !test_bit(R_Wantwrite, &dev->flags))
                                set_bit(R_Wantread, &dev->flags);
                }
        }
        return (rcw <= rmw);
}

void handle_stripe(struct stripe_head *sh, struct mddev *mddev)
{
        int rcw;
        rcw = handle_stripe_dirtying(sh, mddev->level, mddev->data_disks + mddev->parity_disks);
        // ops_run_io(sh, mddev->data_disks + mddev->parity_disks, IO_READ);
        // schedule_reconstruction(sh, rcw);
        // raid_run_ops(sh);
        // ops_run_io(sh, mddev->data_disks + mddev->parity_disks, IO_WRITE);

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

void print_sh(struct stripe_head *sh, struct mddev *mddev)
{
        int i;
        for (i = 0; i < mddev->data_disks; i++)
                printf("sh->dev[%d] = %lu\n", i, sh->dev[i].sector);
}


void make_request(struct mddev *mddev, struct io *io)
{
        int i, dd_idx;

        unsigned long stripe_number;
        unsigned long stripe_offset;

        unsigned long logical_sector = io->logical_sector / SECTOR_SIZE;
        unsigned long last_sector = logical_sector + io->length / SECTOR_SIZE;
        unsigned long new_sector = 0;

        struct stripe_head *sh;

        init_handle_list(mddev);

        for (; logical_sector < last_sector; logical_sector += STRIPE_SECTORS) {
                new_sector = raid_compute_sector(mddev, logical_sector, &dd_idx, NULL,
                                                 &stripe_number, &stripe_offset);

                // sleep(5000);
                sh = get_active_stripe(mddev, stripe_number, stripe_offset);

                pr_debug("raid456: make_request, sector %llu logical %llu dd_idx %d pd_idx %d " 
                         "stripe_number %lu, stripe_offset %lu\n",
                        (unsigned long long)new_sector,
                        (unsigned long long)logical_sector,
                        dd_idx, sh->lpd1_idx, stripe_number, stripe_offset);

                sh->dev[dd_idx].sector = new_sector;
                set_bit(R_Wantwrite, &sh->dev[dd_idx].flags);
                
                raid_compute_sector(mddev, logical_sector, &dd_idx, sh,
                                    &stripe_number, &stripe_offset);

                pr_debug("stripe, stripe_number %d, stripe_offset %d\n",
                         sh->stripe_number, sh->stripe_offset);

                if (mddev->level > 5) {
                        set_bit(R_Wantwrite, &sh->dev[sh->gqd1_idx].flags);
                }
        }
        
        for (i = 0; mddev->handle_list[i]; i++) {
                handle_stripe(mddev->handle_list[i], mddev);
        }
        
        print_handle_list(mddev);
        free_handle_list(mddev);
}