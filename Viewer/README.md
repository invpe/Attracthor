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


`A perfect randomness` - No patterns visible, randomly distributed cloud of points.

* rpi-armv7l-5.10.63.v7.txt `generate2.sh`

  ![image](https://github.com/invpe/Attracthor/assets/106522950/ae0dd648-5d61-4a22-a5c6-2c562c062643)

* linux-x86_64-5.15.0.88.generic.txt `generate2.sh`

  ![image](https://github.com/invpe/Attracthor/assets/106522950/08b628af-cae8-4c3f-8a12-a2d42c868705)

* esp32 no-wifi `esp_random()`

  ![image](https://github.com/invpe/Attracthor/assets/106522950/25dfa05d-0e28-4ded-a260-cb12494b4061)

* esp32 wifi `esp_random()`

  ![image](https://github.com/invpe/Attracthor/assets/106522950/d2f24e09-586b-4f6a-bf99-ff4ccd5bdcca)

