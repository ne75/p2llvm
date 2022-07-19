#pragma once

#include <pins.h>
#include <propeller.h>
#include <assert.h>
#include <stdint.h>

/**
 * Abstract class representing a smart pin. 
 * 
 * All of these are control classes, underlying pin control instructions can still be used if desired
 */
class SmartPin {
protected:
    int sp_mode;
    int r = 0, x = 0, y = 0; // local copies of r, x, and y pin registers since we can't read them directly

    void set_mode(int sp_mode) {
        r &= ~(0b11111 << 1);
        r |= sp_mode;
        wrpin(r, pin);
    }

public:
    const int pin;
    
    enum InputMode {
        TRUE = 0b0000,
        INVERT = 0b1000,
    };

    enum InputSource {
        PIN = 0b000,
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

    enum OutControl {
        OUTBIT = 0,
        OTHER = 1
    };

    /**
     * @param p: the pin to control
     */
    SmartPin(int p) : pin(p) {};

    /**
     * Initialize the pin and release it from reset
     */
    virtual void init() {};

    /**
     * Hold the smart pin in reset
     */
    virtual void reset() {
        dirl(pin);
    };

    /**
     * Set the A input mode and source, if applicable
     * 
     * @param source: The source for this pin's A input
     * @param mode: The input mode for this pin's A input
     */
    void a_input(InputSource source, InputMode mode = TRUE) {
        r &= ~(0b1111 << 28);
        r |= (mode | source) << 28;
        wrpin(r, pin);
    }

    /**
     * Set the A input to a specific pin number
     * 
     * @param p: The source for this pin's A input. Must be within 3 of this pin. 
     * @param m: The input mode
     */
    void a_input_pin(int p, InputMode m = TRUE) {
        int a_delta = p - pin;
        assert(a_delta < 4 && a_delta > -4 && "desired A input pin is too far from this smart pin");

        a_input((InputSource)(a_delta & 0b111), m);
    }

    /**
     * Set the B input mode and source, if applicable
     *
     * @param source: The source for this pin's B input
     * @param mode: The input mode for this pin's B input
     */
    void b_input(InputSource source, InputMode mode = TRUE) {
        r &= ~(0b1111 << 24);
        r |= (mode | source) << 24;
        wrpin(r, pin);
    }

    /**
     * Set the B input to a specific pin number
     * 
     * @param p: The source for this pin's B input. Must be within 3 of this pin. 
     * @param m: The input mode
     */
    void b_input_pin(int p, InputMode m = TRUE) {
        int b_delta = p - pin;
        assert(b_delta < 4 || b_delta > -4 && "desired B input pin is too far from this smart pin");

        b_input(InputSource(b_delta & 0b111), m);
    }

    /**
     * Set the A/B logic filter mode (if applicable)
     * 
     * @param: The logic/filtering applied to the A input (or both in the case of filtering)
     */
    void ab_logic(InputLogic logic) {
        r &= ~(0b111 << 21);
        r |= logic << 21;
        wrpin(r, pin);
    }

    /**
     * Set the high drive mode (if applicable)
     * 
     * @param h: The high drive mode
     */
    void high_drive(DriveMode h) {
        r &= ~(0b111 << 11);
        r |= h << 11;
        wrpin(r, pin);
    }

    /**
     * Set the low drive mode (if applicable)
     * 
     * @param l: The low drive mode 
     */
    void low_drive(DriveMode l) {
        r &= ~(0b111 << 8);
        r |= l << 8;
        wrpin(r, pin);
    }

    /**
     * Set input to be clocked (if applicable)
     * 
     * @param c: Should the input be clocked or not
     */
    void clocked(int c) {
        r &= 1 << 16;
        r |= c << 16;
        wrpin(r, pin);
    }

    /**
     * Set the raw input to be inverted (if applicable)
     * 
     * @param inv_in: Should the IN state be inverted (after the logic)
     */
    void invert_input(int inv_in) {
        r &= 1 << 15;
        r |= inv_in << 15;
        wrpin(r, pin);
    }

