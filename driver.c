/*lcd driver*/
#include <LPC21xx.H>
#include"header.h"
#define SI ((I2CONSET>>3)&1)
#define STOP (1<<4)
#define START (1<<5)
#define en_i2c (1<<6)
extern u8 flag;
void EINT0_Handler(void) __irq{
     flag=1;
    EXTINT|=1;    //reset the flag
    VICVectAddr=0;
}
void config_vic_for_eint0(void)
{
    VICIntSelect=0;   //select fiq or irq 
    
    VICVectCntl0=14 | (1<<5);       //interrupt number
    VICVectAddr0=(u32)EINT0_Handler;  //assign the address
    VICIntEnable=(1<<14); //enable EXTINT0 on VIC
}

void config_eint0(void)
{
    PINSEL1|=1;    //p0.16 for EINT0
    EXTMODE|=1;     //EINT0 edge triggred
    EXTPOLAR|=0;    //EINT0 is active low
}

void i2c_init(void)
{
	PINSEL0|=0x50;
	I2SCLL=I2SCLH=75;
	I2CONSET=en_i2c;
}

void i2c_byte_write_frame(u8 SA,u8 MR,u8 DATA)
{
	/*1. generate start condition*/
	I2CONSET=START;
	I2CONCLR=(1<<3);
	while(SI==0);
	I2CONCLR=START;
	if(I2STAT!=0X08)
	{
	uart0_tx_string("err: start\r\n");
	goto exit;
	}
	/*2. send sa and wait for ack*/
	I2DAT=SA;
	I2CONCLR=(1<<3);
	while(SI==0);
	if(I2STAT==0X20)
	{
		uart0_tx_string("err: sa\r\n");
		goto exit;
	}
	/*3. send mr addr and wait for ack*/
	 I2DAT=MR;
	 I2CONCLR=(1<<3);
	 while(SI==0);
	 if(I2STAT==0X30)
	 {
		 uart0_tx_string("err: mr\r\n");
		 goto exit;
	 }
	/*4. send data and wait for ack*/
	  I2DAT=DATA;
	  I2CONCLR=(1<<3);
	  while(SI==0);
	  if(I2STAT==0X30)
		{
			uart0_tx_string("err: data\r\n");
			goto exit;
		}
	/*5. genereate stop conditon*/
	exit:
		 I2CONCLR=(1<<3);
		 I2CONSET=STOP;
}
u8 i2c_byte_read_frame(u8 SA,u8 MR)
{
		u8 temp;
		/*1. generate start condition*/
		I2CONSET=START;
		I2CONCLR=(1<<3);
		while(SI==0);
		I2CONCLR=START;
		if(I2STAT!=0X08)
		{
		 uart0_tx_string("err: start\r\n");
		 goto exit;
		}
		/*2. send sa+w and check ack*/
		I2DAT=SA;
		I2CONCLR=(1<<3);
		while(SI==0);
		if(I2STAT==0X20)
		{
			uart0_tx_string("err: sa\r\n");
			goto exit;
		}
		/*3. send mr addr and check ack*/
		I2DAT=MR;
		I2CONCLR=(1<<3);
		while(SI==0);
		if(I2STAT==0X30)
		{
		 uart0_tx_string("err: mr\r\n");
		 goto exit;
		}
		/*4. generate restart*/
		I2CONSET=START;
		I2CONCLR=(1<<3);
		while(SI==0);
		I2CONCLR=START;
		if(I2STAT!=0X10)
		{
			uart0_tx_string("err: restart\r\n");
			goto exit;
		}
		/*5. semd sa+r and check ack*/
		 I2DAT=SA|1;
		I2CONCLR=(1<<3);
		while(SI==0);
		if(I2STAT==0X48)
		{
				uart0_tx_string("err: sa+r\r\n");
				goto exit;
		}
		/*6. read data*/
		I2CONCLR=(1<<3);
		while(SI==0);
		temp=I2DAT;
		/*7. stop condition*/
		exit:
		 I2CONCLR=(1<<3);
		I2CONSET=STOP;
		/*8. return the data*/
		return temp;
}
void delay_sec(unsigned int sec)
{
	T0PC=0;
	T0PR=15000000-1;
	T0TC=0;
	T0TCR=1;
	while(T0TC<sec);
	T0TCR=0;
}

void delay_ms(unsigned int ms)
{
	T0PC=0;
	T0PR=15000-1;
	T0TC=0;
	T0TCR=1;
	while(T0TC<ms);
	T0TCR=0;
}

void lcd_data(unsigned char DATA)
{
	unsigned int T;
	
	//HIGHER NIBBLE
	IOCLR1=0XFE<<16;
	T=(DATA & 0XF0)<<16;
	IOSET1=T;
	IOSET1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
	
	//LOWER NIBBLE
	IOCLR1=0XFE<<16;
	T=(DATA & 0X0F)<<20;
	IOSET1=T;
	IOSET1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;

}

void lcd_cmd(unsigned char CMD)
{
	unsigned int T;
	 //HIGHER NIBBLE
	IOCLR1=0XFE<<16;
	T=(CMD & 0XF0)<<16;
	IOSET1=T;
	IOCLR1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
	
	//LOWER NIBBLE
	IOCLR1=0XFE<<16;
	T=(CMD & 0X0F)<<20;
	IOSET1=T;
	IOCLR1=1<<17;
	IOCLR1=1<<18;
	IOSET1=1<<19;
	delay_ms(2);
	IOCLR1=1<<19;
}


