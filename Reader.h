#pragma once

#include <string>
#include "mbed.h"
#include "Translator.h"

// Reads the input buffer and notifies when a new message is ready
class Reader {
    
private:
    // PC interaction object
    Serial pc;
    
    static const int _maxSize = 256;          // Limitation of MCP4725, NOT as described according to protocol
    bool _isNewMessageReceived;               // True if a complete message was read
    Translator::MessageInfo _lastMessageInfo; // Contains formatted message info
    char _lastMessage[_maxSize];// last complete message received
    int _lastMessageSize;       // size of last complete message received
    char _buffer[_maxSize]; // temporary storage while reading a message
    int _bufferSize;        // size of the buffer
    
    char _lastChar;         // last byte read
    int _index;             // current number of characters read
    
public:
    // Default constructor, attaches a callback to pc
    Reader();
    
    // Returns true only once when a complete message was read
    bool IsNewMessageReceived();
    Translator::MessageInfo GetLastMessageInfo() const;
    // Returns the last message (without header)
    const char* GetLastMessage() const;
    // Returns the size of the last message
    int GetLastMessageSize() const;
    
private:
    // Function to be attached to pc
    void pcCallback();
    // Resets the buffer
    void Reset();
};