    /**
     * Set the output to be inverted (if applicable)
     * 
     * @param inv_out: Should the OUT state be inverted (after the pin control)
     */
    void invert_output(int inv_out) {
        r &= 1 << 14;
        r |= inv_out << 14;
        wrpin(r, pin);
    }

    /**
     * Set up the pin control. 
     * 
     * @param out: Output mode (OUT or OTHER, Smart mode will override)
     * @param enabled: Output is enabled, regardless of DIR bit
     * 
     */
    void pin_control(OutControl out, bool enabled) {
        r &= ~(0b11 << 6);
        r |= (out << 7) | (enabled << 6);
        wrpin(r, pin);
    }

    /**
     * Get the value of the pin's IN bit (0 or 1)
     */
    int get_in() {
        int v;
        testp(pin, v);
        return v;
    }

    /**
     * wrappers around internal instructions 
     */
    inline void outhi() {outh(pin);}
    inline void outlo() {outl(pin);}
    inline void dirhi() {dirh(pin);}
    inline void dirlo() {dirl(pin);}
    inline void drvhi() {drvh(pin);}
    inline void drvlo() {drvl(pin);}
    inline void ack() {akpin(pin);}
};

class PulsePin : public SmartPin {
public:
    PulsePin(int p) : SmartPin(p) {};

    /**
     * Initialize a pin in Pulse mode.
     * 
     * @param base_period: The 16 bit base period that the smart pin will operate at
     * @param compare: The value the pin's counter will be compared to to determine it's high/low state
     */
    void init(int base_period, int compare) {
        dirl(pin);

        set_mode(P_PULSE);
        pin_control(OutControl::OUTBIT, true);
        x = (base_period & 0xffff) | ((compare & 0xffff) << 16);
        wxpin(x, pin);

        dirh(pin);
    }

    /**
     * Pulse the pin a given number of times. Uses event 4
     * 
     * @param n: number of times to pulse
     * @param wait: wait for pulses to send
     */
    void pulse(int n, bool wait=false) {
        setse4(E_IN_RISE | pin);
        y = n;
        wypin(y, pin);
        if (wait) {
            waitse4();
        }
    }
};

class PWMTrianglePin : public SmartPin {
    int frame_period = 0;
    int base_period = 0;

public:
    PWMTrianglePin(int p) : SmartPin(p) {};

    void init(int frame_period, int base_period) {
        this->frame_period = frame_period;
        this->base_period = base_period;

        dirl(pin);

        set_mode(P_PWM_TRIANGLE);
        pin_control(OUTBIT, true);
        
        x = (frame_period << 16) | base_period;
        wxpin(x, pin);
        
        dirh(pin);
    }

    void sync() {
        ack();
        setse4(E_IN_RISE | pin);
        waitse4();
    }

    void set_dc(int dc) {
        y = dc;
        wypin(y, pin);
    }
};

/**
 * Quadrature mode
 */
class QuadraturePin : public SmartPin {
    int bpin = -1;
public:
    QuadraturePin(int p) : SmartPin(p) {};

    void init(SmartPin bpin, int period = 0) {
        dirl(pin);

        this->bpin = bpin.pin;

        b_input_pin(bpin.pin);
        set_mode(P_QUADRATURE);
        
        wxpin(period, pin);

        dirh(pin);
        dirl(bpin.pin); // float the B input
    }

    int count() {
        int v;
        rdpin(v, pin);
        return v;
    }

};

/**
 * ADC Mode
 */
class ADCPin : public SmartPin {
    int sample_ticks = 0;

public:
    int vio = 0;
    int gio = 0;

    enum ADCMode {
        SINC2_SAMPLING = 0b00,
        SINC2_FILTERING = 0b01,
        SINC3_FILTERING = 0b10,
        BITSTREAM_CAPTURE = 0b11
    };

    enum ADCSourceMode {
        GIO = 0b100000,
        VIO = 0b100001,
        FLOAT = 0b100010,
        PIN_1X = 0b100011,
        PIN_3_16X = 0b100100,
        PIN_10X = 0b100101,
        PIN_31_6X = 0b100110,
        PIN_100X = 0b100111
    };

    int bits = 0;

    ADCPin(int p) : SmartPin(p) {};

