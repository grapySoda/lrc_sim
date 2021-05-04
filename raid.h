#ifndef _RAID_H_
#define _RAID_H_

#define ALL_CMR 		0	/* All of disks used CMR tech */
#define ALL_SMR 		1	/* All of disks used SMR tech */
#define DATA_CMR_PARITY_SMR	2	/* All of data disks used CMR tech, parity disks used SMR tech */
#define DATA_SMR_PARITY_CMR	3	/* All of data disks used SMR tech, parity disks used CMR tech */

#define PAGE_SHIFT		12

#define SECTOR_SIZE		512
#define STRIPE_SECTORS		8

#define CHUNK_SECTORS		1024

#define HANDLE_LIST_SIZE	65536
#define MAPPING_TABLE_SIZE	65536

#define BUF_READ_TIME		100
#define BUF_WRITE_TIME		200
// #define BUF_WB_TIME		200

#define DISK_READ_TIME		1000
#define DISK_WRITE_TIME		2000

#define PREXOR_TIME		200
#define XOR_TIME		200
#define GEN_SYNDROME_TIME	200

#define DISK_HIT		100
#define BUF_SIZE		256

// typedef int (*smr_translate_fn) (struct dm_target *target,
// 			  unsigned int argc, char **argv);
// typedef int (*smr_reportzone_fn) (struct dm_target *target,
// 			  unsigned int argc, char **argv);
// typedef int (*smr_reset_wp_fn) (struct dm_target *target,
// 			  unsigned int argc, char **argv);
// typedef int (*smr_finish_fn) (struct dm_target *target,
// 			  unsigned int argc, char **argv);

// struct smr {
// 	unsigned long		*ps_table;	/* persistent cache */
	
// 	struct stl {
// 		unsigned long	pba;
// 		unsigned long	lba;
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
	const char 		*name;		/* RAID algorithm. */
	const unsigned long 	buffer_size;	/* disk buf size */
	const unsigned long	disk_capacity;	/* disk capacity */
	const unsigned int	rpm;		/* rpm */
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

struct rdev {
	struct superblock	*sb;
	struct buf		*buf;

	char 			*disk_type;
	int			*buf_ptr;
	unsigned long		nr_buf;
	// unsigned long		sector;		/* sector of this row */
	// unsigned long		flags;
};

struct shdev {
	struct superblock	*sb;
	struct buf		*buf;

	int			*buf_ptr;
	unsigned long		nr_buf;
	unsigned long		sector;
	unsigned long		flags;
};

struct stripe_head {
	int 			stripe_number;	/* chunk number */
	int 			stripe_offset;	/* chunk offset */
	
	short			lpd1_idx;	/* local parity disk1 index */
        short			lpd2_idx;	/* local parity disk2 index */
	short			gqd1_idx;	/* (global parity disk1)'Q' disk index for raid6 */
        short			gqd2_idx;	/* (global parity disk2)'Q' disk index for raid6 */
	
	unsigned long		state;		/* state flags */
	int			disks;		/* disks in stripe */

	struct shdev 		*dev;		/* allocated with extra space depending of RAID geometry */
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
        unsigned long   	logical_sector;
        unsigned long   	length;
};

void make_request(struct mddev *mddev, struct io *io);

void init_handle_list(struct mddev *mddev);
void init_disk(struct mddev *mddev);

#endif