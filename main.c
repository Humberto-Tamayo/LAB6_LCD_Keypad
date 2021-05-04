//Practica 6 LCD Equipo 1
////+++++++++++++++++++++++++++++++++++++| LIBRARIES / HEADERS |+++++++++++++++++++++++++++++++++++++
#include "device_config.h"
#include <stdint.h>
#include <math.h>
#include <stdio.h>

//+++++++++++++++++++++++++++++++++++++| DIRECTIVES |+++++++++++++++++++++++++++++++++++++

#define _XTAL_FREQ 2000000
#define SWEEP_STEP 250
#define SWEEP_FREQ 20

#define LCD_DATA_R          PORTD
#define LCD_DATA_W          LATD
#define LCD_DATA_DIR        TRISD
#define LCD_RS              LATCbits.LATC2
#define LCD_RS_DIR          TRISCbits.TRISC2
#define LCD_RW              LATCbits.LATC1
#define LCD_RW_DIR          TRISCbits.TRISC1
#define LCD_E               LATCbits.LATC0
#define LCD_E_DIR           TRISCbits.TRISC0

//+++++++++++++++++++++++++++++++++++++| DATA TYPES |+++++++++++++++++++++++++++++++++++++
enum por_ACDC {digital, analog};          // digital = 0, analog = 1
enum por_dir{output, input};

////+++++++++++++++++++++++++++++++++++++| ISRs |+++++++++++++++++++++++++++++++++++++
//// ISR for high priority
//void __interrupt ( high_priority ) high_isr( void );
//// ISR for low priority
//void __interrupt ( low_priority ) low_isr( void );

//+++++++++++++++++++++++++++++++++++++| FUNCTION DECLARATIONS |+++++++++++++++++++++++++++++++++++++
void LCD_rdy(void);
void LCD_init(void);
void LCD_cmd(char cx);
void send2LCD(char xy);
char key_scanner(void);
void portsInit(void);

//+++++++++++++++++++++++++++++++++++++| MAIN |+++++++++++++++++++++++++++++++++++++
void main(void){ 
    LCD_init();
    portsInit();
    char num1 = 0x00;
    char num2 = 0x00;
    char op =  0x00;
    char igual = 0x00;
    char resultado = 0x00;
    char primero = 0x00;
    char segundo = 0x00;
    while(1){
        __delay_ms(SWEEP_STEP);
        if ((num1 == 0x00) & (op == 0x00) & (num2 == 0x00)){
            num1 = key_scanner();
            LCD_cmd (0x8D);
            send2LCD(num1);
            __delay_ms(SWEEP_STEP);
            if (num1 == 0x3E){
                LCD_cmd (0x01);
                num1 = 0x00;
                num2 = 0x00;
                op = 0x00;
                igual = 0x00;
                resultado = 0x00;
                primero = 0x00;
                segundo = 0x00;
            }
        }
        
        if ((num1 != 0x00) & (op == 0x00) & (num2 == 0x00)){
            op = key_scanner();
            LCD_cmd (0x8E);
            send2LCD(op); 
            __delay_ms(SWEEP_STEP);
            if (op == 0x3E){
                LCD_cmd (0x01);
                num1 = 0x00;
                num2 = 0x00;
                op = 0x00;
                igual = 0x00;
                resultado = 0x00;
                primero = 0x00;
                segundo = 0x00;
            }
        }
        
        if ((num1 != 0x00) & (op != 0x00) & (num2 == 0x00)){
            num2 = key_scanner();
            LCD_cmd (0x8F);
            send2LCD(num2); 
            __delay_ms(SWEEP_STEP);
            if (num2 == 0x3E){
                LCD_cmd (0x01);
                num1 = 0x00;
                num2 = 0x00;
                op = 0x00;
                igual = 0x00;
                resultado = 0x00;
                primero = 0x00;
                segundo = 0x00;
            }
        }
        
        if ((num1 != 0x00) & (op != 0x00) & (num2 != 0x00)){
            igual = key_scanner();
            if (igual == 0x3D) {
                if (op == 0x2B){
                    resultado = (num1 - 0x30) + (num2 - 0x30);
                    primero = resultado/10;
                    segundo = resultado%10;
                    LCD_cmd (0xCE);
                    send2LCD(primero + 0x30);
                    LCD_cmd (0xCF);
                    send2LCD(segundo + 0x30);
                }
                if (op == 0x2D){
                    resultado = (num1 - 0x30) - (num2 - 0x30);
                    primero = resultado/10;
                    segundo = resultado%10;
                    LCD_cmd (0xCE);
                    send2LCD(primero + 0x30);
                    LCD_cmd (0xCF);
                    send2LCD(segundo + 0x30);
                }
                if (op == 0x2A){
                    resultado = (num1 - 0x30) * (num2 - 0x30);
                    primero = resultado/10;
                    segundo = resultado%10;
                    LCD_cmd (0xCE);
                    send2LCD(primero + 0x30);
                    LCD_cmd (0xCF);
                    send2LCD(segundo + 0x30);
                }
                if (op == 0xFD){
                    resultado = (num1 - 0x30) / (num2 - 0x30);
                    primero = resultado/10;
                    segundo = resultado%10;
                    LCD_cmd (0xCE);
                    send2LCD(primero + 0x30);
                    LCD_cmd (0xCF);
                    send2LCD(segundo + 0x30);
                }
            }
            __delay_ms(SWEEP_STEP);
            if (igual == 0x3E){
                LCD_cmd (0x01);
                num1 = 0x00;
                num2 = 0x00;
                op = 0x00;
                igual = 0x00;
                resultado = 0x00;
                primero = 0x00;
                segundo = 0x00;
            }
        }
    }
}

