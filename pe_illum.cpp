/*! 
 * @file pe_illum.cpp
 *
 * @mainpage Photon Energy illumination board driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for PE illumination board's control interface.
 * Arduino Library to control LED_ILLUM Board
 * To achive the high pwm frequency, we use timer 4
 *
 * @section dependencies Dependencies
 *
 * This library depends on the Arduino Core Libraries. The needed internal
 * functions are under utility folder. 
 *
 * @section author Author
 *
 * Electronic designer: Alexander Siekiera(alexander.siekiera@photon-energy.de),
 * Programming:         Junchao Tang(junchao.tang@photon-energy.de). 
 * Copyright(C) by Photon Energy GmbH.
 * 
 * @section version Version
 *
 * Version: V2.0
 */
#include <pe_illum.h>

LedIllum LedCtrl;

const cmd_tbl_t cmd_tbl[] =
{
    /* cmd, function, help */
    { "_getversion"    , cmd_getversion      , "print the version info." },
    { "_ledwr"         , cmd_ledwrite        , "write LED"},
    { "_ledrd"         , cmd_ledread         , "read LED"},
    { NULL }
    
};

/*
 *  Instantiation of the available commands
 */
void cmdInst(void){
    int i = 0;
    LedCtrl.init();
    Serial.begin(115200);
    cmdInit(&Serial);
    
    while(cmd_tbl[i].cmd != NULL){
        cmdAdd(cmd_tbl[i].cmd, cmd_tbl[i].run_cmd);
        i++;
    }
}


/*
 * Command to read version
 */
int cmd_getversion(int argc, char *argv[]){
    if( argc!=1 ){
        cmd_printf("Usage: getversion\n"); 
        return RUN_FAIL;
    }

    cmd_printf("led illumination board V2.0.3\n");
    return RUN_SUCCESS;
}


/*
 * Command interface for writing operation
 */
int cmd_ledwrite(int argc, char *argv[]){
    int id, addr, value, ret;
    
    if(argc!=4){
        cmd_printf("Usage: ledwr <id> <addr> <value>\n");
        return RUN_FAIL;
    }
    
    id    = cmdStr2Num(argv[1], 0);
    addr  = cmdStr2Num(argv[2], 0);
    value = cmdStr2Num(argv[3], 0);
    
    ret = led_write(id, addr, value);    
    
    if(ret!= E_OK){
        _error_log_write(id, addr, ret); 
        return RUN_FAIL;
    }
    else{
        cmd_printf("ledwr 0x%02X 0x%02X %d\n", id, addr, value);
        return RUN_SUCCESS;
    }

}

/*
 * Command interface for reading operation
 */
int cmd_ledread(int argc, char *argv[]){
    int id, addr;
    int value = 0;
    int ret = -1;
    
    if(argc!=3){
        cmd_printf("Usage: ledrd <id> <addr>\n");
        return RUN_FAIL;
    }
    
    id     = cmdStr2Num(argv[1], 0);
    addr   = cmdStr2Num(argv[2], 0);
    
    ret = led_read(id, addr, &value);
    
    if(ret!= E_OK){
        _error_log_read(id, addr, ret);
        return RUN_FAIL;
    }
    else{
        cmd_printf("ledrd 0x%02X 0x%02X %d\n", id, addr, value);
        cmd_printf("<RST:%d:RST>\n", value);
        return RUN_SUCCESS;
    }
    
}

/* 
 * Error message handling
 */
void _error_log_read(int id, int addr, int ret)
{
    cmd_printf("Error %d reading 0x%02X : 0x%02X\n", ret, id, addr);
    _error_msg(ret);
    return;
} 

void _error_log_write(int id, int addr, int ret)
{
    cmd_printf("Error %d writing 0x%02X : 0x%02X\n", ret, id, addr);
    _error_msg(ret);
    return;
} 

// internal error message 
void _error_msg(int ret)
{
    switch (ret){
        case E_FAIL    : cmd_printf(" processing failed\n");       break;
        case E_BREAK   : cmd_printf(" processing break up\n");     break;
        case E_VALID   : cmd_printf(" validation failed\n");       break;    
        case E_RANGE   : cmd_printf(" out of range\n");            break;
        case E_UNVALID : cmd_printf(" unvalid\n");                 break;  
        case E_SAFETY  : cmd_printf(" error because of safety\n"); break;
        case E_NOEXIST : cmd_printf(" not existed\n");             break;
        default        : break; 
    }
}

