
esphome-pace-bms
-
This is an **ESPHome** component that supports "**paceic**" protocol **version 20 and 25** which is used by seemingly the majority of low-cost rack-mount Lithium Iron (LiFePO4) battery packs (but occasionally a different chemistry as well) manufactured in SE Asia.  The BMS can be communicated with over **RS485** or **RS232** and is manufactured by PACE (or is a clone).

Example PACE BMS board:
![Example PACE BMS board (front)](images/example-board-front.png)
![Example PACE BMS board (back)](images/example-board-back.png)

The protocol is characterized by both requests and responses beginning with a '**~**' (tilde) character followed by two ASCII numbers either "**20**" or "**25**" and ending with a '**\r**' (carriage return) character.

I strongly encourage you to read through this entire document, but here are some Quick Links:
- [What Battery Packs are supported?](fixme)
- [I just want an ESPHome config](fixme)
- [How do I wire my ESP to the RS485/RS232 port?](fixme)
- [I'm having a problem using this component](fixme)
- [I want to talk to a battery that isn't listed](fixme)

Paceic Protocol Version 20
-
  This protocol version is spoken by older battery packs and has several variants, with firmware customized by individual manufacturers.  Three protocol variants are currently known/supported:
 - **EG4**
 - **PYLON**
 - **SEPLOS**

Different manufacturers will have different BMS management software (it will not be PbmsTools) but typically it speaks a variant of paceic version 20.  These older BMSes will usually have two RS485 ports (looks like an ethernet socket) and may have an RS232 port (looks like a telephone socket).  They usually won't have a CAN bus port.

There is a high likelyhood that one of these protocol variants will work for battery packs speaking protocol version 20 which are branded by a different manufacturer than those listed, but if you can find a spec doc for a new variant that behaves differently, I can probably add support.  See [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/paceic/0x20) for documentation on currently known version 20 protocol variants. 

Example protocol version 20 BMS front-panel:
![EG4 Protocol 20 Front Panel](images/EG4-0x20.webp)

Paceic Protocol Version 25
-
This version seems more standardized, with an official protocol specification from PACE itself.  As far as I know, all newer battery packs speaking this protocol version should be supported.  See [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/paceic/0x25) for documentation on protocol version 25.

These BMSes speaking paceic version 25 will invariably use PbmsTools for their BMS management software (or a rebadged version of it) which looks like this:

![PbmsTools Screenshot](images/PbmsTools.jpg)

The exact look isn't important, just the tabs and general layout will look like this.  This is PbmsTools regardless of any specific brand badging and indicates that your BMS supports protocol version 25.

These BMSes will typically have two RS485 ports (looks like an ethernet socket) an RS232 port (looks like a telephone socket) and possibly a CAN bus port and an LCD display as well, especially if newer.

Example protocol version 25 BMS front-panel:
![Jakiper Protocol 25 Front Panel](images/Jakiper-0x25.png)

Pace MODBUS Protocol
-
Some BMS firmwares also support reading data via MODBUS protocol over the RS485 port.  I haven't looked into this yet.  It seems like it may co-exist with Paceic version 25.  Documentation can be found [here](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation/modbus)

Supported Values/Status (read only)
-
- All "Analog Information"
	- **Cell Count**
	- **Cell Voltage** (V) - up to x16 depending on your battery pack
	- **Temperature Count**
	- **Temperature** (°C) - up to x6 depending on your battery pack, typically this will be:
		-  *Cell Temperature* 1-4 
		- *MOSFET Temperature* 
		- *Environment Temperature*
	- **Total Voltage** (V)
	- **Current** (A) - positive or negative for charge/discharge
	- **Power** (W) - positive or negative for charge/discharge
	- **Remaining Capacity** (Ah)
	- **Full Capacity** (Ah)
	- **Design Capacity** (Ah)
	- **State of Charge** (%)
	- **State of Health** (%)
	- **Cycle Count**
	- **Minimum Cell Voltage** (V)
	- **Maximum Cell Voltage** (V)
	- **Average Cell Voltage** (V)
	- **Max Cell Differential** (V) - difference between minimum and maximum cell voltage
- All "Status Information" information decoded to human-readable text format
	- **Warning Text** - A list of any warnings reported by the BMS
	- **Protection Text** - If the BMS has protected itself or the batteries, for example disabling charging if the temperature is too low, or a cell voltage is too high, it will be listed here
	- **Fault Text** - A list of any faults reported by the BMS
	- **System Text** - Current system status such as "Charging"
	- **Configuration Text** - System configuration such as "Warning Buzzer Enabled"
	- **Balancing Text** - If any cells are currently balancing, they will be listed here
	- (individual status flag values) - These are what the text fields are decoded from, and are documented separately.  You probably won't need them, but they are available.  There are a lot of them and they vary by protocol version and variant.
(READ/WRITE)
- **Hardware Version** - The BMS hardware version (string)
- **Serial Number** - The BMS serial number (string)

Supported Configuration (read / write)
-
- **System Date and Time** - Allows access to the BMS internal real-time clock 
- **Shutdown** - A button which sends the shutdown command to the BMS

Supported Configuration (read / write) - **Version 25 ONLY**
-
It is difficult to find good documentation on either of these protocols.  All the references I have are incomplete.  For version 25 I was able to sniff the exchanges between PbmsTools and my battery pack in order to decode all of the commands necessary for setting these configuration values.  However, the only battery pack I own which speaks version 20, is sending some very strange non-paceic commands for configuration settings.  Unfortunately I was unable to decode those, and even if I did, I'm not sure if it would apply to all brands of battery pack speaking version 20.  For that reason, I didn't pursue it further, and these settings are only applicable to battery packs speaking version 25.

- Toggles (switches) that turn various features on/off
	- **Buzzer Alarm**
	- **LED Alarm**
	- **Charge Current Limiter**
	- **Charge MOSFET**
	- **Discharge MOSFET**
- Selects (drop-lists) that allow configuring various features
	- **Charge Current Limiter Gear** - set to High or Low
	- **Protocol (CAN)** - Allows selection of various protocols spoken on the CAN bus, typically to match your inverter
	- **Protocol (RS485)** - Allows selection of various protocols spoken on the RS485 bus, typically to match your inverter
	- **Protocol Type** - Auto or Manual
- Configuration
  - Cell Over Voltage
	- **Cell Over Voltage Alarm** (V)
	- **Cell Over Voltage Protection** (V)
	- **Cell Over Voltage Protection Release** (V)
	- **Cell Over Voltage Delay** (seconds)
  - Pack Over Voltage
	- **Pack Over Voltage Alarm** (V)
	- **Pack Over Voltage Protection** (V)
	- **Pack Over Voltage Protection Release** (V)
	- **Pack Over Voltage Delay** (seconds)
  - Cell Under Voltage
	- **Cell Under Voltage Alarm** (V)
	- **Cell Under Voltage Protection** (V)
	- **Cell Under Voltage Protection Release** (V)
	- **Cell Under Voltage Delay** (seconds)
  - Pack Under Voltage
	- **Pack Under Voltage Alarm** (V)
	- **Pack Under Voltage Protection** (V)
	- **Pack Under Voltage Protection Release** (V)
	- **Pack Under Voltage Delay** (seconds)
  - Discharge Over Current 1
	- **Discharge Over Current 1 Alarm** (V)
	- **Discharge Over Current 1 Protection** (V)
	- **Discharge Over Current 1 Delay** (seconds)
  - Discharge Over Current 2
	- **Discharge Over Current 2 Protection** (V)
	- **Discharge Over Current 2 Delay** (seconds)
  - Discharge Short Circuit
	- **Discharge Short Circuit Protection Delay** (milliseconds)
  - Cell Balancing
	- **Cell Balancing Threshold** (V)
	- **Cell Balancing Delta** (V)
  - Sleep
	- **Sleep Cell Voltage** (V)
	- **Sleep Delay** (minutes)
  - Full Charge
	- **Full Charge Voltage** (V)
	- **Full Charge Amps** (A)
  - Low Charge
	- **Low Charge Alarm** (%)
  - Charge Over Temperature
	- **Charge Over Temperature Alarm** (°C)
	- **Charge Over Temperature Protection** (°C)
	- **Charge Over Temperature Protection Release** (°C)
  - Discharge Over Temperature
	- **Discharge Over Temperature Alarm** (°C)
	- **Discharge Over Temperature Protection** (°C)
	- **Discharge Over Temperature Protection Release** (°C)
  - Charge Under Temperature
	- **Charge Under Temperature Alarm** (°C)
	- **Charge Under Temperature Protection** (°C)
	- **Charge Under Temperature Protection Release** (°C)
  - Discharge Under Temperature
	- **Discharge Under Temperature Alarm** (°C)
	- **Discharge Under Temperature Protection** (°C)
	- **Discharge Under Temperature Protection Release** (°C)
  - MOSFET Over Temperature
	- **MOSFET Over Temperature Alarm** (°C)
	- **MOSFET Over Temperature Protection** (°C)
	- **MOSFET Over Temperature Protection Release** (°C)
  - Environment Over Temperature
	- **Environment Over Temperature Alarm** (°C)
	- **Environment Over Temperature Protection** (°C)
	- **Environment Over Temperature Protection Release** (°C)
  - Environment Under Temperature
	- **Environment Under Temperature Alarm** (°C)
	- **Environment Under Temperature Protection** (°C)
	- **Environment Under Temperature Protection Release** (°C)

What Battery Packs are Supported?
- 
As far as I know, many/most.  Any not listed should simply require a small tweak to the configuration.  

However, I'd like to keep a full list here if only for search engine discoverability, so if you find that it does work with your battery pack, please contact me with the configuration tweaks required, the make/model of battery pack (and a link to the exact model on the manufacturer's website if possible), and what it reports for the hardware version.

For help figuring out how to do those configuration tweaks to get your battery pack working, see [here](FIXME)

**Known working protocol version 20 battery packs:**
- **EG4 LIFEPOWER4**
  - hardware versions: 
	  - **QTHN 0d[3][6]**
	    - ![EG4 LIFEPOWER4](images/EG4-0x20-320.png)
  - required config: 
	  - `protocol_commandset: 0x20`
	  - `protocol_variant: "EG4"`
	  - `battery_chemistry: 0x4A`
  - notes:
	  - The BMS is a bit slow, so don't reduce the timeouts too much. I have found the following settings prevent lockup from querying it too quickly:
		  -   `request_throttle: 200ms`
		  -   `response_timeout: 2000ms`


**Known working protocol version 25 battery packs:**
- **Jakiper JK48V100**
  - hardware versions: 
	  - **FIXME**
	    - ![EG4 LIFEPOWER4](images/Jakiper-0x25-320.png)
  - required config: 
	  - `protocol_commandset: 0x25`

What ESPs are Supported?
- 
Both ESP8266 and ESP32 are supported, though an ESP32 class device is recommended.  

Any board which gives you access to a hardware UART (both RX and TX) is fine.  Software UART on GPIO pins is not recommended.  

You cannot connect the UART RX/TX pins directly to either the RS232 or RS485 port, a converter chip for RS485 or RS232 signal levels is required.  Some boards may have such a converter chip built-in, or you can use a breakout board.  

RS485 will require at least one additional GPIO pin for flow control in addition to the UART RX and TX pins.  RS232 will require only the UART RX and TX.

If using an 8266, you will need to redirect serial logs to the second UART (which is TX only, but that's fine for logging).  An example config for that is included below in the YAML section.

How do I wire my ESP to the RS485 port?
- 
You will need a converter chip.  I have had success with the MAX485.  It's designed for 5v but I've had no issues using it at 3.3v with an ESP.  [Here](https://www.amazon.com/gp/product/B00NIOLNAG) is an example breakout board for the MAX485 chip.  You may be able to find ESP boards with such a chip already integrated.

![MAX485 Breakout Board](images/max485.jpg)

This example breakout separates out the flow control pins **DE** and **R̅E̅**, but they need to be tied together which you can do by either bridging the solder blobs on the back of the pins, or otherwise wiring both pins together.  

Connect the breakout board to the **ESP**:
* **DI** (driver input) **->** ESP UART **TX** 
* **RO** (receiver output) **->** ESP UART **RX**
* **DE** (Driver Output Enable) and **R̅E̅** (Receiver Output Enable, active low) -> any ESP **GPIO**, from here out referred to as "the **Flow Control** pin"

Connect the breakout board to the **BMS**:
* **A** aka **D+** -> pin / blade **7** (white with a brown stripe if using ethernet cabling)
* **B** aka **D-** -> pin / blade **8** (solid brown if using ethernet cabling)

![RJ-45 Socket and Connector with Pin Numbers and Color Codes](images/rj45.png)

Lastly, don't forget to connect power (3.3v) and ground to the breakout board.

How do I wire my ESP to the RS232 port?
- 
You will need a converter chip.  I have had success with the SP232.  It's compatible with the ESP 3.3v power and signaling levels.  [Here](https://www.amazon.com/gp/product/B091TN2ZPY) is an example breakout board for the SP232 chip.  You may be able to find ESP boards with such a chip already integrated.

![SP232 Breakout Board](images/sp232.jpg)

Connect the breakout board to the **ESP**:
* TTL **TXD** **->** ESP UART **RX** 
* TTL **RXD** **->** ESP UART **TX**

Connect the breakout board to the **BMS**:
* RS232 **TXD** -> pin / blade **?** (???)
* RS232 **RXD** -> pin / blade **?** (???)
* RS232 **GND** -> pin / blade **?** (???) (make sure you don't create a ground loop through the ESP power supply - if you don't know what that means, maybe use RS485 instead)

**DON'T TRUST THE COLOR CODES** in this diagram, telephone cables are "straight through" and colors will be "mirrored" between two ends of an extension cord.  Plus the colors aren't always standard.  Use the pin/blade numbering for wiring the proper connections.

![RJ-11 Socket and Connector with Pin Numbers and Color Codes](images/rj11.png)

Lastly, don't forget to connect power (3.3v) and ground to the breakout board.

Example ESPHome configuration YAML
-
A full ESPHome configuration will consist of thee parts:

1. The basic stuff like board type, wifi credentials, api or mqtt configuration, web_server if used, and so on. 
2. Configuration of the UART and the pace_bms component to speak with your battery pack.
3. Exposing the values / status / configuration that you want accessible via the web_server dashboard, homeassistant, or mqtt.

I won't go over 1 since that will be specific to your setup, except to say that if you want to use `web_server` then you should probably add `version: 3` and click the dark mode icon whenever you open it up because it is a *significant* improvement over version 2, but not yet set as the default.

First lets configure the UART and pace_bms component.

```yaml
uart:
    id: uart_0
    baud_rate: 9600 
    tx_pin: GPIO2
    rx_pin: GPIO1
    rx_buffer_size: 256

pace_bms:
  id: pace_bms_at_address_1
  address: 1
  uart_id: uart_0
  flow_control_pin: GPIO0 
  update_interval: 5s
  request_throttle: 200ms 
  response_timeout: 2000ms 

  protocol_commandset: 0x20
  protocol_variant: "EG4"
  protocol_version: 0x20 
  battery_chemistry: 0x4A 
```

Next, lets go over making things available to the web_server dashboard, homeassistant, or mqtt.  This is going to differ slightly 

Example 1: 


I'm having a problem using this component
- 


I want to talk to a battery that isn't listed
- 


Each Configuration Entry in Excruciating Detail
- 


Decoding the Status Values (but you probably don't want to)
- 

Miscellaneous Notes
- 
- My personal preference is for the [C# Style Guidelines](https://learn.microsoft.com/en-us/dotnet/standard/design-guidelines/) but the idea is to get this into ESPHome and [their guidelines](https://esphome.io/guides/contributing.html#codebase-standards) are different.  It's currently a bit of a mishmash until I can refactor over to ESPHome's style completely.

- Huge shout-out to https://github.com/syssi/esphome-seplos-bms who implemented an initial decode and compiled some documentation, and who shared a bit of his time on Discord, without which I might never have gotten started on, or been motivated to finish, this more complete implementation of the protocol.

Helping Out
- 
- I would like to make additions to the [known supported battery packs](fixme) section.  If you have a pack that works, please share!

- If you can locate any new [documentation](https://github.com/nkinnan/esphome-pace-bms/tree/main/protocol_documentation) on the protocol, particularly for version 20 variants, or if you find a variation on version 25 (I'm not aware of any at this time), please let me know!

- Want to contribute more directly? Found a bug? Submit a PR! Could be helpful to discuss it with me first if it's non-trivial design change, or adding a new variant.  You can also file an issue but I may not have time to follow up on it.

- And of course, if you appreciate the work that went into this, you can always [buy me a coffee](https://www.buymeacoffee.com/nkinnan) :)



