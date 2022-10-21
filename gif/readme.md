# Initialize LED

## use GPIO as INPUT

![img](https://github.com/kop123meter/ese5190-2022-lab2b-esp/blob/main/gif/197077044-de2b0232-0753-49f7-bfb0-079f3cda41a2.png)

![img](https://github.com/kop123meter/ese5190-2022-lab2b-esp/blob/main/gif/WechatIMG293.jpeg)

![gif](https://github.com/kop123meter/ese5190-2022-lab2b-esp/blob/main/gif/tutieshi_640x1137_1s.gif)

# LED can be controlled by RP2040

1.input is 'l', LED will be lighted.
2.input is 'd', LED will be darked.

![gif](https://github.com/kop123meter/ese5190-2022-lab2b-esp/blob/main/gif/tutieshi_640x360_10s.gif)

# Proposal

worked with HaoZe Wu and XuanBiao Zhu

## 1.Introduction
 
By using DHT11 Sensor and RP2040 and LED, we can realize a simple smart system.The input is the data which can be sensored by DHT11.With the input,Rp2040 can send related control signals to LED which can make users to control the temperature and so on.

## 2.PIN

we will use the Stemma Qt to send the data,use the a0 to control the voltage of LED circuit.

# Components

1.RP2040
2.LED
3.one 360 Ohm
4.DHT11 sensor
5.some wires
