## PE-Illum_V2.1.5 (2018-10-18)
* Change:
- Separate the real brightness and the wanted brightness. During global disabled
  state, real brightness stays at 0, lightcode will be hooked on the wanted brightness.  
- The brightness value is initated to 0%, the wanted brightness is initated to
  100%.

## PE-Illum_V2.1.4 (2018-10-11)
* Bug fix:
- LightCode will be set with only one dim down and dim up.
- "ledwr": Add segment safety checking if switching global enable from OFF to ON.

## PE-Illum_V2.1.3 (2018-10-10)
* Bug fix:
- Segment was changed one after another with dim. Segments is newly changed all
  together with only one time dim down and dim up.
- The 1% of brightness is changed to 5% of older brightness. 0% brightness
  corresponds to the real 0. 
- Synchronize the control state between control interface of "lightcode" and "ledwr".
* Change:
- The brightness value is set to 100% after hardware power-on. 
- Lightcode will be set with a sequence of: "global enable" -> "segments" ->
  "brightness".

## PE-Illum_V2.1.2 (2018-09-28)
* Add new command: 
   "lightcode": read out light codes for vis- and ir-leds.
   "lightcode <viscode> <ircode>": write light codes. 
* Light code format:
    bit[0]: global enable
	bit[1]: segment 1 enable
	bit[2]: segment 2 enable
	bit[3]: segment 3 enable
	bit[4]: segment 4 enable
	bit[5-11]: brightness percentage value
   

## PE-Illum_V2.1.1  (2018-09-24)
* Adapt to the nearly design: more IR-Leds, led driver changed. 
* Exchange all IR-LED control pins and VIS-LED control pins.
| Description           | Before   | New   |
| --------------------- |:--------:| -----:|
| PWR_IR_ENA            | PIN18    | PIN19 |
| IR_SEG1_UCENA         | PIN26    | PIN41 |
| IR_SEG2_UCENA         | PIN12    | PIN40 |
| IR_SEG3_UCENA         | PIN20    | PIN27 |
| IR_SEG4_UCENA         | PIN21    | PIN28 |
| PWM_IR                | PIN29    | PIN31 |

| PWR_WHITE_ENA         | PIN19    | PIN18 |
| WHITE_SEG1_UCENA      | PIN41    | PIN26 |
| WHITE_SEG2_UCENA      | PIN40    | PIN12 |
| WHITE_SEG3_UCENA      | PIN27    | PIN20 |
| WHITE_SEG4_UCENA      | PIN28    | PIN21 |
| PWM_WHITE             | PIN31    | PIN29 |

## PE_Illum_V2_1_0_PE_pullup
* Using Vendor-ID of Microchip, Production ID of Photon Energy.
* Unused pins with internal pull-up.
 

## PE_Illum_V2_0_3_Leon_Ramp_5Pin
* Add ramp-PWM during switch on/off segment.
* If the wanted operation for segment is same as before, nothing will be done.
* 5-Pins changed:
| Description           | Before   | New   |
| --------------------- |:--------:| -----:|
| IR_SEG1_UCENA/2.3A    | PIN30    | PIN26 |
| WHITE_SEG3_UCENA/3.2C | PIN39    | PIN27 |
| WHITE_SEG4_UCENA/3.2C | PIN38    | PIN28 |
| IR_SEG3_UCENA/2.3B    | PIN37    | PIN20 |
| IR_SEG4_UCENA/2.3C    | PIN36    | PIN21 |
* Using Vendor-ID of Arduino.
