# SAGE

## SAGE ELECTRONICS

- The main SAGE electronics consist of a Red Pitaya FPGA/linux embedded controller, a custom IO PCB, a custom photodiode detector PCB, and a BME280 PCB.
- The Red Pitaya's main fuction is to provide the laser modulation signal and acquire the photodiode signal.
- The custom IO PCB has the following functions:
  - Power regulation for input voltage (usually 24V), 12V, 5V.  All are reverse protected to 40V.  Overcurrent protected. Can take input 12.1-50V.
  - Power switching on 8 channels.  PWM voltage control for each channel.
  - Two i2c pressure sensors.
  - Supplies power to laser and Red Pitaya.
  - Serial communication to Red Pitaya.
  - ESP32 microcontroller - onboard ADCs read thermistors, one onbaord DAC controls laser temperature, one onboard DAC can modulate laser (not preferred), DIO switches power channels, talks to PWM controllers for power modulation (16 on one chip, only using 8), gets time from clock, interfaces with SD Card.
-  The photodiode board contains a photodiode and a dual stage amplifier.  Amplifier resistance and capacitance can be tuned to adjust gain and filtering.
-  A custom PCB for the BME280 contains only a BME 280 on a small circular board.
     
## Custom PCBs

### [SAGE_IO PCB 17-0010](https://github.com/williamapardis/SAGE_IO/tree/main/electrical/SAGE/17-%200010%20ESP32-S3%20Control%20IO)
<table>
  <tr>
    <td>Full View PCB</td>
     <td>Zoomed View</td>
  </tr>
  <tr>
    <td><img src="https://user-images.githubusercontent.com/57682790/235982042-21e1d6ae-0467-47f3-bfc1-7cfede07f680.png"></td>
    <td><img src="https://user-images.githubusercontent.com/57682790/235982125-0cb705ef-d757-4d2d-b897-8445e63818fc.png"></td>
  </tr>
</table>

#### Load Control Voltage Selection
![image](https://github.com/williamapardis/SAGE/assets/57682790/8bb1f7cd-f956-4e1b-88b2-b1ade272ba36)

### [SAGE Detect PCB](https://github.com/williamapardis/SAGE/tree/main/electrical/SAGE/17-0001%20SAGE%20DETECT) 
<table>
  <tr>
    <td>Full View PCB</td>
     <td>Zoomed View</td>
  </tr>
  <tr>
    <td><img src="https://github.com/williamapardis/SAGE/assets/57682790/577d950b-95b4-4467-8439-4f323bca229a" width="425"></td>
    <td><img src="https://github.com/williamapardis/SAGE/assets/57682790/3dcccdbc-766d-4914-8538-4b8c2ae8657d" width="425"></td>
  </tr>
</table>


### RP Adaptor PCB
<table>
  <tr>
    <td>Full View PCB</td>
     <td>Zoomed View</td>
  </tr>
  <tr>
    <td><img src="https://github.com/williamapardis/SAGE/assets/57682790/d10c15d2-64a0-404d-9eed-91261adfe727" width="425"></td>
    <td><img src="https://github.com/williamapardis/SAGE/assets/57682790/f8461d56-a2dc-4e97-90d8-e00c208fbd5b" width="425"></td>
  </tr>
</table>

### BME280 PCB
<table>
  <tr>
    <td>Full View PCB</td>
     <td>Zoomed View</td>
  </tr>
  <tr>
    <td><img src="https://github.com/williamapardis/SAGE/assets/57682790/3d6d8114-18a5-40a4-a5c2-5ebca11d9b07" width="425"></td>
    <td><img src="https://github.com/williamapardis/SAGE/assets/57682790/5ea38db0-ee3a-4cc6-90c7-b6d0ec8001c7" width="425"></td>
  </tr>
</table>

## Software

### Set Commands

#### Set Load -> SL#:$$ 
#  = 1-8 FETS
$$ = 0-1 %

#### Set Time (two options)
  sync NTP (current servers: "time.nist.gov", "0.pool.ntp.org", "1.pool.ntp.org")
    SN
  manually
    ST:YYYYMMDD:hhmmss
    
####
    
## Get Commands

