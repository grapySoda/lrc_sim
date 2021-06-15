#ifndef _RAID_H_
#define _RAID_H_

#define DISK_STR_SIZE		8

#define ALL_CMR 		0	/* All of disks used CMR tech */
#define ALL_SMR 		1	/* All of disks used SMR tech */
#define DATA_CMR_PARITY_SMR	2	/* All of data disks used CMR tech, parity disks used SMR tech */
#define DATA_SMR_PARITY_CMR	3	/* All of data disks used SMR tech, parity disks used CMR tech */

#define NS2MS			1000000

#define PAGE_SHIFT		12
#define ZONE_SHIFT		28

#define PAGE_SIZE		4096
#define SECTORS_PER_PAGE	8

#define SECTOR_SIZE		512
#define STRIPE_SECTORS		8

#define CHUNK_SECTORS		1024

#define HANDLE_LIST_SIZE	65536
#define MAPPING_TABLE_SIZE	65536

/* base on ns */
#define ROTATE_PER_TRACK	8333330	 	/* 8.3 ms */
#define ROTATE_PER_SECTOR	120000		/* 130 us */
// #define ROTATE_PER_SECTOR	39150		/* 60  us */
#define TRANSFER_TIME		39062		/* 39  us */
// #define SEEK_BETWEEN_TRACK	8		/* 8   ns */
#define SEEK_BETWEEN_TRACK	8		/* 8   ns */
#define CONTROLLER_TIME		1000000		/* 2.5 ms */

#define RAID_5_CAL_DELAY	2500000		/* 2.5 ms */

/* delay time is based on micro-second */
// #define BUF_READ_TIME		42000
// #define BUF_WRITE_TIME		42000

// #define BUF_DEVIATION		3000		/* 3 microsecond deviation */
// #define BUF_FLUSH_DEVIATION	100		/* 100 requests deviation */
// #define BUF_FLUSH_THRESHOLD	0.9
// #define BUF_FLUSH_RATIO		450		/* flush once per 500 write requests */
// #define BUF_WB_TIME		200

#define DISK_READ_TIME		6344000
#define DISK_WRITE_TIME		7979000

#define PREXOR_TIME		200
#define XOR_TIME		200
#define GEN_SYNDROME_TIME	200

#define DISK_HIT		100
#define BUF_SIZE		256		/* MG */

#define ZONE_EMPTY		0
#define ZONE_OPEN		1
#define ZONE_FULL		2

#define SMR_ERROR		1

// typedef int (*smr_translate_fn) (struct dm_target *target,
// 			  unsigned int argc, char **argv);
// typedef int (*smr_reportzone_fn) (struct dm_target *target,
// 			  unsigned int argc, char **argv);
// typedef int (*smr_reset_wp_fn) (struct dm_target *target,
// 			  unsigned int argc, char **argv);
// typedef int (*smr_finish_fn) (struct dm_target *target,
// 			  unsigned int argc, char **argv);

// struct zone {
// 	unsigned long 		*bitmap;
// 	unsigned long		w_ptr;

// 	struct mapping_table {
// 		int 		page_number;
// 		unsigned long	*addr;
// 	} mapping_table[MAPPING_TABLE_SIZE];
// };

// struct dm_smr {
// 	struct zone		*zone;
// 	unsigned long		*ps_bitmap;	/* persistent cache's bitmap */
	
// 	struct stl {
// 		unsigned long	*pba;
// 		unsigned long	*lba;
// 	} *stl_table;

// 	smr_translate_fn	translate;
// 	smr_reportzone_fn	report_zone;
// 	smr_reset_wp_fn		reset_sp;
// 	smr_finish_fn		finish_zone;
// };

# define sector_div(n, b)( \
{ \
	int _res; \
	_res = (n) % (b); \
	(n) /= (b); \
	_res; \
} \
)

struct raid_type {
	const char 		*name;		/* RAID algorithm. */
	const char 		*descr;		/* Descriptor text for logging. */
	const unsigned 		parity_devs;	/* # of parity devices. */
	const unsigned 		minimal_devs;	/* minimal # of devices in set. */
	const unsigned 		level;		/* RAID level. */
	const unsigned 		disk_type_dist;	/* disk type distribution */
};

struct disk_info {
	const char 		*type;		/* disk type */
	const unsigned long 	buffer_size;	/* disk buf size */
	const unsigned long	disk_capacity;	/* disk capacity */
	const unsigned int	rpm;		/* rpm */
	const unsigned int	nr_sectors;	/* sectors per track */
	const unsigned int 	nr_cylinders;	/* cylinders per disk */
	const int		heads;		/* heads */

