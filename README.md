# STM32 BluePill SCServo library
STM32 BluePill SCServo (Waveshare/FeeTech) SDK port for LL

Library uses STM32 USART Single Wire (Half-Duplex) and controls motor directly without TTLinker / SCM-1 / [FE-URT-1](https://feetechrc.com/FE-URT1-C001.html) / [Buffer](https://www.waveshare.com/w/upload/d/d3/Bus_servo_control_circuit.pdf) [Circuits](http://grobotronics.com/images/companies/1/datasheets/SCS15&SCS115%20Manual.pdf) etc. 

In USART Single Wire mode Pin is configured as open-drain and require external pull-up resitor (~500 Ohm) to 5V. USART Pin must be 5V tolerant (USART1 - PA9, USART3 - PB10, see 'FT' I/O level)

```
  ┌──────┐          6..8V        ┌──┬─────┬──┐
  └─┬──┬─┘            ▲          │  │ USB │  │
┌───┴──┴───────────┐  │          │  └─────┘  │
│                  │  │   ┌───5V─┤ 5V        │
│                  │  │  ┌┴┐     │           │
│              PWR ├──┘  │ │ 0.5 │           │
│  SC15            │     │ │ kΩ  │           │
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
