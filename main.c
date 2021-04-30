#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raid.h"

// #define BUF_SIZE        100

#define DISKS_INFO      "disks.info"
#define RAID_CONFIG     "raid.config"
#define TRACE_FILE      "randwrite.trace"

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
	        {"cmr",	268435456, 1099511627776, 7200},        /* cmr, 256 MB, 1 TB, 7200 RPM */
	        {"smr",	268435456, 1099511627776, 5400}         /* smr, 256 MB, 1 TB, 7200 RPM */
        };
        // char data_disk_type[] = "cmr";
        // char parity_disk_type[] = "cmr";

        mddev->data_disks = 3;
        mddev->level = 5;
        mddev->parity_disks = 1;
        mddev->chunk_sectors = CHUNK_SECTORS;

        mddev->data_disk_info = &disk_info[0];
        mddev->parity_disk_info = &disk_info[1];

        // mddev->data_disk_type = data_disk_type;
        // mddev->parity_disk_type = parity_disk_type;

        init_disk(mddev);
        init_handle_list(mddev);

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
        struct mddev *mddev = malloc(sizeof(struct mddev));
        struct io *io = malloc(sizeof(struct io));

        // unsigned long a = 1099511627776;

        FILE *fp_trace = NULL;
        fp_trace = fopen(TRACE_FILE, "r");

        raid_ctr_init(mddev);

        fscanf(fp_trace, "%lu %lu", &io->logical_sector, &io->length);
        while (io) {
                make_request(mddev, io);
                if (fscanf(fp_trace, "%lu %lu", &io->logical_sector, &io->length) == EOF)
                        io = NULL;
        }
        fclose(fp_trace);
        free(io);
        return 0;
}