# STM32 Configuration options for the GIGA-R1 board

## CORTEX_M7 Mode and Configuration

### Speculation default mode Settings

(ref PM0253 section 2.3)
- Speculation default mode: enabled

### Cortex Interface Settings

- CPU ICache: disabled
- CPU DCache: disabled

### Cortex Memory Protection Unit Control Settings
(ref PM0253 section 4)

- MPU Control Mode: Background Region Privileged accesses only + MPU disabled during hard fault, NMI and FAULTMASK handlers

### Cortex Memory Protection Unit Region 0 Settings

- MPU Region: Enabled
- MPU Region Base Address: 0x0
- MPU Region Size: 4GB
- MPU SubRegion Disable: 0x87
- MPU TEX field level: level 0
- MPU Access Permission: ALL_ACCESS_NOT_PERMITTED
- MPU Instruction Access: disable
- MPU Shareability Permission: enable
- MPU Cacheability Permission: disable
- MPU Bufferable Permission: disable

### Cortex Memory Protection Unit Region [1-15]: disabled

- all regions are disabled, and when enabled they present the same options as the Region 0 does


## RCC Mode and Configuration

### Mode

*Runtime Contexts*
- Cortex-M7: enabled
- Cortex-M4: enabled
- Power Domain: D3

*Settings*
- High Speed Clock (HSE): Crystal/Ceramic Resonator
- Low Speed Clock (LSE): Crystal/Ceramic Resonator
- Master Clock Output 1: disabled
- Master Clock Output 2: disabled
- Audio Clock Input (I2S_CKIN): disabled
- CRS_SYNC: disable

### Configuration

#### Parameter Settings

*Power Parameters* (ref AN5215 section 2.2.1 Voltage Regulator (LDO) Table 1)
- SupplySource: PWR_LDO_SUPPLY
- Power Regulator Voltage Scale: Power Regulator Voltage Scale 3

*RCC Parameters* (ref AN2867 section 3.6, RM0399 section 9)
- TIM Prescaler Selection: Disabled
- HSE Startup Timout Value (ms): 100
- LSE Startup Timout Value (ms): 5000
- CSI Calibration Value: 32
- HSI Calibration Value: 64

*System Parameters* (ref RM0399 section 4, ds12930 section 6.3.1)
- VDD voltage (V): 3.3V
- Flash Latency (WS): 1 WS (2 CPU Cycle)
- Product Revision: rev.V

#### User Constants

*none*

#### NVIC (Nested Vectored Interrupt Controller) Settings

*NVIC1 Interrupt Table* (ref RM0399 section 9)
- RCC Global Interrupt: disabled

## USB_OTG_FS

*Mode*
- Mode: Device_Only

## Notes

- The RCC Global Interrupt is worth inspecting when we are building the real-time motion controller
- When considering the firmware, we need to make a map of the memory space. the MPU will allow us 
    to control who has what kind of access to specific regions, and allow us to specify a particular
    region for shared memory between the devices.
    This is particularly relevant for sharing data between the M7 and M4 cores.
    and may become relevant for fault containment when considering running specific modules of 
    our firmware
    an important note, the MPU configuration for the M7 and M4 cores will have to be independantly
    setup to agree with one another.
- The systick interrupt is commonly used for the HALs millisecond time base
    This can be a point to consider, as HAL functions which depend on the tick may not 
    be safe to call from a higher priority interrupt if they wait for the tick to advance.
    this could be relevant for USB or FreeRTOS
- it may be worth updating the hard fault handler with debug information capture,
    stacked program counter, link register, fault status registers, etc.
- the M4 has strict timing requirement, we need to ensure that interrupts are configured such that 
    we can meet those requirements. The USB handling interrupt is one such interrupt.
    a Global Interrupt Priority policy is worth creation for this project.


