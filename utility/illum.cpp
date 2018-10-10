#include "utility/illum.h" 
#include "cmd_uart.h"
#include "utility/statuscode.h"

/* static functions decleration */ 
static void _pwm_config(int mode);
static int _ramp_pwm(int id, int src, int desti);
static void _setPWM(int id, int duty);

LedIllum::LedIllum()
{
    pin_init();
    vis_init();
    ir_init();
}

int LedIllum::vis_init(void) 
{
    pin_vis_global_wr(DISABLE);
    pin_vis_seg1_wr(ENABLE);
    pin_vis_seg2_wr(ENABLE);
    pin_vis_seg3_wr(ENABLE);
    pin_vis_seg4_wr(ENABLE);
    pin_vis_pwm_ramp(0, 100);  
    this->VISstate.global = 0;
    this->VISstate.segment = {1, 1, 1, 1}; 
    this->VISstate.brightness = 100;
    return E_OK;
}

int LedIllum::ir_init(void)
{
    pin_ir_global_wr(DISABLE);
    pin_ir_seg1_wr(ENABLE);
    pin_ir_seg2_wr(ENABLE);
    pin_ir_seg3_wr(ENABLE);
    pin_ir_seg4_wr(ENABLE);
    pin_ir_pwm_ramp(0, 100);
    this->IRstate.global = 0;
    this->IRstate.segment = {1, 1, 1, 1};
    this->IRstate.brightness = 100;
    return E_OK;
}

LedState_t LedIllum::GetVisState(void)
{
    return this->VISstate;
}

LedState_t LedIllum::GetIrState(void)
{
    return this->IRstate; 
}

int LedIllum::SetVisState(LedState_t vis_wanted)
{
//    Update_Vis_Global(vis_wanted.global);
//    Update_Vis_Segment(vis_wanted.segment);
//    Update_Vis_Brightness(vis_wanted.brightness);
    
    this->VISstate = vis_wanted;
    return E_OK;
}

int LedIllum::SetIrState(LedState_t ir_wanted)
{
    this->IRstate = ir_wanted;
    return E_OK;
}

int LedIllum::Update_Vis_Global(int vis_global_wanted)
{
    if(vis_global_wanted == this->VISstate.global){
        // same state, nothing to do
        return E_OK; 
    } 

    // change global enable state
    if(vis_global_wanted == TURN_OFF){      // want to turn off
        pin_vis_pwm_ramp(this->VISstate.brightness, 0);  // safely dim to 0 before turn off  
        pin_vis_global_wr(TURN_OFF);                     // turn off
    }
    else{
        pin_vis_pwm_ramp(this->VISstate.brightness, 0);  // it was still off, we ramp to 0 for safety turn on 
        pin_vis_global_wr(TURN_ON);                      // safely turn on
        pin_vis_pwm_ramp(0, this->VISstate.brightness);  // ramp back 
    }
    // report new state 
    this->VISstate.global = vis_global_wanted;
    return E_OK;
}

int LedIllum::Update_Vis_Segment(int seg1_w, int seg2_w, int seg3_w, int seg4_w)
{
    SegState_t vis_seg_wanted = {seg1_w, seg2_w, seg3_w, seg4_w}; 
    /* 0. compare state */
    SegState_t current = this->VISstate.segment;
    if(   (vis_seg_wanted.seg1 == current.seg1) 
       && (vis_seg_wanted.seg2 == current.seg2) 
       && (vis_seg_wanted.seg3 == current.seg3)
       && (vis_seg_wanted.seg4 == current.seg4)
     ){  return E_OK; } 
    
    // 1. pwm ramp down
    pin_vis_pwm_ramp(this->VISstate.brightness, 0);

    // 2. pin state change 
    pin_vis_seg1_wr(vis_seg_wanted.seg1);
    pin_vis_seg2_wr(vis_seg_wanted.seg2);
    pin_vis_seg3_wr(vis_seg_wanted.seg3);
    pin_vis_seg4_wr(vis_seg_wanted.seg4);

    // 3. pwm ramp high
    pin_vis_pwm_ramp(0, this->VISstate.brightness); 

    // 4. report state 
    this->VISstate.segment = vis_seg_wanted;
    return E_OK; 
}

int LedIllum::Update_Vis_Brightness(int vis_brightness_wanted)
{
    int ret=-1;
    ret = pin_vis_pwm_ramp(this->VISstate.brightness, vis_brightness_wanted);
    if(ret!=E_OK){ return ret;  }

    this->VISstate.brightness = vis_brightness_wanted;
    return E_OK;
}

