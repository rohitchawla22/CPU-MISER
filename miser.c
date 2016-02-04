#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <inttypes.h>
#include <sys/types.h>

#define OVERLAP_FACTOR 1  /*Out of order overlap factor-vary its value from 0.1 to 1.0*/
//#include "version.h"
/*available frequencies in GHz-2.27,1.87,1.47,1.07*/

int main()
{

uint32_t reg1=0x186,reg2=0x187,reg3=0xc1,reg4=0xc2;
        uint64_t data1=0x4301c2/*uops*/,data2=0x4300c0,data3=0x00,data,data5=0x43412e,data6=0x43003c;
        int fd;
        int c;
        int cpu = 0;
        unsigned long arg;
        char *endarg;
        char msr_file_name[64];
int timer=250000;/*time-slice value set as 250 ms*/

uint64_t f1=0x600061a,f2=0x6000716, f3=0x6000822 ,f4=0x6000920;/*Frequency P state id's to be written to MSR register 0x199*/
double instr,cycles,mem,toff,ton,r1,r2,r3,r4;
sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
fd = open(msr_file_name, O_RDWR);
printf("%d\n",fd);
pwrite(fd, &data2, sizeof data2, reg1);
pwrite(fd, &data5, sizeof data5, reg2);
system("echo userspace > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor && echo userspace > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor");/*Change the frequency scaling governor to userspace*/

system("echo 2100000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed && echo 2100000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed");/*Change the frequency to the highest value*/


while(1){

usleep(timer);

pread(fd, &data, sizeof data, reg3);/*read the value of instructions retired*/
instr=data;
//printf("Uops=%ld\t",data);
pread(fd, &data, sizeof data, reg4);/*read the value of memory operations*/
toff=OVERLAP_FACTOR*data*500/1000000;/* calculate the stalled off chip time*/
//printf("%f   %f   3.0\n",instr,mem);
ton=0.25-toff;/*calculate the on chip time*/

r1=(ton*0.3125)/(ton+toff);
r2=(ton*0.75)/(ton+toff);


/*Change the frequency as per the target performance loss of 5%*/
if(r1<0.1)
{

system("echo 1600000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed && echo 1600000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed");

printf("frequency is set to 1.6 GHz\n");
}

if(r2<0.1)
{
system("echo 1200000 > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed && echo 1200000 > /sys/devices/system/cpu/cpu1/cpufreq/scaling_setspeed");
printf("frequency is set to 1.2 GHz\n");
}



pwrite(fd, &data3, sizeof data3, reg3);/*write 0 to the instruction and memory counters*/
pwrite(fd, &data3, sizeof data3, reg4);

}

return 0;
}