	/* if disk type is smr */
	const unsigned int	zone_size;	/* bytes per zone */
};

enum {
	STRIPE_OP_PREXOR,
	STRIPE_OP_BIODRAIN,
	STRIPE_OP_RECONSTRUCT,
};

enum {
	IO_READ,
	IO_WRITE,
};

enum rdev_flags {
	R_Wantread,
	R_Wantwrite,
	R_Appended,
	R_Reversed,
};

// struct disk_info {
// 	const unsigned long	buffer_size;
// 	const unsigned long	capacity;
// 	const unsigned long	rpm;
// };

// struct mapping_table {
// 	int 			page_num;
// 	long long 		*addr;
// };

// #define MAPPING_TABLE_SIZE 256

// struct superblock {
// 	int 			*bitmap;
// 	int 			*invalid_map;

// 	struct mapping_table {
// 		int 		page_num;
// 		unsigned long 	*addr;
// 	} mapping_table[MAPPING_TABLE_SIZE];
// };

// struct handle_list {
// 	struct stripe_head	*sh;
// 	struct handle_list	*last;
// 	struct handle_list	*next;
// };

struct superblock {
	unsigned long 		*bitmap;
	unsigned long		*invalid_map;

	struct mapping_table {
		int 		page_number;
		unsigned long	*addr;
	} mapping_table[MAPPING_TABLE_SIZE];
};

struct buf {
	int 			number;
	int			offset;
};

struct disk_head {
	unsigned int 		sector;
	unsigned int 		cylinder;
};

struct zone {
	int					state;
	unsigned long		start_lba;
	unsigned long		end_lba;
	unsigned long		wp;		/* write pointer */
	unsigned int		invalid_pages;
	unsigned int		used_pages;
	unsigned int		num;
};

struct rdev {
	int 			num;		/* just for debug */
	struct superblock	*sb;
	struct buf		*buf;

	char 			*disk_type;
	
	struct disk_head 	disk_head; 

	unsigned int		buf_write_ptr;
	unsigned int 		buf_clean_ptr;

	unsigned long		buf_usage;
	unsigned long		nr_buf;
	unsigned long		sector;		/* sector of this row */
	unsigned long		flags;

	unsigned int		nr_sectors;	/* sectors per track */
	unsigned int 		nr_cylinders;	/* cylinders per disk */
	
	int			heads;		/* the heads' position */

	/* if disk type is smr */
	unsigned int		zone_size;	/* bytes per zone */
	unsigned long 		nr_zones;	/* number of zones per disk */
	unsigned int		nr_pages;	/* number of pages per zone */

	unsigned int		start_lba;
	unsigned int		end_lba;

	struct zone		*zones;
	int			*mt;		/* mapping table */
	unsigned long 		algo;
};

// struct shdev {
// 	struct superblock	*sb;
// 	struct buf		*buf;

// 	int			*buf_ptr;
// 	unsigned long		nr_buf;
// 	unsigned long		sector;
// 	unsigned long		flags;
// };

struct stripe_head {
	unsigned long 		stripe_number;	/* chunk number */
	unsigned long		stripe_offset;	/* chunk offset */
	
	short			lpd1_idx;	/* local parity disk1 index */
        short			lpd2_idx;	/* local parity disk2 index */
	short			gqd1_idx;	/* (global parity disk1)'Q' disk index for raid6 */
        short			gqd2_idx;	/* (global parity disk2)'Q' disk index for raid6 */
	
	unsigned long		sector;
	unsigned long		state;		/* state flags */
	int			disks;		/* disks in stripe */

	struct rdev 		**dev;		/* allocated with extra space depending of RAID geometry */
};

struct mddev {
        // struct disk_info 	cmr;
	// struct disk_info 	smr;

	struct stripe_head	*handle_list[HANDLE_LIST_SIZE];

	struct disk_info	*data_disk_info;
	struct disk_info	*parity_disk_info;

	int 			parity_disks;
	int 			data_disks;
	int 			algorithm;
	int 			level;
	int 			raid_disks;
	int 			chunk_sectors;

	unsigned long		flags;

	struct rdev		*rdev;
};

struct io {
	unsigned int		time;
	unsigned int		times;
	unsigned int		lat;
        unsigned long   	logical_sector;
        unsigned long   	length;
};

unsigned long make_request(struct mddev *mddev, struct io *io);
unsigned long test_single_disk(struct mddev *mddev, struct io *io, short rw);

void init_handle_list(struct mddev *mddev);
void init_disk(struct mddev *mddev);

#endif