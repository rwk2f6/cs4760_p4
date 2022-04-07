Author: Ryan Kelly

Class: Computer Science 4760 - Operating Systems

School: University of Missouri - Saint Louis

Assignment: Project 4 - OSS Simulator

Due Date: 4/7/2022

Language Used: C

Description: A process named oss forks off children based off of a built in system clock that iterates in nanoseconds. The oss process should fork children and schedule them to run by sending them a message. The children should then run, either finishing, requiring more time to run on the system, or randomly terminate. If the process needs more time to run, it will be put in another queue with a longer time quantum. The process should fork up only 18 children at a time, but can generate a total of 50 processes.

Build Instructions (Linux Terminal): make 

Delete oss and uprocess Executables and osslogfile: make clean 

How to Invoke: ./oss

Note: This project isn't complete, as I didn't give myself enough time to finish it. It is able to fork children and send messages to them, as well as receive messages back. I am going to focus on Test 2 and Project 5 so I can do a better job on them.