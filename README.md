# Voronoi implementation

## Build

```shell script
make build
```

## Run

```shell script
./dist/VoronoiBrut --rowCount 300 --columnCount 300 --germs 100 --distanceType 1

# Distance
# 0 -> de(P, P')
# 1 -> d1(P, P')
# 2 -> d∞(P, P')

Execution done in 286 ms and 9000000 iterations. Create a 300 x 300 (90000 pixels) image with 100 germs and distance D1


./dist/VoronoiSequential --columnCount 1200 --rowCount 1200 --germs 10000

Execution done in 1176 ms and 14385604 iteration. 
Create a 1200 x 1200 (1440000 pixels) image with 1000 germs. 
0.069444% of the points in this image are germs.
```