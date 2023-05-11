/*
* MumbaiRun_V1.c
*
* Created: 06-12-2022 18:03:48
*  Author: abhis
*/


/*
* complete_runV1.c
*
* Created: 10/6/2022 5:58:23 PM
* Author : USER
*/
// com3


#include <avr/io.h>
#include <electus_at16_pid.h>


//macros
#define led1_on              digital_write(d6,high)                   //led1 on
#define led1_off             digital_write(d6,low)                              // led1 off
#define led2_on              digital_write(d7,high)                                        //led2 on
#define led2_off             digital_write(d7,low)                                                  // led2 off
#define beep                 digital_write(b7,high);delay(10);digital_write(b7,low); // buzzer on and off
#define s1_pressed          (digital_read(b4)==low)                     //switch 1
#define s2_pressed          (digital_read(b5)==low)                                 //switch 2
#define s3_pressed          (digital_read(b6)==low)                                             //switch 3

int pp=100;
int t_point=0;
char run = '0';
int end_count=0;

void just_follow (void) // move a little bit forward
{
	for (int i=0;i<=80;i++)
	{
		follow_line(read_line());
	}
	
}
void rot_right (void)
{
	motor1_speed=100;
	motor2_speed=100;
	motor1_clk;
	motor2_aclk;
	while (!s7_white); // while not  white that is black
	delay(50); //do not change delay
	while(s7_white); // while white
	delay(65);
	motor1_off;
	motor2_off;
	delay(100);
	led2_on;
	int temp_kp=kp; // original save values  stored in a temporary variable
	int temp_ki=ki;
	int temp_kd=kd;
	kp=kp*1.05; // modify values
	ki=ki*1.05;
	kd=kd*1.05;
	just_follow();
	kp=temp_kp; // again regaining the original values
	kd=temp_kd;
	ki=temp_ki;
	
	led2_off;
}

void rot_left (void)
{
	motor1_speed=100;
	motor2_speed=100;
	motor1_aclk;
	motor2_clk;
	while (!s0_white); // while not  white that is black
	delay(50);                                     // do not change delay
	while(s0_white); // while white
	delay(65);                                     // rotation delay
	motor1_off;
	motor2_off;
	delay(100);
	led1_on;
	int temp_kp=kp; // original save values  stored in a temporary variable
	int temp_ki=ki;
	int temp_kd=kd;
	kp=kp*1.05; // modify values
	ki=ki*1.05;
	kd=kd*1.05;
	just_follow();
	kp=temp_kp; // again regaining the original values
	kd=temp_kd;
	ki=temp_ki;
	led1_off;
}



void final_run (void)
{
	m_base=200;
	kp=150;
	ki=0;
	kd=60;
	pp=100;
	lcd_clrscr();
	lcd_write_string("final start...");
	while (1)
	{
		while(stop==1)
		{
			motor1_off;
			motor2_off;
			delay(100);
		}
		int error=read_line();
		if(error==29)
		{
			serial_write_char('U');
			motor1_speed=100;
			motor2_speed=100;;
			motor1_off;
			motor2_off;
			delay(100);//reduce if needed
			rot_left();
		}
		else
		{
			follow_line(error);
		}
		
		if(s0_white || s7_white)
		{
			if(s0_white)
			{
				motor1_speed=100;
				motor2_speed=50;
			}
			if(s7_white)
			{
				motor2_speed=100;
				motor1_speed=50;
			}
			motor1_clk;
			motor2_clk;
			end_count=0;
			while(s0_white  ||  s7_white)
			{
				end_count++;
				delay(10);
				if(end_count==80)
				{
					motor2_off;
					motor1_off;
					led1_on;
					led2_on;
					while (1)
					{
						;
					}
				}
			}
			motor1_off;
			motor2_off;
			delay(100);
			if(eeprom_read(pp)=='L')
			{
				beep;
				rot_left();
			}
			else if (eeprom_read(pp)=='R')
			{
				beep;
				rot_right();
			}
			else if (eeprom_read(pp)=='S')
			{
				beep;
				just_follow();
			}
			pp++;
		}
		
		
	}
}

