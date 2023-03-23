# STM32 BluePill SCServo library
STM32 BluePill SCServo (Waveshare/FeeTech) SDK port for LL

Library uses STM32 USART Single Wire (Half-Duplex) and controls motor directly without TTLinker / SCM-1 / [FE-URT-1](https://feetechrc.com/FE-URT1-C001.html) / [Buffer](https://www.waveshare.com/w/upload/d/d3/Bus_servo_control_circuit.pdf) [Circuits](http://grobotronics.com/images/companies/1/datasheets/SCS15&SCS115%20Manual.pdf) etc. 

In USART Single Wire mode Pin is configured as open-drain and require external pull-up resitor (~500..1000 Ohm) to 3.3V.

```
  ┌──────┐          6..8V        ┌──┬─────┬──┐
  └─┬──┬─┘            ▲          │  │ USB │  │
┌───┴──┴───────────┐  │          │  └─────┘  │
│                  │  │   ┌───3V─┤ 3.3V      │
│                  │  │  ┌┴┐     │           │
│              PWR ├──┘  │ │ 1kΩ │           │
│  SC15            │     │ │     │           │
│                  │     └┬┘     │           │
│             DATA ├──────┴───30─┤ PA9       │
│                  │             │           │
│                  │             │           │
│              GND ├──────┬──GND─┤ GND       │
└──────────────────┘      │      └───────────┘
                          │        BluePill
                         ─┴─
```
 
Library itself is well-designed, but some modifications were required for use with LL.

Source: [FeeTech SCS SDK](https://gitee.com/ftservo/SCServoSDK)
