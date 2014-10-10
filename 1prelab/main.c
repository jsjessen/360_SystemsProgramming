#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

char buf[512];
int sector;

main()
{
   int fd, r;
   sector = 10;                           // sector # 10 (count from 0)

   fd = open("vdisk", O_WRONLY);          // USE A VIRTUAL DISK image file
   strcpy(buf, "cs360 is fun");           // create a string in buf[]
   lseek(fd, (long)sector*512, 0);        // seek to the sector 10
   r = write(fd, buf, 512);               // write 512 bytes to fd
   printf("r=%d bytes written\n", r);     // show number of bytes written
   close(fd);                             // close the file descriptor fd

   fd = open("vdisk", O_RDONLY);          // open disk iamge file for READ
   lseek(fd, (long)sector*512, 0);        // seek to sector 10           
   r = read(fd, buf, 512);                // read 512 bytes into buf
   printf("r=%d read  buf=%s\n", r, buf); // show result
   close(fd);                             // close opened file
}
