#pragma once
#include <string>
#include <vector>
#include "SerialManager.h"
#include "DataBuffer.h"

class AppUI {
public:
    void Initialize(SerialManager* serialInstance);
    void Render();
    void Shutdown();

private:
    // ==============================================================
    // CORE UI MODIFIERS & THEME
    // ==============================================================
    void SetupIndustrialTheme();
    void AddLog(const std::string& msg);
    
    // ==============================================================
    // DATA & SERIAL HANDLERS
    // ==============================================================
    void ProcessIncomingSerialData();
    void ParseScanData(const std::string& line);
    void ParseFrequencyData(const std::string& line);
    
    // ==============================================================
    // FILE I/O (WINDOWS NATIVE)
    // ==============================================================
    std::string OpenSaveDialog(); 
    void SaveDataToCSV(const std::string& filepath, int dataType); 

    // ==============================================================
    // MODULAR RENDERING FUNCTIONS
    // ==============================================================
    void RenderHardwareStatusPanel(const struct ImVec2& work_pos, const struct ImVec2& work_size);
    void RenderOperationPanel(const struct ImVec2& work_pos, const struct ImVec2& work_size);
    
    void RenderTabLinearRead();
    void RenderTabFrequencyRead();
    void RenderTabMechanicalSetup();

    // ==============================================================
    // HARDWARE TRACKING (BARU)
    // ==============================================================
    void RefreshPorts();

    // ==============================================================
    // POINTERS & DATA BUFFERS
    // ==============================================================
    SerialManager* serial;
    std::vector<std::string> logMessages;
    DataBuffer scanData;        
    DataBuffer freqData;        
    std::string rx_accumulator; 
    
    // ==============================================================
    // STATE VARIABLES: Hardware Status
    // ==============================================================
    std::vector<std::string> available_ports; // Penampung COM Port dinamis
    int current_port_idx = 0;                 // Indeks port yang terpilih
    
    int current_gain = 0;       
    float current_pos = 0.0f;
    bool log_autoscroll = true; 
    
    // ==============================================================
    // STATE VARIABLES: Tab 1 (Linear Read)
    // ==============================================================
    float scan_progress = 0.0f;
    bool is_scanning = false;
    bool auto_fit_scan = true;  
    
    // ==============================================================
    // STATE VARIABLES: Tab 2 (Frequency Read)
    // ==============================================================
    float read_time_s = 10.0f;
    int freq_mode = 0;          
    bool is_reading_freq = false;
    bool auto_fit_freq = true;  
    
    // ==============================================================
    // STATE VARIABLES: Tab 3 (Mechanical Setup)
    // ==============================================================
    float target_mm = 0.0f;
    bool adv_settings = false;
    float steps_per_mm = 800.0f;
    float motor_speed = 15.0f;
};