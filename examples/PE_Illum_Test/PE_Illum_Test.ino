/* Control interface for the LED-ILLUM Board
 * Photon Energy GmbH
 * Version V1.0.0 @09.04.2018
 * Attention: Timer 4 is configurated to generate the Fast PWM,
 *            check the compability if using other Pins.
 */

#include <pe_illum.h>

//======================================================================//
void setup(){
    cmdInst();
}

void loop(){
    cmdPoll();           // Attention for some long process!
}
//======================================================================//