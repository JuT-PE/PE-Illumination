/*!
 * \file illum.cpp
 * Arduino Library to control LED_ILLUM Board
 * To achive the high pwm frequency, we use timer 4
 * Version 1.0.1
 * Copyright (C) 2018 Photon Energy GmbH 
 * Autor: junchao.tang@photon-energy.de
 */

#include <ILLUM.h>

LedIllum::LedIllum(){
}

void LedIllum::init(void){
    // set pin mode 
//    MCUCR |= (1<<JTD); 
//    MCUCR |= (1<<JTD);
    pwm_config(PWM187k);  
    pinMode(PinVIS_EN, OUTPUT);
    pinMode(PinIR_EN,  OUTPUT);
    pinMode(PinVIS_SEG1, OUTPUT);
    pinMode(PinVIS_SEG2, OUTPUT);
    pinMode(PinVIS_SEG3, OUTPUT);
    pinMode(PinVIS_SEG4, OUTPUT);
    pinMode(PinIR_SEG1, OUTPUT);
    pinMode(PinIR_SEG2, OUTPUT);
    pinMode(PinIR_SEG3, OUTPUT);
    pinMode(PinIR_SEG4, OUTPUT);
    pinMode(PinVIS_PWM, OUTPUT);
    pinMode(PinIR_PWM, OUTPUT);

    // initiate global Enable 
    ledDigitalWrite(PinVIS_EN, HIGH); // pin converted, HIGH means off
    ledDigitalWrite(PinIR_EN,  HIGH); // the set state is updated during this setting
 
    // initiate PWM
    setPWM(LED_ID_VIS, 0);
    setPWM(LED_ID_IR, 0);
    dutyVIS = INIT_VIS_PWM;           // only update the default value, during first time ON, ramp to this default setting
    dutyIR  = INIT_IR_PWM;            
 
    // initiate Segment 
    ledDigitalWrite(PinVIS_SEG1, HIGH); // normal pin, init as on, the state is updated during this setting 
    ledDigitalWrite(PinVIS_SEG2, HIGH);
    ledDigitalWrite(PinVIS_SEG3, HIGH);
    ledDigitalWrite(PinVIS_SEG4, HIGH);
    ledDigitalWrite(PinIR_SEG1, HIGH);
    ledDigitalWrite(PinIR_SEG2, HIGH);
    ledDigitalWrite(PinIR_SEG3, HIGH);
    ledDigitalWrite(PinIR_SEG4, HIGH);
}

/************************ ENABLE relavant ***********************************/
/* enable Visible LED */
int LedIllum::Vis_Enable_Write(int state){
    // want to switch on VIS-LEDs
    if(state== ON){                           
        switch (stateVIS) {
            case OFF :                                // disable -> enable
                ledDigitalWrite(PinVIS_EN, LOW);
                ramp(LED_ID_VIS, 0, dutyVIS, DELAY_DEF);
                break;
            case ON :                                // enable -> enable
                // nothing to do
                break; 
            default : 
                break;
        }
        return E_OK;
    }
    // want to switch off VIS-LEDs
    else{                                 
        ramp(LED_ID_VIS, dutyVIS, 0, DELAY_DEF);
        ledDigitalWrite(PinVIS_EN, HIGH);
        return E_OK;
    }
    
    return E_RANGE;
}

/* enable Infrared */
int LedIllum::Ir_Enable_Write(int state){
    // want to switch on IR-LEDs
    if(state==ON){                           
        switch (stateIR) {
            case OFF :                                // disable -> enable
                ledDigitalWrite(PinIR_EN, LOW);
                ramp(LED_ID_IR, 0, dutyIR, DELAY_DEF);
                break;
            case ON :                                // enable -> enable
                // nothing to do
                break; 
            default : 
                break;
        }
        return E_OK;
    }
    // want to switch off IR-LEDs
    else{                                 
        ramp(LED_ID_IR, dutyIR, 0, DELAY_DEF);
        ledDigitalWrite(PinIR_EN, HIGH);
        return E_OK;
    }
    
    return E_RANGE;
}

