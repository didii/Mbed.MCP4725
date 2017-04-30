#pragma once

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#include <string>
#include "mbed.h"

// Implements the Data-Link layer, translating a raw message to MessageInfo and back
class Translator {
    
public:
    enum EErrorCode {
        SUCCES,
        INCOMPLETE,
        INVALID
    };
    
    // Rules over how the command is structured, indexes are per byte
    struct Rules {
        static const int StartCharIndex = 0;   // Location of starting char
        static const int CharsToReadIndex = 1; // Location of chars to read value
        static const int AddressIndex = 2;     // Location of pin address
        static const int CommandIndex = 3;     // Location of command type char
        static const int DataIndex = 4;        // Location of data start

        static const char StartChar = '!'; // Starting character
        static const char ReadChar = 'r';  // Character to indicate read command
        static const char WriteChar = 'w'; // Character to indicate write command
        static const char OtherChar = 'o'; // Character to indicate other command
        static const char ErrorChar = 'e'; // Character to indicate an error

        static const int PrefixSize = CharsToReadIndex + 1; // Number of chars in the prefix
        static const int ReadDataSize = 5;   // Number of chars used in read data
        static const int WriteDataSize = 3;   // Number of chars used in write data
        static const int MaxDataSize = MAX(ReadDataSize, WriteDataSize); // Maximum size of data array
        static const int EmptyDataCmdSize = DataIndex; // Number of chars in command if no data
        static const int ReadCmdSize = EmptyDataCmdSize + ReadDataSize;  // Size of a read command
        static const int WriteCmdSize = EmptyDataCmdSize + WriteDataSize; // Size of a write command
        static const int ErrorCmdSize = EmptyDataCmdSize; // Size of an error command

        static const int MaxCmdSize = MAX(ReadCmdSize, WriteCmdSize); // Maximum total command size
    };
    
    // Contains easy retreivable message info
    struct MessageInfo {
        enum ECommandType { NONE, READ, WRITE, ERROR } CommandType;
        int Address;
        char Data[Rules::MaxCmdSize];
        
        MessageInfo();
        
        // Function to more easily check CommandType
        bool IsNone();
        bool IsRead();
        bool IsWrite();
        bool IsError();

        // Resets all values to default
        void Reset();
    };

private:
    // Private constructor: no class instance allowed
    Translator();
    
public:
    // <Handig om weten: dit is een NULL ('\0') terminated character array dat terug wordt gegeven.
    //  Dit is nodig omdat printf("%s", cmd) niet kan weten hoe lang de array is en zal simpelweg
    //  blijven lezen tot het toevallig een NULL karakter tegenkomt.
    //  cmdSize geeft de grootte van de pointer ZONDER het null karakter zodat het zowel veilig
    //  kan gebruikt worden in printf en andere functies die geen null karakter vereisen (en waarbij
    //  dus het NULL karakter niet zal ingelezen worden>
    // Translate MessageInfo to raw char* (NULL terminated)
    static bool Translate(MessageInfo info, char cmd[Rules::MaxCmdSize], int* const cmdSize);
    // <Hier bijvoorbeeld is het NULL karakter niet vereist omdat de grootte wordt opgevraagt
    //  Indien de char* van vorige functie hier terug wordt in gezet volstaat het om ook zijn
    //  cmdSize te gebruiken: het NULL karakter valt hier buiten en wordt daarom dus niet gelezen
    //  Dit zal ik toevoegen aan de ondervonden problemen.>
    // Translate raw char* message to MessageInfo
    // Returns 0 if succesful, -1 if message too short, 1 for faulty/unrecoverable message
    static EErrorCode Translate(const char* const cmd, int cmdSize, MessageInfo* const info);

};