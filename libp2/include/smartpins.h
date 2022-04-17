#ifndef _SMARTPINS_H
#define _SMARTPINS_H

#define P_TRUE_A            0x00
#define P_INVERT_A          0x80000000
#define P_LOCAL_A           0x00
#define P_PLUS1_A           0x10000000
#define P_PLUS2_A           0x20000000
#define P_PLUS3_A           0x30000000
#define P_OUTBIT_A          0x40000000
#define P_MINUS3_A          0x50000000
#define P_MINUS2_A          0x60000000
#define P_MINUS1_A          0x70000000
#define P_TRUE_B            0x00
#define P_INVERT_B          0x8000000
#define P_LOCAL_B           0x00
#define P_PLUS1_B           0x1000000
#define P_PLUS2_B           0x2000000
#define P_PLUS3_B           0x3000000
#define P_OUTBIT_B          0x4000000
#define P_MINUS3_B          0x5000000
#define P_MINUS2_B          0x6000000
#define P_MINUS1_B          0x7000000
#define P_PASS_AB           0x00
#define P_AND_AB            0x200000
#define P_OR_AB             0x400000
#define P_XOR_AB            0x600000
#define P_FILT0_AB          0x800000
#define P_FILT1_AB          0xa00000
#define P_FILT2_AB          0xc00000
#define P_FILT3_AB          0xe00000
#define P_LOGIC_A           0x00
#define P_LOGIC_A_FB        0x20000
#define P_LOGIC_B_FB        0x40000
#define P_SCHMITT_A         0x60000
#define P_SCHMITT_A_FB      0x80000
#define P_SCHMITT_B_FB      0xa0000
#define P_COMPARE_AB        0xc0000
#define P_COMPARE_AB_FB     0xe0000
#define P_ADC_GIO           0x100000
#define P_ADC_VIO           0x108000
#define P_ADC_FLOAT         0x110000
#define P_ADC_1X            0x118000
#define P_ADC_3X            0x120000
#define P_ADC_10X           0x128000
#define P_ADC_30X           0x130000
#define P_ADC_100X          0x138000
#define P_DAC_990R_3V       0x140000
#define P_DAC_600R_2V       0x150000
#define P_DAC_124R_3V       0x160000
#define P_DAC_75R_2V        0x170000
#define P_LEVEL_A           0x180000
#define P_LEVEL_A_FBN       0x1a0000
#define P_LEVEL_B_FBP       0x1c0000
#define P_LEVEL_B_FBN       0x1e0000
#define P_ASYNC_IO          0x00
#define P_SYNC_IO           0x10000
#define P_TRUE_IN           0x00
#define P_INVERT_IN         0x8000
#define P_TRUE_OUTPUT       0x00
#define P_INVERT_OUTPUT     0x4000
#define P_HIGH_FAST         0x00
#define P_HIGH_1K5          0x800
#define P_HIGH_15K          0x1000
#define P_HIGH_150K         0x1800
#define P_HIGH_1MA          0x2000
#define P_HIGH_100UA        0x2800
#define P_HIGH_10UA         0x3000
#define P_HIGH_FLOAT        0x3800
#define P_LOW_FAST          0x00
#define P_LOW_1K5           0x100
#define P_LOW_15K           0x200
#define P_LOW_150K          0x300
#define P_LOW_1MA           0x400
#define P_LOW_100UA         0x500
#define P_LOW_10UA          0x600
#define P_LOW_FLOAT         0x700
#define P_TT_00             0x00
#define P_TT_01             0x40
#define P_TT_10             0x80
#define P_TT_11             0xc0
#define P_OE                0x40
#define P_CHANNEL           0x40
#define P_BITDAC            0x80
#define P_NORMAL            0x00
#define P_REPOSITORY        0x02
#define P_DAC_NOISE         0x02
#define P_DAC_DITHER_RND    0x04
#define P_DAC_DITHER_PWM    0x06
#define P_PULSE             0x08
#define P_TRANSITION        0x0a
#define P_NCO_FREQ          0x0c
#define P_NCO_DUTY          0x0e
#define P_PWM_TRIANGLE      0x10
#define P_PWM_SAWTOOTH      0x12
#define P_PWM_SMPS          0x14
#define P_QUADRATURE        0x16
#define P_REG_UP            0x18
#define P_REG_UP_DOWN       0x1a
#define P_COUNT_RISES       0x1c
#define P_COUNT_HIGHS       0x1e
#define P_STATE_TICKS       0x20
#define P_HIGH_TICKS        0x22
#define P_EVENTS_TICKS      0x24
#define P_PERIODS_TICKS     0x26
#define P_PERIODS_HIGHS     0x28
#define P_COUNTER_TICKS     0x2a
#define P_COUNTER_HIGHS     0x2c
#define P_COUNTER_PERIODS   0x2e
#define P_ADC               0x30
#define P_ADC_EXT           0x32
#define P_ADC_SCOPE         0x34
#define P_USB_PAIR          0x36
#define P_SYNC_TX           0x38
#define P_SYNC_RX           0x3a
#define P_ASYNC_TX          0x3c
#define P_ASYNC_RX          0x3e

