/*!
 * @file pe_illum.h
 * Arduino Library to control LED_ILLUM Board
 * To achive the high pwm frequency, we use timer 4
 * Version 2.0
 * Copyright @ 2018 Photon Energy GmbH 
 * Autor: junchao.tang@photon-energy.de
 */
#ifndef PE_ILLUM_H
#define PE_ILLUM_H

#include <utility/illum.h> 
#include <utility/cmd_uart.h>
#include <utility/statuscode.h>

typedef struct _cmd_tbl_s
{
    char *cmd;
//    unsigned char cmd_max_args;
//    unsigned char viewable_cmd;
    int (*run_cmd)(int, char**);
    char *help;
}cmd_tbl_t;

#define SHOW_CMD      1
#define HIDE_CMD      0

/****************** Function decleration **********/

int led_read(int id, int addr, int* value);

int led_write(int id, int addr, int value);

int _vis_write(int addr, int value);

int _ir_write(int addr, int value);

int _vis_read(int addr, int* value);

int _ir_read(int addr, int* value);

void _error_log_read(int id, int addr, int ret);

void _error_log_write(int id, int addr, int ret);

void _error_msg(int ret);

/*
 * Command to read version
 */
int cmd_getversion(int argc, char *argv[]);

int cmd_lightwr(int argc, char *argv[]);

int cmd_lightrd(int argc, char *argv[]);

/*
 * Command interface for reading operation
 */
int cmd_ledread(int argc, char *argv[]);

/*
 * Command interface for writing operation
 */
int cmd_ledwrite(int argc, char *argv[]);

int cmd_lightcode(int argc, char *argv[]);

/*
 *  Instantiation of the available commands
 */
void cmdInst(void);

#endif
