#include "REGX52.H"
#include "intrins.h"
#include "PID.h"

static unsigned int display_timer = 0;

int PWM = 0;                 // 初始化 PWM 值
unsigned int time = 100; // 定义时间间隔 
unsigned int num;
unsigned int num2;
unsigned int rpm = 0;        // 转速（每分钟转数）
unsigned int pulse_count = 0; // 记录脉冲数
#define ENCODER_PULSES 25    // 定义编码器每转的脉冲数

unsigned char code digit_table[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void delay(uint j)
{
    for (; j > 0; j--)
    {
        _nop_();
    }
}

void clear()
{
    P1_0 = 1; P1_1 = 1; P1_2 = 1; P1_3 = 1;
    delay(time);
}

void display_real(unsigned int num)
{
    clear();
    P1_0 = 1; P1_1 = 1; P1_2 = 1; P1_3 = 1;
    P0 = digit_table[num / 1000];  // 显示千位数字
    delay(time);

    clear();
    P1_0 = 0; P1_1 = 1; P1_2 = 1; P1_3 = 1;
    P0 = digit_table[(num / 100) % 10];  // 显示百位数字
    delay(time);

    clear();
    P1_0 = 1; P1_1 = 0; P1_2 = 1; P1_3 = 1;
    P0 = digit_table[(num / 10) % 10];  // 显示十位数字
    delay(time);

    clear();
    P1_0 = 1; P1_1 = 1; P1_2 = 0; P1_3 = 1;
    P0 = digit_table[num % 10];  // 显示个位数字
    delay(time);
}

void display_set(unsigned int num2)
{
    clear();
    P1_4 = 1; P1_5 = 1; P1_6 = 1; P1_7 = 1;
    P2 = digit_table[num2 / 1000];  // 显示千位数字
    delay(time);

    clear();
    P1_4 = 0; P1_5 = 1; P1_6 = 1; P1_7 = 1;
    P2 = digit_table[(num2 / 100) % 10];  // 显示百位数字
    delay(time);

    clear();
    P1_4 = 1; P1_5 = 0; P1_6 = 1; P1_7 = 1;
    P2 = digit_table[(num2 / 10) % 10];  // 显示十位数字
    delay(time);

    clear();
    P1_4 = 1; P1_5 = 1; P1_6 = 0; P1_7 = 1;
    P2 = digit_table[num2 % 10];  // 显示个位数字
    delay(time);
}

void Timer0_Init()
{
    TMOD |= 0x01;  // 定时器0模式1
    TH0 = 0xFC;    // 初值（1ms定时）
    TL0 = 0x18;
    ET0 = 1;       // 使能定时器0中断
    EA = 1;        // 总中断使能
    TR0 = 1;       // 启动定时器0
}

void ExternalInterrupt_Init()
{
    IT0 = 1;  // 下降沿触发
    EX0 = 1;  // 外部中断0使能
    EA = 1;   // 总中断使能
}

void Timer0_ISR() interrupt 1
{
    static unsigned int ms_count = 0;
    TH0 = 0xFC;    // 重装初值
    TL0 = 0x18;
    ms_count++;

    if (ms_count >= 1000)  // 每秒计算一次转速
    {
        rpm = (pulse_count * 60) / ENCODER_PULSES;
        pulse_count = 0;  // 清零脉冲计数
        ms_count = 0;
    }
}

void INT0_ISR() interrupt 0
{
    pulse_count++;  // 每次外部中断增加脉冲计数
}

void main(void)
{
    Timer0_Init();            // 初始化定时器
    ExternalInterrupt_Init(); // 初始化外部中断
    DIR = 0;
    while (1)
    {
        if (!INC){
            delay(200);  // 防抖动延时
            PWM = PWM > 10 ? PWM - 20 : 0;  // 减少 PWM
        }
        if (!DEC){
            delay(200);  // 防抖动延时
            PWM = PWM < 990 ? PWM + 20 : 1000;  // 增加 PWM
        }
        if(PWM <= 10){
            OUTPUT = 0;    // PWM 低电平
        }else{
            OUTPUT = 1;    // PWM 高电平
            delay(PWM);
            OUTPUT = 0;    // PWM 低电平
            delay(1000 - PWM);
        }
            display_set(rpm);
            display_real(rpm);  // 显示转速
    }
}
