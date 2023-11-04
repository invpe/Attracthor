// https://github.com/invpe/Attracthor/tree/main SDR dataset builder - invpe
#include <iomanip>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <vector>
#include <chrono>
#include <thread>
#include <rtl-sdr.h>
#include <complex>

#define GAIN_AUTO 0
#define GAIN_MANUAL 1

int precision = 0;
int iFrequencyMHZ = 900;
const int write_interval = 10; // Write every 10th sample
const int capture_duration = 60; // Duration of capture in seconds
const double sample_rate = 1024000; // Sample rate in samples per second
const int num_samples = 512; // Number of samples per batch
std::vector<uint8_t> iq_samples(num_samples * 2); // Each sample contains 2 bytes (I and Q)

std::vector<float> calculateMagnitudes(const std::vector<uint8_t>& iq_samples) {
    std::vector<float> magnitudes;
    magnitudes.reserve(iq_samples.size() / 2);

    for (size_t i = 0; i < iq_samples.size(); i += 2) {
        int8_t I = static_cast<int8_t>(iq_samples[i]) - 128;
        int8_t Q = static_cast<int8_t>(iq_samples[i + 1]) - 128;
        float magnitude = std::sqrt(I * I + Q * Q);
        magnitudes.push_back(magnitude);
    }

    return magnitudes;
}
 

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: ./start freqMHZ" << std::endl;
        return EXIT_FAILURE;
    }

    iFrequencyMHZ = std::stoi(argv[1]);

    rtlsdr_dev_t *dev = nullptr;
    if (rtlsdr_open(&dev, 0) < 0) { // Assuming device index 0
        std::cerr << "Error opening RTLSDR device." << std::endl;
        return EXIT_FAILURE;
    }

    rtlsdr_set_tuner_bandwidth(dev, 12500); // Bandwidth in Hz
    rtlsdr_set_sample_rate(dev, static_cast<uint32_t>(sample_rate));
    rtlsdr_set_tuner_gain_mode(dev, GAIN_AUTO);
    rtlsdr_reset_buffer(dev);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    uint32_t rtl_freq = static_cast<uint32_t>(iFrequencyMHZ * 1e6);
    rtlsdr_set_center_freq(dev, rtl_freq);

    std::cout << "Capturing at: " << iFrequencyMHZ << " MHz" << std::endl;
    std::cout << "Capturing for: " << capture_duration << " seconds" << std::endl;
    std::cout << "Capturing every: "<<write_interval<<" magnitude"<<std::endl;

     std::ofstream outputFile("output"+std::to_string(iFrequencyMHZ)+"MHZ.txt", std::ofstream::out | std::ofstream::trunc);

    auto start_time = std::chrono::steady_clock::now();
    uint32_t uiTotal = 0;

    while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(capture_duration)) {
        int num_read_samples;
        if (rtlsdr_read_sync(dev, iq_samples.data(), num_samples * 2, &num_read_samples) < 0) {
            std::cerr << "Error reading samples." << std::endl;
            break;
        }
 
            std::vector<float> magnitudes = calculateMagnitudes(iq_samples);
            for(int x = 0; x < magnitudes.size(); x+=write_interval)
            outputFile << std::fixed << std::setprecision(precision) << magnitudes[x] << "\n";
            
        uiTotal += num_read_samples;
    }

    rtlsdr_close(dev);
    outputFile.close();

    std::cout << "Total samples read: " << uiTotal << std::endl;

    return EXIT_SUCCESS;
}
