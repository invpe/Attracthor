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

int iFrequencyMHZ = 900;
const int capture_duration = 1;  
int precision = 0;
const double sample_rate = 1024000;  
const int num_samples = 512;
std::vector<uint8_t> iq_samples(num_samples * 2);  // Each sample contains 2 bytes (I and Q)

std::vector<float> calculateMagnitudes(const std::vector<uint8_t>& iq_samples) {
    std::vector<float> magnitudes;
    magnitudes.reserve(iq_samples.size() / 2);

    for (size_t i = 0; i < iq_samples.size(); i += 2) { 
        int8_t I = static_cast<int8_t>(iq_samples[i]) - 128;
        int8_t Q = static_cast<int8_t>(iq_samples[i + 1]) - 128;
        
        // Calculate the magnitude
        float magnitude = std::sqrt(I * I + Q * Q);
        magnitudes.push_back(magnitude);
    }

    return magnitudes;
}
std::string getCurrentDateTime()
{
    std::time_t now = std::time(0);
    std::tm *timeinfo = std::localtime(&now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}
 
int main(int argc, char*argv[])
{
    if(argc<2)
    {
        printf("./start freqMHZ\n");
        exit(0);
    }
 
    // Initialize the RTLSDR dongle
    int device_index = 0; 
    int freq_correction = 0;    
    if (rtlsdr_get_device_count() - 1 < device_index)
    {
        fprintf(stderr, "! Device %d not found\n", device_index);
        exit(EXIT_FAILURE);
    }
    // Open the RTLSDR device
    rtlsdr_dev_t *dev = nullptr;
    if (rtlsdr_open(&dev, device_index) < 0)
    {
        std::cerr << "Error opening RTLSDR device." << std::endl;
        return 1;
    }
    rtlsdr_set_tuner_bandwidth(dev, 12500); //12.5khz
    rtlsdr_set_sample_rate(dev, sample_rate);
    rtlsdr_set_tuner_gain_mode(dev, GAIN_AUTO);
    rtlsdr_reset_buffer(dev);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    //
    uint32_t rtl_freq = iFrequencyMHZ * 1e6;
    rtlsdr_set_center_freq(dev, rtl_freq);
 
    //
    printf("Capturing at: %i mhz\n",iFrequencyMHZ);
    printf("Capturing for: %i seconds\n",capture_duration);
    printf("Capturing with: %i precision\n", precision);

    std::ofstream outputFile("output"+std::to_string(iFrequencyMHZ)+"MHZ.txt", std::ofstream::out | std::ofstream::trunc);


    uint32_t uiTotal = 0;
    auto start_time = std::chrono::steady_clock::now();
    
    while (true) {
        auto current_time = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count() >= capture_duration) {
            break; 
        }

        // Read samples
        int num_read_samples;
        if (rtlsdr_read_sync(dev, iq_samples.data(), num_samples, &num_read_samples) < 0) {
            std::cerr << "Error reading samples." << std::endl;
            break; 
        }

        // Calculate magnitudes and write to file
        std::vector<float> magnitudes = calculateMagnitudes(iq_samples);
        for (float magnitude : magnitudes) {
            outputFile << std::fixed << std::setprecision(precision) << magnitude << "\n";
        }
        uiTotal+=num_read_samples;
    }

    // Close the RTLSDR device
    rtlsdr_close(dev);
    outputFile.close();

    printf("Read %i\n", uiTotal);

    return 0;
}
