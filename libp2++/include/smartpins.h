#pragma once

#include <pins.h>
#include <propeller.h>

/**
 * Abstract class representing a smart pin. 
 * 
 * All of these are control classes, underlying pin control instructions can still be used if desired
 */
class SmartPin {
    int pin;
    int sp_mode;
    int r, x, y; // local copies of r, x, and y pin registers since we can't read them directly

public:
    /*
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
    */
    enum InputMode {
        TRUE = 0b0000,
        INVERT = 0b1000,
    };

    enum InputSource {
        LOCAL = 0b000,
        PLUS1 = 0b001,
        PLUS2 = 0b010,
        PLUS3 = 0b011,
        OUT = 0b100,
        MINUS3 = 0b101,
        MINUS2 = 0b110,
        MINUS1 = 0b111
    };

    enum InputLogic {
        A_B = 0b000,
        A_AND_B = 0b001,
        A_OR_B = 0b010,
        A_XOR_B = 0b011,
        FILT0 = 0b100,
        FILT1 = 0b101,
        FILT2 = 0b110,
        FILT3 = 0b111
    };

    enum DriveMode {
        FAST = 0b000,
        RES_1K5 = 0b001,
        RES_15K = 0b010,
        RES_150K = 0b011,
        CURRENT_1MA = 0b100,
        CURRENT_100UA = 0b101,
        CURRENT_10UA = 0b110,
        FLOAT = 0b111
    };  

    SmartPin(int p);

    /**
     * Initialize the pin and release it from reset
     */
    virtual void init();

    /**
     * Hold the smart pin in reset
     */
    virtual void reset();

    /**
     * Set the A input mode and source, if applicable
     */
    void set_a_input(InputSource source, InputMode mode) {
        r &= ~(0b1111 << 28);
        r |= (mode | source) << 28;
        wrpin(r, pin);
    }

    /**
     * Set the B input mode and source, if applicable
     */
    void set_b_input(InputSource source, InputMode mode) {
        r &= ~(0b1111 << 24);
        r |= (mode | source) << 24;
        wrpin(r, pin);
    }

    /**
     * Set the A/B logic filter mode (if applicable)
     */
    void set_ab_logic(InputLogic logic) {
        r &= ~(0b111 << 21);
        r |= logic << 21;
        wrpin(r, pin);
    }

    /**
     * Set the high drive mode (if applicable)
     */
    void set_high_drive(DriveMode h) {
        r &= ~(0b111 << 11);
        r |= h << 11;
        wrpin(r, pin);
    }

    /**
     * Set the low drive mode (if applicable)
     */
    void set_low_drive(DriveMode l) {
        r &= ~(0b111 << 8);
        r |= l << 8;
        wrpin(r, pin);
    }

    /**
     * Set input to be clocked (if applicable)
     */
    void set_clocked(int c);

    /**
     * Set the raw input to be inverted (if applicable)
     */
    void set_invert_input(int inv);

    /**
     * Set the output to be inverted (if applicable)
     */
    void set_invert_output(int out);

    /**
     * wrappers around internal instructions 
     */
    inline void outhi() {outh(pin);};
    inline void outlo() {outl(pin);};
    inline void dirhi() {dirh(pin);};
    inline void dirlo() {dirl(pin);};
    inline void drvhi() {drvh(pin);};
    inline void drvlo() {drvl(pin);};
};

/**
 * Simple pin mode (no smart pin is active)
 */
class SimplePin : public SmartPin {
public:
    SimplePin(int p);
    void init() override;
};

class PulsePin : public SmartPin {
public:
    PulsePin(int p);
    void init(int base_period, int compare); 
    void pulse(int n);
};

class ADCPin : public SmartPin {
public:
    ADCPin(int p);
    void init(int )

};

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