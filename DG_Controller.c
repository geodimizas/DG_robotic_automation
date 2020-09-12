
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#define _XTAL_FREQ 4000000
    //INPUTS TO PORTB
    
#define reset PORTBbits.RB6
#define auto_start PORTBbits.RB5
#define manual_start PORTBbits.RB4
#define up PORTBbits.RB3
#define down PORTBbits.RB2
#define forward PORTBbits.RB1
#define backward PORTBbits.RB0

//OUTPUTS TO PORT C
#define move_up PORTCbits.RC0
#define move_forward PORTCbits.RC1
#define inj1 PORTCbits.RC2
#define inj2 PORTCbits.RC3
#define turn PORTCbits.RC4
#define finish PORTCbits.RC5
#define fault PORTCbits.RC6

    
#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>


//LCD Functions Developed by Circuit Digest.
void Lcd_SetBit(char data_bit) //Based on the Hex value Set the Bits of the Data Lines
{
    if(data_bit& 1) 
        D4 = 1;
    else
        D4 = 0;

    if(data_bit& 2)
        D5 = 1;
    else
        D5 = 0;

    if(data_bit& 4)
        D6 = 1;
    else
        D6 = 0;

    if(data_bit& 8) 
        D7 = 1;
    else
        D7 = 0;
}

void Lcd_Cmd(char a)
{
    RS = 0;           
    Lcd_SetBit(a); //Incoming Hex value
    EN  = 1;         
        __delay_ms(4);
        EN  = 0;         
}

Lcd_Clear()
{
    Lcd_Cmd(0); //Clear the LCD
    Lcd_Cmd(1); //Move the curser to first position
}

