#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raid.h"

// #define BUF_SIZE        100

#define DISKS_INFO              "disks.info"
#define RAID_CONFIG             "raid.config"
#define TRACE_FILE              "/home/ccs/test/lrc_sim/tests/rand-write_lat.1.log"
#define OUTPUT_LAT_LOG          "/home/ccs/test/lrc_sim/tests/sim-rand-write_lat.1.log"
// #define TRACE_FILE      "randwrite.trace"

// raid_types[] = {
// 	{"raid10",   "RAID10 (striped mirrors)",        0, 2, 2, ALL_CMR},
// 	{"raid4",    "RAID4 (dedicated parity disk)",	1, 2, 3, ALL_CMR},
// 	{"raid5_la", "RAID5 (left asymmetric)",		1, 2, 5, ALL_CMR},
// 	{"raid6_zr", "RAID6 (zero restart)",		2, 4, 6, ALL_CMR},
// 	{"lrc",	     "LRC (6, 2, 2)",	 		4, 6, 7, ALL_CMR}
// };

void raid_ctr_init(struct mddev *mddev)
{
        struct disk_info disk_info[] = {
                /* cmr, 64 MB, 1 TB, 7,200 RPM, 
                 * 63        sectors/track, 
                 * 1,938,021 cylinders
                 * 2         heads
                 */
	        {"cmr",	67108864, 1099511627776, 7200, 63, 1938021, 2, 0},          
                /* smr, 64 MB, 1 TB, 7,200 RPM, 
                 * 63        sectors/track, 
                 * 1,938,021 cylinders
                 * 2         heads
                 */
	        {"smr",	67108864, 1099511627776, 5400, 63, 1938021, 2, 268435456}
        };
        // char data_disk_type[] = "cmr";
        // char parity_disk_type[] = "cmr";

        mddev->data_disks = 2;
        mddev->level = 5;
        mddev->parity_disks = 2;
        mddev->chunk_sectors = CHUNK_SECTORS;

        // mddev->data_disk_info = &disk_info[0];
        mddev->data_disk_info = &disk_info[0];
        mddev->parity_disk_info = &disk_info[0];

        // mddev->data_disk_type = data_disk_type;
        // mddev->parity_disk_type = parity_disk_type;
        init_disk(mddev);
        // for (i = 0; i < HANDLE_LIST_SIZE; i++) {
        //         mddev->handle_list[i] = ;
        // }
        // init_handle_list(mddev);

        mddev->flags = 0;

        // switch(mddev->level) {
        //         case 4:
        //                 mddev->chunk_sectors = CHUNK_SECTORS * mddev->data_disks;
        //                 break;
        //         case 5:
        //                 mddev->chunk_sectors = CHUNK_SECTORS * (mddev->data_disks + mddev->parity_disks);
	// 		break;
        //         case 6:
        //                 mddev->chunk_sectors = CHUNK_SECTORS * (mddev->data_disks + mddev->parity_disks);
	// 		break;
        // }
}

// void fetch_io(struct io *io, FILE *fp_trace)
// {
//         if (fscanf(fp_trace, "%lu %lu", &io->logical_sector, &io->length) == EOF) {
//                 printf("\nend\n");
//                 io = NULL;
//                 return;
//         }

//         printf("\nlogical_sector: %lu\n"
//                "length: %lu\n", io->logical_sector, io->length);
// }

int main(void)
{
        puts("");
        unsigned int i = 1;
        struct mddev *mddev = malloc(sizeof(struct mddev));
        struct io *io = malloc(sizeof(struct io));

        unsigned long resp_time;
        long arrival_time;

        FILE *fp_trace = NULL;
        FILE *fp_output_log = NULL;

        fp_trace = fopen(TRACE_FILE, "r");
        fp_output_log = fopen(OUTPUT_LAT_LOG, "w");

        raid_ctr_init(mddev);
        puts("");

        fscanf(fp_trace, "%u, %u, %u, %lu, %lu",
               &io->time, &io->lat, &io->times, &io->length, &io->logical_sector);

        arrival_time = io->time - (io->lat / NS2MS);

        // printf("logical_sector: %lu, io->length: %lu\n", io->logical_sector, io->length);
        while (io) {
                resp_time = make_request(mddev, io);
                // resp_time = test_single_disk(mddev, io, IO_WRITE);

                // if (io->lat > 600579)
                // if (io->lat > 600000)
                //         printf("[request %u] lat: %u\n", i, io->lat);

                fprintf(fp_output_log, "%u, %lu, %u, %lu, %lu\n",
                        io->time, resp_time, io->times, io->length, io->logical_sector);

                if (fscanf(fp_trace, "%u, %u, %u, %lu, %lu",
                           &io->time, &io->lat, &io->times, &io->length, &io->logical_sector) == EOF)
                        io = NULL;

                if (io)
                        arrival_time = io->time - (io->lat / NS2MS);

                i++;
        }
        fclose(fp_trace);
        fclose(fp_output_log);
        free(io);
        return 0;
}