/************************ PWM relavant **************************************/
/* set visible LED pwm */
int LedIllum::Vis_Pwm_Write(int duty){
    /* check validation */
    if( (duty<0) || (duty>100) ){
        return E_RANGE;
    }
    
    /* do the operation */
    switch (stateVIS){
        case OFF :
            dutyVIS = duty;   // nothing to ramp, because it was still not enabled
            break;        
        case ON : 
            ramp(LED_ID_VIS, dutyVIS, duty, DELAY_DEF); // if VIS-Leds were already enabled, ramp it
            dutyVIS = duty ;  // update the parameter
            break;        
        default:
            break;        
    }
    return E_OK;
}

/* set infrared LED pwm */
int LedIllum::Ir_Pwm_Write(int duty){
    /* check validation */
    if( (duty<0) || (duty>100) ){
        return E_RANGE;
    }

    /* do operation */
    switch (stateIR){
        case OFF :
            dutyIR = duty ;      // nothing to ramp, because it was still not enabled
            break;        
        case ON : 
            ramp(LED_ID_IR, dutyIR, duty, DELAY_DEF); // if IR-Leds were already enabled, ramp it
            dutyIR = duty;
            break;        
        default:
            break;            
    }
    return E_OK;
}
    
/********************** SEGMENT relavant ************************************/
int LedIllum::Vis_Segment_Write(int segaddr, int value)
{
    int ret = -1;
    switch(segaddr){
        case LED_AD_SEG1:
            if(value == stateVIS_SEG1) { return E_OK; }      // same state, nothing to do 
            ret = _segwrite(LED_ID_VIS, PinVIS_SEG1, value);
            break;

         case LED_AD_SEG2:
            if(value == stateVIS_SEG2) { return E_OK; }      // same state, nothing to do 
            ret = _segwrite(LED_ID_VIS, PinVIS_SEG2, value);
            break;

         case LED_AD_SEG3:
            if(value == stateVIS_SEG3) { return E_OK; }      // same state, nothing to do
            ret = _segwrite(LED_ID_VIS, PinVIS_SEG3, value);
            break;
        
         case LED_AD_SEG4:
            if(value == stateVIS_SEG4) { return E_OK; }      // same state, nothing to do
            ret = _segwrite(LED_ID_VIS, PinVIS_SEG4, value);
            break;

         default:
            ret = E_NOEXIST;
            break;
    }

    return ret; 
}

int LedIllum::Ir_Segment_Write(int segaddr, int value)
{
    int ret = -1;
    switch(segaddr){
        case LED_AD_SEG1:
            if(value == stateIR_SEG1) { return E_OK; }       // same state, nothing to do 
            ret = _segwrite(LED_ID_IR, PinIR_SEG1, value);
            break;

        case LED_AD_SEG2:
            if(value == stateIR_SEG2) { return E_OK; }       // same state, nothing to do 
            ret = _segwrite(LED_ID_IR, PinIR_SEG2, value);
            break;

        case LED_AD_SEG3:
            if(value == stateIR_SEG3) { return E_OK; }       // same state, nothing to do 
            ret = _segwrite(LED_ID_IR, PinIR_SEG3, value);
            break;

        case LED_AD_SEG4:
            if(value == stateIR_SEG4) { return E_OK; }       // same state, nothing to do 
            ret = _segwrite(LED_ID_IR, PinIR_SEG4, value);
            break;

        default:
            ret = E_NOEXIST;
            break;
    }

    return ret;
}

/// low level segment operation, with pwm ramp, safety check was before done,
/// ramp pwm but NEVER update global status.
int LedIllum::_segwrite(int id, int seg_pin_addr, int value)
{
    int ret = -1;
    switch(id){
        case LED_ID_VIS:
            if( value==1 ){
                ramp(LED_ID_VIS, dutyVIS, 0, DELAY_HALF );           // ramp vis down
                ledDigitalWrite(seg_pin_addr, HIGH);    // switch on, no safety checking needed  
                ramp(LED_ID_VIS, 0, dutyVIS, DELAY_HALF );           // ramp vis back 
                ret = E_OK;
            }
            else if (value == 0){
                if( _IsVisSegmentSafe() ){              // switch off operation need SAFETY checking!! 
                   ramp(LED_ID_VIS, dutyVIS, 0, DELAY_HALF);        // ramp vis down 
                   ledDigitalWrite(seg_pin_addr, LOW);  // switch off  
                   ramp(LED_ID_VIS, 0, dutyVIS, DELAY_HALF);        // ramp vis back 
                   ret = E_OK;
                }
                else {
                   ret = E_SAFETY;                      // switch off safety checking failed!
                }
            } 
            break;
            
        case LED_ID_IR:
            if( value==1 ){
                ramp(LED_ID_IR, dutyIR, 0, DELAY_HALF);
                ledDigitalWrite(seg_pin_addr, HIGH);
                ramp(LED_ID_IR, 0, dutyIR, DELAY_HALF); 
                ret = E_OK;
            }
            else if ( value==0 ){
                if( _IsIrSegmentSafe() ){
                    ramp(LED_ID_IR, dutyIR, 0, DELAY_HALF);
                    ledDigitalWrite(seg_pin_addr, LOW);
                    ramp(LED_ID_IR, 0, dutyIR, DELAY_HALF);
                    ret = E_OK;
                }
                else{
                    ret = E_SAFETY;
                } 
            }
            break; 

        default:
            ret = E_NOEXIST;
            break;
    } 

    return ret;
}

