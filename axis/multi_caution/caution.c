#include <stdio.h>  
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
/*
#include <stdbool.h>
#include <stdlib.h>
#include <sys/ioctl.h>
*/

#define	DEVFILE_STORED_CAUTION	"/dev/mmcblk0p6"
#define	DEVFILE_MEMORY			"/dev/mem"

#define	PIXELS_WIDTH	800
#define	PIXELS_HEIGHT	80
#define	BYTES_PER_PIXEL	4	// ARGB
#define	BYTES_PER_LINE	(PIXELS_WIDTH*BYTES_PER_PIXEL)
#define	BYTES_OF_IMAGE	(BYTES_PER_LINE*PIXELS_HEIGHT)

#define	BYTES_OF_HEADER	54	// common header + BITMAPINFOHEADER

typedef struct {
	unsigned short masic_num;
	unsigned int file_length;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int pixel_start;
	// there are more fields for BITMAPINFOHEADER
} bitmap_header_type;

int check_image_file(bitmap_header_type *p_header)
{
	return 0;
}

void *get_mapped_addr(char *p_filename, int map_size, int offset)
{
	int fd;
	void *p_mapped_addr;

	fd = open(p_filename, O_RDWR);
	if(fd < 0)
	{
		printf("%s(): fail to open() for file [%s], err no %d\n", __func__, p_filename, errno);
		return NULL;
	}

	p_mapped_addr = mmap(NULL, map_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, (off_t)offset);
	printf("p_mapped_addr : 0x%x\n", p_mapped_addr);

	close(fd);

	if (p_mapped_addr == MAP_FAILED)
	{
		printf("%s(): fail to mmap() with size [%d] and offset [%p], err no %d\n",
			__func__, map_size, offset, errno);
		return NULL;
	}

	return p_mapped_addr;
}


void dump(char *p_mem, int size)
{
	int i;

	for(i = 0; i < size; i++, p_mem++)
	{
		if(i % 16 == 0)
			printf("%p", p_mem);

		if(i % 8 == 0)
			printf(" ");

		printf(" %02x", *p_mem);

		if(i % 16 == 15)
			printf("\n");
	}
	printf("\n");
}

#define	BITMAP_FILE_MAPPING_LEN	(BYTES_PER_LINE * PIXELS_HEIGHT + BYTES_OF_HEADER)

#define	ADDR_PARKING_GUIDE		0x86a00000
#define	SIZE_PER_GUIDEIMAGE		0x00200000		// guideline + caution text
#define OFFSET_CAUTION			0x00138800		// start of caution test on each image
#define	PARKING_MAPPING_LEN		(SIZE_PER_GUIDEIMAGE * 4)

