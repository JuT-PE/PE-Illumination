#include "utility/led_gpio.h"
#include "utility/statuscode.h"
#include <Arduino.h>
//#include "utility/cmd_uart.h"

/* static functions decleration */ 
static void _pwm_config(int mode);
static int _ramp_pwm(int id, int src, int desti);
static void _setPWM(int id, int duty);

/* pin configure and initiation */
void pin_init(void)
{
    _pwm_config(PWM187k);
}

/* visible enables */
int pin_vis_global_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=LOW) : (pinstate=HIGH);   // pin converted, LOW means turning on, HIGH means turning off
    digitalWrite(PinVIS_EN, pinstate); 
//    cmd_printf("Vis global enable: %d\n", state);
    return E_OK;
} 

int pin_vis_seg1_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    digitalWrite(PinVIS_SEG1, pinstate);
//    cmd_printf("Vis seg1 enable:%d\n", state); 
    return E_OK;
}

int pin_vis_seg2_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    digitalWrite(PinVIS_SEG2, pinstate);
//    cmd_printf("Vis seg2 enable:%d\n", state);
    return E_OK;
}

int pin_vis_seg3_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    digitalWrite(PinVIS_SEG3, pinstate);
//    cmd_printf("Vis seg3 enable:%d\n", state);
    return E_OK;
}

int pin_vis_seg4_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    digitalWrite(PinVIS_SEG4, pinstate);
//    cmd_printf("Vis seg4 enable:%d\n", state);
    return E_OK;
}

/* infrared enables */
int pin_ir_global_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=LOW) : (pinstate=HIGH);   // pin converted, LOW means turning on, HIGH means turning off  
//    cmd_printf("Ir globle enable:%d\n", state);
    return E_OK;
}

int pin_ir_seg1_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
//    cmd_printf("Ir seg1 enable:%d\n", state);
    return E_OK;
}

int pin_ir_seg2_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
//    cmd_printf("Ir seg2 enable:%d\n", state);
    return E_OK; 
} 

int pin_ir_seg3_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
//    cmd_printf("Ir seg3 enable:%d\n", state);
    return E_OK;
}

int pin_ir_seg4_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
//    cmd_printf("Ir seg4 enable:%d\n", state);
    return E_OK;
}

/* visible and infrared pwm */
int pin_vis_pwm_ramp(int src, int desti)
{
//    cmd_printf("Vis pwm %d->%d\n", src, desti);
    return _ramp_pwm(LED_ID_VIS, src, desti);
}

int pin_ir_pwm_ramp(int src, int desti)
{
 //   cmd_printf("Ir pwm %d->%d\n", src, desti);
    return _ramp_pwm(LED_ID_IR, src, desti);
}

static void _pwm_config(int mode){
    // Configure Visible PWM port pin PC6/OC4A
    DDRC  |= _BV(PC6); 
    // Default to High output
    PORTC |= _BV(PC6);
    
    // Configure IR PWM port pin PB5/OC4B
    DDRB  |= _BV(PB5);
    // Default to High output
    PORTB |= _BV(PB5);
    
    // Configure unused pin as input, and set internal pull-up
    // For LedIllum-V2, these pins are still free, and unconnected.  
    DDRB &= ~( _BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB3) | _BV(PB6) );  // set as input
    PORTB |= ( _BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB3) | _BV(PB6) );  // internal pull-up
    DDRC &= ~( _BV(PC7) );
    PORTC |= ( _BV(PC7) );
    DDRD &= ~( _BV(PD4) );
    PORTD |= ( _BV(PD4) );
    DDRE &= ~( _BV(PE6) );
    PORTE |= ( _BV(PE6) );
    // PF4, PF5, PF6, PF7 are Jtag Pins, need to be deactivated, otherweise it overrides
    // I/O operation
    MCUCR |= (1<<JTD); 
    MCUCR |= (1<<JTD);
    DDRF &= ~( _BV(PF4) | _BV(PF5) | _BV(PF6) | _BV(PF7) );
    PORTF |= ( _BV(PF4) | _BV(PF5) | _BV(PF6) | _BV(PF7) );
    // Ignore 10-bit mode for ease use
    
    // TCCR4C configuration, nothing to configure
    // TCCR4C = 0;
    
    // Configure Timer4 for fast PWM, set PWM4B and PWM4C with WGM4 1:0 = 0b00
    TCCR4D &= ~( _BV(WGM41) | _BV(WGM40) );
    
    // PLL Configuration, use 98 MHz / 2 = 48 MHz
//    PLLFRQ = (PLLFRQ & 0xCF) | (0x30);              // if we do this, the mode frequency will be divided by 2
    // PLL Configuration, use 98 MHz
    PLLFRQ = (PLLFRQ & 0xCF) | 0x10 ;
    
    // TCCR4B Configuration
    TCCR4B = mode;
    
    // Set Waveform Output and the connection of output compare Pin.
    TCCR4A |= _BV(COM4A0) | _BV(COM4B0) | _BV(PWM4A) | _BV(PWM4B);
    TCCR4A &= ~( _BV(COM4A1) | _BV(COM4B1) );   
}


static int _ramp_pwm(int id, int src, int desti)
{
    int i = 0;
    int state = src;
    int mode = RAMP_MOD_NONE;       // recorde which direction to ramp 
    
    // check which direction should be ramped
    if (desti>src)       { mode = RAMP_MOD_HIGH; }
    else if(desti<src)   { mode = RAMP_MOD_LOW;  }     
    else                 { mode = RAMP_MOD_NONE; }

    // do ramp
    switch(mode) {
        case RAMP_MOD_HIGH:             // ramp higher
          for(i=0; i<((desti-src)/RAMP_CONF_STEP); i++){
              state = src + (i+1)*RAMP_CONF_STEP;
              _setPWM(id, state);
              delayMicroseconds(RAMP_CONF_DELAY);
          }  
          if(state!=desti){             // check if a last step is needed
              _setPWM(id, desti);
              delayMicroseconds(RAMP_CONF_DELAY);
          }
          break;
        
        case RAMP_MOD_LOW:              // ramp lower 
          for(i=0; i<((src-desti)/RAMP_CONF_STEP); i++){
              state = src - (i+1)*RAMP_CONF_STEP;
              _setPWM(id, state);
              delayMicroseconds(RAMP_CONF_DELAY);
          }
          if(state!=desti){             // check if a last step is needed
              _setPWM(id, desti);
              delayMicroseconds(RAMP_CONF_DELAY);
          }
          break;

        case RAMP_MOD_NONE:
              break;

        default:
             return E_NOEXIST; 
    }  

    return E_OK;
}

static void _setPWM(int id, int duty)
{
    int value = 0;
    // for pwm, IC FL7760 is used, need a remapping for the value, 0%-100% remap
    // to 8%-66%
    value = map(duty, 0, 100, 8, 66);
    switch (id){
        case LED_ID_VIS:
            REG_PWMVIS = DUTY2PWM(value);
            break;
        
        case LED_ID_IR:
            REG_PWMIR = DUTY2PWM(value);
            break;
        
        default:
            break;
    }
    return;
}