void Lcd_Set_Cursor(char a, char b)
{
    char temp,z,y;
    if(a== 1)
    {
      temp = 0x80 + b - 1; //80H is used to move the curser
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
    else if(a== 2)
    {
        temp = 0xC0 + b - 1;
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
}

void Lcd_Start()
{
  Lcd_SetBit(0x00);
  for(int i=1065244; i<=0; i--)  NOP();  
  Lcd_Cmd(0x03);
    __delay_ms(5);
  Lcd_Cmd(0x03);
    __delay_ms(11);
  Lcd_Cmd(0x03); 
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x08); //Select Row 1
  Lcd_Cmd(0x00); //Clear Row 1 Display
  Lcd_Cmd(0x0C); //Select Row 2
  Lcd_Cmd(0x00); //Clear Row 2 Display
  Lcd_Cmd(0x06);
}

void Lcd_Print_Char(char data)  //Send 8-bits through 4-bit mode
{
   char Lower_Nibble,Upper_Nibble;
   Lower_Nibble = data&0x0F;
   Upper_Nibble = data&0xF0;
   RS = 1;             // => RS = 1
   Lcd_SetBit(Upper_Nibble>>4);             //Send upper half by shifting by 4
   EN = 1;
   for(int i=2130483; i<=0; i--)  NOP(); 
   EN = 0;
   Lcd_SetBit(Lower_Nibble); //Send Lower half
   EN = 1;
   for(int i=2130483; i<=0; i--)  NOP();
   EN = 0;
}

void Lcd_Print_String(char *a)
{
    int i;
    for(i=0;a[i]!='\0';i++)
       Lcd_Print_Char(a[i]);  //Split the string using pointers and call the Char function 
}

int error()
{
    fault = 1;
    while (!reset) {} // WAIT TILL RESET = 1 
}


int main()
{
    
    start:
    TRISD = 0x00;
    TRISC = 0x00;
    TRISB = 0xFF;    
    
    int i;
    move_up  = 0;
    move_forward = 0; 
    inj1 = 0;
    inj2 = 0;
    turn = 0;
    finish = 0;
    fault = 0;
    
    unsigned int a;
    
    //LCD STARTING MESSEGES
    Lcd_Start();
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Print_String("    DiGiTRON");
    Lcd_Set_Cursor(2,1);
    Lcd_Print_String("RESEARCH COMPANY");
    for (i = 0; i < 4; i++ )__delay_ms(500);
    
    Lcd_Start();
    Lcd_Clear();
    Lcd_Set_Cursor(2,1);
    Lcd_Print_String("STARTING..");   
    
    int prev_step = 5;
    int turning = 0, blowing1 = 0, blowing2 = 0;

     while (1)
    {
        // WAITING FOR SIGNAL
        Lcd_Start();
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Print_String("    WAITING");
        Lcd_Set_Cursor(2,1);
        Lcd_Print_String("   FOR SIGNAL"); 
        
        while ((!auto_start) && (!manual_start)) {}
        
        finish = 0;
        if (reset) goto start;
        
        if (((up) && (down)) || ((!up) && (!down)))
        {
            Lcd_Start();
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Print_String("     ERROR");
            Lcd_Set_Cursor(2,1);
            Lcd_Print_String("ON UP-DOWN ARM");
            error ();
            goto start;    
        }
       
        if (((forward) && (backward)) || ((!forward) && (!backward)))
        {
            Lcd_Start();
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Print_String("     ERROR");
            Lcd_Set_Cursor(2,1);
            Lcd_Print_String("ON FRONT-BACK ARM");
            error ();
            goto start;    
        }
        
        if (auto_start)
        {
            
            //STEP1
            if ((down) && (prev_step == 5) && (backward)) 
            {
                step1:
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("   MOVING UP");
                // LCD START
            
                prev_step = 1;
                move_up = 1;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
            
                if ((!up) && (auto_start)) 
                {
                    Lcd_Start();
                    Lcd_Clear();
                    Lcd_Set_Cursor(1,1);
                    Lcd_Print_String("     ERROR");
                    Lcd_Set_Cursor(2,1);
                    Lcd_Print_String("ON MOVING UP");
                    error ();
                    goto start;
                }
            
            }         
        
            if (reset) goto start;
        
            //STEP2
            if ((up) && (prev_step == 1) && (backward)) 
            {
                step2:
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("  MOVING FRONT");
                // LCD STOP
            
                move_forward = 1;             
                prev_step = 2;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                if ((!forward) && (auto_start)) 
                {
                    Lcd_Start();
                    Lcd_Clear();
                    Lcd_Set_Cursor(1,1);
                    Lcd_Print_String("     ERROR");
                    Lcd_Set_Cursor(2,1);
                    Lcd_Print_String("ON MOVING FRONT");
                    error ();
                    goto start;
                }
 
            }
     
            if (reset) goto start;
        
            //STEP3
            if ((forward) && (prev_step == 2) && (up))
            {
                step3:
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String(" MOTOR TURNING");
                // LCD END
            
                turn = 1;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                turn = 0;
            
                if (reset) goto start;
            
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String(" MOVING DOWN");
                // LCD END
            
                move_up = 0;
                prev_step = 3;
            
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }            
                if ((!down) && (auto_start)) 
                {
                    Lcd_Start();
                    Lcd_Clear();
                    Lcd_Set_Cursor(1,1);
                    Lcd_Print_String("     ERROR");
                    Lcd_Set_Cursor(2,1);
                    Lcd_Print_String("ON MOVING DOWN");
                    error ();
                    goto start;
                }
            }
        
            if (reset) goto start;
        
            //STEP4
            if ((down) && (prev_step == 3) && (forward)) 
            {
                step4:
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("    BLOWING");
                // LCD END
            
                inj1 = 1; 
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                inj1 = 0;
            
                if (reset) goto start;
            
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("  MOVING BACK");
                // LCD END
           
                move_forward = 0;
                prev_step = 4;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
            
                if ((!backward) && (auto_start)) 
                {
                    Lcd_Start();
                    Lcd_Clear();
                    Lcd_Set_Cursor(1,1);
                    Lcd_Print_String("     ERROR");
                    Lcd_Set_Cursor(2,1);
                    Lcd_Print_String("ON MOVING BACK");
                    for (i = 0; i < 10; i++ ) 
                    {
                        __delay_ms(500); //wait 5 sec
                        if (reset) goto start;
                    }
                    error ();
                    goto start;
                }    
            }
     
            if (reset) goto start;
        
            //STEP5
            if ((backward) && (prev_step == 4)) 
            {
                step5:
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("    BLOWING");
                // LCD END
            
                inj2 = 1;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                inj2 = 0;
            
                if (reset) goto start;
            
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String(" MOTOR TURNING");
                // LCD END
            
                turn = 1;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                turn = 0;
                prev_step =5;
            
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("     FINISH");
                // LCD START
            
                finish = 1;
                move_forward = 0; 
                inj1 = 0;
                inj2 = 0;
                turn = 0;
                for (i = 0; i < 20; i++ ) 
                {
                    __delay_ms(500); //wait 10 sec
                    if (reset) goto start;
                }
            }
     
        }

        
        //MANUAL
            
            //STEPA
            if ((manual_start) && (down)&& (backward) && (!turning) && (!blowing1) && (!blowing2)) // if situation 1
            {
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("   MOVING UP");
                // LCD START
            
                move_up = 1;
                for (i = 0; i < 10; i++ )__delay_ms(500); //wait 5 sec            
            }         
        
            if (reset) goto start;
        
            //STEPB
            if ((manual_start) && (up) && (backward) && (!turning) && (!blowing1) && (!blowing2)) 
            {
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("  MOVING FRONT");
                // LCD STOP
            
                move_forward = 1;  
                for (i = 0; i < 10; i++ )__delay_ms(500); //wait 5 sec  
            }
     
            if (reset) goto start;
        
            //STEPC
            if ((manual_start) && (forward) && (up) && (!turning) && (!blowing1) && (!blowing2))
            {
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String(" MOTOR TURNING");
                // LCD END
            
                turn = 1;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                turn = 0;
                
                turning = 1; // lamp for turning = 1
            }
            
             if (reset) goto start;
            
            //STEPD
            if ((manual_start) && (up) && (forward) && (turning) && (!blowing1) && (!blowing2))
            {            
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String(" MOVING DOWN");
                // LCD END
            
                move_up = 0;
                for (i = 0; i < 10; i++ )__delay_ms(500); //wait 5 sec                  
            }
        
            if (reset) goto start;
        
            //STEPE
            if ((manual_start) && (down) && (forward) && (turning) && (!blowing1) && (!blowing2)) 
            {
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("    BLOWING");
                // LCD END
            
                inj1 = 1; 
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                inj1 = 0;
                blowing1 = 1; //lamp for injextion1 = 1
            }
            
            if (reset) goto start;
            
            //STEPF
            if ((manual_start) && (down) && (forward) && (turning) && (blowing1) && (!blowing2)) 
            {           
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("  MOVING BACK");
                // LCD END
           
                move_forward = 0;
                for (i = 0; i < 10; i++ )__delay_ms(500); //wait 5 sec                  
            }
     
            if (reset) goto start;
        
            //STEPG
            if ((manual_start) && (down) && (backward) && (turning) && (blowing1) && (!blowing2)) 
            {
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("    BLOWING");
                // LCD END
            
                inj2 = 1;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                inj2 = 0;
                blowing2 = 1; //lamp for injection2 = 1
            }
            
            if (reset) goto start;
            
            //STEPH
            if ((manual_start) && (down) && (backward) && (turning) && (blowing1) && (blowing2)) 
            {            
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String(" MOTOR TURNING");
                // LCD END
            
                turn = 1;
                for (i = 0; i < 10; i++ ) 
                {
                    __delay_ms(500); //wait 5 sec
                    if (reset) goto start;
                }
                turn = 0;
                prev_step =5;
            
                // LCD START
                Lcd_Start();
                Lcd_Clear();
                Lcd_Set_Cursor(1,1);
                Lcd_Print_String("  ");
                Lcd_Set_Cursor(2,1);
                Lcd_Print_String("     FINISH");
                // LCD START
            
                finish = 1;
                move_forward = 0; 
                inj1 = 0;
                inj2 = 0;
                turn = 0;
                turning = 0;
                blowing1 = 0;
                blowing2 = 0;
            }
        
        
        if (reset) goto start;     
     
    }
    return 0;
}

