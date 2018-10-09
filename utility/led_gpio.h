#ifndef LED_GPIO_H
#define LED_GPIO_H

/* Pin Mapping */
#define PinIR_EN       2
#define PinVIS_EN      3 
#define PinIR_SEG1    A5 
#define PinIR_SEG2    A4
#define PinIR_SEG3     6 
#define PinIR_SEG4     8 
#define PinVIS_SEG1   12
#define PinVIS_SEG2   11 
#define PinVIS_SEG3    0
#define PinVIS_SEG4    1
#define PinIR_PWM      5  
#define PinVIS_PWM     9 

#define ENABLE         0x1
#define DISABLE        0x0
//#define HIGH           1
//#define LOW            0 

/* PWM Ramp related definition */ 
#define RAMP_CONF_STEP     1
#define RAMP_CONF_DELAY    1

#define RAMP_MOD_NONE      0
#define RAMP_MOD_HIGH      1
#define RAMP_MOD_LOW       2

// Macro to converts from duty (0..100) to PWM (0..255)
#define DUTY2PWM(x) ((255*(x))/100)

//direct PWM change register
#define REG_PWMIR      OCR4A
#define REG_PWMVIS     OCR4B

// pwm frequency
#define PWM187k     1   // 187500 Hz
#define PWM94k      2   //  93750 Hz
#define PWM47k      3   //  46875 Hz
#define PWM23k      4   //  23437 Hz
#define PWM12k      5   //  11719 Hz
#define PWM6k       6   //   5859 Hz
#define PWM3k       7   //   2930 Hz

/* function prototypes */ 
void pin_init(void);

int pin_vis_global_wr(int state);
int pin_vis_seg1_wr(int state);
int pin_vis_seg2_wr(int state);
int pin_vis_seg3_wr(int state);
int pin_vis_seg4_wr(int state);
int pin_vis_pwm_ramp(int src, int desti);  

int pin_ir_global_wr(int state);
int pin_ir_seg1_wr(int state);
int pin_ir_seg2_wr(int state);
int pin_ir_seg3_wr(int state);
int pin_ir_seg4_wr(int state);
int pin_ir_pwm_ramp(int src, int desti);

#endif /* end of header */ 

