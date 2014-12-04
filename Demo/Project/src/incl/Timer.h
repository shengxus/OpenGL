#ifndef TIMER_H
#define TIMER_H

class Timer {
private:
    int frame;
    float timeBase;
    char s[30];
public:
    Timer():frame(0),timeBase(0){};
    void showTime(int, int);
};

#endif

