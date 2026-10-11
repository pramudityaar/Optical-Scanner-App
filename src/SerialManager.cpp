#include "SerialManager.h"
#include <iostream>

SerialManager::SerialManager() : hSerial(INVALID_HANDLE_VALUE), connected(false) {}

SerialManager::~SerialManager() {
    Disconnect();
}

bool SerialManager::Connect(const std::string& portName, int baudRate) {
    if (connected) Disconnect();

    // Prefix wajib Windows untuk membaca COM10+
    std::string portPath = "\\\\.\\" + portName;
    hSerial = CreateFileA(portPath.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) return false;

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) { Disconnect(); return false; }

    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) { Disconnect(); return false; }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 1;

    if (!SetCommTimeouts(hSerial, &timeouts)) { Disconnect(); return false; }

    connected = true;
    return true;
}

void SerialManager::Disconnect() {
    if (connected) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        connected = false;
    }
}

bool SerialManager::IsConnected() const { return connected; }

void SerialManager::WriteCommand(const std::string& cmd) {
    if (!connected) return;
    DWORD bytesWritten;
    WriteFile(hSerial, cmd.c_str(), cmd.length(), &bytesWritten, NULL);
}

std::string SerialManager::ReadData() {
    if (!connected) return "";
    char buffer[1024];
    DWORD bytesRead;
    
    if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        return std::string(buffer);
    }
    return "";
}

std::vector<std::string> SerialManager::GetAvailablePorts() {
    std::vector<std::string> ports;
    char path_buffer[5000]; 

    for (int i = 1; i <= 255; i++) {
        std::string portName = "COM" + std::to_string(i);
        DWORD test = QueryDosDeviceA(portName.c_str(), path_buffer, 5000);
        if (test != 0) {
            ports.push_back(portName);
        }
    }
    return ports;
}