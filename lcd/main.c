#include "msp430g2553.h"
#include "lcdLib.h"

#define RXD       BIT1 
#define TXD       BIT2 
#define RX_BUFFER_LENGTH 50


char rx_buffer[RX_BUFFER_LENGTH] = {'\0'};
char temp= 0;
char first_line[20]; 
char second_line[20]; 
unsigned int count,i = 0; 

void _clear_rx_buffer();
void bluetooth_init();
void bluetooth_send(char * tx_data);


void bluetooth_init()
{
  P1SEL = RXD + TXD;                // Select TX and RX functionality for P1.1 & P1.2
  P1SEL2 = RXD + TXD;               //
  UCA0CTL1 |= UCSSEL_2;             // Have USCI use System Master Clock: AKA core clk 1MHz
  UCA0BR0 = 104;                    // 1MHz 9600, see user manual
  UCA0BR1 = 0;                      //
  UCA0MCTL = UCBRS0;                // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;             // Start USCI state machine
  IE2 |= UCA0RXIE;                  // RXD kesmesi aktif edildi
   __bis_SR_register(GIE);          // Enter LPM0, interrupts enabled
}

void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;
  BCSCTL1 = CALBC1_1MHZ;            
  DCOCTL = CALDCO_1MHZ;

  lcdInit();
  lcdSetText("Program started",0,0);
  __delay_cycles(1000000);// 1ms bekleniyor
  lcdClear();
  bluetooth_init();

  while(1)
  {  
    if(rx_buffer[0] == '1' && rx_buffer[1] == '1')
    {
      lcdClear();
      sprintf(first_line,"%c.button is ",rx_buffer[1]);
      sprintf(second_line,"pressed from %c",rx_buffer[0]);
      lcdSetText(first_line,0,0);
      lcdSetText(second_line,0,1);
      _clear_rx_buffer();
    }
    if(rx_buffer[0] == '1' && rx_buffer[1] == '2')
    {
      lcdClear();
      sprintf(first_line,"%c.button is",rx_buffer[1]);
      sprintf(second_line,"pressed from %c",rx_buffer[0]);
      lcdSetText(first_line,0,0);
      lcdSetText(second_line,0,1);
      _clear_rx_buffer();
    }
    if(rx_buffer[0] == '1' && rx_buffer[1] == '3')
    {
      lcdClear();
      sprintf(first_line,"%c.button is",rx_buffer[1]);
      sprintf(second_line,"pressed from %c",rx_buffer[0]);
      lcdSetText(first_line,0,0);
      lcdSetText(second_line,0,1);
      _clear_rx_buffer();
    }
    if(rx_buffer[0] == '2' && rx_buffer[1] == '1')
    {
      lcdClear();
      sprintf(first_line,"%c.button is",rx_buffer[1]);
      sprintf(second_line,"pressed from %c",rx_buffer[0]);
      lcdSetText(first_line,0,0);
      lcdSetText(second_line,0,1);
      _clear_rx_buffer();
    }
    if(rx_buffer[0] == '2' && rx_buffer[1] == '2')
    {
      lcdClear();
      sprintf(first_line,"%c.button is",rx_buffer[1]);
      sprintf(second_line,"pressed from %c",rx_buffer[0]);
      lcdSetText(first_line,0,0);
      lcdSetText(second_line,0,1);
      _clear_rx_buffer();
    }
    if(rx_buffer[0] == '2' && rx_buffer[1] == '3')
    {
      lcdClear();
      sprintf(first_line,"%c.button is",rx_buffer[1]);
      sprintf(second_line,"pressed from %c",rx_buffer[0]);
      lcdSetText(first_line,0,0);
      lcdSetText(second_line,0,1);
      _clear_rx_buffer();
    }
  }
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{            
 
  temp = UCA0RXBUF; 
  rx_buffer[count++] = temp;
  
  if(count > RX_BUFFER_LENGTH || count == RX_BUFFER_LENGTH)
  {
    _clear_rx_buffer();
    return;
  }
}

void _clear_rx_buffer()
{
  for (i= 0; i< RX_BUFFER_LENGTH; i++)
    rx_buffer[i] = '\0';
  count = 0;
}


void bluetooth_send(char *tx_data)
{
  i=0;
  while(tx_data[i]) // Increment through array, look for null pointer (0) at end of string
  {
    while ((UCA0STAT & UCBUSY)); // Wait if line TX/RX module is busy with data
    UCA0TXBUF = tx_data[i]; // Send out element i of tx_data array on UART bus
    i++; // Increment variable for array address
  }
}
