#include "reg51.h"
#include "stdio.h"

#define uchar unsigned char
#define uint unsigned int

/*-------------LCD1602声明函数----------------*/
void intint();
void WR_data(uchar u,uchar *p,uchar set);
void write_com(uchar com);
void WR_data_one(uchar u,uchar p,uchar set);

uchar code Frequency[]="f&Vo:0000Hz/0 V";
uchar code SHA[]="Shap:";

uchar code shap_a[]="NULL      ";		  
uchar code shap_b[]="Square    ";
uchar code shap_c[]="Triangular";
uchar code shap_d[]="Sine      ";
uchar code shap_e[]="Sawtooth  ";		
						

sbit key_a=P2^7;
sbit key_b=P2^6;
sbit key_c=P2^5;

void delayms(uint ms)
{
	uchar i;
	while(ms--) for(i=0;i<120;i++);
}  	 
             
uchar code sin[]={
					0x80,0x83,0x86,0x89,0x8d,0x90,0x93,0x96,0x99,0x9c,0x9f,0xa2,
					0xa5,0xa8,0xab,0xae,0xb1,0xb4,0xb7,0xba,0xbc,0xbf,0xc2,0xc5,
					0xc7,0xca,0xcc,0xcf,0xd1,0xd4,0xd6,0xd8,0xda,0xdd,0xdf,0xe1,
					0xe3,0xe5,0xe7,0xe9,0xea,0xec,0xee,0xef,0xf1,0xf2,0xf4,0xf5,
					0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfd,0xfe,0xff,0xff,
					0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfd,
					0xfd,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,0xf6,0xf5,0xf4,0xf2,0xf1,
					0xef,0xee,0xec,0xea,0xe9,0xe7,0xe5,0xe3,0xe1,0xde,0xdd,0xda,
					0xd8,0xd6,0xd4,0xd1,0xcf,0xcc,0xca,0xc7,0xc5,0xc2,0xbf,0xbc,
					0xba,0xb7,0xb4,0xb1,0xae,0xab,0xa8,0xa5,0xa2,0x9f,0x9c,0x99,
					0x96,0x93,0x90,0x8d,0x89,0x86,0x83,0x80,0x80,0x7c,0x79,0x76,
					0x72,0x6f,0x6c,0x69,0x66,0x63,0x60,0x5d,0x5a,0x57,0x55,0x51,
					0x4e,0x4c,0x48,0x45,0x43,0x40,0x3d,0x3a,0x38,0x35,0x33,0x30,
					0x2e,0x2b,0x29,0x27,0x25,0x22,0x20,0x1e,0x1c,0x1a,0x18,0x16,
					0x15,0x13,0x11,0x10,0x0e,0x0d,0x0b,0x0a,0x09,0x08,0x07,0x06,
					0x05,0x04,0x03,0x02,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x02,0x03,0x04,0x05,
					0x06,0x07,0x08,0x09,0x0a,0x0b,0x0d,0x0e,0x10,0x11,0x13,0x15,
					0x16,0x18,0x1a,0x1c,0x1e,0x20,0x22,0x25,0x27,0x29,0x2b,0x2e,
					0x30,0x33,0x35,0x38,0x3a,0x3d,0x40,0x43,0x45,0x48,0x4c,0x4e,
					0x51,0x55,0x57,0x5a,0x5d,0x60,0x63,0x66 ,0x69,0x6c,0x6f,0x72,
					0x76,0x79,0x7c,0x80
					};
				    
				    
uchar iu=0,ii=0,mode_a=0,mode_b=0,count_a=0;
uint first=0,second=0,third=0,forth=0;
uint full=255;
uint freq=0,iii=0;
uint freq_sine=43;
float freq_que=0;
uint vol=0;
void DAC0832(unsigned char x)
{
  P1=x;
}
void Timer0Init(void)		//0.001毫秒@12.000MHz
{
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xFE;			//设置定时初值
	TH0 = 0xFF;			//设置定时初值
	TF0 = 0;			//清除TF0标志
	TR0 = 1;			//定时器0开始计时
}

