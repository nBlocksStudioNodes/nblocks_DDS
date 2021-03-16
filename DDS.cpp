#include "DDS.h"                                        // for RESERVE pin21(P2_5) as PWM1[6], ATTENTION: this style brakes the nblocksStudio common practice
// DDS
nBlock_DDS::nBlock_DDS(PinName MOSI, PinName SCK, PinName pinSelect, uint32_t freqDefault):
    spi(MOSI, NC, SCK), 
    _fsync(pinSelect) {     

    _spi.format(16,2);               
    _spi._frequency(1000000);         
    _fsync = 1;
    _frequency = freqDefault; 
    _function = SINUS;       
}

void nBlock_DDS::triggerInput(uint32_t inputNumber, uint32_t value) // Scan the inputs and prepare Carrot
    {   	
    PwmOut fmclck(P2_5); 
    if (inputNumber == 0){
        _frequency = message.intValue;
        Position1 = 1;              
    }
    if(inputNumber ==1){
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
        setFunction(function);
		Position2 = 0;
		
	}	
}


void setFreq(uint32_t FREQ) {       ** TO UPDATE **
    int32_t freq_MSB;   //define freq MSB reg value
    int32_t freq_LSB;   //define freq LSB reg value
    uint32_t freq_cal;  //define freq calculated value
    float freq_val = 0.00000000;    //define ferq calculate tempotary value
    freq_val = (((float)(FREQ))/25000000);
    freq_cal = freq_val*0x10000000;
    freq_MSB = (int)((freq_cal & 0xFFFC000)>>14); // shift 14 bits
    freq_LSB = (int)(freq_cal & 0x3FFF);
    freq_MSB = freq_MSB | 0x4000;  //assign freq reg address (D14=1)
    freq_LSB = freq_LSB | 0x4000;
    printf("freq_MSB 0x%x freq_LSB 0x%x\n\r", freq_MSB, freq_LSB);

    cRegister = setBit(cRegister,  B28);
    cRegister = setBit(cRegister,  RESET);
    // write_SPI(0x2100);      //write control reg - apply reset | D13 (B28) =1 | D8(RESET) =1
    printf("cRegister 0x%x\n\r", cRegister);
    write_SPI(cRegister); 
    write_SPI(freq_LSB);    //write freq reg - LSB
    write_SPI(freq_MSB);    //write freq reg - MSB
    write_SPI(0xC000);      //write phase reg - 0 for now  PHASE0: address=CC, content=000
    //cRegister = setBit(cRegister,  TRIANGLE);
    cRegister = setBit(cRegister,  SQUARE1);
    cRegister = setBit(cRegister,  SQUARE2);
    cRegister = clearBit(cRegister,  RESET);
    write_SPI(cRegister);
    //write_SPI(TRIANGLE_WAVE);      //write control reg - disable reset
}

void setFunction(uint32_t FUNCTION) { // *********** to UPDATE THIS FOR FUNCTION ONLY *************
    int32_t freq_MSB;   //define freq MSB reg value
    int32_t freq_LSB;   //define freq LSB reg value
    uint32_t freq_cal;  //define freq calculated value
    float freq_val = 0.00000000;    //define ferq calculate tempotary value
    freq_val = (((float)(FREQ))/25000000);
    freq_cal = freq_val*0x10000000;
    freq_MSB = (int)((freq_cal & 0xFFFC000)>>14); // shift 14 bits
    freq_LSB = (int)(freq_cal & 0x3FFF);
    freq_MSB = freq_MSB | 0x4000;  //assign freq reg address (D14=1)
    freq_LSB = freq_LSB | 0x4000;
    printf("freq_MSB 0x%x freq_LSB 0x%x\n\r", freq_MSB, freq_LSB);

    cRegister = setBit(cRegister,  B28);
    cRegister = setBit(cRegister,  RESET);
    // write_SPI(0x2100);      //write control reg - apply reset | D13 (B28) =1 | D8(RESET) =1
    printf("cRegister 0x%x\n\r", cRegister);
    write_SPI(cRegister); 
    write_SPI(freq_LSB);    //write freq reg - LSB
    write_SPI(freq_MSB);    //write freq reg - MSB
    write_SPI(0xC000);      //write phase reg - 0 for now  PHASE0: address=CC, content=000
    //cRegister = setBit(cRegister,  TRIANGLE);
    cRegister = setBit(cRegister,  SQUARE1);
    cRegister = setBit(cRegister,  SQUARE2);
    cRegister = clearBit(cRegister,  RESET);
    write_SPI(cRegister);
    //write_SPI(TRIANGLE_WAVE);      //write control reg - disable reset
}

void write_SPI(uint16_t dat) {
    fSync = 0;
    AD9833_SPI.write(dat);
    fSync = 1;
}

uint16_t setBit(uint16_t reg, uint16_t bit){
    reg |= 1UL << bit;
    return reg;
}

uint16_t clearBit(uint16_t reg, uint16_t bit){
    reg &= ~(1UL << bit);
    return reg;
}
