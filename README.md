# PIC32-EXB

Replace a dead computer on a magnetic resistance exercise bike, new design implemented with a PIC32MX270F256B microcontroller.

TODO: everything.




## 1. ORIGINAL COMPONENTS

Microcontroller (Unknown - covered with a blob of resin).

LCD Display and backlight (perspex sheet with six white LEDs in one end).

Heart rate monitor add-on board.

EEPROM ???

L911 H Bridge motor controller IC.

2 x 3.3v regulators.

DC Motor and gearhead.

Reed switch for cadence measurement.

Control console keypad.

2 x 5 Kohm potentiometers for motor position sensing.



### Resistance Control:

The pedal resistance is controlled by a set of magnets on a lever arm just in front of the steel rear wheel.
The distance between the magnets and the wheel rim is adjusted by a DC motor attached via a cable to the
lever arm. The closer the magnets get to the wheel rim the greater the resistance. The user adjusts the resistance
by pressing UP/DOWN buttons on the control console or selecting a programmed routine. 
The controller provides 10 levels of resistance from very easy to incredibly hard.


### Pedal Crank RPM Measurement:

The cadence (pedal speed) is measured by a reed switch attached to the frame just above the crank axle ("spindle").
The reed switch is normally open, it is closed once every pedal revolution by a magnet attached to the chainwheel
(front sprocket). The reed switch is connected between the microcontroller and ground with a Vdd pull up resistor 
to maintain 3.3V at the input pin until the magnet closes the switch and the voltage at the microcontroller input 
pin goes to ground.


### Heart Rate Monitor:

A small add-on board uses a bio-impedance circuit to measure heart rate via two metal pads on the handle bars. 
Both hands must be on the pads to get a HRM signal.
(https://en.wikipedia.org/wiki/Impedance_cardiography)


### LCD Display:

Custom LCD mounted on the main board with two elastomeric rubber strip conductors. 
(https://en.wikipedia.org/wiki/Elastomeric_connector)


### Console Keypad:

7 Push Buttons on control console.

  - Start/Stop.
  - Increase Resistance.
  - Decrease Resistance.
  - Reset.
  - Select Program.
  - Up.
  - Down.



## 2. NEW DESIGN

Parts List:

PIC32MX270F256B microcontroller.

Microbridge PIC32 programmer with USB-Serial interface and 3.3V supply voltage regulator.
(http://geoffg.net/microbridge.html)

Open Smart 2.4" TFT LCD Touch screen.

LED Pulse Sensor.

Laser ToF Proximity Sensor.

L9110 H-Bridge Motor Driver.

Protoyping vero board.

2 x 100nF ceramic capacitors.

1 x 10uF Tantalum capacitor.

1 x 47uF Electrolytic capacitor.

2 x LEDs.

2 x 330ohm resistors.

7 x 10kohm resistors.

DC - DC Switch Mode Buck Regulator.

3 x Push Button Momentary Switches.


### Circuit Diagram:


!["CIRCUIT"](https://github.com/dchad/PIC32-EXB/blob/master/resources/exercisebike.png "Circuit Prototype")


!["CIRCUIT"](https://github.com/dchad/PIC32-EXB/blob/master/resources/lcd-test-sm.jpg "Circuit Prototype")

    Testing LCD screen and pulse sensor.

!["CIRCUIT"](https://github.com/dchad/PIC32-EXB/blob/master/resources/motor-test-sm.jpg "Circuit Prototype")

    Testing motor and gearhead assembly.

!["CIRCUIT"](https://github.com/dchad/PIC32-EXB/blob/master/resources/motor-prox-sm.jpg "Circuit Prototype")

    Testing VL6180X proximity sensor mounted on the magnet lever arm assembly.
    
    

### Speed and Distance Calculation:


### Rear Wheel/Front Sprocket ratio.


## 3. ACKNOWLEDGEMENTS


## 4. TECHNICAL NOTES

(LED HRM sensor http://www.ni.com/white-paper/14248/en/)

(LED HRM sensor https://www.sparkfun.com/products/11574)