void portsInit(void) {
    ANSELA = digital;   // Set port A as Digital for keypad driving
    TRISA  = 0xF0;      // For Port A, set pins 4 to 7 as inputs (columns), and pins 0 to 3 as outputs (rows)
    //OSCCON = 0x74;      // Set the internal oscillator to 8MHz and stable
}

char key_scanner(void){
    LATAbits.LA0 = 0;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 0x31;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 0x32;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 0x33;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 0x2B;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 0;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 0x34;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 0x35;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 0x36;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 0x2D;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 0;
    LATAbits.LA3 = 1;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 0x37;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 0x38;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 0x39;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 0x2A;}
    LATAbits.LA0 = 1;
    LATAbits.LA1 = 1;
    LATAbits.LA2 = 1;
    LATAbits.LA3 = 0;
    __delay_ms(SWEEP_FREQ);
    if      (PORTAbits.RA4 == 0) {__delay_ms(SWEEP_FREQ); return 0x3D;}
    else if (PORTAbits.RA5 == 0) {__delay_ms(SWEEP_FREQ); return 0x30;}
    else if (PORTAbits.RA6 == 0) {__delay_ms(SWEEP_FREQ); return 0x3E;}
    else if (PORTAbits.RA7 == 0) {__delay_ms(SWEEP_FREQ); return 0xFD;}
    else return 0x00;
}


//+++++++++++++++++++++++++++++++++++++| FUNCTIONS |+++++++++++++++++++++++++++++++++++++
// Function to wait until the LCD is not busy
void LCD_rdy(void){
    char test;
    // configure LCD data bus for input
    LCD_DATA_DIR = 0b11111111;
    test = 0x80;
    while(test){
        LCD_RS = 0;         // select IR register
        LCD_RW = 1;         // set READ mode
        LCD_E  = 1;         // setup to clock data
        test = LCD_DATA_R;
        Nop();
        LCD_E = 0;          // complete the READ cycle
        test &= 0x80;       // check BUSY FLAG 
    }
    LCD_DATA_DIR = 0b00000000;
}
//
// LCD initialization function
void LCD_init(void){
    LATC = 0;               // Make sure LCD control port is low
    LCD_E_DIR = 0;          // Set Enable as output
    LCD_RS_DIR = 0;         // Set RS as output 
    LCD_RW_DIR = 0;         // Set R/W as output
    LCD_cmd(0x38);          // Display to 2x40
    LCD_cmd(0x0F);          // Display on, cursor on and blinking
    LCD_cmd(0x01);          // Clear display and move cursor home
    
    
    //OSCCON = 0x74;      // Set the internal oscillator to 8MHz and stable
}

// Send command to the LCD
void LCD_cmd(char cx) {
    LCD_rdy();              // wait until LCD is ready
    LCD_RS = 0;             // select IR register
    LCD_RW = 0;             // set WRITE mode
    LCD_E  = 1;             // set to clock data
    Nop();
    LCD_DATA_W = cx;        // send out command
    Nop();                  // No operation (small delay to lengthen E pulse)
    LCD_E = 0;              // complete external write cycle
}

// Function to display data on the LCD
void send2LCD(char xy){
    LCD_RS = 1;
    LCD_RW = 0;
    LCD_E  = 1;
    LCD_DATA_W = xy;
    Nop();
    Nop();
    LCD_E  = 0;
}