// return value:
// 	0 if success,
// 	-1 if fail
int change_caution_text(char *p_bitmap_file)
{
	char *p_bitmap, *p_stored_caution, *p_pmap_parking, *p_caution0, *p_caution3;
	const char *p_cur_bm;
	char *p_cur_c0, *p_cur_c3;
	char *p_cur_c1, *p_cur_c2;
	int line;


	p_bitmap = get_mapped_addr(p_bitmap_file, BITMAP_FILE_MAPPING_LEN, 0);
	if(!p_bitmap)
		return -1;
	printf("p_bitmap : 0x%x\n", p_bitmap);

	check_image_file((bitmap_header_type *)p_bitmap);
	
	p_pmap_parking = get_mapped_addr(DEVFILE_MEMORY, PARKING_MAPPING_LEN, ADDR_PARKING_GUIDE);
	if(!p_pmap_parking)
	{
		munmap(p_bitmap, BITMAP_FILE_MAPPING_LEN);
		return -1;
	}
	printf("p_pmap_parking : 0x%x\n", p_pmap_parking);

	p_cur_bm = p_bitmap + BYTES_OF_HEADER;		// skip header
	//dump(p_cur_bm, 256);

//*********

	printf("** just to check the offse. 256 bytes from 0x86a00000+0x00138800\n");
	dump(p_pmap_parking+0x138800, 256);

	printf("** before copying on p_pmap_parking [0x%08x]\n", p_pmap_parking);
	dump(p_pmap_parking, 256);
	//memcpy(p_pmap_parking, p_cur_bm, 800*80*4);
	memcpy(p_pmap_parking+SIZE_PER_GUIDEIMAGE*0+OFFSET_CAUTION, p_cur_bm, 800*80*4);
	printf("** after copying on p_pmap_parking [0x%08x]\n", p_pmap_parking);
	dump(p_pmap_parking, 256);
	printf("copying on p_pmap_parking [0x%08x] 0\n", p_pmap_parking);
	memcpy(p_pmap_parking+SIZE_PER_GUIDEIMAGE*1+OFFSET_CAUTION, p_cur_bm, 800*80*4);
	printf("copying on p_pmap_parking [0x%08x] 1\n", p_pmap_parking);
	memcpy(p_pmap_parking+SIZE_PER_GUIDEIMAGE*2+OFFSET_CAUTION, p_cur_bm, 800*80*4);
	printf("copying on p_pmap_parking [0x%08x] 2\n", p_pmap_parking);
	memcpy(p_pmap_parking+SIZE_PER_GUIDEIMAGE*3+OFFSET_CAUTION, p_cur_bm, 800*80*4);
	printf("copying on p_pmap_parking [0x%08x] 3\n", p_pmap_parking);

	return 0;
//*********
	p_caution0 = p_pmap_parking + OFFSET_CAUTION;
	p_caution3 = p_pmap_parking + SIZE_PER_GUIDEIMAGE * 3 + OFFSET_CAUTION;
	printf("p_pmap_parking [0x%08x] p_caution3 [0x%08x]\n", (int)p_pmap_parking, (int)p_caution3);

	// line order is reverse in bitmap file format
	p_cur_c0 = p_caution0 + (PIXELS_HEIGHT - 1) * BYTES_PER_LINE;
	p_cur_c1 = p_caution0 + SIZE_PER_GUIDEIMAGE + (PIXELS_HEIGHT - 1) * BYTES_PER_LINE;
	p_cur_c2 = p_caution0 + SIZE_PER_GUIDEIMAGE * 2 + (PIXELS_HEIGHT - 1) * BYTES_PER_LINE;
	p_cur_c3 = p_caution3 + (PIXELS_HEIGHT - 1) * BYTES_PER_LINE;
	printf("p_cur_c0 [0x%08x] p_cur_c1 [0x%08x] p_cur_c2 [0x%08x] p_cur_c3 [0x%08x] \n",
		(int)p_cur_c0, (int)p_cur_c1, (int)p_cur_c2, (int)p_cur_c3);

	for(line = 0; line < PIXELS_HEIGHT; line++)
	{

		//memcpy(addr_bitmap+line*800*4, addr_stored_caution*(bitmap_header.lines-lines)*4, 800*4);

		//memcpy(addr_bitmap+line*800*4, addr_pmap_caution0*(bitmap_header.lines-lines)*4, 800*4);
		memcpy(p_cur_c0, p_cur_bm, BYTES_PER_LINE);
		memcpy(p_cur_c1, p_cur_bm, BYTES_PER_LINE);
		memcpy(p_cur_c2, p_cur_bm, BYTES_PER_LINE);
		memcpy(p_cur_c3, p_cur_bm, BYTES_PER_LINE);
		printf("p_cur_c3 [0x%08x] p_cur_bm [0x%08x]\n", (int)p_cur_c3, (int)p_cur_bm);

		p_cur_bm += BYTES_PER_LINE;
		p_cur_c0 -= BYTES_PER_LINE;
		p_cur_c1 -= BYTES_PER_LINE;
		p_cur_c2 -= BYTES_PER_LINE;
		p_cur_c3 -= BYTES_PER_LINE;
	}

	munmap(p_bitmap, BITMAP_FILE_MAPPING_LEN);
	munmap(p_pmap_parking, PARKING_MAPPING_LEN);

	return 0;
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Usage: %s image_file\n", argv[0]);
		printf("\timage_file: the name of image file for caution text.\n");
	}

	change_caution_text(argv[1]);
}


