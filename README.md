# beta4
Partition (and Knapsack) problem
<br><br>
This repo focuses mainly on the Partition problem but it can do Knapsack as well.
<br>
To use the program to solve the Knapsack problem instead uncomment the line
```c
#define KNAPSACK 60
```
in the part.h header file. The define value is the maximum weight of the knapsack.
<br><br>
### <ins>VERSION DESCRIPTION</ins>
|version|description|
|-----------------------|-----------------|
|**v1**|single threaded brute force method|
|**v2**|multi threaded brute force method|
|**v3**|multi threaded genetic algorithm method|
|**v4**|multi threaded genetic algorithm on multiple machines in a distributed network|

<br><br>
### <ins>BUILDING</ins>
The only user side dependency of this project is a compiler. Using gcc (mingw) you will be able to successfully compile this repository on Windows and *NIX systems. All other dependencies are bundled with the project.

To build any of the versions run the make command:
```bash
make v<version number>
```
so for version 1 the command would be:
```bash
make v1
```

you will be left with an executable file in the ```bin``` folder
<br><br>
### <ins>USAGE</ins>
Each of the executable files will tell you its usage when you run it with insufficient number of arguments. Here's a quick rundown.

#### v1
```bash
./beta4-v1.0 <number of items>
```

#### v2
```bash
./beta4-v2.0 <number of items>
```

#### v3
```bash
./beta4-v3.0 <n items> <max gen> <per gen>
```
```<n items>``` - number of items<br>
```<max gen>``` - maximum number of generations to simulate per thread<br>
```<per gen>``` - population number per generation<br>

#### v4
```bash
./beta4-v4.0 <COMMANDER|WORKER> <IP ADDRESS> <PORT> [COMMANDER: N ITEMS] [COMMANDER: MAX GEN] [COMMANDER: PER GEN]
```
The usage of version 4 is a bit more complicated.<br>
The network always needs to have exactly one COMMANDER program and at least one WORKER program to operate.<br>WORKER programs actually perform the computations while the COMMANDER program manages all of the WORKER programs. COMMANDER program specifies the<br>
```[COMMANDER: N ITEMS]``` - number of items<br>
```[COMMANDER: MAX GEN]``` - maximum number of generations to simulate per thread<br>
```[COMMANDER: PER GEN]``` - population number per generation<br>
which the WORKER program receives on connect.<br>
When you are done connecting all WORKER programs to the COMMANDER program press R (needs to be capital, hold down shift) and then ENTER. This will send a RUN command to all the WORKER programs. When a WORKER program is done with simulating the specified number of generations it sends its best result to the COMMANDER program. Once all WORKER programs are done simulating the final "best" solution is printed out on the COMMANDER program which exits right after, disconnecting all the WORKER programs. To exit the COMMANDER program a ctrl + c won't do, because of the non-blocking input that runs in a separate thread. To exit the COMMANDER program when needed press Q (needs to be capital, again) and then ENTER.







