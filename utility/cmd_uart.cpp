/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
/*!
    @file cmd_uart.cpp
    \file Cmd.c

    This implements a simple command line interface for the Arduino so that
    its possible to execute individual functions within the sketch.
*/
/**************************************************************************/
/**************************************************************************
    Modified at 09.04.2018
    change prompt and don't show back underline character
***************************************************************************/
#include <avr/pgmspace.h>
#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include "HardwareSerial.h"
#include "cmd_uart.h"
#include "statuscode.h"

// command line message buffer and pointer
static uint8_t msg[MAX_MSG_SIZE];
static uint8_t *msg_ptr;

// linked list for command table
static cmd_t *cmd_tbl_list, *cmd_tbl;

// text strings for command prompt (stored in flash)
//const char cmd_banner[] PROGMEM = "*************** CMD *******************";
const char cmd_prompt[] PROGMEM = "led >> ";
const char cmd_unrecog[] PROGMEM = "Command not recognized.";

static Stream* stream;

/**************************************************************************/
/*!
    Generate the main command prompt
*/
/**************************************************************************/
void cmd_display()
{
    char buf[50];

    stream->println();    

//   strcpy_P(buf, cmd_banner);
//    stream->println(buf);

    strcpy_P(buf, cmd_prompt);
    stream->print(buf);
}

/**************************************************************************/
/*!
    Parse the command line. This function tokenizes the command input, then
    searches for the command table entry associated with the commmand. Once found,
    it will jump to the corresponding function.
*/
/**************************************************************************/
int cmd_parse(char *cmd)
{
    int ret = -1;

    uint8_t argc, i = 0;
    char *argv[30];
    char buf[50];
    cmd_t *cmd_entry;

    fflush(stdout);

    // parse the command line statement and break it up into space-delimited
    // strings. the array of strings will be saved in the argv array.
    argv[i] = strtok(cmd, " ");
    do
    {
        argv[++i] = strtok(NULL, " ");
    } while ((i < 30) && (argv[i] != NULL));

    // save off the number of arguments for the particular command.
    argc = i;

    // parse the command table for valid command. used argv[0] which is the
    // actual command name typed in at the prompt
    for (cmd_entry = cmd_tbl; cmd_entry != NULL; cmd_entry = cmd_entry->next)
    {
        if (!strcmp(argv[0], cmd_entry->cmd))
        {
//            stream->println();
            stream->println("<B:1:B>");
            ret = cmd_entry->func(argc, argv);
//            stream->println();
            stream->println("<B:0:B>");
            //cmd_display();
            return ret;
        }
    }

    // command not recognized. print message and re-generate prompt.
    strcpy_P(buf, cmd_unrecog);
    stream->println(buf);
    return RUN_FAIL;

    //cmd_display();
}

/**************************************************************************/
/*!
    This function processes the individual characters typed into the command
    prompt. It saves them off into the message buffer unless its a "backspace"
    or "enter" key.
*/
/**************************************************************************/
void cmd_handler()
{
    int ret = -1;
    char c = stream->read();

    switch (c)
    {
    case '.':
    case '\r':
        // terminate the msg and reset the msg ptr. then send
        // it to the handler for processing.
        *msg_ptr = '\0';
        stream->print("\r\n");
        ret = cmd_parse((char *)msg);
//        stream->println();
        if(ret == RUN_SUCCESS){  stream->println("<E:0:E>");  } 
        else { stream->println("<E:1:E>");  }
        msg_ptr = msg;
        break;

    case '\n':
        // ignore newline characters. they usually come in pairs
        // with the \r characters we use for newline detection.
        break;

    case '\b':
        // backspace
        stream->print(c);
        if (msg_ptr > msg)
        {
            msg_ptr--;
        }
        break;
        
    case 0x5F:
        // underline @NEW
        *msg_ptr++ = c;
        break;

    default:
        // normal character entered. add it to the buffer
        stream->print(c);
        *msg_ptr++ = c;
        break;
    }
}

/**************************************************************************/
/*!
    This function should be set inside the main loop. It needs to be called
    constantly to check if there is any available input at the command prompt.
*/
/**************************************************************************/
void cmdPoll()
{
    while (stream->available())
    {
        cmd_handler();
    }
}

/**************************************************************************/
/*!
    Initialize the command line interface. This sets the terminal speed and
    and initializes things.
*/
/**************************************************************************/
void cmdInit(Stream *str)
{
    stream = str;
    // init the msg ptr
    msg_ptr = msg;

    // init the command table
    cmd_tbl_list = NULL;

}

/**************************************************************************/
/*!
    Add a command to the command table. The commands should be added in
    at the setup() portion of the sketch.
*/
/**************************************************************************/
void cmdAdd(const char *name, int (*func)(int argc, char **argv))
{
    // alloc memory for command struct
    cmd_tbl = (cmd_t *)malloc(sizeof(cmd_t));

    // alloc memory for command name
    char *cmd_name = (char *)malloc(strlen(name)+1);

    // copy command name
    strcpy(cmd_name, name);

    // terminate the command name
    cmd_name[strlen(name)] = '\0';

    // fill out structure
    cmd_tbl->cmd = cmd_name;
    cmd_tbl->func = func;
    cmd_tbl->next = cmd_tbl_list;
    cmd_tbl_list = cmd_tbl;
}

/**************************************************************************/
/*!
    Get a pointer to the stream used by the interpreter. This allows
    commands to use the same communication channel as the interpreter
    without tracking it in the main program.
*/
/**************************************************************************/
Stream* cmdGetStream()
{
    return stream;
}

/* FUNCTION *****************************************************************
** Description:     send a formatted string to command UART
** parameters:      formatted string
**                  additional parameter
** Return value:    none
*****************************************************************************/
void cmd_printf(const char *format, ...)
{
    static char line[80];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(line, sizeof(line), format, args);
    va_end(args);
    for (char *p = &line[0]; *p; p++) {
        if (*p == '\n') {
            Serial.write('\r');
        }
        Serial.write(*p);
    }
    if (len >= sizeof(line))
        Serial.write('$');
}



/**************************************************************************/
/*!
    Convert a string to a number. The base must be specified, ie: "32" is a
    different value in base 10 (decimal) and base 16 (hexadecimal).
*/
/**************************************************************************/
uint32_t cmdStr2Num(const char *str, uint8_t base)
{
    return strtol(str, NULL, base);
}


unsigned long cmdStr2ul(const char *str, uint8_t base)
{
    return strtoul(str, NULL, base);
}
