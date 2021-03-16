#ifndef __NB_DDS
#define __NB_DDS

#include "nworkbench.h"

class nBlock_DDS: public nBlockNode {
public:
    nBlock_DDS(uint32_t THECLOCK, uint32_t freq);
    uint32_t outputAvailable(uint32_t outputNumber);
    uint32_t readOutput(uint32_t outputNumber);
    void triggerInput(uint32_t inputNumber, uint32_t value);
    void step(void);
private:
    fifo internal_fifo;
    uint32_t input_0;
    uint32_t system_clock;
};

#endif
