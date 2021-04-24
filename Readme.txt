
#########################################################################################################################
***********************************************Course Information********************************************************
                            	        CSE 522: Real Time Embedded Systems           
                                                Sem - Spring 2021                     
                                            Arizona State University                    
                                          Instructor: Dr. Yann-Hang Lee                 
*************************************************************************************************************************
#########################################################################################################################



#########################################################################################################################
----------------------------------------------Assignment Information-----------------------------------------------------

Author: Viraj Savaliya          ; ASU ID - 1217678787

Assignment 4 : Schedulability Analysis

Date : 04/23/2021

-------------------------------------------------------------------------------------------------------------------------
---------------------------------------Instructions to Compile and Execute-----------------------------------------------

How to execute:
1. From the command line do 'make' or 'make all' to generate an exceutable binary named 'assignment4' from c++ file in the directory
2. From the command line do './assignment4 inputfile outputfile'
3. The above command will run the binary, take 'inputfile' as name of input file and generate a new file named
   'outputfile' to store the analysis output
   E.g: ./assignment4 A4_test.txt Output_report.txt
        Here A4_test.txt is the input filename and Output_report.txt is the output filename
4. The generated binary 'assignment4' can also be deleted using 'make clean'

NOTE: Make sure the input file is present in the directory or else the program will exit with an error.

-------------------------------------------------------------------------------------------------------------------------
#########################################################################################################################



#########################################################################################################################
------------------------------------------------Sample Output------------------------------------------------------------

Command line:

viraj@ubuntu:~/RTES_github$ make
g++ -o assignment4 viraj.cpp -lm -Wall
viraj@ubuntu:~/RTES_github$ ./assignment4 A4_test.txt Output_report.txt


Output from outputfile:

================ CSE 522:RTES - Assignment 4 Output Report ================

**************** Input Tasksets ****************

Taskset 0 Task 0: WCET = 5.000000, Deadline = 12.000000, Period = 12.000000
Taskset 0 Task 1: WCET = 15.000000, Deadline = 54.000000, Period = 54.000000
Taskset 0 Task 2: WCET = 18.000000, Deadline = 72.000000, Period = 72.000000
Taskset 1 Task 0: WCET = 5.000000, Deadline = 12.000000, Period = 12.000000
Taskset 1 Task 1: WCET = 10.000000, Deadline = 54.000000, Period = 54.000000
Taskset 1 Task 2: WCET = 18.000000, Deadline = 40.000000, Period = 72.000000
Taskset 2 Task 0: WCET = 5.000000, Deadline = 30.000000, Period = 30.000000
Taskset 2 Task 1: WCET = 6.000000, Deadline = 40.000000, Period = 40.000000
Taskset 2 Task 2: WCET = 12.000000, Deadline = 72.000000, Period = 72.000000
Taskset 2 Task 3: WCET = 32.000000, Deadline = 156.000000, Period = 156.000000
Taskset 2 Task 4: WCET = 24.000000, Deadline = 240.000000, Period = 240.000000
Taskset 2 Task 5: WCET = 45.000000, Deadline = 320.000000, Period = 320.000000
Taskset 2 Task 6: WCET = 40.000000, Deadline = 768.000000, Period = 768.000000
Taskset 3 Task 0: WCET = 5.000000, Deadline = 30.000000, Period = 30.000000
Taskset 3 Task 1: WCET = 6.000000, Deadline = 36.000000, Period = 40.000000
Taskset 3 Task 2: WCET = 12.000000, Deadline = 72.000000, Period = 72.000000
Taskset 3 Task 3: WCET = 30.000000, Deadline = 60.000000, Period = 156.000000
Taskset 3 Task 4: WCET = 24.000000, Deadline = 240.000000, Period = 240.000000
Taskset 3 Task 5: WCET = 45.000000, Deadline = 180.000000, Period = 320.000000
Taskset 3 Task 6: WCET = 40.000000, Deadline = 768.000000, Period = 768.000000


**************** Ouput Analysis ****************


---------------- EDF Analysis ----------------

~~~~~~~~~~~~~ Taskset 0 ~~~~~~~~~~~~~
Utilisation= 0.944444
Taskset 0 schedulable using EDF, 
since utilisation <= 1 

~~~~~~~~~~~~~ Taskset 1 ~~~~~~~~~~~~~
Utilisation= 1.051852
Taskset 1 Deadline < Period, Utilisation > 1 
Taskset 1 need to do using Loading factor analysis
Taskset 1 Sync Busy period is 48
Taskset 1 No deadline missed, schedulable!

~~~~~~~~~~~~~ Taskset 2 ~~~~~~~~~~~~~
Utilisation= 0.981170
Taskset 2 schedulable using EDF, 
since utilisation <= 1 

~~~~~~~~~~~~~ Taskset 3 ~~~~~~~~~~~~~
Utilisation= 1.402083
Taskset 3 Deadline < Period, Utilisation > 1 
Taskset 3 need to do using Loading factor analysis
Taskset 3 Sync Busy period is 1508
Taskset 3 First deadline missed at 240, Not schedulable!


---------------- RM/DM Analysis ----------------

~~~~~~~~~~~~~ Taskset 0 ~~~~~~~~~~~~~
Utilisation= 0.944444
Taskset 0 Utilisation <= 1 and Deadline = Period, performing RM
Taskset 0 Utilisation Bound analysis not satisfied
Taskset 0 need to do using Response time analysis for RM
Task 2 failed utilisation bound, WCRT = 73, Period = 72.000000
Taskset 0 task 2 response time NOT schedulable

~~~~~~~~~~~~~ Taskset 1 ~~~~~~~~~~~~~
Utilisation= 1.051852
Taskset 1 utilisation > 1 -- RM / DM FAIL!

~~~~~~~~~~~~~ Taskset 2 ~~~~~~~~~~~~~
Utilisation= 0.981170
Taskset 2 Utilisation <= 1 and Deadline = Period, performing RM
Taskset 2 Utilisation Bound analysis not satisfied
Taskset 2 need to do using Response time analysis for RM
Task 4 failed utilisation bound, WCRT = 118, Period = 240.000000
Taskset 2 task 4, response_time 118 schedulable
Task 5 failed utilisation bound, WCRT = 352, Period = 320.000000
Taskset 2 task 5 response time NOT schedulable
Task 6 failed utilisation bound, WCRT = 802, Period = 768.000000
Taskset 2 task 6 response time NOT schedulable

~~~~~~~~~~~~~ Taskset 3 ~~~~~~~~~~~~~
Utilisation= 1.402083
Taskset 3 utilisation > 1 -- RM / DM FAIL!


================ END OF REPORT ================


-------------------------------------------------------------------------------------------------------------------------
#########################################################################################################################
