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
typedef struct {
    int x;
    int y;
}pos;

void display_print_info(struct fb_var_screeninfo vinfo,struct fb_fix_screeninfo finfo);

int main(int argc, char *argv[])
{
	int file_descriptor = open("/dev/fb0", O_RDWR);
	int i,x,y; // counters

	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	
    // Info
	ioctl(file_descriptor, FBIOGET_FSCREENINFO, &finfo);
	ioctl(file_descriptor, FBIOGET_VSCREENINFO, &vinfo);
	display_print_info(vinfo,finfo);
    // Initialize a black screen & memory-map
	uint16_t* display = (uint16_t*)mmap(0,finfo.smem_len, PROT_WRITE|PROT_READ, MAP_SHARED, file_descriptor, 0);
	struct fb_copyarea rect = {dx:0,dy:0,width:320,height:240};
    for (x=0;x<320;x++){
        for(y=0;y<240;y++){
            display[320*y + x] = 0x0000;
        }
    }
    ioctl(file_descriptor, 0x4680, &rect);
    


    int level = 1; 
    int quit = 1;

    // Dynamic snake 
    int snakeLength = 0;
    pos* snake = malloc(snakeLength+1*sizeof(pos));


    // Positions + initial snake
    pos dir = {x:1,y:0};
    pos apple = { 160,140 };
    pos next = { 160,120 };
    rect.width = 10; 
    rect.height = 10;

    rect.dx = next.x;
    rect.dy = next.y;

    // Starting the game
    printf("Initializing game\n");
    while(quit != 0){

        next.x = next.x + 10*dir.x;
        next.y = next.y + 10*dir.y;

        // In case it's running offscreen -> aka go trough the wall
        if(next.y >= 240 || next.y < 0){         
            next.y = abs(next.y - 240);
        }else if(next.x >= 320 || next.x < 0){    
            next.x = abs(next.x - 320);
        }

        
        // 3 different cases
        if ( display[next.y*320+next.x] == 0xffff){

            printf("Game over!\n");
            break;
        }
        else if(next.x == apple.x && next.y == apple.y){

            snakeLength++;

            

        }
        else{

            // Delete tail visual
            rect.dx = snake[snakeLength].x;
            rect.dy = snake[snakeLength].y;
            for(x=rect.dx; x<rect.dx+10; x++){
                for(y=rect.dy; y<rect.dy+10;y++){
                    display[320*y + x] = 0x0000;
                }
            }
            ioctl(file_descriptor, 0x4680, &rect);

            //Update snake
            for (i=snakeLength;i>0;i--){
                snake[i] = snake[i-1];
            }
            snake[0] = next;


        }
        //Generate head
        rect.dx = next.x;
        rect.dy = next.y;
        for(x=rect.dx; x<rect.dx+9; x++){
            for(y=rect.dy; y<rect.dy+9;y++){
                display[320*y + x] = 0xffff;
            }
        }
        ioctl(file_descriptor, 0x4680, &rect);

        printf("This is now the snake:\n");
        for (i=0;i<snakeLength+1;i++){
            printf("x:%d, y:%d \n",snake[i].x,snake[i].y );
        }


        sleep(1/level);
    }


    free(snake);
    munmap(display,finfo.smem_len);
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


/*
void drawApple(int file_descriptor, int x, int y){




}
*/