void lcd_init()
{
	IODIR1=0XFE<<16;
	IOCLR1=1<<19;
	PINSEL2|=0X0;
	lcd_cmd(0x02);
	lcd_cmd(0x28);
	lcd_cmd(0x03);
	lcd_cmd(0x0e);
	lcd_cmd(0x01);
}

void lcd_string(char *p)
{
	while(*p)
	{
		lcd_data(*p);
		p++;
	}
}

//void lcd_integer(int num)
//{  
//int a[10];
//int i;
//	if(num==0)
//	{
//	    lcd_data('0');
//		return ;
//	}
//	if(num<0)
//	{
//		lcd_data('-');
//		num=-num;
//	}
//
//	while(num)
//	{
//		a[i]=(num%10)+48;
//		num/=10;
//		i++;
//	}
//
//	for(i=i-1;i>=0;i--)
//	lcd_data(a[i]);
//}

void lcd_float(float num)
{
int temp,temp1,i;
char s[16];
	if(num==0)
	{
		lcd_string("0.000000");
		return;
	}
	if(num<0)
	{
		lcd_data('-');
		num=-num;
	}
	temp=num;
	temp1=num;

	for(i=0;temp;temp/=10,i++)
	s[i]=(temp%10)+48;
	i--;
	for(;i>=0;i--)
	lcd_data(s[i]);

	lcd_data('.');

	temp=(num-temp1)*1000000;

	for(i=0;temp;temp/=10,i++)
	s[i]=(temp%10)+48;
	i--;
	for(i=i;i>=0;i--)
	lcd_data(s[i]);
}
void lcd_hexa(int num)
{
    unsigned char temp;
    temp=(((num>>4)&0x0f));
	if(temp<10)
	{
	 	lcd_data(temp+48);
	}
	else
	{
		lcd_data(temp+55);
	}
	temp=((num&0x0f));
	if(temp<10)
	{
	 	lcd_data(temp+48);
	}
	else
	{
		lcd_data(temp+55);
	}
}

void lcd_timer(int m,int s)
{
	for(m=m;m>=0;m--)
	{
		for(s=s;s>=0;s--)
		{
	     	lcd_cmd(0x01);
			lcd_string("timer=");
			lcd_data((m/10)+48);
			lcd_data((m%10)+48);
			lcd_data(':');
			lcd_data((s/10)+48);
			lcd_data((s%10)+48);
			delay_sec(1);
		}
		s=60;
	}
	lcd_cmd(0x01);
		lcd_string("timer=");
		lcd_string("00");
		lcd_data(':');
		lcd_string("00");

}

unsigned char b[10]={0x0,0xA,0XA,0X0,0X0,0X11,0XE,0X0};
void lcd_cgram(void)
{
	int i;
	lcd_cmd(0x40);
	for(i=0;i<=7;i++)
	lcd_data(b[i]);
}

void uart0_init(unsigned int baud)
{
	unsigned int result=0;
	int a[5]={15,60,30,15,15};
	unsigned int pclk=a[VPBDIV]*1000000;
	result=pclk/(16*baud);
	PINSEL0|=5;
	U0LCR=0X83;
	U0DLL=result&0xff;
	U0DLM=(result>>8)&0xff;
	U0LCR=3;
}

#define THRE ((U0LSR>>5)&1)
void uart0_tx(unsigned char data)
{
	U0THR=data;
	while(THRE==0);
}

void uart0_tx_string(char *ptr)
{
	while(*ptr)
	{
		U0THR=*ptr;
		while(THRE==0);
		ptr++;
	}
}

#define RDR (U0LSR&1)
unsigned char uart0_rx(void)
{
	while(RDR==0);
	return U0RBR;
}

void uart0_hexa(int num)
{
	unsigned char temp;
    uart0_tx('0');
    uart0_tx('X');

    temp=(((num>>4)&0x0f));
	if(temp<10)
	{
	 	uart0_tx(temp+48);
	}
	else
	{
		uart0_tx(temp+55);
	}

    temp=((num&0x0f));
	if(temp<10)
	{
	 	uart0_tx(temp+48);
	}
	else
	{
		uart0_tx(temp+55);
	}
}

void uart0_binary(int num)
{
    int pos,c;
    for(pos=7;pos>=0;pos--)
    {
        c=num>>pos&1;
        if(c)
            uart0_tx(49);
        else
            uart0_tx(48);        
    }
}

void uart0_tx_integer(int num)
{
    char a[10],i=0;
    while(num)
    {
      a[i]=(num%10)+48;
      num/=10;
      i++;
    }
    i--;
    while(1)
    {
        uart0_tx(a[i]);
		if(i==0)
		break;
        i--;
    }
}

void uart0_tx_float(float num)
{
    int temp,temp1;
    if(num==0)
    {
        uart0_tx_string("0.00");
        return;
    }
    if(temp<0)
    {
        uart0_tx('-');
        temp=-temp;
    }
    
    uart0_tx_integer(temp);
    uart0_tx('.');
    temp1=(temp-num)*100;
    uart0_tx_integer(temp1);
}

