//********** my_ptable.c **********

/* James Jessen
 * 10918967
 *
 * CptS 360
 * Lab #1
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define SECTOR_SIZE 512
#define PARTITION_TABLE_OFFSET 0x1BE

static int fd = 0, nr = 0;
static char buf[SECTOR_SIZE];

struct partition
{
	unsigned char drive;            /* 0x80 - active */

	unsigned char head;             /* starting head */
	unsigned char sector;           /* starting sector */
	unsigned char cylinder;         /* starting cylinder */

	unsigned char sys_type;         /* partition type, 5 = EXT */

	unsigned char end_head;         /* end head */
	unsigned char end_sector;       /* end sector */
	unsigned char end_cylinder;     /* end cylinder */

	unsigned long start_sector;     /* starting sector counting from 0 */
	unsigned long nr_sectors;       /* nr of sectors in partition */
};

void print_raw(struct partition *p)
{
	/*  The cylinder value is actually 10 bits.
	*  The highest 2 bits of cylinder value are in
	*  the leftmost 2 bits of sector, which is only 6 bits.
	*/
	// Sector:   |_|_|_|_|_|_|_|_|	
	//               |-----------|
	//         &  0 0 1 1 1 1 1 1     Use bit masking
	//           |-------|-------|	
	//				 3	     F

	int start_sect = p->sector & 0x3F;
	int end_sect = p->end_sector & 0x3F;

	//             From Sector
	// Cylinder:    | v | v |_|_|_|_|_|_|_|_|	2 + 8 = 10 bytes

	int start_cyl = ((p->sector >> 6) << 8) + p->cylinder;
	int end_cyl = ((p->end_sector >> 6) << 8) + p->end_cylinder;

	printf("%2X", p->drive);
	printf("%4X", p->head);
	printf("%4d", start_sect);
	printf("%4d", start_cyl);
	printf("%4X", p->sys_type);
	printf("%4X", p->end_head);
	printf("%4d", end_sect);
	printf("%4d", end_cyl + 1);
	printf("%6lu", p->start_sector);
	printf("%6lu", p->nr_sectors);
	printf("\n");
}

void print_fdisk(struct partition *p)
{
	int start_cyl = ((p->sector >> 6) << 8) + p->cylinder;
	int end_cyl = ((p->end_sector >> 6) << 8) + p->end_cylinder;

	printf("%5d", start_cyl + 1);
	printf("%10d", end_cyl + 1);
	printf("%10lu", p->nr_sectors / 2);
	printf("%6X", p->sys_type);
	printf("\n");
}

void rec_print_extend(struct partition *p, unsigned long base_sector)
{
	int i = 0;
	unsigned long abs_start_sector = 0;
	/*
	If a partition is EXTEND type (type=5), the partition may be further divided
	into more partitions. The extended partitions forms a link-list.

	Assume P4 is EXT type:
	P4's beginSector = MBR
						P5's beginSector
						P6's MBR's sector# = MBR
							(r.e. to P4)    P6's begin sector#
											P7's MBR r.e. to P4 --> etc.

	In the extended partitions, all the sector numbers are RELATIVE to the
	beginSector of the extend partition.

	The first sector of each extended partition is a local MBR. Each local MBR also
	has a partition table at the offset PARTITION_TABLE_OFFSET. It contains only 2 entries. The first
	entry defines that extended partition, and the second entry points to the MBR of
	the next extended partition. As usual, the link list ends with a 0 pointer.
	*/

	printf("Relative Sector: %lu\n", p->start_sector);

	if (p->start_sector == 0)
		return;

	if (base_sector == 0)
	{
		base_sector = p->start_sector;
		abs_start_sector = p->start_sector;
	}
	else
	{
		abs_start_sector = p->start_sector + base_sector;
	}

	printf("Absolute Sector: %lu\n", abs_start_sector);

	lseek(fd, (long)abs_start_sector * SECTOR_SIZE, 0);

	if ((nr = read(fd, buf, sizeof(buf))) == -1)
	{
		perror("Read");
		exit(1);
	}

	for (i = 0; i < 2; i++)
	{
		p = (struct partition *)(buf + PARTITION_TABLE_OFFSET + (16 * i));
		print_fdisk(p);
	}

	rec_print_extend(p, base_sector);
}

int main()
{
	int i = 0, ret = 0;
	struct partition *p;

	if ((fd = open("vdisk", O_RDONLY)) == -1)
	{
		perror("Open");
		exit(1);
	}

	if ((nr = read(fd, buf, sizeof(buf))) == -1)
	{
		perror("Read");
		exit(1);
	}

	printf("\n---------------- RAW Form ----------------\n");
	for (i = 0; i < 4; i++)
	{
		p = (struct partition *)(buf + PARTITION_TABLE_OFFSET + (16 * i));
		print_raw(p);
	}

	printf("\n------------ Linux fdisk Form -----------\n");
	printf("start_cyl  end_cyl  blocks  type\n");
	for (i = 0; i < 4; i++)
	{
		p = (struct partition *)(buf + PARTITION_TABLE_OFFSET + (16 * i));
		print_fdisk(p);

		if (p->sys_type == 5)
		{
			printf("\n****** Look for Extend Partition ******\n");
			rec_print_extend(p, 0);
		}
	}

	if (ret = close(fd) == -1)
	{
		perror("Close");
		exit(1);
	}

	return 0;
}