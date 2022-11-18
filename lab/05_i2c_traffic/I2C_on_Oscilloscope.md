# I2C on Oscilloscope
We check The I2C pins' waveforms on the oscilloscope. Just confirm that I2C communication is active (taking place) between the QT Py (KB2040 and sensor (APDS-9960). This can be done by flashinging the 'firefly' code on to the QT Py, which in ture is connected to the sensor (APDS-9960).
We connect the first probe to SCL (CLK or clock) while the associated black wire to ground of the microcontroller. Similarly, the second probe is connected to SDA while the associated black wire to ground.
___
| ![I2C_1.jpg](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/ESE5190_Lab2B_I2C_1.png) |
|:--:|
| ![I2C_2.jpg](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/ESE5190_Lab2B_I2C_2.png) |
| ![I2C_2.jpg](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/ESE5190_Lab2B_I2C_3.png) |

## Some tips
If you are getting noisy output,
- Use the the pins on the board directly instead of the Stemma 4-pin cconnectors
- Use actual scope probes (if not using the Agilent Keysight ones, ensure that the probe is set up properly. For instance, if the probe is at 10:1, then switch on the probe and set 10:1 in the oscilloscope for the corresponding channel), not the BNC to mini grabbers
___
| ![ScopeProbes_2.jpg](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/ScopeProbes_2.jpg) |
|:--:|
| <b>Agilent Keysight Probes</b>|
| ![ScopeProbes_nonKeysight.jpg](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/ScopeProbe_nonKeysight.jpg) |
| <b>Not Agilent Keysight Probes</b>|
___
- Lower Intensity to 50%
- Change Trigger to I2C (or Falling Edge)
- Make sure that Ground is connected ;P
- Alternatively use logic analyser:
![LogicAnalyser.jpg](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/LogicAnalyser.jpg)
  - Select Digital on the Oscilloscope
  - Select D0 (instead of CLK) and D1 (instead of DATA0) in the oscilloscope
  - Connect SCL pin to D0 and SDA pin to D1 of Logic Analyser cable probe
  ![LogicAnalyser_Pins.jpg](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/LogicAnalyser_Pins.jpg)

### Snapshots of the oscilloscope screen
To take snapshots of the oscilloscope screen, use **Agilent Intuilink Data Capture**
- log in to the same station's desktop computer
- From the start Menu, select _Agilent Intuilink_ > _Run Intuilink Data Capture_ \
___
| ![AgIntDtCap_1](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/AgIntDtCap_1.png) |
|:--:|
| ![AgIntDtCap_2](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/AgIntDtCap_2.png) |
- _Intruments_ > _Agilent 7000 Series_ > _Find Instrument_ > Then select the address that appears > _Identify Instruments_ > _Select Oscilloscope_ > _OK_ > _Get waveform Data_
___
| ![AgIntDtCap_3](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/AgIntDtCap_3.png) |
|:--:|
- Ensure that '_Get Screen Image_' and '_Use Current Acquisition_' are checked and nothing else is checked > _OK_
___
| ![AgIntDtCap_4](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/AgIntDtCap_4.png) |
|:--:|
| ![AgIntDtCap_5](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/AgIntDtCap_5.png) |
- Then save the image; good job.
___
| ![AgIntDtCap_6](https://github.com/Aurunima/ESE5190-Fall22-Lab2B/blob/main/img/AgIntDtCap_6.png) |
|:--:|
