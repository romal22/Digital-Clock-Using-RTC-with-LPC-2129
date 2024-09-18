#include <LPC21xx.H>
#include"header.h"
u8 flag=0,_month=0,_year=0;
#define sw1 ((IOPIN0>>14)&1)
#define sw2 ((IOPIN0>>15)&1)
#define sw3 ((IOPIN0>>16)&1)
/////////////////////////////////////////////////
void function(u8 add,u8 counter)
{
    u8 c=0;
    u32 temp;
    while(1)
    {
    if(sw1==0)
    {
        while(sw1==0);
        c++;
        if(c==(counter+1))
		c=0;
        lcd_cmd(0xc0);
		lcd_data((c/10)+48);
	    lcd_data((c%10)+48);
    }
    if(sw2==0)
    {
        while(sw2==0);
        temp=((c/10)<<4);
        temp=temp|(c%10);
        if(add==0x5)
        _month=0x0f&c;
        if(add==0x6)
        _year=0x0f&c;
        i2c_byte_write_frame(0xd0,add,temp);
        break;
    }
    }
}
///////////////////////////////////////////////////
main()
{
    u8 s,m,h,date,month,year,dow;
    u8 a[]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    i2c_init();
    lcd_init();
	config_vic_for_eint0();
	config_eint0();
    uart0_init(9600);
    
    i2c_byte_write_frame(0xd0,0x0,0x55);
    i2c_byte_write_frame(0xd0,0x1,0x13);
    i2c_byte_write_frame(0xd0,0x2,0x08);
    i2c_byte_write_frame(0xd0,0x4,0x22);
    i2c_byte_write_frame(0xd0,0x5,0x02);
    i2c_byte_write_frame(0xd0,0x6,0x03);
    i2c_byte_write_frame(0xd0,0x3,0x7);
    while(1)
    {
        s=i2c_byte_read_frame(0xd0,0x0);
        m=i2c_byte_read_frame(0xd0,0x1);
        h=i2c_byte_read_frame(0xd0,0x2);
        date=i2c_byte_read_frame(0xd0,0x4);
        month=i2c_byte_read_frame(0xd0,0x5);
        year=i2c_byte_read_frame(0xd0,0x6);
        dow=i2c_byte_read_frame(0xd0,0x3);
        
        lcd_cmd(0x80);
        lcd_data((h/0x10)+48);
        lcd_data((h%0x10)+48);
        lcd_data(':');
        lcd_data((m/0x10)+48);
        lcd_data((m%0x10)+48);
        lcd_data(':'); 
        lcd_data((s/0x10)+48);
        lcd_data((s%0x10)+48);
        lcd_cmd(0xc0);
        lcd_data((date/0x10)+48);
        lcd_data((date%0x10)+48);
        lcd_data('/');
        lcd_data((month/0x10)+48);
        lcd_data((month%0x10)+48);
        lcd_data('/');
        lcd_data('2');
        lcd_data('0');
        lcd_data((year/0x10)+48);
        lcd_data((year%0x10)+48);
        lcd_data(' ');
        switch(dow)
        {
            case 1: lcd_string("SUN"); break;
            case 2: lcd_string("MON"); break;
            case 3: lcd_string("TUE"); break;
            case 4: lcd_string("WED"); break;
            case 5: lcd_string("THU"); break;
            case 6: lcd_string("FRI"); break;
            case 7: lcd_string("SAT"); break;
        }
        
        if(flag==1)
        {
            flag=0;
            ////////hours//////
            lcd_cmd(0x1);
            lcd_cmd(0x80);
            lcd_string("enter hours");
            lcd_cmd(0xc0);
            lcd_data('0');
			lcd_data('0');
            function(0x2,23);  //1-24
            /////////minutes/////////////
            lcd_cmd(0x1);
            lcd_cmd(0x80);
            lcd_string("enter minutes");
            lcd_cmd(0xc0);
            lcd_data('0');
			lcd_data('0');
            function(0x1,60);  //1-60
            ///////////seconds///////////////
            lcd_cmd(0x1);
            lcd_cmd(0x80);
            lcd_string("enter seconds");
            lcd_cmd(0xc0);
            lcd_data('0');
			lcd_data('0');
            function(0x0,60);  //1-60
            /////////month//////////
            lcd_cmd(0x1);
            lcd_cmd(0x80);
            lcd_string("enter month");
            lcd_cmd(0xc0);
            lcd_data('0');
			lcd_data('0');
            function(0x5,12);  //month in b/w 1-12
            ////////year/////////
            lcd_cmd(0x1);
            lcd_cmd(0x80);
            lcd_string("enter year");
            lcd_cmd(0xc0);
            lcd_data('0');
			lcd_data('0');
            function(0x6,99);   //range of year is 2000-2099
            ///////date/////////
            lcd_cmd(0x1);
            lcd_cmd(0x80);
            lcd_string("enter date");
            lcd_cmd(0xc0);
            lcd_data('0');
			lcd_data('0');
            _year=_year+2000;  //make year in 2000 series
            if(_year/4==0)   //not a leap year
                function(0x4,a[_month]);
            else if(_month==2 && _year/4!=0)
                function(0x4,29);    //if leap year and 2nd month
            else
            function(0x4,a[_month]); //if leap year but not a feb month
            /////////dow///////
            lcd_cmd(0x1);
            lcd_cmd(0x80);
            lcd_string("enter day in int");
            lcd_cmd(0xc0);
            lcd_data('0');
			lcd_data('0');
            function(0x3,7);
			lcd_cmd(0x1);
			goto abc;         // day 1-7 as sun-sat
            /////////////////////
        }
		abc:
		flag=0;
    }
}


