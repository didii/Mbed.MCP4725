#include "Reader.h"

// Default constructor
Reader::Reader()
    : pc(USBTX, USBRX),
    _isNewMessageReceived(false),
    _lastMessage(),
    _lastMessageSize(0),
    _buffer(),
    _bufferSize(0),
    _lastChar(0),
    _index(0)
{
    // Attach read function to pc
    pc.attach(this, &Reader::pcCallback);
}


bool Reader::IsNewMessageReceived() {
    // If no message received, return false
    if (!_isNewMessageReceived)
        return false;
    
    // If message received, set to false and return true
    _isNewMessageReceived = false;
    return true;
}

Translator::MessageInfo Reader::GetLastMessageInfo() const {
    return _lastMessageInfo;
}

const char* Reader::GetLastMessage() const {
    return _lastMessage;
}

int Reader::GetLastMessageSize() const {
    return _lastMessageSize;
}

void Reader::pcCallback() {
    // Get the sent character
    _lastChar = pc.getc();
    
    // If message not started and StartChar was read
    //if (!_isMessageStarted && _lastChar == Translator::Rules::StartChar) {
    //    // Reset and signal start
    //    _index = 0;
    //    _isMessageStarted = true;
    //}
    
    // If ongoing message
    //if (_isMessageStarted) {
        
        // Add character to buffer
        _buffer[_index] = _lastChar;
        // Check if valid message by translating it
        Translator::MessageInfo info;
        
        switch (Translator::Translate(_buffer, _index + 1, &info)) {
        case Translator::INCOMPLETE:
            // Keep adding chars
            _index++;
            break;;
        case Translator::SUCCES:
            // Copy message info
            _lastMessageInfo = info;
            // Notify user
            _isNewMessageReceived = true;
            // Reset counter
            _index = 0;
            break;
        case Translator::INVALID:
        default:
            // Reset buffer
            _index = 0;
            break;
        }
    //}
    return;
}