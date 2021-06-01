#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raid.h"

// #define BUF_SIZE        100

#define TRACE_FILE              "/home/ccs/test/lrc_sim/tests/rand-write_lat.1.log"
#define OUTPUT_LAT_LOG          "/home/ccs/test/lrc_sim/tests/new-rand-write_lat.1.log"

int main(void)
{
        puts("");
        unsigned int i = 1;
        struct mddev *mddev = malloc(sizeof(struct mddev));
        struct io *io = malloc(sizeof(struct io));

        unsigned long resp_time;
        unsigned long arrival_time;

        FILE *fp_trace = NULL;
        FILE *fp_output_log = NULL;

        fp_trace = fopen(TRACE_FILE, "r");
        fp_output_log = fopen(OUTPUT_LAT_LOG, "w");

        fscanf(fp_trace, "%u, %u, %u, %lu, %lu",
               &io->time, &io->lat, &io->times, &io->length, &io->logical_sector);

        arrival_time = io->time - (io->lat / NS2MS);

        while (io) {
                fprintf(fp_output_log, "%lu, %u, %lu, %u, %lu, %lu\n",
                        arrival_time, io->time, io->lat, io->times, io->length, io->logical_sector);
                
                if (io->lat > 11000000)
                        printf("%lu, %u, %lu, %u, %lu, %lu\n",
                                arrival_time, io->time, io->lat, io->times, io->length, io->logical_sector);

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