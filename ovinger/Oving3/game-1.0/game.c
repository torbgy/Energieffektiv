#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//fb0
#include <linux/fb.h>
//open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// ioctl
#include <sys/ioctl.h>
//mmap
#include <sys/mman.h>
//sleep
#include <unistd.h>



/*
    Screen size: 320x240
    Bits per pixel: 16 ( 5 red, 5 blue, 6 green ) 

*/

void display_print_info(struct fb_var_screeninfo vinfo,struct fb_fix_screeninfo finfo);

int main(int argc, char *argv[])
{
	int file_descriptor = open("/dev/fb0", O_RDWR);
	
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	
	ioctl(file_descriptor, FBIOGET_FSCREENINFO, &finfo);
	ioctl(file_descriptor, FBIOGET_VSCREENINFO, &vinfo);
	
	uint16_t* display = (uint16_t*)mmap(0,finfo.smem_len, PROT_WRITE|PROT_READ, MAP_SHARED, file_descriptor, 0);
	
	
	display_print_info(vinfo,finfo);
	
	
	  
    struct fb_copyarea rect;
    rect.dx = 0;
    rect.dy = 0;
    rect.width = 320;
    rect.height = 240;
    
    int y, x;
    printf("game: before the loop\n");
    while(1){
        for( y = rect.dy ; y < rect.height+rect.dy ; y++){
            for( x = rect.dx ; x < rect.width+rect.dx ; x++){
            
                display[320*y + x] += 0x0001;
            
            }
        }
        
     
        ioctl(file_descriptor, 0x4680, &rect);
        
        //sleep(1);
    }



    munmap(display,320*240);
    close(file_descriptor);
	exit(EXIT_SUCCESS);
}

void display_print_info(struct fb_var_screeninfo vinfo,struct fb_fix_screeninfo finfo){
    
    printf("smem length:    %d \n", finfo.smem_len);
    printf("line length:    %d \n", finfo.line_length);
    printf("xres:           %d \n", vinfo.xres);
    printf("yres:           %d \n", vinfo.yres);
    printf("xres_virtual:   %d \n", vinfo.xres_virtual);
    printf("yres_virtual:   %d \n", vinfo.yres_virtual);
    printf("xoffset:        %d \n", vinfo.xoffset);
    printf("yoffset:        %d \n", vinfo.yoffset);
    printf("bits per pixel: %d \n", vinfo.bits_per_pixel);
    printf("height:         %d \n", vinfo.height);
    printf("width:          %d \n", vinfo.width); 
}
