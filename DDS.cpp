#include "DDS.h"                                       // for RESERVE pin21(P2_5) as PWM1[6], ATTENTION: this style brakes the nblocksStudio common practice
// DDS
nBlock_DDS::nBlock_DDS(uint32_t THECLOCK, uint32_t freq){     // Constructor, One Parameter: THECLOCK of the LPC1768
    uint32_t system_clock;
    int32_t divider;// = 96000;
    system_clock = THECLOCK;
    divider = system_clock/freq;
PwmOut fmclck(P2_5); 
    LPC_PWM1->TCR = (1 << 1);               				// 1)Reset counter, disable PWM
    LPC_SC->PCLKSEL0 &= ~(0x3 << 12);  
    LPC_SC->PCLKSEL0 |= (1 << 12);          				// 2)Set peripheral clock divider to /1, i.e. system clock
    LPC_PWM1->MR0 = divider - 1;            				// 3)Match Register 0 is shared period counter for all PWM1
    LPC_PWM1->MR6 = (divider + 1)>> 1;      				// divider=96000 for 1KHz when inputs are not connected
    LPC_PWM1->LER |= 1;                  					       
    LPC_PWM1->TCR = (1 << 0) || (1 << 3);                   // 5)Enable counter and PWM 

    return;
}
uint32_t nBlock_DDS::outputAvailable(uint32_t outputNumber) { //Check for available Carrots
    return internal_fifo.available();
}
uint32_t nBlock_DDS::readOutput(uint32_t outputNumber) {  // Pass the Carrot to the Next
    uint32_t tmp;
    internal_fifo.read(&tmp);
    return tmp;
}
void nBlock_DDS::triggerInput(uint32_t inputNumber, uint32_t value) // Scan the inputs and prepare Carrot
    {   	
	uint32_t outFrequency;
    int32_t divider;
    PwmOut fmclck(P2_5); 
    if (inputNumber == 0){                                      // "value" contains the Frequency
	    divider = system_clock/value;
	    outFrequency = system_clock/divider;
	    LPC_PWM1->TCR = (1 << 1);               				// 1)Reset counter, disable PWM
        LPC_SC->PCLKSEL0 &= ~(0x3 << 12);  
        LPC_SC->PCLKSEL0 |= (1 << 12);          				// 2)Set peripheral clock divider to /1, i.e. system clock
        LPC_PWM1->MR0 = divider - 1;            				// 3)Match Register 0 is shared period counter for all PWM1
        LPC_PWM1->MR6 = (divider + 1)>> 1;      				// 
        LPC_PWM1->LER |= 1;                     			    // 4)Start updating at next period start                       
    }
    if(inputNumber ==1){                                        // "value" contains the ENABLE
        if(value > 0 ) {LPC_PWM1->TCR = (1 << 0) || (1 << 3); } // 5)Enable counter and PWM 
	    if(value == 0) {LPC_PWM1->TCR = (1 << 1);             } // 1)Reset counter, disable PWM 
    } 
    internal_fifo.put(outFrequency);                            // pass calculated outFrequency to fifo	              									 
}
void nBlock_DDS::step(void) {
    uint32_t tmp;
    internal_fifo.get(&tmp);
    return;
}
