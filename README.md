# Queing-Theory
C++ program to simulate a queuing system consisting of 2 oncatenated systems:

First of all, there is a queue with a finite length of K1, following a Poisson process (memoryless) with an arrival rate of λ elements/time unit. The first server has an exponential (memoryless) service time distribution with a service rate of µ1 elements/time unit (in this example 2 elements/time unit).This first part of the system is analog to an M/M/1/K system following the Kendall’s notation.

Afterwards there is the queue of a second server which has a length of K2. The output rate of the first server will determine the arrival rate to the second queue (not memoryless).The second server has a µ2 service rate (in this example 1 element/time unit).

Given an arrival rate introduced by the user, this programs calculateds the following parameters:

•	Blocked elements in queue 1	
•	Blocking Probability Queue 1	
•	Theoretical BP Queue 1	
•	Blocked elements in queue 2	
•	Blocking Probability Queue 2	
•	Theoretical BP Queue 2	
•	Total Blocking Probability	
•	Throughput rate	
•	Mean nº of elements in the system	
•	Mean Sojourn Time	
•	Server 1 utilization rate	
•	Server 2 utilization rate

And stores all values on a txt file.
