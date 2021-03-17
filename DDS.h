#ifndef __NB_DDS
#define __NB_DDS

#include "nworkbench.h"

class nBlock_DDS: public nBlockSimpleNode<1> {
public:
    nBlock_DDS(PinName MOSI, PinName SCK, PinName pinSelect, uint32_t freqDefault);
    uint16_t cRegister = 0; 
    
    enum input{
        RESET  =    8,     
        B28    =   13, 
        TRIANGLE =  1,  
        SQUARE1  =  5,  
        SQUARE2  =  3,
        SINUS    =  0
    };
   
    uint16_t setBit(uint16_t reg, uint16_t bit);
    uint16_t clearBit(uint16_t reg, uint16_t bit);

    void write_SPI(uint16_t dat);
    void setFreq(uint32_t FREQ);
    void setFunction(uint32_t FUNCTION);

    void        triggerInput(nBlocks_Message message);
	void endFrame();

    uint32_t _frequency;
    uint32_t _function;
    uint16_t ccRegister = 0; 

    uint32_t    Position1;
    uint32_t    Position2;
    DigitalOut _fsync;

private:
    SPI _spi;

    uint32_t must_update = 0;
};

#endif
