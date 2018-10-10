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
#include "pe_illum.h"

LedIllum LedCtrl;

const cmd_tbl_t cmd_tbl[] =
{
    /* cmd, function, help */
    { "getversion"    , cmd_getversion      , "print the version info." },
    { "ledwr"         , cmd_ledwrite        , "write LED"},
    { "ledrd"         , cmd_ledread         , "read LED"},
    { "lightcode"     , cmd_lightcode       , "write or read using light code"},
    { NULL }
};

/*
 *  Instantiation of the available commands
 */
void cmdInst(void){
    int i = 0;
//    LedCtrl.init();
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

    cmd_printf("led illumination board V2.1.4\n");
    return RUN_SUCCESS;
}

int _lightcode_write(unsigned int viscode, unsigned int ircode){
    int ret;

    /* parser */ 
    int vis_global_wanted      = (int) ( (viscode>>0)&0x1 );
    int vis_seg1_wanted        = (int) ( (viscode>>1)&0x1 ); 
    int vis_seg2_wanted        = (int) ( (viscode>>2)&0x1 );
    int vis_seg3_wanted        = (int) ( (viscode>>3)&0x1 );
    int vis_seg4_wanted        = (int) ( (viscode>>4)&0x1 );
    int vis_brightness_wanted  = (int) ( (viscode>>5)&0xFF); 

    int ir_global_wanted       = (int) ( (ircode>>0)&0x1 );
    int ir_seg1_wanted         = (int) ( (ircode>>1)&0x1 ); 
    int ir_seg2_wanted         = (int) ( (ircode>>2)&0x1 );
    int ir_seg3_wanted         = (int) ( (ircode>>3)&0x1 );
    int ir_seg4_wanted         = (int) ( (ircode>>4)&0x1 );
    int ir_brightness_wanted   = (int) ( (ircode>>5)&0xFF); 
   
    /* check range */
    if( vis_global_wanted==1 ){                           // want to switch on
        if( ((viscode>>1)&0xF) == 0){                     // check segment safety
            return E_SAFETY;
        }
    }
    if(vis_brightness_wanted > 100) { return E_RANGE; }   // check brightness percentage value

    if( ir_global_wanted==1 ){
        if( ((ircode>>1)&0xF) == 0){
            return E_SAFETY;
        }
    }
    if(ir_brightness_wanted  > 100) { return E_RANGE; }

    /* do operation */
    ret = LedCtrl.Update_Vis_Global(vis_global_wanted);
    if(ret!=E_OK){
        cmd_printf("Error %d: vis en fault\n", ret);  
        return ret;
    }

    ret = LedCtrl.Update_Vis_Segment(vis_seg1_wanted, vis_seg2_wanted, vis_seg3_wanted, vis_seg4_wanted);
    if(ret!=E_OK){
        cmd_printf("Error %d: vis seg fault %d%d%d%d\n", ret, vis_seg4_wanted, vis_seg3_wanted, vis_seg2_wanted, vis_seg1_wanted);
        return ret;
    }

    ret = LedCtrl.Update_Vis_Brightness(vis_brightness_wanted);
    if(ret!=E_OK){
        cmd_printf("Error %d: vis pwm fault %d\n", ret, vis_brightness_wanted); 
        return ret;
    }

    ret = LedCtrl.Update_Ir_Global(ir_global_wanted);
    if(ret!=E_OK){
        cmd_printf("Error %d: ir en fault\n", ret);
        return ret;
    }

    ret = LedCtrl.Update_Ir_Segment(ir_seg1_wanted, ir_seg2_wanted, ir_seg3_wanted, ir_seg4_wanted);
    if(ret!=E_OK){
        cmd_printf("Error %d: ir seg fault %d%d%d%d\n", ret, ir_seg4_wanted, ir_seg3_wanted, ir_seg2_wanted, ir_seg1_wanted);
        return ret;
    }

    ret = LedCtrl.Update_Ir_Brightness(ir_brightness_wanted);
    if(ret!=E_OK){
        cmd_printf("Error %d: ir pwm fault %d\n", ret, ir_brightness_wanted); 
        return ret;
    }

    return E_OK;

    /* VIS: check if change is needed, if so, do it */
/*    if(vis_global_wanted != LedCtrl.VISstate.global) { 
        ret = _vis_write(LED_AD_EN, vis_global_wanted);
        if(ret!=E_OK){ 
            _error_log_write(LED_ID_VIS, LED_AD_EN, vis_global_wanted);
            return ret;
        }
    }

    if(vis_seg1_wanted != LedCtrl.VISstate.segment.seg1) { 
        ret = _vis_write(LED_AD_SEG1, vis_seg1_wanted);
        if(ret!=E_OK){ 
            _error_log_write(LED_ID_VIS, LED_AD_SEG1, vis_seg1_wanted);
            return ret;
        }
    }

    if(vis_seg2_wanted != LedCtrl.VISstate.segment.seg2) {
        ret = _vis_write(LED_AD_SEG2, vis_seg2_wanted);
        if(ret!=E_OK){
            _error_log_write(LED_ID_VIS, LED_AD_SEG2, vis_seg2_wanted);
            return ret;
        }
    }

    if(vis_seg3_wanted != LedCtrl.VISstate.segment.seg3) {
        ret = _vis_write(LED_AD_SEG3, vis_seg3_wanted);
        if(ret!=E_OK){
           _error_log_write(LED_ID_VIS, LED_AD_SEG3, vis_seg3_wanted); 
           return ret;
        }
    }

    if(vis_seg4_wanted != LedCtrl.VISstate.segment.seg4) {
        ret = _vis_write(LED_AD_SEG4, vis_seg4_wanted);
        if(ret!=E_OK){
            _error_log_write(LED_ID_VIS, LED_AD_SEG4, vis_seg4_wanted);
            return ret;
        }
    }

    if(vis_brightness_wanted != LedCtrl.VISstate.brightness) {
        ret = _vis_write(LED_AD_PWM, vis_brightness_wanted);
        if(ret!=E_OK){
            _error_log_write(LED_ID_VIS, LED_AD_PWM, vis_brightness_wanted);
            return ret;
        }
    }

    // IR: check if change is needed, if so, do it 

    if(ir_global_wanted != LedCtrl.IRstate.global) { 
        ret = _ir_write(LED_AD_EN, ir_global_wanted);
        if(ret!=E_OK){ 
            _error_log_write(LED_ID_IR, LED_AD_EN, ir_global_wanted);
            return ret;
        }
    }

    if(ir_seg1_wanted != LedCtrl.IRstate.segment.seg1) { 
        ret = _ir_write(LED_AD_SEG1, ir_seg1_wanted);
        if(ret!=E_OK){ 
            _error_log_write(LED_ID_IR, LED_AD_SEG1, ir_seg1_wanted);
            return ret;
        }
    }

    if(ir_seg2_wanted != LedCtrl.IRstate.segment.seg2) {
        ret = _ir_write(LED_AD_SEG2, ir_seg2_wanted);
        if(ret!=E_OK){
            _error_log_write(LED_ID_IR, LED_AD_SEG2, ir_seg2_wanted);
            return ret;
        }
    }

    if(ir_seg3_wanted != LedCtrl.IRstate.segment.seg3) {
        ret = _ir_write(LED_AD_SEG3, ir_seg3_wanted);
        if(ret!=E_OK){
           _error_log_write(LED_ID_IR, LED_AD_SEG3, ir_seg3_wanted); 
           return ret;
        }
    }

    if(ir_seg4_wanted != LedCtrl.IRstate.segment.seg4) {
        ret = _ir_write(LED_AD_SEG4, ir_seg4_wanted);
        if(ret!=E_OK){
            _error_log_write(LED_ID_IR, LED_AD_SEG4, ir_seg4_wanted);
            return ret;
        }
    }

    if(ir_brightness_wanted != LedCtrl.IRstate.brightness) {
        ret = _ir_write(LED_AD_PWM, ir_brightness_wanted);
        if(ret!=E_OK){
            _error_log_write(LED_ID_IR, LED_AD_PWM, ir_brightness_wanted);
            return ret;
        }
    }*/

    return E_OK; 
}

