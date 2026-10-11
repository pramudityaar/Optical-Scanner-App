#pragma once
#include <vector>

// Struktur Ember Data yang efisien untuk ImPlot
struct DataBuffer {
    std::vector<float> X;
    std::vector<float> Y;

    DataBuffer() {
        // Reservasi memori awal untuk 100.000 titik agar CPU tidak ngos-ngosan
        X.reserve(100000);
        Y.reserve(100000);
    }

    void AddPoint(float x, float y) {
        X.push_back(x);
        Y.push_back(y);
    }

    void Clear() {
        X.clear();
        Y.clear();
    }
};