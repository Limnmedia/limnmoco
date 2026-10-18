# STM32H747 Clock Frequency, Voltage Scaling, and Power Tradeoffs

**Project:** Arduino GIGA R1 WiFi firmware (STM32H747XIH6, Cortex-M7 + Cortex-M4)  
**Recorded:** 2026-10-17  
**Status:** Future optimization note; **not** a decision to lower the current bring-up clock.

## Decision to revisit

Use the conventional **480 MHz Cortex-M7 / 240 MHz Cortex-M4** operating point for initial bring-up. Once the firmware is functioning, benchmark lower frequencies (especially **400 MHz M7 / 200 MHz M4**) to avoid consuming more power than necessary to meet worst-case timing requirements. Consider USB functionality, motion-control deadlines, CPU load, interrupt latency, and thermal behavior when comparing configurations.

## Why frequency alone does not predict power

A first-order CMOS dynamic power model is

\[
P_{\mathrm{dynamic}} \approx \alpha C V_{\mathrm{core}}^2 f
\]

where \(\alpha\) is switching activity, \(C\) is switched capacitance, \(V_{\mathrm{core}}\) is logic supply voltage, and \(f\) is clock frequency. Increasing frequency from 400 to 480 MHz is a **20% frequency increase**, assuming the same switching activity per cycle. But this comparison also changes the STM32H747's required voltage scale:

| M7 / M4 clocks | LDO voltage scale | Typical internal VCORE | Maximum permitted M7 clock |
|---|---|---:|---:|
| 400 / 200 MHz | VOS1 | 1.20 V | 400 MHz |
| 480 / 240 MHz | VOS0 | 1.35 V | 480 MHz |

These nominal VCORE figures are documented hardware characteristics, not an assumed percentage. The theoretical dynamic-power ratio if switching capacitance/activity per cycle were identical is

\[
\frac{P_{480}}{P_{400}} \approx \frac{480}{400} \left(\frac{1.35}{1.20}\right)^2 \approx 1.52.
\]

**This ~52% is a simplified logic dynamic-power estimate, not the measured MCU supply-current ratio.** Leakage, regulator behavior, memories, buses, and peripherals complicate the actual power draw. [1, 2]

## Datasheet evidence: supply current in Run mode

STM32H747xI/G datasheet **DS12930 Rev 3, Table 24** (some earlier datasheet revisions number this table **Table 34**): code and data processing from Flash, **both cores running**, cache and ART accelerator enabled, internal LDO regulator on, typical junction temperature **25°C**. [1]

| Peripheral state | 400/200 MHz, VOS1 | 480/240 MHz, VOS0 | Relative increase |
|---|---:|---:|---:|
| All peripherals disabled | 128 mA | 173 mA | **35.2%** |
| All peripherals enabled | 178 mA | 242 mA | **36.0%** |

Calculations:

- Peripherals disabled: \((173/128 - 1) \times 100 = 35.16\%\).
- Peripherals enabled: \((242/178 - 1) \times 100 = 35.96\%\).

A useful additional control point from that same table is **400/200 MHz at VOS0: 147 mA** with all peripherals disabled, compared to **128 mA at 400/200 MHz at VOS1**. Holding clock frequencies constant while changing voltage scale therefore increases measured current by **14.8%** in that specific benchmark. [1]

**Interpretation:** Running at 480/240 MHz with VOS0 consumes roughly **35–36% more typical MCU supply current per unit time** than 400/200 MHz with VOS1 for the documented conditions. This does **not** directly measure whole-board GIGA consumption or an individual application workload. It also does not assert that all peripherals can literally be simultaneously active in a real application.

## Energy to complete a fixed amount of work

If a computation consumes the same number of clock cycles at both operating points and scales ideally with frequency, then execution time at 480 MHz is \(400/480=0.8333\) times that at 400 MHz, or **16.7% shorter**.

For the **peripherals-disabled** datasheet example, assuming the supply voltage used to calculate energy is unchanged:

\[
\frac{E_{480}}{E_{400}} = \frac{I_{480}}{I_{400}} \frac{t_{480}}{t_{400}}
= \frac{173}{128}\frac{400}{480}\approx 1.126.
\]

Under these assumptions, the faster setting spends **~12.6% more energy on that fixed-cycle computation**, despite finishing sooner. For the peripherals-enabled values, the analogous calculation is \((242/178)(400/480)\approx1.133\), or **~13.3% more energy**.

**Caveats:** Actual execution may not scale linearly due to memory stalls, instruction cache behavior, bus contention, flash wait states, or DMA. A full system may enter an idle/sleep state sooner when work completes faster, making total energy different from this simplified run-mode estimate. The M4 frequency also changes from 200 to 240 MHz in the cited measurements.

## Engineering follow-up

1. Bring the application up at **480/240 MHz, VOS0**, with appropriate power regulator, Flash latency, and APB/AHB prescalers.
2. Record **worst-case execution time**, not just average time, for the M4's stepper-control timing loop and any time-critical interrupt handlers.
3. Measure utilization and latency for M7 USB command handling and any background processing.
4. Reconfigure and benchmark **400/200 MHz, VOS1**, preserving valid clock trees and peripheral kernel clocks (notably USB's required clock).
5. Measure actual **board supply current** under representative idle, nominal, and peak workloads. Use an appropriate external meter/power analyzer; datasheet IDD is a reference, not a board measurement.
6. Select the lowest-power configuration that still leaves sufficient worst-case timing and thermal margin. Consider independently optimizing idle behavior and peripheral clocks.

## References

1. STMicroelectronics, **STM32H747xI/G datasheet, DS12930 Rev 3**. See *Electrical characteristics*: **Table 24**, Run-mode current with both cores running, caches enabled and LDO on; also the *General operating conditions* VCORE/VOS frequency limits. https://www.st.com/resource/en/datasheet/stm32h747zi.pdf
2. STMicroelectronics, **AN5215 — How to implement advanced power management on STM32H747/757 MCUs**, Section 2.2.1, *Voltage regulator (LDO)*, Table 1 (VOS0 1.35 V / 480 MHz; VOS1 1.2 V / 400 MHz). https://www.st.com.cn/resource/en/application_note/an5215-how-to-implement-advanced-power-management-on-stm32h747757-mcus-stmicroelectronics.pdf
3. STMicroelectronics, **DS12930 Rev 2**, same run-mode figures in **Table 34**; useful if comparing older PDFs. https://www.st.com/resource/en/datasheet/DM00602212.pdf

> **Reminder:** Datasheet figures describe specified device characterization conditions, not an empirical measurement of this Arduino GIGA firmware or of complete-board power. Revalidate against the exact chip revision, power wiring, and operating conditions during optimization.
