# Synopsis
A gallery of visuals made when looking for strange attractors from various datasets.
This is still a work in progress.

# Setup
Following setup is used in this experiment.

- Datasets are raw txt files with numbers

```
13786
11979
16525
30653
18735
12201
12511
```

- All datapoints are applied DSC

```
tPosition _Position;
_Position.x = vValues[a] - vValues[a-1];
_Position.y = vValues[a-1] - vValues[a-2];
_Position.z = vValues[a-2] - vValues[a-3];
```

- Then values are normalized to `<0,1>` so the datapoints once rendered will fit this range in 3D space.

```
_Position.x = (_Position.x-iMinValue)/(iMaxValue-iMinValue);
_Position.y = (_Position.y-iMinValue)/(iMaxValue-iMinValue);
_Position.z = (_Position.z-iMinValue)/(iMaxValue-iMinValue);
```

# Random numbers
Random numbers are values that are generated unpredictably or without a discernible pattern. They are used in various applications across mathematics, science, statistics, computer science, and many other fields. Random numbers play a crucial role in simulations, games, cryptography, and statistical analysis, among other areas.

## Linux
Data obtained with the use of `generate2.sh`.

Outcomes: _Perfect randomness_ - by the looks of the visualization - no patterns, random distribution of points in the space.

* rpi-armv7l-5.10.63.v7.txt  

  ![image](https://github.com/invpe/Attracthor/assets/106522950/ae0dd648-5d61-4a22-a5c6-2c562c062643)

* linux-x86_64-5.15.0.88.generic.txt 

  ![image](https://github.com/invpe/Attracthor/assets/106522950/08b628af-cae8-4c3f-8a12-a2d42c868705)

* macosx-darwin-23.0.0-x86_64.txt

  ![image](https://github.com/invpe/Attracthor/assets/106522950/c2a3f289-d7f0-474d-8eed-6553b95a93f3)

* random.org
  
  ![image](https://github.com/invpe/Attracthor/assets/106522950/2cba7e32-c45c-4a6f-b07a-3ba94fab3bcb)



## ESP32

ESP32 contains a hardware random number generator, values from it can be obtained using the APIs esp_random() 
Get one random 32-bit word from hardware RNG.

If Wi-Fi or Bluetooth are enabled, this function returns true random numbers. In other situations, if true random numbers are required then consult the ESP-IDF Programming Guide “Random Number Generation” section for necessary prerequisites.

This function automatically busy-waits to ensure enough external entropy has been introduced into the hardware RNG state, before returning a new random number. This delay is very short (always less than 100 CPU cycles).

Outcomes: _Perfect randomness_ - by the looks of the visualization - no patterns, random distribution of points in the space.

* esp32 no-wifi  
  

  ![image](https://github.com/invpe/Attracthor/assets/106522950/25dfa05d-0e28-4ded-a260-cb12494b4061)

* esp32 wifi 

  ![image](https://github.com/invpe/Attracthor/assets/106522950/d2f24e09-586b-4f6a-bf99-ff4ccd5bdcca)


# Analog readings

* esp32 analog read port 26 (M5Stack AtomU)

  ![image](https://github.com/invpe/Attracthor/assets/106522950/2ee59848-103f-4e1d-93ae-521903c6daea)
  ![image](https://github.com/invpe/Attracthor/assets/106522950/5fb0e38d-9fba-4930-924f-c2d8cff32bd3)



# ENVIRONMENT

## PM10

* PM10 readings from 2022 (full year)
  
  ![image](https://github.com/invpe/Attracthor/assets/106522950/2e813856-39db-47e8-945d-01a4d82b78b4)
  ![image](https://github.com/invpe/Attracthor/assets/106522950/ce4ec855-a513-4501-a713-c7ecc64e91f8)

  
  A dense core with filaments or strands radiating out, it may indicate a chaotic attractor with a strong pull back to a central set of states (the dense part), with occasional forays into more extreme conditions (the strands).

## HUMIDITY

* Humidity readings from 2022 (full year)

  ![image](https://github.com/invpe/Attracthor/assets/106522950/90afb67f-092f-401b-86cd-b87fbcafec62)

## TEMPERATURE

* Temperature readings from 2022 (full year)

  ![image](https://github.com/invpe/Attracthor/assets/106522950/c8fbdb82-fca1-42f7-a470-a04d96195edb)


  

# SDR
Using an SDR (Software Defined Radio) device to explore the noise from surroundings.
The electromagnetic noise captured by an SDR is essentially a time series of signal strengths that can be quite complex, influenced by a wide array of factors such as human-made signals, natural atmospheric phenomena, and cosmic sources. 

## Capture the Data

- Tune to an Empty Frequency: Find a frequency that is not used for any active transmission. The noise in this empty channel might be more indicative of natural or environmental electromagnetic noise.
- Broad Spectrum Analysis: Alternatively, capture a broad spectrum of frequencies. This can give more comprehensive dataset that includes various sources of noise and signals.
- Raw IQ Data: Record the raw IQ (in-phase and quadrature) data, which preserves the full information of the received signal and would be more suitable for complex analysis.

  