bool LedIllum::_IsVisSegmentSafe(void)
{
    int cnt = 0;
    if( ledDigitalRead(PinVIS_SEG1) == HIGH )  { cnt++; }
    if( ledDigitalRead(PinVIS_SEG2) == HIGH )  { cnt++; } 
    if( ledDigitalRead(PinVIS_SEG3) == HIGH )  { cnt++; }
    if( ledDigitalRead(PinVIS_SEG4) == HIGH )  { cnt++; } 

    if( cnt<2 ){ return false; }
    else { return true; }
}
 
bool LedIllum::_IsIrSegmentSafe(void)
{
    int cnt = 0;
    if( ledDigitalRead(PinIR_SEG1) == HIGH )  { cnt++; }
    if( ledDigitalRead(PinIR_SEG2) == HIGH )  { cnt++; } 
    if( ledDigitalRead(PinIR_SEG3) == HIGH )  { cnt++; }
    if( ledDigitalRead(PinIR_SEG4) == HIGH )  { cnt++; } 

    if( cnt<2 ){ return false; }
    else { return true; }
}
 

void LedIllum::pwm_config(int mode){
    // Configure Visible PWM port pin PC6/OC4A
    DDRC  |= _BV(PC6); 
    // Default to High output
    PORTC |= _BV(PC6);
    
    // Configure IR PWM port pin PB5/OC4B
    DDRB  |= _BV(PB5);
    // Default to High output
    PORTB |= _BV(PB5);
    
    // Configure portF
//    DDRF  |= _BV(PF0) | _BV(PF1) | _BV(PF4) | _BV(PF5) | _BV(PF6) | _BV(PF7);
//    PORTF &= ~( _BV(PF0) | _BV(PF1) | _BV(PF4) | _BV(PF5) | _BV(PF6) | _BV(PF7) );  
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

/****************** private tools **********************/
///
/// Ramper 
///
//void LedIllum::ramp(int id, int source, int destination)
void LedIllum::ramp(int id, int source, int destination, int rampdelay)
{
    int i = 0;
    int state = source;
    int rampmode = 0;
    int rampstep = 1;
//    int rampdelay = 2; 
    
    if(rampstep < 1){                              // avoid dividing by zero 
        return;
    }
    
    // decision the direction to ramp
    if(destination>source){
        rampmode = RAMPHIGH;                       // ramp higher
    }else if(destination < source){
        rampmode = RAMPLOW;                        // ramp lower
    }
    else{}                                         // nothing to do
    
    // ramp it now
    switch (rampmode){
        case RAMPHIGH :
            for(i=0; i<((destination-source)/rampstep); i++ ){
                state = source+(i+1)*rampstep;
                setPWM(id, state);
                delay(rampdelay);
            }
            if(state != destination){              // check if we need a last step
                setPWM(id, destination);
                delay(rampdelay);
            }
            break;
        
        case RAMPLOW :
            for(i=0; i<((source-destination)/rampstep); i++ ){
                state = source-(i+1)*rampstep;
                setPWM(id, state);
                delay(rampdelay);
            }
            if(state != destination){              // check if we need a last step
                setPWM(id, destination);
                delay(rampdelay);
            }
            break;
                    
        default:
            break;
        
    }    
}

/*
 * Route to set which pwm
 */
void LedIllum::setPWM(int id, int duty)
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

/* function to read out the states */
/******** VIS Read Operations *********/
int LedIllum::Vis_Enable_Read(int segaddr, int* value)
{
    *value = ledDigitalRead(PinVIS_EN);
    return E_OK;
}

int LedIllum::Vis_Pwm_Read(int segaddr, int* value)
{
    *value = dutyVIS;
    return E_OK;
}  

int LedIllum::Vis_Segment_Read(int segaddr, int* value)
{
    int ret = -1;

    switch (segaddr){
    case LED_AD_SEG1:
        *value = ledDigitalRead(PinVIS_SEG1);
        ret = E_OK;
        break;

    case LED_AD_SEG2:
        *value = ledDigitalRead(PinVIS_SEG2);
        ret = E_OK;
        break;

    case LED_AD_SEG3:
        *value = ledDigitalRead(PinVIS_SEG3);
        ret = E_OK;
        break;

    case LED_AD_SEG4:
        *value = ledDigitalRead(PinVIS_SEG4);
        ret = E_OK;
        break;

    default:
        ret = E_UNVALID;
        break;
    }

    return ret;
}

/******** IR Read Operations *********/
int LedIllum::Ir_Enable_Read(int segaddr, int* value)
{
    *value = ledDigitalRead(PinIR_EN);
    return E_OK;
}

int LedIllum::Ir_Pwm_Read(int segaddr, int* value)
{
    *value = dutyIR;
    return E_OK;
}

int LedIllum::Ir_Segment_Read(int segaddr, int* value)
{
    int ret = -1;

    switch (segaddr){
    case LED_AD_SEG1:
        *value = ledDigitalRead(PinIR_SEG1);
        ret = E_OK;
        break;

    case LED_AD_SEG2:
        *value = ledDigitalRead(PinIR_SEG2);
        ret = E_OK;
        break;

    case LED_AD_SEG3:
        *value = ledDigitalRead(PinIR_SEG3);
        ret = E_OK;
        break;

    case LED_AD_SEG4:
        *value = ledDigitalRead(PinIR_SEG4);
        ret = E_OK;
        break;

    default:
        ret = E_UNVALID;
        break;
    }

    return ret;
}



/* self definiert digitalWrite */ 
void LedIllum::ledDigitalWrite(int pin, int state)
{
    digitalWrite(pin, state);           // store the state
    switch (pin){
        case PinVIS_EN:
            (state == LOW) ? (stateVIS = ON) : (stateVIS = OFF);  // pin converted, HIGH:OFF, LOW:ON  
            break;
        case PinVIS_SEG1:
            stateVIS_SEG1 = state;
            break;
        case PinVIS_SEG2:
            stateVIS_SEG2 = state;
            break;
        case PinVIS_SEG3:
            stateVIS_SEG3 = state; 
            break;
        case PinVIS_SEG4:
            stateVIS_SEG4 = state;
            break;
            
        case PinIR_EN:
            (state == LOW) ? (stateIR = HIGH) : (stateIR = LOW);
            break; 
        case PinIR_SEG1:
            stateIR_SEG1 = state;
            break;
        case PinIR_SEG2:
            stateIR_SEG2 = state;
            break;
        case PinIR_SEG3:
            stateIR_SEG3 = state;
            break;
        case PinIR_SEG4:
            stateIR_SEG4 = state;
            break;
            
        default:
            break;
    }
}

/* self definiert digitalRead */
int LedIllum::ledDigitalRead(int pin)
{
    int state = -1;

    switch (pin){
        case PinVIS_EN:
           state = stateVIS; 
           break;
        case PinVIS_SEG1:
           state = stateVIS_SEG1;
           break;
        case PinVIS_SEG2:
           state = stateVIS_SEG2;
           break;
        case PinVIS_SEG3:
           state = stateVIS_SEG3;
           break;
        case PinVIS_SEG4:
           state = stateVIS_SEG4;
           break;

        case PinIR_EN:
           state = stateIR;
           break;
        case PinIR_SEG1:
           state = stateIR_SEG1;
           break;
        case PinIR_SEG2:
           state = stateIR_SEG2;
           break;
        case PinIR_SEG3:
           state = stateIR_SEG3;
           break;
        case PinIR_SEG4:
           state = stateIR_SEG4;
           break;

        default:
           break;
    }
    return state;
}

