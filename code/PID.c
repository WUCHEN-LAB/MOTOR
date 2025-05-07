#include "REGX52.H"
#include "intrins.h"
#include "PID.h"

void delay(uint j)
{
    for(;j>0;j--)
    {
        _nop_();
    }
}
int PWM=900; // Initialize PWM value
void main(void)
{
    DIR=1; // Set direction to clockwise
    while(1)
    {
        if(!INC)
        PWM = PWM>0?PWM-1:0; // Decrease PWM value
        if(!DEC)
        PWM = PWM<1000?PWM+1:1000; // Increase PWM value

        P3_7=1;
        delay(PWM);
        P3_7=0;
        delay(1000-PWM);
    }
}
