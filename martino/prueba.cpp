#include<chrono>
#include<thread>
#include<pigpio.h>
#include<stdio.h>
#include<stdlib.h>

int pi;
void set_speed(int vel){
    //for(int  i = 1; i <= 4; i++){
        int x1 = (vel / 254) + 1, x2 = (vel % 254) + 1;
       
        serWriteByte(pi, (uint8_t)(00));
        serWriteByte(pi, (uint8_t)(01));
        serWriteByte(pi, (uint8_t)(01));
        serWriteByte(pi, (uint8_t)(x1));
        serWriteByte(pi, (uint8_t)(x2));
        serWriteByte(pi, (uint8_t)(255));
        /*
        char a[50] = {
        '0', '0','0', '0','0', '0','0', '0',
        '0', '0','0', '0','0', '0', '0', i+'0', 
        '0', '0','0', '0','0', '0', '0', '1', 
        ((x1>>7)&1) + '0', ((x1>>6)&1) + '0', ((x1>>5)&1) + '0', ((x1>>4)&1) + '0', 
        ((x1>>3)&1) + '0', ((x1>>2)&1) + '0', ((x1>>1)&1) + '0', (x1&1) + '0', 
        ((x2>>7)&1) + '0', ((x2>>6)&1) + '0', ((x2>>5)&1) + '0', ((x2>>4)&1) + '0', 
        ((x2>>3)&1) + '0', ((x2>>2)&1) + '0', ((x2>>1)&1) + '0', (x2&1) + '0',
        '1','1','1','1','1','1','1','1', '\0'};
        for(int i = 0; i < 48; i++){
            serWriteByte(pi, a[i]-'0');
        }*/
        //int y = serWrite(pi, a, 48); 
       // printf("%d\n", y);
       //serWrite(pi,"01", 2);
   // }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    return;
}

int main(){
    printf("running pigpiod\n"); 
    if(gpioInitialise()<0){
        printf("failed");
        return -1;
    }


  //  gpioSetMode(3, PI_OUTPUT);
    char dir[10] = {'/','d', 'e', 'v', '/', 't', 't', 'y', 'S', '0'};
    ///dev/ttyS0
    pi = serOpen("/dev/ttyS0", 115200, 0);
    printf("%d", pi);
  //  gpioSetPWMfrequency(3, 5000); 

    int vel=1; 
   // while(true){
       // scanf("%d", &vel);
        set_speed(500);
   // }

    gpioTerminate();
        printf("finished");
    return 0;
}
