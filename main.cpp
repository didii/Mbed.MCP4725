#define MCP4728EXAMPLEVER "1.10                    "

#include <string>
#include "mbed.h"

#if   defined (TARGET_KL25Z) || defined (TARGET_KL46Z)
  PinName const SDA = PTE25;
  PinName const SCL = PTE24;
#elif defined (TARGET_KL05Z)
  PinName const SDA = PTB4;
  PinName const SCL = PTB3;
#elif defined (TARGET_K20D50M)
  PinName const SDA = PTB1;
  PinName const SCL = PTB0;
#else
  #error TARGET NOT DEFINED
#endif


#include "I2C.h"
#include "I2CInterface.h" 
#include "MBEDI2CInterface.h" 
#include "DACInterface.h" 
#include "dev_interface_def.h"
#include "mcp4725.h"
#include "Translator.h"
#include "Reader.h"

// Initialize LEDs
PwmOut rled(LED1);
PwmOut gled(LED2);
PwmOut bled(LED3);

//Serial pc(USBTX, USBRX);

// Sends the message to the pc
bool Send(Translator::MessageInfo info);
void SendError();
void SendError(int address);
void SendError(char* data, int dataSize);
void SendError(int address, char* data, int dataSize);
void SendError(Translator::MessageInfo info);

// Turns LED blue
void SignalWaiting();
// Turns LED green
void SignalReceived();
// Turns LED red
void SignalError();

int main(void) {
    // <SDA en SCL zijn de pins hierboven gedefinieerd, wss hardware eigenschappen dat je mag negeren>
    // Create MbedI2C interface
    MBEDI2CInterface mbedi2c(SDA, SCL);
    // <volgende lijn is voor indien de Mbed veranderd, enkel bovenstaande lijn geupdate moet worden>
    // Create pointer to MBEDI2CInterface's parent
    I2CInterface* i2cdev= &mbedi2c;
    // Set maximum voltage of I2C board
    float Vdd = 4.7;
    // Create communication object using the MbedI2C interface
    MCP4725 dac(i2cdev ,0, Vdd);
    
    // Create input buffer reader
    Reader reader;
    
    // Start infinite loop
    while (true) {
        SignalWaiting();
        if (!reader.IsNewMessageReceived())
            continue; // Do nothing as long as no new complete message was received
        SignalReceived();
        
        
        
        // Get the formatted message
        Translator::MessageInfo info = reader.GetLastMessageInfo();
        
        // If write was specified
        if (info.IsWrite()) {
            // Issue write
            if (i2cdev->write(info.Address, info.Data, Translator::Rules::WriteDataSize, false) != 0) {
            //if (dac.setDACvalue((int)(unsigned char)info.Data[1]<<4+(int)(unsigned char)info.Data[2]>>4, 0) != 0) {
                SendError(info);
                continue;
            }
            // Issue also a read
            //if (i2cdev->read(info.Address, info.Data, Translator::Rules::ReadDataSize, false) != 0) {
            //    //SendError(info);
            //    continue;
            //}
            if (!Send(info)) {
                SendError(info);
            }
        }
        // If read was specified
        else if (info.IsRead()) {
            // read from DAC
            if (i2cdev->read(info.Address, info.Data, sizeof(info.Data)/sizeof(info.Data[0]), false) != 0) {
                SendError(info);
                continue;
            }
            // send read values to pc
            if (!Send(info))
                SendError();
        }
        // If error was specified
        else if (info.IsError()) {
            // Send the error back
            if (!Send(info))
                SendError();
        }
        
        // Don't overdo it
        wait_ms(500);
    }
}


bool Send(Translator::MessageInfo info) {
    // Create buffer
    int bufferSize;
    char buffer[Translator::Rules::MaxCmdSize];
    // Translate to raw message
    if (!Translator::Translate(info, buffer, &bufferSize))
        return false;
    // Send every character
    for (int i = 0; i < bufferSize; i++)
        printf("%c", buffer[i]);
    return true;
}

void SendError() {
    SendError(0,0,0);
}

void SendError(int address) {
    SendError(address, 0, 0);
}

void SendError(char* data, int dataSize) {
    SendError(0, data, dataSize);
}

void SendError(int address, char* data , int dataSize) {
    // Create MessageInfo with error
    Translator::MessageInfo info;
    info.Address = address;
    for (int i = 0; i < dataSize; i++)
        info.Data[i] = data[i];
    SendError(info);
}

void SendError(Translator::MessageInfo info) {
    SignalError();
    info.CommandType = Translator::MessageInfo::ERROR;
    Send(info);
}

void SignalWaiting() {
    // Blue light
    rled = 1.0f;
    gled = 1.0f;
    bled = 0.0f;
}

void SignalReceived() {
    // Green light
    rled = 1.0f;
    gled = 0.0f;
    bled = 1.0f;
}

void SignalError() {
    // Red light
    rled = 0.0f;
    gled = 1.0f;
    bled = 1.0f;
}










