#include "Translator.h"

Translator::MessageInfo::MessageInfo() {
    Reset();
}

bool Translator::MessageInfo::IsNone() {
    return CommandType == NONE;
}

bool Translator::MessageInfo::IsRead() {
    return CommandType == READ;
}

bool Translator::MessageInfo::IsWrite() {
    return CommandType == WRITE;
}

bool Translator::MessageInfo::IsError() {
    return CommandType = ERROR;
}

void Translator::MessageInfo::Reset() {
    CommandType = NONE;
    Address = 0;
    for (int i = 0; i < Rules::MaxDataSize; i++)
        Data[i] = 0;
}

Translator::Translator() {}


bool Translator::Translate(MessageInfo info, char cmd[Rules::MaxCmdSize], int* const cmdSize) {
    if (MessageInfo::NONE)
        return false;
    
    cmd[Rules::StartCharIndex] = Rules::StartChar;
    cmd[Rules::AddressIndex] = info.Address;
    for (int i = 0; i < Rules::MaxDataSize; i++)
        cmd[Rules::DataIndex+i] = info.Data[i];
    
    switch (info.CommandType) {
        case MessageInfo::NONE:
            return false;
        case MessageInfo::READ:
            *cmdSize = Rules::ReadCmdSize;
            cmd[Rules::CharsToReadIndex] = Rules::ReadCmdSize-Rules::PrefixSize;
            cmd[Rules::CommandIndex] = Rules::ReadChar;
            break;
        case MessageInfo::WRITE:
            *cmdSize = Rules::WriteCmdSize;
            cmd[Rules::CharsToReadIndex] = Rules::WriteCmdSize - Rules::PrefixSize;
            cmd[Rules::CommandIndex] = Rules::WriteChar;
            break;
        case MessageInfo::ERROR:
            *cmdSize = Rules::MaxCmdSize;
            cmd[Rules::CharsToReadIndex] = Rules::MaxCmdSize - Rules::PrefixSize;
            cmd[Rules::CommandIndex] = Rules::ErrorChar;
            break;
        default:
            return false;
    }

    return true;
}


Translator::EErrorCode Translator::Translate(const char* const cmd, int cmdSize, MessageInfo* const info) {
    // Check starting character
    if (cmd[Rules::StartCharIndex] != Rules::StartChar) {
        return INVALID;
    }
    // Check length of command
    if (cmdSize < Rules::PrefixSize || (int)(unsigned char)cmd[Rules::CharsToReadIndex] > cmdSize - Rules::PrefixSize)
        return INCOMPLETE;
    if ((int)(unsigned char)cmd[Rules::CharsToReadIndex] < cmdSize - Rules::PrefixSize)
        return INVALID;
        
    // If complete message was read, check if correct size with respect to default read/write length
    if ((cmd[Rules::CommandIndex] == 'r' && cmdSize != Rules::ReadCmdSize) || (cmd[Rules::CommandIndex] == 'w' && cmdSize != Rules::WriteCmdSize))
        return INVALID;
    
    // Message should be correct
    info->Reset();

    // Populate info
    info->Address = (int)(unsigned char)cmd[Rules::AddressIndex];
    switch (cmd[Rules::CommandIndex]) {
    case 'r':
        info->CommandType = MessageInfo::READ;
        for (int i = 0; i < Rules::ReadDataSize; i++)
            info->Data[i] = cmd[Rules::DataIndex+i];
        break;
    case 'w':
        info->CommandType = MessageInfo::WRITE;
        for (int i = 0; i < Rules::WriteDataSize; i++)
            info->Data[i] = cmd[Rules::DataIndex+i];
        break;
    case 'e':
        info->CommandType = MessageInfo::ERROR;
        break;
    default:
        return INVALID;
    }
    for (int i = 0; i < cmdSize-Rules::DataIndex; i++)
        info->Data[i] = cmd[Rules::DataIndex+i];
    
    return SUCCES;
}