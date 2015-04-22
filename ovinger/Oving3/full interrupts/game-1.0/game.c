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
//signals
#include <linux/fs.h>
#include <fcntl.h>
#include <signal.h>

typedef struct {
    int x;
    int y;
}pos;

void sig_handler(int sig_num);
void newApple(uint16_t* display,int fd_random, int fd_fb,pos* apple,pos* next, struct fb_copyarea *rect);

pos dir = {1,0};
int speed = 2; 
int quit = 0;
int fd_gamepad;
int fd_fb;
int fd_random;

int main(int argc, char *argv[])
{   

    //Devices to use
	fd_fb = open("/dev/fb0", O_RDWR);
    fd_random = open("/dev/urandom", O_RDONLY);
    fd_gamepad = open("/dev/gamepad", O_RDONLY);


    // Counters
	int x,y; 


    // Initialize a black screen & memory-map
	uint16_t* display = (uint16_t*)mmap(0,320*240*2, PROT_WRITE|PROT_READ, MAP_SHARED, fd_fb, 0);
	struct fb_copyarea rect = {dx:0,dy:0,width:320,height:240};
    for (x=0;x<320;x++){
        for(y=0;y<240;y++){
            display[320*y + x] = 0x0000;
        }
    }
    ioctl(fd_fb, 0x4680, &rect);
    




    // Dynamic snake 
    unsigned int snakeLength = 0;
    pos* snake = malloc(snakeLength+1*sizeof(pos));


    // Positions + initial snake
    
    pos apple;
    pos next = { 160,120 };
    rect.width = 10; 
    rect.height = 10;
    newApple(display,fd_random,fd_fb,&apple,&next,&rect);

    // Signals
    signal(SIGIO, sig_handler); 
    fcntl(fd_gamepad, F_SETOWN, getpid());
    int oflags = fcntl(fd_gamepad, F_GETFL);
    fcntl( fd_gamepad, F_SETFL, oflags | FASYNC );


    // Starting the game
    while(quit == 0){        


        next.x = next.x + 10*dir.x;
        next.y = next.y + 10*dir.y;

        // In case it's running offscreen -> aka go trough the wall
        if(next.y >= 240 || next.y < 0){         
            next.y = abs(abs(next.y) - 240);
        }else if(next.x >= 320 || next.x < 0){    
            next.x = abs(abs(next.x) - 320);
        }

        
        // 3 different cases
        if ( display[next.y*320+next.x] == 0xffff){

            printf("Game over!\n");
            break;
        }
        else if(next.x == apple.x && next.y == apple.y){
            // Create a new apple
            newApple(display,fd_random,fd_fb,&apple,&next,&rect);
        

            // Update snake
            snakeLength++;
            realloc(snake,snakeLength*sizeof(pos));
            for (x=snakeLength;x>0;x--){
                snake[x] = snake[x-1];
            }
            snake[0]=next;
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
            ioctl(fd_fb, 0x4680, &rect);

            //Update snake
            for (x=snakeLength;x>0;x--){
                snake[x] = snake[x-1];
            }
            snake[0] = next;


        }
        //Generate head
        rect.dx = next.x;
        rect.dy = next.y;
        for(x=rect.dx; x<rect.dx+9; x++){
            for(y=rect.dy; y<rect.dy+8;y++){
                display[320*y + x] = 0xffff;
            }
        }
        ioctl(fd_fb, 0x4680, &rect);


        usleep(1000*100 - speed*200);
    }

    printf("Your score: %d\n",snakeLength);
    free(snake);
    munmap(display,320*240*2);
    close(fd_fb);
    close(fd_random);
    close(fd_gamepad);
	exit(EXIT_SUCCESS);
}

void sig_handler(int sig_num){

    uint8_t buffer;
    read(fd_gamepad,&buffer,1);

    switch(buffer | 0xff00){
        case (0xfffe) : // 1
            if (dir.x == 0){
                dir.y = 0;
                dir.x = -1; 
            }
            return;
        case (0xfffd) : // 2
            if(dir.y == 0){
                dir.y = 1;
                dir.x = 0; 
            }
            return;
        case (0xfffb) : // 3
            if (dir.x == 0){
                dir.y = 0;
                dir.x = 1;
            }
            return;
        case (0xfff7) : // 4
            if(dir.y == 0){
                dir.y = -1;
                dir.x = 0;
            }
            return;
        case (0xffef) : // 5
            return;
            
        case (0xffdf) : // 6
            speed++;
            return;
            
        case (0xffbf) : // 7
            quit = 1;
            return;
        case (0xff7f) : // 8
            speed--;
            return;
        default:
            return;    
        }


}




void newApple(uint16_t* display,int fd_random, int fd_fb,pos* apple,pos* next, struct fb_copyarea *rect){
    unsigned int random;
    unsigned int appleFound = 1;
    int x,y;    
    while(appleFound != 0){

        appleFound++;
        read(fd_random,&random,5);
        apple->x = 10*(random % 32);
        read(fd_random,&random,5);
        apple->y = 10*(random % 24);


        if ( display[320*apple->y + apple->x] != 0xffff && apple->x != next->x && apple->y != next->y){
            appleFound = 1;
            //Draw apple
            rect->dx = apple->x;
            rect->dy = apple->y;
            for(x=rect->dx; x<rect->dx+10; x++){
                for(y=rect->dy; y<rect->dy+10;y++){
                    display[320*y + x] = 0xf000;
                }
            }
            ioctl(fd_fb, 0x4680, rect);
            break;
        }else if(appleFound>1000){
            printf("Can't find location for a new apple, exiting\n");
            exit(1);
        }
    }
    return;
}


