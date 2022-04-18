#include <propeller.h>
#include <smartpins.h>
#include <stdio.h>

int main() {
    printf("$\n");
    SmartPin led0(56);
    SmartPin led1(57);

    ADCPin adc(0);
    adc.init(1024, ADCPin::SINC2_SAMPLING, ADCPin::PIN_1X);

    led1.invert_output(true);

    while(1) {
        led0.drvlo();
        led1.drvlo();
        waitx(CLKFREQ/5);
        led0.drvhi();
        led1.drvhi();
        waitx(CLKFREQ/5);

        int s = adc.sample();
        printf("sample: %d\n", s);
    }

    printf("~\n");

    busywait();
}