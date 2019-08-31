#include "msp430g2553.h"

#define RXD         BIT1 // P1.1 --> RX   
#define TXD         BIT2 // P1.2 --> TX
#define BUTTON_1    BIT0 // P2.0 --> 1.buton
#define BUTTON_2    BIT1 // P2.1 --> 2.buton
#define BUTTON_3    BIT2 // P2.2 --> 3.buton
#define BT_ENABLE   BIT4 // P1.4 --> Bluetooth enable pini
#define BT_STATE    BIT5 // P1.5 --> Bluetooth state pini

void bluetooth_send(char * tx_data);
void bluetooth_init();

int main( void )
{
  ///Watchdog timer kapatildi ve islemcinin calisma frekansi 1MHz olarak ayarlandi.
  WDTCTL = WDTPW + WDTHOLD;      
  BCSCTL1 = CALBC1_1MHZ;       
  DCOCTL = CALDCO_1MHZ;         
  
  /// Bluetoothun state pini input, enable pini ise output olarak ayarlandi.
  P1DIR |= BT_ENABLE;  
  P1OUT &= ~(BT_ENABLE); 
  P1DIR &= ~(BT_STATE); 
  /// Butonlar input olarak tanimlandi.
  P2DIR &=~(BUTTON_1); 
  P2DIR &=~(BUTTON_2);   
  P2DIR &=~(BUTTON_3);  
  ///Butonlar falling edge (dusen kenarda) interrupta girecek sekilde ayarlandi
  P2IES |= BUTTON_1;   
  P2IES |= BUTTON_2;    
  P2IES |= BUTTON_3;    
  ///Butonlarin interruplari aktif edildi.
  P2IE  |= BUTTON_1;  
  P2IE  |= BUTTON_2;  
  P2IE  |= BUTTON_3;    
  ///Butonlar için devrede yer alan pull-up direncleri aktif edildi.
  P2REN |= BUTTON_1;  
  P2REN |= BUTTON_2;   
  P2REN |= BUTTON_3;    
  ///Butonlarin bagli oldugu pinlere default olarak 3.3V uygulandi.
  P2OUT |= BUTTON_1; 
  P2OUT |= BUTTON_2;   
  P2OUT |= BUTTON_3;     
 
  bluetooth_init();       // Bluetoothu baslatma fonksiyonu cagrildi.
  __bis_SR_register(GIE); // Tüm Interruptlar aktif edildi.
  
  for(;;)
  {  
  }
}

///Bluetooth ile mikroislemcinin haberlesmesinde kullanilmak icin UART konfigure edildi. 
void bluetooth_init()   
{
  P1SEL = RXD + TXD;    // P1SEL registeri kullanarak ilgili pinleri UART icin kullanmak uzere tanimladik.  
  P1SEL2 = RXD + TXD;   // P1.1= RX P1.2= TX olarak ayarlandi
  
  UCA0CTL1 |= UCSSEL_2; // UART'in calisma frekansi 1MHz olarak ayarlandi 
                        // UCSSEL_2 --> SMCLK UART'ta kullanilacak olan calisma frekansini 
                        // mikroislemcinin calisma frekansi olarak ayarlar.
  
  UCA0BR0 = 104;        // 1Mhz/104 =9600 baudrate 
  UCA0BR1 = 0;          // 9600 baudrate icin UCABR1 registerina 0 yazmamiz gerekiyor.
  UCA0MCTL = UCBRS0;    // UART Baudrate hassas ayar        
  UCA0CTL1 &= ~UCSWRST; // UART islemlerinin gerceklesmesi icin UCSWRST registerini temizlememiz gerekiyor.                        
}
///Bluetooth'tan veri gönderme icin kullanilacak olan fonksiyon.
void bluetooth_send(char *tx_data) 
{
  unsigned int i=0;
  while(tx_data[i]) // Increment through array, look for null pointer (0) at end of string
  {
    while ((UCA0STAT & UCBUSY)); // UART modülünün mesgul olup olmadigi kontrol ediliyor ve mesguliyet durumu 
                                 // (öncesinde veri gönderme veya alma durumu) bitene kadar bekleniyor.
    UCA0TXBUF = tx_data[i];      // Karakter dizi (tx_data) icerisinde yer alan veriler gonderiliyor.
    i++; 
  }
}

//Butonlara basildigi zaman kod P2 portuna bagli olan butonlar (kurulu olan pull-up devresinden dolayi
//3.3V'tan 0V seviyesine inmekte ve düsen kenar tetiklenmektedir ve interrupt fonksiyonu calismaktadir.

#pragma vector=PORT2_VECTOR  
__interrupt void Port_2(void)
{ 
  if(P2IFG&BUTTON_1)    //  P2.0 pinine bagli olan 1.buton(BUTTON_1)'e basildigi zaman P2IFG  registerininin 0 numarali biti 
                        // otomatik olarak 1 olmakta ve bu satirda yer alan if kosuluyla bunun kontrolu yapilmaktadir. 
  {
    P1OUT |= BT_ENABLE;        // Veri göndermek icin ilk etapta bluetooth uzerinde yer alan ENABLE pinini aktif ediyoruz. 
    while(!(P1IN & BT_STATE)); // ENABLE pini aktif olduktan sonra diger bluetooth ile olan baglanti kurulma islemi basliyor ve diger bluetooth
                               // ile baglanti kuruldugu zaman bluetooth üzerinde yer alan STATE pini aktif oluyor ve bu satirda yer alan kodda 
                               // bunun kontrolünü yapiyoruz.
    __delay_cycles(100000);    //Baglanti kurulduktan sonra 1ms bekliyoruz.
    bluetooth_send("11");      // 11 verisini gonderiyoruz.
    __delay_cycles(100000);    //Tekrar bekliyoruz.
    P1OUT &=~(BT_ENABLE);      //Veri gonderildikten sonra ENABLE pinini kapatiyoruz.
    P2IFG &= ~(BUTTON_1);      //1.butona basildiktan sonra aktif olan interrupt flagini tekrar 0'liyoruz.
  }
  else if(P2IFG&BUTTON_2)  
  {
    P1OUT |= BT_ENABLE; 
    while(!(P1IN & BT_STATE));
    __delay_cycles(100000);
    bluetooth_send("12");
    __delay_cycles(100000);
    P1OUT &=~(BT_ENABLE); 
    P2IFG &= ~(BUTTON_2);
  }
  else if(P2IFG&BUTTON_3) 
  {
    P1OUT |= BT_ENABLE; 
    while(!(P1IN & BT_STATE));
    __delay_cycles(100000);
    bluetooth_send("13");
    __delay_cycles(100000);
    P1OUT &=~(BT_ENABLE); 
    P2IFG &= ~(BUTTON_3);
  }
  else
    __no_operation(); 