void main()
{
	uchar dsy[2];
	intint();							//液晶屏初始化
	WR_data(1,Frequency,0);				//使液晶片显示提示信息
	WR_data(2,SHA,0);
	WR_data(2,shap_a,5);
	write_com(0x0c);
	Timer0Init();						//定时器0初始化
	EA=1;								//中断允许开
	ET0=1;								//定时器0中断开
	IT0=1;								//设置外部中断触发方式为负边沿触发
	EX0=1;								//开启外部中断0
	while(1)
    { 
		switch(mode_b)					//液晶屏显示设置
		{
			case 0:break;
			case 1:sprintf(dsy,"%d",first);WR_data_one(1,dsy[0],8);TR0=0;break;		 //设置频率值并关闭波形输出
			case 2:sprintf(dsy,"%d",second);WR_data_one(1,dsy[0],7);break;
			case 3:sprintf(dsy,"%d",third);WR_data_one(1,dsy[0],6);break;
			case 4:sprintf(dsy,"%d",forth);WR_data_one(1,dsy[0],5);break;
			case 5:sprintf(dsy,"%d",vol);WR_data(1,dsy,12);break;
			case 6:
					write_com(0x0c);mode_b=0;TR0=1;							 //完成频率值的设置并开启波形输出
					freq=first+second*10+third*100+forth*1000;
					freq_que=10000/freq;
					freq_sine=43/freq;
					break;

			default:break;	
		}	   
    }
}
void INT_0() interrupt 0			   //外部中断0服务子程序
{
	if(mode_b==0)					   //波形选择键屏蔽/开启
	if(key_a==0)					   //波形选择键判断
	{
		delayms(5);
		while(key_a==0);
		mode_a=(mode_a+1)%5;
		switch(mode_a)				   //在液晶屏上显示波形信息
		{
			case 0:WR_data(2,shap_a,5);write_com(0x0c);break;
			case 1:WR_data(2,shap_b,5);write_com(0x0c);break;
			case 2:WR_data(2,shap_c,5);write_com(0x0c);break;
			case 3:WR_data(2,shap_d,5);write_com(0x0c);break;
			case 4:WR_data(2,shap_e,5);write_com(0x0c);break;
			default:break;
		}
	}
	if(key_b==0)						//功能选择键判断
	{
		delayms(5);
		while(key_b==0);
		mode_b=(mode_b+1)%7;
	}
	if(key_c==0)						//频率设置键判断
	{
		delayms(5);
		while(key_c==0);
		switch(mode_b)
		{
			case 1:first=(first+1)%10;break;
			case 2:second=(second+1)%10;break;
			case 3:third=(third+1)%10;break;
			case 4:forth=(forth+1)%10;break;
			case 5:vol=(vol+1)%11;break;
			default:break;
		}
	}
}
void Time0int() interrupt 1			   //定时器0中断服务子程序
{
	TL0 = 0xFE;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	
	
	switch(mode_a)					   //波形输出控制
	{
		case 0: iu=0;break;
		case 1:	iii++;
				if(iii==freq_que)
				{
					iii=0;full=~full;
					iu=(full*vol/10);
				}
				break;
		case 2:	iii++;
				if(iii==freq_sine)
				{
					iii=0;
					if(count_a<128) iu=count_a*vol/5;
					else  iu=(255-count_a)*vol/5;
					count_a+=1;
				}
				break;
		case 3:	iii++;
				if(iii==freq_sine)
				{
					iii=0;
					ii=(ii+1)%255;
					iu=sin[ii]*vol/10;
				}
				break;
		case 4:iii++;
				if(iii==freq_sine)
				{
					iii=0;
					iu=count_a*vol/20;
					count_a+=1;
				}
				break;
		default:iu=0;  
	}	
	DAC0832(iu);	
}
