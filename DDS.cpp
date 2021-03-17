#include "DDS.h"                                        // for RESERVE pin21(P2_5) as PWM1[6], ATTENTION: this style brakes the nblocksStudio common practice
// DDS
nBlock_DDS::nBlock_DDS(PinName MOSI, PinName SCK, PinName pinSelect, uint32_t freqDefault):
    _spi(MOSI, NC, SCK), 
    _fsync(pinSelect){     

   
    _spi.format(16,2);               
    _spi.frequency(1000000);         
    _fsync = 1;
    _frequency = freqDefault; 
    _function = SINUS; 
    setFreq(_frequency);      
}

void nBlock_DDS::triggerInput(nBlocks_Message message){ 	
    PwmOut fmclck(P2_5); 
    if (message.inputNumber == 0){
        _frequency = message.intValue;
        Position1 = 1;              
    }
    if(message.inputNumber ==1){
        _function = message.intValue;
        Position2 = 1;
    }                  									 
}

void nBlock_DDS::endFrame(void){    
	if (Position1) {
        setFreq(_frequency);
		Position1 = 0;
		
    }
	if (Position2) {
        setFunction(_function);
		Position2 = 0;
		
	}	
}


void nBlock_DDS::setFreq(uint32_t FREQ) {       //** TO UPDATE **
    int32_t freq_MSB;   //define freq MSB reg value
    int32_t freq_LSB;   //define freq LSB reg value
    uint32_t freq_cal;  //define freq calculated value
    float freq_val = 0.00000000;    //define freq calculate temp value
    uint16_t ccRegister=0;

    freq_val = (((float)(FREQ))/25000000);
    freq_cal = freq_val*0x10000000;
    freq_MSB = (int)((freq_cal & 0xFFFC000)>>14); // shift 14 bits
    freq_LSB = (int)(freq_cal & 0x3FFF);
    freq_MSB = freq_MSB | 0x4000;  //assign freq reg address (D14=1)
    freq_LSB = freq_LSB | 0x4000;

    ccRegister = setBit(ccRegister,  B28);
    ccRegister = setBit(ccRegister,  RST);
    write_SPI(ccRegister); 
    write_SPI(freq_LSB);    //write freq reg - LSB
    write_SPI(freq_MSB);    //write freq reg - MSB
    write_SPI(0xC000);      //write phase reg - 0 for now  PHASE0: address=CC, content=000
    ccRegister = clearBit(ccRegister,  SQUARE1);
    ccRegister = clearBit(ccRegister,  SQUARE2);
    ccRegister = clearBit(ccRegister,  TRIANGLE);
    write_SPI(ccRegister);
    ccRegister = clearBit(ccRegister,  RST);
    write_SPI(ccRegister);
}

void nBlock_DDS::setFunction(uint32_t FUNCTION) { 
    ccRegister = setBit(ccRegister,  RST);
    write_SPI(ccRegister);
    if(_function ==1) ccRegister = setBit(ccRegister,  SINUS); 
    if(_function ==2) {
        ccRegister = setBit(ccRegister,  SQUARE1);
        ccRegister = setBit(ccRegister,  SQUARE2);
    }
    if(_function ==3) ccRegister = setBit(ccRegister,  TRIANGLE);
    ccRegister = clearBit(ccRegister,  RST);
    write_SPI(ccRegister);   
}

void nBlock_DDS::write_SPI(uint16_t dat) {
    _fsync = 0;
    _spi.write(dat);
    _fsync = 1;
}

uint16_t nBlock_DDS::setBit(uint16_t reg, uint16_t bit){
    reg |= 1UL << bit;
    return reg;
}

uint16_t nBlock_DDS::clearBit(uint16_t reg, uint16_t bit){
    reg &= ~(1UL << bit);
    return reg;
}
