This is the interface to control Led-Illumination Board, which based on ATmega32U4 chip.

## Discription
This is the firmware used to control led-illumination board. To be compatible
with the first two boards, brightness value will be treated separately during
global enable state and global disabled state. During global disabled state,
brightness is hooked on the wanted value, real brightness stays at 0%. During
next global enabling, brightness will ramp to the wanted value. This maybe a
little confused.

## Commandor

  * _getversion : read out the version information
  * _ledwr <id> <value> : interface to enable and set the brightness of visible- and infrarot-led
  * _ledrd <id> : read out the current setting
  * _lightcode <viscode> <ircode> : control the vis- and ir-leds using lightcode
  * _lightcode  : read out lightcode of led states 

The value of brightness is set per percentage, it should be within 0 - 100.

## Compatibility

The Timer4 is used to generate fast PWM with 187kHz.

## Files & Configuration

  * Install Arduino IDE from https://www.arduino.cc/en/Main/Software take notice of the installation path.
  * Extract the "PE-Illumination.zip" file to the Arduino IDE library fold, which under the installation path (for example "C:\Program Files (x86)\Arduino\libraries").
  * Open Arduino IDE, open the tab "Datei -> Beispiele -> PE-IllumBoard -> PE_Illum_Test".
  * Open the tab "Werkzeuge -> Board -> Arduino Leonardo" to select the board, and set the corresponding COM-Port per "Werkzeuge -> Port".
  * Click the button "Überprüfen", if successfully finished, click the button "Hochladen", and wait for finishing.

## Error diagonse
   0    - succeed
  -1    - common failure 
  -2    - processing break
  -3    - validation failed    
  -4    - out of range
  -5    - unvalid 
  -6    - error because of safety
  -7    - not existed
