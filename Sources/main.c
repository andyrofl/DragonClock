#include <hidef.h>      /* common defines and macros */
#include <mc9s12dg256.h>     /* derivative information */
#pragma LINK_INFO DERIVATIVE "mc9s12dg256b"
#include "main_asm.h" /* interface to the assembly module */

char seconds; char minutes; char hours; char tickFlag;
int ticks;
char alarmTime[] = "01234567";

void AddAlarm(char slot, char h, char m);
void SetAlarms();
char CheckAlarms();

char CountTick();  //return 1 if need to update seconds. 2 if minutes. 3 if hours.
void AddSecond();
void AddMinute();
char AddHour();    //return 1 if need to reset clock.
void ResetClock();

void interrupt 7 handler(){
  ticks++;
  clear_RTI_flag();
}

void main(){
  PLL_init();
  DDRM = 0xFF;
  RTI_init();
  lcd_init();
  SCI0_init(9600);
  SetAlarms();
  while(1){
    //timer
    tickFlag = CountTick();

    if(tickFlag == 1){
      AddSecond();
      set_lcd_addr(0x06);
      write_time_lcd(seconds);
    }
    else if(tickFlag == 2){
      AddMinute();
      set_lcd_addr(0x03);
      write_time_lcd(minutes);
      set_lcd_addr(0x06);
      write_time_lcd(seconds);
      
      if(CheckAlarms() > -1){
        set_lcd_addr(0x40);
        type_lcd("alarm"); 
      }
    }
    else if(tickFlag == 3){
      set_lcd_addr(0x00);
      write_time_lcd(hours);
      set_lcd_addr(0x03);
      write_time_lcd(minutes);
      set_lcd_addr(0x06);
      write_time_lcd(seconds);
      if(AddHour()){
        ResetClock(); 
      }  
    }
     
  }
}

//=================================================================
//
// INPUT
//
//=================================================================

void SetAlarms(){
  char index = 0;
  //toggle index with buttons on bottom row. take values with top three rows.
  //set the indicated time to the selected switch. break when 15 is pressed, set another when 12 is pressed.
}

//=================================================================
//
// ALARM HHMM
//
//=================================================================
/**
 * Add an alarm of h:m at index slot
 */
void AddAlarm(char slot, char h, char m){
  alarmTime[slot] = (h * 100) + m;
}        

/**
 * Check alarms.
 * return index of alarm currently going off. -1 if none.
 */
char CheckAlarms(){
  char i = -1;
  char time;
  for(i = 0; i < 8; i++){
    if(alarmTime[i] != 0){
      time = alarmTime[i];
      if(time / 100 == hours){
        if(time % 100 == minutes){
          return i;
        }
      }
    }
  }
  return -1;
}

//=================================================================
//
//  TIMER
//
//=================================================================

/**
 * Add one tick to the counter
 * Return 1 if one second has passed. 2 if one minute. Reset tick counter if at minute.
 */
char CountTick(){
  if(ticks > 5859374){    //exact ticks for one hour (5859375)
    ticks = 0;
    return 3;
  }
  else if(ticks % 5859 == 0){  //approximate ticks for one minute
    return 2;
  }
  else if(ticks % 98 == 0){    //approximate ticks for one second
    return 1;
  }
  return 0;
}
/**
 * Add one second to the counter
 * Return 1 and reset seconds to 0 if seconds is 59 or greater upon entry
 */
void AddSecond(){
  if(seconds > 59){
    seconds = 00;
    return;
  }
  seconds++;  
}
/**
 * Add one minute to the counter
 * Return 1 and reset minutes if seconds is 59 or greater upon entry
 */
void AddMinute(){
  if(minutes > 59){
    minutes = 00;
    return;
  }
  minutes++;
}
/**
 * Add one minute to the counter
 * Return 1 and return 1 if hours is 23 or greater upon entry 
 */
char AddHour(){
  if(hours > 22){
    return 1;
  }
  hours++;
  return 0;
}
/**
 *  Reset all counters to roll over into a new day.
 */
void ResetClock(){
  seconds = 0;
  minutes = 0;
  hours = 0;
}