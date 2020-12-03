# ProcessCostCalc
Program that simultes a buffer using a queue, renders each process given to it and adds their cost. Programmed in C for Operating Systems in 2020.
This was done as a group project with Angel Rojas and Alejandro Rivero.

*Explanation*
First we developed the queue, which has the funciton of storing the process`s. This queue works as a cirular buffer. 
Then we develop the cost calculator. For this we implement a mutex with two variables that we will use to avoid the buffer from collapsing. Also two pointers that will direct to the queue and also allow to store the process. Each line of the code has more detailed explanations of what everything is doing, unfortunately it is in spanish as it was used for a spanish course. My first time programming with C. Enjoy!