/*
 * Internal operation to write operation
 */
int led_write(int id, int addr, int value){

    int ret = -1; 

    switch(id){
    case LED_ID_VIS:
        //ret = LedCtrl.Vis_Segment_Write(addr, value);
        ret = _vis_write(addr, value);
        break;        

    case LED_ID_IR:
        //ret = LedCtrl.Ir_Segment_Write(addr, value);
        ret = _ir_write(addr, value);
        break;
    
    default:
        ret = E_NOEXIST;
        break;
    }
    
    return ret;
}

/*
 * Internal operation to read operation
 */
int led_read(int id, int addr, int* value){
    int ret = -1;
    switch(id){
    case LED_ID_VIS:
        ret = _vis_read(addr, value); 
        break;
        
    case LED_ID_IR:
        ret = _ir_read(addr, value);
        break;
        
    default:
        ret = E_NOEXIST;
        break;
    }
    
    return ret;
    
}

int _vis_write(int addr, int value){
    int ret = -1;
    switch(addr){
        case LED_AD_EN:
            if( (value==1) || (value==0)  ){
                ret = LedCtrl.Vis_Enable_Write(value);
            }
            else {
                ret = E_RANGE;
            }
            break;
        
        case LED_AD_SEG1:
        case LED_AD_SEG2:
        case LED_AD_SEG3:
        case LED_AD_SEG4:
            if ( (value==1) || (value==0) ){
                ret = LedCtrl.Vis_Segment_Write(addr, value);
            } 
            else {
                ret = E_RANGE;
            }
            break;

        case LED_AD_PWM:
            if ( (value>=0) && (value<101) ){
                ret = LedCtrl.Vis_Pwm_Write(value);
            }
            else {
                ret = E_RANGE;
            }
            break;

        default :
            ret = E_NOEXIST;
            break;
    }

    return ret;
}

int _ir_write(int addr, int value){
    int ret =  -1; 

    switch(addr){
        case LED_AD_EN:
            if( (value==1) || (value==0) ){
                ret = LedCtrl.Ir_Enable_Write(value);
            } 
            else{
                ret = E_RANGE;
            }
            break;

        case LED_AD_SEG1:
        case LED_AD_SEG2:
        case LED_AD_SEG3:
        case LED_AD_SEG4:
            if( (value==1) || (value==0) ){
                ret = LedCtrl.Ir_Segment_Write(addr, value);
            }
            else{
                ret = E_RANGE;
            }
            break;
            
        case LED_AD_PWM:
            if( (value>=0) && (value<101) ){
                ret = LedCtrl.Ir_Pwm_Write(value);
            }
            else {
                ret = E_RANGE;
            }
            break;

        default: 
            ret = E_NOEXIST;
            break;
    } 
    return ret;
}

int _vis_read(int addr, int* value){
    int ret = -1;

    switch (addr){
        case LED_AD_EN:
            ret = LedCtrl.Vis_Enable_Read(addr, value);
            break;
            
        case LED_AD_SEG1:
        case LED_AD_SEG2:
        case LED_AD_SEG3:
        case LED_AD_SEG4:
            ret = LedCtrl.Vis_Segment_Read(addr, value);
            break;

        case LED_AD_PWM:
            ret = LedCtrl.Vis_Pwm_Read(addr, value);
            break;

        default:
            ret = E_NOEXIST;
            break;
    }
    return ret;
}

int _ir_read(int addr, int* value){
    int ret = -1;

    switch (addr){
        case LED_AD_EN:
            ret = LedCtrl.Ir_Enable_Read(addr, value);
            break;

        case LED_AD_SEG1:
        case LED_AD_SEG2:
        case LED_AD_SEG3:
        case LED_AD_SEG4:
            ret = LedCtrl.Ir_Segment_Read(addr, value);
            break;

        case LED_AD_PWM:
            ret = LedCtrl.Ir_Pwm_Read(addr, value);
            break;

        default:
            ret = E_NOEXIST;
            break; 
    
    }
    return ret;
} 
