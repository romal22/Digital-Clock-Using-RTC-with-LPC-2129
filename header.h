typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef char s8;
extern void delay_ms(unsigned int);
extern void lcd_data(unsigned char);
extern void lcd_cmd(unsigned char );
extern void lcd_string(char *);
extern void lcd_integer(unsigned int num);
extern void lcd_float(float f);
extern void lcd_init(void);
extern void delay_sec(unsigned int sec);
extern void lcd_hexa(int num);
extern void cgram(void);
extern void i2c_init(void);
extern void i2c_byte_write_frame(u8 SA,u8 MR,u8 DATA);
extern u8 i2c_byte_read_frame(u8 SA,u8 MR);
extern void uart0_init(unsigned int baud);
extern void uart0_tx(unsigned char data);
extern void uart0_tx_string(char *ptr);
extern unsigned char uart0_rx(void);
extern void uart0_hexa(int num);
extern void uart0_binary(int);
extern void uart0_tx_integer(int);
extern void uart0_tx_float(float);
extern void config_vic_for_eint0(void);
extern void config_eint0(void);