int LedIllum::Update_Ir_Global(int ir_global_wanted)
{
    if(ir_global_wanted == this->IRstate.global){
        return E_OK;                                       // same state, nothing to do
    }

    // change global enable state
    if(ir_global_wanted == TURN_OFF){
        pin_ir_pwm_ramp(this->IRstate.brightness, 0);
        pin_ir_global_wr(TURN_OFF);
    }else{
        pin_ir_pwm_ramp(this->IRstate.brightness, 0);
        pin_ir_global_wr(TURN_ON);
        pin_ir_pwm_ramp(0, this->IRstate.brightness);
    }

    // report new state
    this->IRstate.global = ir_global_wanted;
    return E_OK;
}

int LedIllum::Update_Ir_Segment(int seg1_w, int seg2_w, int seg3_w, int seg4_w)
{
    SegState_t ir_seg_wanted = {seg1_w, seg2_w, seg3_w, seg4_w}; 
    /* 0. compare the state */
    SegState_t current = this->IRstate.segment;
    if(   (ir_seg_wanted.seg1 == current.seg1) 
       && (ir_seg_wanted.seg2 == current.seg2) 
       && (ir_seg_wanted.seg3 == current.seg3)
       && (ir_seg_wanted.seg4 == current.seg4)
     ){  return E_OK; } 

    // 1. pwm ramp down
    pin_ir_pwm_ramp(this->IRstate.brightness, 0);

    // 2. pin state change 
    pin_ir_seg1_wr(ir_seg_wanted.seg1);
    pin_ir_seg2_wr(ir_seg_wanted.seg2);
    pin_ir_seg3_wr(ir_seg_wanted.seg3);
    pin_ir_seg4_wr(ir_seg_wanted.seg4);

    // 3. pwm ramp back 
    pin_ir_pwm_ramp(0, this->IRstate.brightness); 

    // 4. report state 
    this->IRstate.segment = ir_seg_wanted;
    return E_OK;
} 

int LedIllum::Update_Ir_Brightness(int ir_brightness_wanted)
{
    int ret=-1;
    ret = pin_ir_pwm_ramp(this->IRstate.brightness, ir_brightness_wanted);
    if(ret!=E_OK){ return ret; }

    this->IRstate.brightness = ir_brightness_wanted;
    return E_OK;
}

/******************** Independent operation *********************************/
int LedIllum::Vis_Enable_Write(int state)
{
    return Update_Vis_Global(state);
}

int LedIllum::Ir_Enable_Write(int state)
{
    return Update_Ir_Global(state);
}
/*
 *  check if segments wanted state are OK 
 */
int chk_safety(SegState_t segment)
{
    // check parameter validation 
    if(    ( (segment.seg1!=0)&&(segment.seg1!=1) )
        || ( (segment.seg2!=0)&&(segment.seg2!=1) )
        || ( (segment.seg3!=0)&&(segment.seg3!=1) ) 
        || ( (segment.seg4!=0)&&(segment.seg4!=1) )
      ) { return E_NOEXIST; }

    // check safety
    if( (segment.seg1==0) && (segment.seg2==0) && (segment.seg3==0) && (segment.seg4==0) ) {
        return E_SAFETY;
    }

    return E_OK;
} 

/********************** SEGMENT relavant ************************************/
int LedIllum::Vis_Segment_Write(int segaddr, int value)
{
    int ret = -1;
    SegState_t seg_want;                             // declear a temp state struct
    seg_want.seg1 = this->VISstate.segment.seg1;     // sync with current state
    seg_want.seg2 = this->VISstate.segment.seg2;
    seg_want.seg3 = this->VISstate.segment.seg3;  
    seg_want.seg4 = this->VISstate.segment.seg4;

    /* 1. build new command */
    switch(segaddr){
        case LED_AD_SEG1:
            seg_want.seg1 = value;
            break;

         case LED_AD_SEG2:
            seg_want.seg2 = value;
            break;

         case LED_AD_SEG3:
            seg_want.seg3 = value;
            break;
        
         case LED_AD_SEG4:
            seg_want.seg4 = value;
            break;

         default:
            ret = E_NOEXIST;
            return ret;
    }

    /* 2. check safety */
    ret = chk_safety(seg_want);
    if(ret!=E_OK){  return ret;  } 

    /* 3. do setting */
    return Update_Vis_Segment(seg_want.seg1, seg_want.seg2, seg_want.seg3, seg_want.seg4);
}