unsigned int _vis_lightcode_read(void)
{
    unsigned int vis_en_code   = (unsigned int) ((LedCtrl.VISstate.global       &0x1)<<0 );
    unsigned int vis_seg1_code = (unsigned int) ((LedCtrl.VISstate.segment.seg1 &0x1)<<1 ); 
    unsigned int vis_seg2_code = (unsigned int) ((LedCtrl.VISstate.segment.seg2 &0x1)<<2 );
    unsigned int vis_seg3_code = (unsigned int) ((LedCtrl.VISstate.segment.seg3 &0x1)<<3 );
    unsigned int vis_seg4_code = (unsigned int) ((LedCtrl.VISstate.segment.seg4 &0x1)<<4 );
    unsigned int vis_pwm_code  = (unsigned int) ((LedCtrl.VISstate.brightness   &0xFF)<<5 );

    cmd_printf("VIS: pwm:%d seg4:%d seg3:%d seg2:%d seg1:%d en:%d\n", LedCtrl.VISstate.brightness, LedCtrl.VISstate.segment.seg4, LedCtrl.VISstate.segment.seg3, LedCtrl.VISstate.segment.seg2, LedCtrl.VISstate.segment.seg1, LedCtrl.VISstate.global);
    return (vis_en_code | vis_seg1_code | vis_seg2_code | vis_seg3_code | vis_seg4_code | vis_pwm_code);  
}