void dry_run (void)
{
	while (1)
	{
		while(stop==1)
		{
			motor1_off;
			motor2_off;
			delay(100);
		}
		int error=read_line();
		if(error==29)
		{
			serial_write_char('U');
			eeprom_write(pp++,'U');
			motor1_speed=100;
			motor2_speed=100;;
			motor1_off;
			motor2_off;
			delay(100);//reduce if needed
			rot_left();
		}
		else
		{
			follow_line(error);
		}
		
		if (s7_white)      //right
		{
			beep;
			motor1_speed=80;     // tedha hua to rukte vakt   start ruke bot
			motor2_speed=100;
			motor1_clk;
			motor2_clk;
			while(s7_white)//line ke aage rukna hai
			{
				if(s0_white)
				{
					t_point=1;
					break;
				}
			}
			if (t_point==0)
			{
				delay(50);//optional if bot passes the line original is delay(200);
				if (run == 'D')       // modified LHS
				{
					just_follow();
				}
				else
				{
					motor1_off;
					motor2_off;
					delay(80);// original delay(100);
					serial_write_char('R');
					eeprom_write(pp++,'R');
					rot_right();
				}
				
			}
			
		}
		if (s0_white && t_point==0) //left
		{
			beep;
			motor1_speed=100;
			motor2_speed=80;
			motor1_clk;
			motor2_clk;
			while(s0_white)//line ke aage rukna hai
			{
				if (s7_white)
				{
					t_point=1;
					break;
				}
			}
			if(t_point == 0) // not a t_point in s0_white
			{
				delay(50);//optional if bot passes the line (line stop delay)
				if (run == 'E')    // Modified RHS
				{
					just_follow();
				}
				else
				{
					motor1_off;
					motor2_off;
					delay(100);
					serial_write_char('L');
					eeprom_write(pp++,'L');
					rot_left();
				}
				
			}
		}
		
		if (t_point == 1)// if t_point set from above
		{
			serial_write_char('T');
			t_point=0;
			motor1_speed=100;
			motor2_speed=100;
			motor1_clk;
			motor2_clk;
			end_count=0;
			while(s0_white||s7_white)
			{
				end_count++;// last stage and bot should stop  here
				delay(10);
				if(end_count==70)           //--||--
				{
					motor2_off;
					motor1_off;
					led1_on;
					led2_on;
					serial_write_char('E');
					eeprom_write(pp++,'E');
					solve_maze();
					while (!s1_pressed);
					led1_off;
					led2_off;
					while (s1_pressed);
					delay(100);
					final_run();
				}
			}
			motor1_off;
			motor2_off;
			delay(100);
			if(run == 'B' || run == 'D')
			{
				serial_write_char('L');
				eeprom_write(pp++,'L');
				rot_left();
			}
			if(run == 'C' || run == 'E')
			{
				serial_write_char('R');
				eeprom_write(pp++,'R');
				rot_right();
			}
			
		}
	}
}

int main(void)
{
	lcd_init();             // lcd initialization
	adc_init();                   // adc initialization
	pin_init(d0,input_pullup);     // bluetooth remove karne par bhi work karna chahiye
	//pin_init(d1,input_pullup);
	serial_init(19200);                  // serial line initialization
	motor_init();                               // motor initialization
	pin_init(d6,output);                                  //led1 initialization
	pin_init(d7,output);                                              // led2 pin initialization
	pin_init(b7,output);                                       //buzzer initialization
	pin_init(b4,input_pullup);                          // switch1 initialization
	pin_init(b5,input_pullup);                // switch2 initialization
	pin_init(b6,input_pullup);   // switch3 initialization
	
	lcd_clrscr();
	lcd_write_string("I am Batman...");
	serial_write_string("I am Batman...\r\n");
	run = 'A';
	while(!s2_pressed)           // wait till sw2 is unpressed
	{
		if (s1_pressed)
		{
			run++;
			if (run == 'F')      // 5 Algorithms
			{
				run = 'A';
			}
			lcd_clrscr();
			lcd_write_string("Go with:");
			lcd_write_char(run);
			delay(200);
			
		}
	}
	
	calibrate_sensors(); // calibrating sensors and motors
	
	beep;
	while (!s1_pressed) // wait till sw1 is unpressed
	{
		int error=read_line();
		lcd_clrscr();
		lcd_write_char(sen0);
		lcd_write_char(sen1);
		lcd_write_char(sen2);
		lcd_write_char(sen3);
		lcd_write_char(sen4);
		lcd_write_char(sen5);
		lcd_write_char(sen6);
		lcd_write_char(sen7);
		lcd_write_string("\nE:");
		lcd_write_int(error);
		delay(100);
		
	}
	
	while(s1_pressed); //wait till sw is pressed
	
	delay(100);
	led1_off;
	led2_off;
	
	m_base=180;
	kp=150;
	ki=0;
	kd=60;
	if (run == 'A') // final run
	{
		final_run();
	}
	else
	{
		dry_run();
	}

}