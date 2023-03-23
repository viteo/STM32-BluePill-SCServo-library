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
│  SC15            │     │ │     │  BluePill │
│                  │     └┬┘     │           │
│             DATA ├──────┴───30─┤ PA9       │
│                  │             │           │
│                  │             │           │
│              GND ├──────┬──GND─┤ GND       │
└──────────────────┘      │      └───────────┘
                          │
                         ─┴─
```
 
Library itself is well-designed, but some modifications were required for use with LL.

## How to use

- Add as submodule or clone into your project folder.
- Configure in CubeMX (Cube configuration tool) USART as Half-Duplex (default servo baudrate is 1000000), enable USART Global Interrupt
- In code: 
   - Put `USART_HD_IRQHandler()` in your USARTx interrupt routine 
   - Call `USART_HD_PostInit()` after peripheral initialization
- Use

### Note

There is a variable in `SCS.c` called `End`, which determines endiannes conversion between servo and mcu. For STM32 it should be set to 1.

Source: [FeeTech SCS SDK](https://gitee.com/ftservo/SCServoSDK)