    void init(int sample_period, ADCMode mode, ADCSourceMode source_mode) {
        dirl(pin);

        sample_ticks = sample_period;
        int sp = _encod(sample_period);
        int ones = _ones(sample_period);

        assert(ones == 1 && "Invalid sample period, must be a power of 2");
        assert(sample_period <= 32768 && "Invalid sample period, must be less than 32768");

        r = P_ADC | (source_mode << 15);

        wrpin(r, pin);
        wxpin(sp | (mode << 4), pin);

        switch (mode) {
            case SINC2_SAMPLING:
                bits = sp + 1;
                break;
            case SINC2_FILTERING:
                assert(false && "sinc2 filter not implemented\n");
                break;
            case SINC3_FILTERING:
                assert(false && "sinc3 filter not implemented\n");
                break;
            case BITSTREAM_CAPTURE:
                assert(false && "bitstream capture not implemented\n");
                break;
        }

        dirh(pin);

        setse4(E_IN_RISE | pin);
        akpin(pin);

        // switch to VIO mode
        wrpin(P_ADC | (VIO << 15), pin);
        for (int i = 0; i < 3; i++) {
            waitse4();
            rdpin(vio, pin);
        }

        // switch to GIO mode
        wrpin(P_ADC | (GIO << 15), pin);
        for (int i = 0; i < 3; i++) {
            waitse4();
            rdpin(gio, pin);
        }

        wrpin(r, pin);
    }

    /**
     * Get an ADC sample. Uses the calling cog's event 4.
     * 
     * @returns the raw sample
     */
    unsigned int raw_sample() {
        setse4(E_IN_HIGH | pin);
        waitse4();

        unsigned int s;
        rdpin(s, pin);
        return s;
    }

    /**
     * Get a precise ADC sample by measuring VIO and GIO to calibrate.
     */
    unsigned int sample() {
        setse4(E_IN_RISE | pin);
        akpin(pin);

        int s = 0;

        // get the sample
        rdpin(s, pin);

        int result = ((s - gio) << bits)/(vio-gio);

        return result;
    }

    int calibrate_sample(unsigned int s) {
        return ((s - gio) << bits)/(vio-gio);
    }
};

class SyncTXPin : public SmartPin {
    int bits;
public:
    enum Mode {
        CONTINUOUS = 0,
        START_STOP = 1
    };

    enum MSBMode {
        LSB_FIRST = 0,
        MSB_FIRST = 1
    };

    SyncTXPin(int p) : SmartPin(p) {}

    void init(SmartPin &clk, Mode m, int bits, bool invert=false) {
        dirl(pin);

        set_mode(P_SYNC_TX);

        if (invert)
            b_input_pin(clk.pin, INVERT);
        else 
            b_input_pin(clk.pin);

        pin_control(OutControl::OUTBIT, true);

        x &= ~(0b111111);
        x |= (m << 5) | ((bits - 1) & 0b11111);
        wxpin(x, pin);

        this->bits = bits;

        dirh(pin);
    }

    void tx(int word, MSBMode msb_mode=LSB_FIRST) {
        if (msb_mode == MSB_FIRST) word = _rev(word << (32-bits));
        wypin(word, pin);
    }
};

class SyncRXPin : public SmartPin {
    int bits;
public:
    enum Mode {
        PRE_EDGE = 0,
        ON_EDGE = 1
    };

    enum MSBMode {
        LSB_FIRST = 0,
        MSB_FIRST = 1
    };

    SyncRXPin(int p) : SmartPin(p) {}

    void init(SmartPin &clk, Mode m, int bits, bool invert=false) {
        dirl(pin);

        set_mode(P_SYNC_RX);

        if (invert)
            b_input_pin(clk.pin, INVERT);
        else
            b_input_pin(clk.pin);

        x &= ~(0b111111);
        x |= (m << 5) | ((bits - 1) & 0b11111);
        wxpin(x, pin);

        this->bits = bits;

        dirh(pin);
    }

    int rx(MSBMode msb_mode=LSB_FIRST) {
        uint32_t v;
        rdpin(v, pin);
        
        if (msb_mode == MSB_FIRST) v = _rev(v) & ~(0xffffffff << bits);
        else v = v >> (32-bits);

        return v;
    }
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