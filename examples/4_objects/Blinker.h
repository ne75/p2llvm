#ifndef _BLINKER_H
#define _BLINKER_H

class Blinker {

    char pin;
    int delay;

public:
    Blinker(char pin, int delay);

    void start();

    static void blink(void *par);
};

#endif