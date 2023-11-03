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


`A perfect randomness` - rpi-armv7l-5.10.63.v7.txt

![image](https://github.com/invpe/Attracthor/assets/106522950/ae0dd648-5d61-4a22-a5c6-2c562c062643)

 