unsigned int _ir_lightcode_read(void)
{
    unsigned int ir_en_code   = (unsigned int) ((LedCtrl.IRstate.global       &0x1)<<0 );
    unsigned int ir_seg1_code = (unsigned int) ((LedCtrl.IRstate.segment.seg1 &0x1)<<1 ); 
    unsigned int ir_seg2_code = (unsigned int) ((LedCtrl.IRstate.segment.seg2 &0x1)<<2 );
    unsigned int ir_seg3_code = (unsigned int) ((LedCtrl.IRstate.segment.seg3 &0x1)<<3 );
    unsigned int ir_seg4_code = (unsigned int) ((LedCtrl.IRstate.segment.seg4 &0x1)<<4 );
    unsigned int ir_pwm_code  = (unsigned int) ((LedCtrl.IRstate.brightness   &0xFF)<<5 );

    cmd_printf("IR: pwm:%d seg4:%d seg3:%d seg2:%d seg1:%d en:%d\n", LedCtrl.IRstate.brightness, LedCtrl.IRstate.segment.seg4, LedCtrl.IRstate.segment.seg3, LedCtrl.IRstate.segment.seg2, LedCtrl.IRstate.segment.seg1, LedCtrl.IRstate.global);
    return (ir_en_code | ir_seg1_code | ir_seg2_code | ir_seg3_code | ir_seg4_code | ir_pwm_code);  
}

int cmd_lightcode(int argc, char *argv[])
{
    int ret; 
    unsigned int viscode, ircode;

    switch(argc){
        case 1:
            viscode = _vis_lightcode_read();
            ircode = _ir_lightcode_read();
            cmd_printf("<RST:%d %d:RST>\n", viscode, ircode);
            ret = E_OK;
            break;

        case 3:
            viscode = cmdStr2ul(argv[1], 0); 
            ircode  = cmdStr2ul(argv[2], 0);
            ret = _lightcode_write(viscode, ircode);
            if(ret!=E_OK){
                cmd_printf("Fail %d: vis=0x%X ir=0x%X\n", ret, viscode, ircode);
            } 
            else{
                cmd_printf("Success: vis=0x%X ir=0x%X\n", viscode, ircode);
            }
            break;

        default:
            cmd_printf("Usage: lightcode \n");
            cmd_printf("       lightcode <viscode> <ircode>\n");
            ret = E_UNVALID;
            break;
    }

    if(ret!=E_OK) { return RUN_FAIL;    }
    else          { return RUN_SUCCESS; }
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
