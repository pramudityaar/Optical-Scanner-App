//fun --pram

#include "AppUI.h"
#include "imgui.h"
#include "implot.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <windows.h> 

// ==============================================================================
// 1. WINDOWS NATIVE FILE I/O & METADATA CSV
// ==============================================================================
std::string AppUI::OpenSaveDialog() {
    OPENFILENAMEA ofn;
    CHAR szFile[260] = {0};
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL; 
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "CSV Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "csv";

    if (GetSaveFileNameA(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    }
    
    return "";
}

void AppUI::SaveDataToCSV(const std::string& filepath, int dataType) {
    std::ofstream file(filepath);
    
    if (!file.is_open()) {
        AddLog("Export Data Failed");
        return;
    }

    // -- GENERATE METADATA --
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    
    file << "# --- EXPERIMENT METADATA ---\n";
    file << "# Time: " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "\n";
    file << "# Gain: " << (current_gain == 0 ? "Photovoltaic" : "Photodiode") << "\n";
    file << "# ADC Resolution: 16-bit (Raw Data)\n"; // Tambahan info metadata
    
    // -- DUMP DATA BUFFER --
    if (dataType == 0) {
        // MODE 0: LINEAR SCAN
        file << "# Mode: Linear Scan\n";
        file << "# ---------------------------\n";
        file << "Position_mm,Intensity_RawADC\n"; // Header diubah ke RawADC
        
        for (size_t i = 0; i < scanData.X.size(); ++i) {
            file << scanData.X[i] << "," << scanData.Y[i] << "\n";
        }
    } else {
        // MODE 1: FREQUENCY READ 
        file << "# Mode: " << (freq_mode == 0 ? "Analog Time-Domain" : "Digital State") << "\n";
        file << "# ---------------------------\n";
        // Header diubah ke RawADC
        file << "Time_s," << (freq_mode == 0 ? "Intensity_RawADC" : "Logic_State") << ",Instant_Freq_Hz\n";
        
        // Algoritma Penentuan Garis Tengah (Threshold)
        float threshold = 0.5f; 
        
        if (freq_mode == 0 && !freqData.Y.empty()) {
            float min_v = freqData.Y[0];
            float max_v = freqData.Y[0];
            
            for (float v : freqData.Y) { 
                if (v < min_v) {
                    min_v = v;
                }
                if (v > max_v) {
                    max_v = v; 
                }
            }
            threshold = (min_v + max_v) / 2.0f; 
        }

        // Kalkulasi Jarak Antar Puncak (Peak-to-Peak)
        float last_peak_time = -1.0f;
        float current_freq = 0.0f;

        for (size_t i = 0; i < freqData.X.size(); ++i) {
            if (i > 0 && freqData.Y[i-1] <= threshold && freqData.Y[i] > threshold) {
                if (last_peak_time > 0.0f) {
                    float delta_t = freqData.X[i] - last_peak_time;
                    if (delta_t > 0) {
                        current_freq = 1.0f / delta_t; 
                    }
                }
                last_peak_time = freqData.X[i];
            }
            
            file << freqData.X[i] << "," << freqData.Y[i] << "," << current_freq << "\n";
        }
    }
    
    file.close();
    AddLog("Data Export Success");
}

// ==============================================================================
// 2. INDUSTRIAL THEME & STYLING
// ==============================================================================
void AppUI::SetupIndustrialTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f; 
    style.FrameRounding = 0.0f; 
    style.TabRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;

    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
    
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f); 
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
    
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.95f, 0.95f, 0.95f, 1.0f); 

    style.Colors[ImGuiCol_Button] = ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
    
    style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); 

    style.Colors[ImGuiCol_Tab] = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.65f, 0.65f, 0.65f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
}

void AppUI::AddLog(const std::string& msg) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    
    oss << "[" << std::put_time(&tm, "%H:%M:%S") << "] " << msg;
    logMessages.push_back(oss.str());
    
    if (logMessages.size() > 100) {
        logMessages.erase(logMessages.begin());
    }
}

