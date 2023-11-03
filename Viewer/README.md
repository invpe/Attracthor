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

At first i am looking into the PRNG's within various operating systems. 
This is the easiest dataset to get and try thus it goes first in the line.

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