int LedIllum::Ir_Segment_Write(int segaddr, int value)
{
    int ret = -1;
    SegState_t seg_want;                            // declear a temp state struct
    seg_want.seg1 = this->IRstate.segment.seg1;     // sync with current state
    seg_want.seg2 = this->IRstate.segment.seg2;
    seg_want.seg3 = this->IRstate.segment.seg3;
    seg_want.seg4 = this->IRstate.segment.seg4;
    /* 1. build command */
    switch(segaddr){
        case LED_AD_SEG1:
            seg_want.seg1 = value;
            break;

        case LED_AD_SEG2:
            seg_want.seg2 = value;
            break;

        case LED_AD_SEG3:
            seg_want.seg3 = value;
            break;

        case LED_AD_SEG4:
            seg_want.seg4 = value;
            break;

        default:
            ret = E_NOEXIST;
            return ret;
    }

    /* 2. check safety */ 
    ret = chk_safety(seg_want);
    if(ret!=E_OK){  return ret;  }

    /* 3. do setting */
    return Update_Ir_Segment(seg_want.seg1, seg_want.seg2, seg_want.seg3, seg_want.seg4);
}

int LedIllum::Vis_Pwm_Write(int duty)
{
    return Update_Vis_Brightness(duty);
}


int LedIllum::Ir_Pwm_Write(int duty)
{
    return Update_Ir_Brightness(duty);
}

/***** Read operation ****/
int LedIllum::Vis_Enable_Read(int segaddr, int *value)
{
    *value = this->VISstate.global;
    return E_OK;
}

int LedIllum::Vis_Segment_Read(int segaddr, int *value)
{
    switch(segaddr){
        case LED_AD_SEG1:
            *value = this->VISstate.segment.seg1;
            break;

        case LED_AD_SEG2:
            *value = this->VISstate.segment.seg2;
            break;

        case LED_AD_SEG3:
            *value = this->VISstate.segment.seg3;
            break;

        case LED_AD_SEG4:
            *value = this->VISstate.segment.seg4;
            break;

        default:
            return E_NOEXIST;
    }

    return E_OK; 
}

int LedIllum::Vis_Pwm_Read(int segaddr, int *value)
{
    *value = this->VISstate.brightness;  
    return E_OK;
}

int LedIllum::Ir_Enable_Read(int segaddr, int *value)
{
    *value = this->IRstate.global;
    return E_OK;
}

int LedIllum::Ir_Segment_Read(int segaddr, int *value)
{
    switch(segaddr){
        case LED_AD_SEG1:
            *value = this->IRstate.segment.seg1;
            break;

        case LED_AD_SEG2:
            *value = this->IRstate.segment.seg2;
            break;

        case LED_AD_SEG3:
            *value = this->IRstate.segment.seg3;
            break;

        case LED_AD_SEG4:
            *value = this->IRstate.segment.seg4;
            break;

        default:
            return E_NOEXIST;
    } 

    return E_OK;
}

int LedIllum::Ir_Pwm_Read(int segaddr, int *value)
{
    *value = this->IRstate.brightness; 
    return E_OK;
}

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
    cmd_printf("Vis global enable: %d\n", state);
    return E_OK;
} 

int pin_vis_seg1_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    digitalWrite(PinVIS_SEG1, pinstate);
    cmd_printf("Vis seg1 enable:%d\n", state); 
    return E_OK;
}

int pin_vis_seg2_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    digitalWrite(PinVIS_SEG2, pinstate);
    cmd_printf("Vis seg2 enable:%d\n", state);
    return E_OK;
}

int pin_vis_seg3_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    digitalWrite(PinVIS_SEG3, pinstate);
    cmd_printf("Vis seg3 enable:%d\n", state);
    return E_OK;
}

int pin_vis_seg4_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    digitalWrite(PinVIS_SEG4, pinstate);
    cmd_printf("Vis seg4 enable:%d\n", state);
    return E_OK;
}

/* infrared enables */
int pin_ir_global_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=LOW) : (pinstate=HIGH);   // pin converted, LOW means turning on, HIGH means turning off  
    cmd_printf("Ir globle enable:%d\n", state);
    return E_OK;
}

int pin_ir_seg1_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    cmd_printf("Ir seg1 enable:%d\n", state);
    return E_OK;
}

int pin_ir_seg2_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    cmd_printf("Ir seg2 enable:%d\n", state);
    return E_OK; 
} 

int pin_ir_seg3_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    cmd_printf("Ir seg3 enable:%d\n", state);
    return E_OK;
}

int pin_ir_seg4_wr(int state)
{
    int pinstate;
    (state==ENABLE) ? (pinstate=HIGH) : (pinstate=LOW);
    cmd_printf("Ir seg4 enable:%d\n", state);
    return E_OK;
}

/* visible and infrared pwm */
int pin_vis_pwm_ramp(int src, int desti)
{
    cmd_printf("Vis pwm %d->%d\n", src, desti);
    return _ramp_pwm(LED_ID_VIS, src, desti);
}

int pin_ir_pwm_ramp(int src, int desti)
{
    cmd_printf("Ir pwm %d->%d\n", src, desti);
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