// ==============================================================================
// 3. ROBUST SERIAL DATA PARSING
// ==============================================================================
void AppUI::ProcessIncomingSerialData() {
    if (!serial->IsConnected()) return;
    
    std::string incoming = serial->ReadData();
    if (incoming.empty()) return;
    
    rx_accumulator += incoming;

    size_t pos;
    while ((pos = rx_accumulator.find('\n')) != std::string::npos) {
        std::string line = rx_accumulator.substr(0, pos);
        rx_accumulator.erase(0, pos + 1);
        
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (line == "SL") { 
            is_scanning = true; 
            scanData.Clear(); 
            AddLog("Linear Scan Started"); 
        }
        else if (line == "EL") { 
            is_scanning = false; 
            AddLog("Linear Scan Done"); 
        }
        else if (line == "SA") { 
            is_reading_freq = true; 
            freqData.Clear(); 
            AddLog("Analog Read Started"); 
        }
        else if (line == "EA") { 
            is_reading_freq = false; 
            AddLog("Analog Read Done"); 
        }
        else if (line == "SD") { 
            is_reading_freq = true; 
            freqData.Clear(); 
            AddLog("Digital Read Started"); 
        }
        else if (line == "ED") { 
            is_reading_freq = false; 
            AddLog("Digital Read Done"); 
        }
        else if (line.rfind("H,1", 0) == 0) {
            AddLog("Homing Complete");
        }
        else if (line.rfind("P,", 0) == 0) { 
             try { 
                 current_pos = std::stof(line.substr(2)); 
             } catch(...) {}
        }
        else if (is_scanning) {
            ParseScanData(line);
        }
        else if (is_reading_freq) {
            ParseFrequencyData(line);
        }
    }
}

void AppUI::ParseScanData(const std::string& line) {
    size_t comma_idx = line.find(',');
    
    if (comma_idx != std::string::npos) {
        try {
            float x_val = std::stof(line.substr(0, comma_idx));
            float y_val = std::stof(line.substr(comma_idx + 1));
            
            scanData.AddPoint(x_val, y_val); 
            current_pos = x_val; 
        } catch (...) {
        } 
    }
}

void AppUI::ParseFrequencyData(const std::string& line) {
    size_t comma_idx = line.find(',');
    
    if (comma_idx != std::string::npos) {
        try {
            float t_val = std::stof(line.substr(0, comma_idx));
            float f_val = std::stof(line.substr(comma_idx + 1));
            
            freqData.AddPoint(t_val, f_val); 
        } catch (...) {
        }
    }
}

// ==============================================================================
// 4. MAIN LIFECYCLE & HARDWARE TRACKING
// ==============================================================================
void AppUI::RefreshPorts() {
    available_ports = serial->GetAvailablePorts();
    
    current_port_idx = 0; 
    
    if (available_ports.empty()) {
        AddLog("No COM Ports Found");
    } else {
        AddLog("Ports Refreshed");
    }
}

void AppUI::Initialize(SerialManager* serialInstance) {
    this->serial = serialInstance; 
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    
    config.RasterizerMultiply = 1.3f; 
    config.PixelSnapH = true;         
    
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 16.0f, &config);
    SetupIndustrialTheme();
    
    RefreshPorts();
    
    AddLog("UI System Ready");
}

void AppUI::Render() {
    ProcessIncomingSerialData();

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;

    RenderHardwareStatusPanel(work_pos, work_size);
    RenderOperationPanel(work_pos, work_size);
}

void AppUI::Shutdown() {
}

