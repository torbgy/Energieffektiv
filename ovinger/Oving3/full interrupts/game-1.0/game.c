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
#define SIG_NUM 50 // Signal number

typedef struct {
    int x;
    int y;
}pos;


void sig_handler(int sig_num, siginfo_t *info,void *unused);
void newApple(uint16_t* display,int fd_random, int fd_fb,pos* apple,pos* next, struct fb_copyarea *rect);

pos dir = {1,0};
int speed = 1; 
int reset = 0;
int pauseGame = 1;
int fd_gamepad;
int fd_fb;
int fd_random;

int main(int argc, char *argv[])
{   

    //Devices to use
	fd_fb = open("/dev/fb0", O_RDWR);
    fd_random = open("/dev/urandom", O_RDONLY);
    fd_gamepad = open("/dev/gamepad", O_WRONLY);

    if (fd_gamepad < 0 || fd_random < 0 || fd_fb < 0){
        printf("game: Error opening one of the drivers\n");

    }
    // Memory Map
    uint16_t* display = (uint16_t*)mmap(0,320*240*2, PROT_WRITE|PROT_READ, MAP_SHARED, fd_fb, 0);
    // Signals
    struct sigaction sig;
    sig.sa_sigaction = sig_handler;
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIG_NUM,&sig,NULL);
    char buf[10];
    sprintf(buf,"%i",getpid());
    write(fd_gamepad,buf,11);

    // Counters & internals
	int x,y,i; 
    pos snake[24*32];
    unsigned int snakeLength;
    struct fb_copyarea rect;
    pos next;
    pos apple;

    while(1){
        reset = 0;
        pauseGame = 1;
        snakeLength = 0;

        // Initialize a black screen	
        rect.dx = 0;
        rect.dy = 0;
        rect.width = 320;
        rect.height = 240;
        for (x=0;x<320;x++){
            for(y=0;y<240;y++){
                display[320*y + x] = 0x0000;
            }
        }

        //Initial Snake;
        next.x = 140;
        next.y = 120;
        for (i=0;i<3;i++){
            
            next.x = next.x + 10*dir.x;
            next.y = next.y + 10*dir.y;

            snake[2-i] = next;
            snakeLength++;

            for(x=next.x; x<next.x+9; x++){
                for(y=next.y; y<next.y+8;y++){
                    display[320*y + x] = 0xffff;
                }
            }            
        }
        ioctl(fd_fb, 0x4680, &rect);
        



        // Inital apple
        
        rect.width = 10; 
        rect.height = 10;
        newApple(display,fd_random,fd_fb,&apple,&next,&rect);



        // Starting the game
        while(reset == 0){        


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
                pauseGame = 1;
                reset = 1;
               
            }
            else if(next.x == apple.x && next.y == apple.y){
                // Create a new apple
                newApple(display,fd_random,fd_fb,&apple,&next,&rect);
            

                // Update snake
                snakeLength++;
                /*
                realloc(snake,snakeLength*sizeof(pos));
                */
                for (x=snakeLength-1;x>0;x--){
                    snake[x] = snake[x-1];
                }
                snake[0]=next;
            }
            else{

                // Delete tail visual
                rect.dx = snake[snakeLength-1].x;
                rect.dy = snake[snakeLength-1].y;
                for(x=rect.dx; x<rect.dx+10; x++){
                    for(y=rect.dy; y<rect.dy+10;y++){
                        display[320*y + x] = 0x0000;
                    }
                }
                ioctl(fd_fb, 0x4680, &rect);

                //Update snake
                for (x=snakeLength-1;x>0;x--){
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

            printf("current snake: snakeLength:%d\n",snakeLength);
            for(x=0;x<snakeLength;x++){
                printf("[%d,%d]\n",snake[x].x,snake[x].y );
            }
            usleep(1000*100 - speed*10*1000);
            while(pauseGame == 1){
                usleep(1000*100);

            }


        }

        printf("Your score: %d\n",snakeLength);
        

    }
    munmap(display,320*240*2);
    close(fd_fb);
    close(fd_random);
    close(fd_gamepad);
	exit(EXIT_SUCCESS);
}


void sig_handler(int sig_num, siginfo_t *info, void *unused){

    uint8_t buffer = info->si_int;

    switch(buffer | 0xff00){
        case (0xfffe) : // 1
            if (dir.x == 0){
                dir.y = 0;
                dir.x = -1; 
            }
            return;
        case (0xfffd) : // 2
            if(dir.y == 0){
                dir.y = -1;
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
                dir.y = 1;
                dir.x = 0;
            }
            return;
        case (0xffef) : // 5
            pauseGame = abs(pauseGame-1);
            return;
            
        case (0xffdf) : // 6
            if(speed < 9){
                speed++;
            }
            
            return;
            
        case (0xffbf) : // 7
            reset = 1;
            return;
        case (0xff7f) : // 8
            if(speed > 1){
                speed--;    
            }
            
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