// #ifdef __cplusplus
// // declare a C++ class for smart pins (to be implemeented by libp2++)
// #include "propeller.h"

// /**
//  * Abstract class representing a smart pin. 
//  * 
//  * All of these are control classes, underlying pin control instructions can still be used if desired
//  */
// class SmartPin {
//     int pin;
//     int sp_mode;

// public:
//     SmartPin(int p);

//     /**
//      * Initialize the pin and release it from reset
//      */
//     virtual void init();

//     /**
//      * Hold the smart pin in reset
//      */
//     virtual void reset();

//     /**
//      * Set the A input mode (if applicable)
//      */
//     void set_a_input(int a);

//     /**
//      * Set the B input mode (if applicable)
//      */
//     void set_b_input(int b);

//     /**
//      * Set the A/B logic filter mode (if applicable)
//      */
//     void set_ab_logic(int logic);

//     /**
//      * Set the high drive mode (if applicable)
//      */
//     void set_high_drive(int h);

//     /**
//      * Set the low drive mode (if applicable)
//      */
//     void set_low_drive(int l);

//     /**
//      * Set input to be clocked (if applicable)
//      */
//     void set_clocked(int c);

//     /**
//      * Set the raw input to be inverted (if applicable)
//      */
//     void set_invert_input(int inv);

//     /**
//      * Set the output to be inverted (if applicable)
//      */
//     void set_invert_output(int out);

//     /**
//      * wrappers around internal instructions 
//      */
//     inline void outhi() {outh(pin);};
//     inline void outlo() {outl(pin);};
//     inline void dirhi() {dirh(pin);};
//     inline void dirlo() {dirl(pin);};
//     inline void drvhi() {drvh(pin);};
//     inline void drvlo() {drvl(pin);};
// };

// /**
//  * Simple pin mode (no smart pin is active)
//  */
// class SimplePin : public SmartPin {
// public:
//     SimplePin(int p);
//     void init() override;
// };

// class PulsePin : public SmartPin {
// public:
//     PulsePin(int p);
//     void init(int base_period, int compare); 
//     void pulse(int n);
// };

// class ADCPin : public SmartPin {
// public:
//     ADCPin(int p);
//     void init(int )

// };

/*
long repository
long repository
long repository
DAC noise
DAC 16-bit dither, noise
DAC 16-bit dither, PWM
pulse/cycle output
transition output
NCO frequency
NCO duty
PWM triangle
PWM sawtooth
PWM switch-mode power supply, V and I feedback
periodic/continuous: A-B quadrature encoder
periodic/continuous: inc on A-rise & B-high
periodic/continuous: inc on A-rise & B-high / dec on A-rise & B-low
periodic/continuous: inc on A-rise {/ dec on B-rise}
periodic/continuous: inc on A-high {/ dec on B-high}
time A-states
time A-highs
time X A-highs/rises/edges -or- timeout on X A-high/rise/edge
for X periods, count time
for X periods, count states
for periods in X+ clocks, count time
for periods in X+ clocks, count states
for periods in X+ clocks, count periods
ADC sample/filter/capture, internally clocked
ADC sample/filter/capture, externally clocked
ADC scope with trigger
USB host/device              (even/odd pin pair = DM/DP)*/

// #endif

#endif