// ==============================================================================
// 5. MODULAR RENDER: PANEL KIRI (HARDWARE STATUS)
// ==============================================================================
void AppUI::RenderHardwareStatusPanel(const ImVec2& work_pos, const ImVec2& work_size) {
    ImGui::SetNextWindowPos(work_pos);
    ImGui::SetNextWindowSize(ImVec2(350, work_size.y));
    
    ImGui::Begin("Hardware Status", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::Text("COM Port");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 100.0f);
    
    if (available_ports.empty()) {
        const char* empty_list[] = { "None" };
        ImGui::Combo("##Port", &current_port_idx, empty_list, 1);
    } else {
        std::vector<const char*> port_ptrs;
        for (const auto& p : available_ports) {
            port_ptrs.push_back(p.c_str());
        }
        ImGui::Combo("##Port", &current_port_idx, port_ptrs.data(), port_ptrs.size());
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Refresh", ImVec2(90, 25))) {
        RefreshPorts();
    }
    
    ImGui::Spacing();
    
    if (serial->IsConnected()) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button("Disconnect", ImVec2(-1, 30))) {
            serial->Disconnect();
            AddLog("Port Disconnected");
        }
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
        if (ImGui::Button("Connect", ImVec2(-1, 30))) {
            if (!available_ports.empty()) {
                if (serial->Connect(available_ports[current_port_idx], 921600)) {
                    AddLog("Port Connected");
                } else {
                    AddLog("Connection Failed");
                }
            } else {
                AddLog("No Port Selected");
            }
        }
        ImGui::PopStyleColor();
    }

    ImGui::Spacing(); 
    ImGui::Separator(); 
    ImGui::Spacing();
    
    const char* gains[] = { "Photovoltaic", "Photodiode" };
    ImGui::Text("ADC Gain");
    ImGui::SetNextItemWidth(-1.0f); 
    
    if (ImGui::Combo("##Gain", &current_gain, gains, IM_ARRAYSIZE(gains))) {
         if (serial->IsConnected()) {
             char cmd[16]; 
             snprintf(cmd, sizeof(cmd), "M%d\n", current_gain); 
             serial->WriteCommand(cmd);
             AddLog("Gain Mode Changed");
         }
    }

    ImGui::Spacing(); 
    ImGui::Separator(); 
    ImGui::Spacing();
    
    ImGui::Text("Current Position");
    ImGui::TextColored(ImVec4(0.8f, 0.1f, 0.1f, 1.0f), "%.2f mm", current_pos);
    
    if (ImGui::Button("Start Homing", ImVec2(-1, 30))) {
        if (serial->IsConnected()) {
            serial->WriteCommand("H\n");
            AddLog("Homing Sequence Sent");
        }
    }

    ImGui::Spacing(); 
    ImGui::Separator(); 
    ImGui::Spacing();
    
    ImGui::Text("System Log");
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 100);
    ImGui::Checkbox("Auto", &log_autoscroll);
    
    ImGui::BeginChild("LogBox", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    for (const auto& log : logMessages) {
        ImGui::TextUnformatted(log.c_str());
    }
    
    if (log_autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
    
    ImGui::End();
}

// ==============================================================================
// 6. MODULAR RENDER: PANEL KANAN (OPERATION)
// ==============================================================================
void AppUI::RenderOperationPanel(const ImVec2& work_pos, const ImVec2& work_size) {
    ImGui::SetNextWindowPos(ImVec2(work_pos.x + 350, work_pos.y));
    ImGui::SetNextWindowSize(ImVec2(work_size.x - 350, work_size.y));
    
    ImGui::Begin("Operation", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    if (ImGui::BeginTabBar("Tabs")) {
        if (ImGui::BeginTabItem("Linear Read")) {
            RenderTabLinearRead();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Frequency Read")) {
            RenderTabFrequencyRead();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Mechanical Setup")) {
            RenderTabMechanicalSetup();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
    ImGui::End();
}

// ==============================================================================
// 7. SUB-RENDERS (TABS)
// ==============================================================================
void AppUI::RenderTabLinearRead() {
    ImGui::Spacing();
    
    if (ImGui::Button("Start Scan", ImVec2(120, 30))) {
        if(serial->IsConnected()) {
            serial->WriteCommand("G\n");
            AddLog("Scan Request Sent");
        }
    }
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); 
    
    if (ImGui::Button("Emergency Stop", ImVec2(140, 30))) {
        if(serial->IsConnected()) {
            serial->WriteCommand("X\n");
            AddLog("Emergency Stop Sent");
        }
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 135);
    
    if (ImGui::Button("Export Data", ImVec2(120, 30))) {
        std::string path = OpenSaveDialog();
        if (!path.empty()) {
            SaveDataToCSV(path, 0); 
        }
    }
    
    ImGui::Spacing(); 
    ImGui::Spacing();
    
    ImGui::Text("Scan Progress");
    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 135);
    ImGui::Checkbox("Auto Fit", &auto_fit_scan);

    ImGui::ProgressBar(scan_progress, ImVec2(-1, 0));

    ImPlotAxisFlags flags = auto_fit_scan ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
    
    if (ImPlot::BeginPlot("Linear Scan Canvas", ImVec2(-1, -1))) {
        // Label Y-Axis diganti dari (mV) menjadi (Raw ADC)
        ImPlot::SetupAxes("Position (mm)", "Intensity (Raw ADC)", flags, flags);
        
        if (!scanData.X.empty()) {
            ImPlot::PlotLine("Intensity", scanData.X.data(), scanData.Y.data(), scanData.X.size());
        }
        
        ImPlot::EndPlot();
    }
}

void AppUI::RenderTabFrequencyRead() {
    ImGui::Spacing();
    
    if (ImGui::BeginTabBar("SubTabs")) {
        if (ImGui::BeginTabItem("Analog Mode")) {
            freq_mode = 0;
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Digital Mode")) {
            freq_mode = 1;
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    
    ImGui::Spacing(); 
    ImGui::Separator(); 
    ImGui::Spacing();
    
    ImGui::InputFloat("Read Time (s)", &read_time_s, 1.0f, 5.0f, "%.1f");
    
    if (ImGui::Button("Start Read", ImVec2(120, 30))) {
        if(serial->IsConnected()) {
            std::string cmd = (freq_mode == 0) ? "A\n" : "W\n"; 
            serial->WriteCommand(cmd);
            AddLog("Read Request Sent");
        }
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Stop Read", ImVec2(120, 30))) {
        if(serial->IsConnected()) {
            serial->WriteCommand("X\n");
            AddLog("Read Stopped");
        }
    }

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 135);
    
    if (ImGui::Button("Export Data", ImVec2(120, 30))) {
        std::string path = OpenSaveDialog();
        if (!path.empty()) {
            SaveDataToCSV(path, 1); 
        }
    }
    
    ImGui::Spacing(); 
    ImGui::Spacing();
    
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 135);
    ImGui::Checkbox("Auto Fit", &auto_fit_freq);

    ImPlotAxisFlags flags = auto_fit_freq ? ImPlotAxisFlags_AutoFit : ImPlotAxisFlags_None;
    
    if (freq_mode == 0) {
        if (ImPlot::BeginPlot("Analog Canvas", ImVec2(-1, -1))) {
            // Label Y-Axis diganti dari (mV) menjadi (Raw ADC)
            ImPlot::SetupAxes("Time (s)", "Intensity (Raw ADC)", flags, flags);
            
            if (!freqData.X.empty()) {
                ImPlot::PlotLine("Intensity", freqData.X.data(), freqData.Y.data(), freqData.X.size());
            }
            
            ImPlot::EndPlot();
        }
    } else {
        if (ImPlot::BeginPlot("State Condition Canvas", ImVec2(-1, -1))) {
            
            if (auto_fit_freq) {
                ImPlot::SetupAxes("Time (s)", "Logic State (0/1)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_Lock);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5, 1.5, ImGuiCond_Always);
            } else {
                ImPlot::SetupAxes("Time (s)", "Logic State (0/1)", flags, ImPlotAxisFlags_Lock);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5, 1.5, ImGuiCond_Once);
            }
            
            if (!freqData.X.empty()) {
                ImPlot::PlotStairs("Digital State", freqData.X.data(), freqData.Y.data(), freqData.X.size());
            }
            
            ImPlot::EndPlot();
        }
    }
}

void AppUI::RenderTabMechanicalSetup() {
    ImGui::Spacing();
    ImGui::InputFloat("Target Input", &target_mm, 0.1f, 1.0f, "%.2f");
    
    if (ImGui::Button("Go To Target", ImVec2(150, 30))) {
        if (serial->IsConnected()) {
            char cmd[32]; 
            snprintf(cmd, sizeof(cmd), "P%.2f\n", target_mm); 
            serial->WriteCommand(cmd);
            AddLog("Target Move Sent");
        }
    }

    ImGui::Spacing(); 
    ImGui::Separator(); 
    ImGui::Spacing();
    
    if (ImGui::Button("Jog Left", ImVec2(100, 30))) { 
        AddLog("Jog Left Sent");
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Jog Right", ImVec2(100, 30))) { 
        AddLog("Jog Right Sent");
    }

    ImGui::Spacing(); 
    ImGui::Separator(); 
    ImGui::Spacing();
    
    ImGui::Checkbox("Advanced Settings", &adv_settings);
    
    if (adv_settings) {
        ImGui::Spacing();
        ImGui::InputFloat("Steps Per mm", &steps_per_mm, 10.0f, 100.0f, "%.1f");
        ImGui::InputFloat("Motor Speed", &motor_speed, 1.0f, 5.0f, "%.1f");
        
        if (ImGui::Button("Apply Settings", ImVec2(150, 30))) {
            if (serial->IsConnected()) {
                char cmd[64]; 
                snprintf(cmd, sizeof(cmd), "C,%.1f,%.1f\n", steps_per_mm, motor_speed); 
                serial->WriteCommand(cmd);
                AddLog("Settings Applied");
            }
        }
    }
}