/*!
 * @file illum.h
 *
 * Arduino Library to control LED_ILLUM Board
 * To achive the high pwm frequency, we use timer 4
 * Version 1.0.1
 * Copyright (C) 2018 Photon Energy GmbH 
 * Autor: junchao.tang@photon-energy.de
 */
#ifndef ILLUM_H
#define ILLUM_H

#include <Arduino.h>
#include <utility/statuscode.h>

// pwm frequency
#define PWM187k     1   // 187500 Hz
#define PWM94k      2   //  93750 Hz
#define PWM47k      3   //  46875 Hz
#define PWM23k      4   //  23437 Hz
#define PWM12k      5   //  11719 Hz
#define PWM6k       6   //   5859 Hz
#define PWM3k       7   //   2930 Hz

// enable pin
#define PinVIS_EN     2
#define PinIR_EN      3 
#define PinVIS_SEG1  A5 
#define PinVIS_SEG2  A4
#define PinVIS_SEG3   6 
#define PinVIS_SEG4   8 
#define PinIR_SEG1   12
#define PinIR_SEG2   11 
#define PinIR_SEG3    0
#define PinIR_SEG4    1
#define PinVIS_PWM    5  
#define PinIR_PWM     9 

//direct PWM change register
#define REG_PWMVIS    OCR4A
#define REG_PWMIR     OCR4B

// ramp direction mode
#define RAMPHIGH          1
#define RAMPLOW           2
#define DELAY_DEF       250 
#define DELAY_HALF      125 

// initiation value 
#define INIT_VIS_EN       0
#define INIT_IR_EN        0 
#define INIT_VIS_PWM    100
#define INIT_IR_PWM     100

// physical state definition
#define OFF               0
#define ON                1 

// Protection mode on
#ifndef LEDPROTECT
    #define LEDPROTECT
#endif

// Macro to converts from duty (0..100) to PWM (0..255)
#define DUTY2PWM(x) ((255*(x))/100)

class LedIllum {
public:
    LedIllum();
    // parameter to record the setting state
    int  stateIR ;
    int  stateVIS;
    int  dutyIR  ;
    int  dutyVIS ; 
    int  stateVIS_SEG1, stateVIS_SEG2, stateVIS_SEG3, stateVIS_SEG4;
    int  stateIR_SEG1, stateIR_SEG2, stateIR_SEG3, stateIR_SEG4;
    // functions
    void init(void);    

    int Vis_Enable_Write(int state);
    int Vis_Pwm_Write(int duty);
    int Vis_Segment_Write(int segaddr, int value);

    int Vis_Enable_Read(int segaddr, int* value);
    int Vis_Pwm_Read(int segaddr, int* value);
    int Vis_Segment_Read(int segaddr, int* value);

    int Ir_Enable_Write(int state);
    int Ir_Pwm_Write(int duty);
    int Ir_Segment_Write(int segaddr, int value);

    int Ir_Enable_Read(int segaddr, int* value);
    int Ir_Pwm_Read(int segaddr, int* value); 
    int Ir_Segment_Read(int segaddr, int* value);

private:
    bool _IsVisSegmentSafe(void);
    bool _IsIrSegmentSafe(void);
    void pwm_config(int mode);
    void ramp(int id, int source, int destination, int rampdelay);
//    void ramp(int id, int source, int destination);
    int _segwrite(int id, int seg_pin_addr, int value);
    void setPWM(int id, int value);
    void ledDigitalWrite(int pin, int state);
    int ledDigitalRead(int pin);
};

#endif
