#pragma once
#include <string>
#include <vector>
#include <windows.h>

class SerialManager {
public:
    SerialManager();
    ~SerialManager();

    bool Connect(const std::string& portName, int baudRate);
    void Disconnect();
    bool IsConnected() const;
    
    void WriteCommand(const std::string& cmd);
    std::string ReadData();
    
    // ==============================================================
    // FITUR BARU: PELACAK HARDWARE USB (WINDOWS API)
    // ==============================================================
    std::vector<std::string> GetAvailablePorts();

private:
    HANDLE hSerial;
    bool connected;
};