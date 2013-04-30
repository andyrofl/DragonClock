#include <hidef.h>      /* common defines and macros */
#include <mc9s12dg256.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"
#include "main_asm.h" /* interface to the assembly module */

void main(){
  PLL_init();
  DDRM = 0xFF;
  RTI_init();
  lcd_init();
  SCI0_init(9600);
  while(1){
     
  }
}