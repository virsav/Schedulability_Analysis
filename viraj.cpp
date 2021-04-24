#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <map>
using namespace std;

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

//Structure to store data for each Task
typedef struct
{
	int set_no;
	float wcet;
	float deadline;
	float period;
}taskInfo;

//Array of structures to store each Task from file
taskInfo *alltasks;

int num_tasksets, total_tasks;

//Array to store number of Tasks in each set
int *set_sizes;

//Using map to store all time instants in ascending order, Key-time, value-wcet
map<float, float> loadFactor;

FILE *fout;

/* Function to read all the data from input file */
void getTasksets(FILE *file_ptr)
{
	fscanf(file_ptr, "%d", &num_tasksets);
	//printf("Total tasksets : %d\n", num_tasksets);
	alltasks = (taskInfo *)malloc(total_tasks * sizeof(taskInfo));
	set_sizes = (int *)malloc(num_tasksets * sizeof(int));

	for(int i=0; i<num_tasksets; i++)
	{
		int num_tasks;
		fscanf(file_ptr, "%d", &num_tasks);
		//printf("Total tasks in %d : %d\n", i, num_tasks);
		total_tasks += num_tasks;
		alltasks = (taskInfo *)realloc(alltasks, total_tasks * sizeof(taskInfo));
		set_sizes[i] = num_tasks;

		for(int j=0; j<num_tasks; j++)
		{
			float wcet, deadline, period;
			alltasks[(total_tasks-num_tasks)+j].set_no = i;
			fscanf(file_ptr, "%f", &wcet);
			fscanf(file_ptr, "%f", &deadline);
			fscanf(file_ptr, "%f", &period);
			alltasks[(total_tasks-num_tasks)+j].wcet = wcet;
			alltasks[(total_tasks-num_tasks)+j].deadline = deadline;
			alltasks[(total_tasks-num_tasks)+j].period = period;
			// printf("Tasks %d in set %d : WCET = %d, Deadline = %d, Period = %d\n", j, i, 
			// 	alltasks[(total_tasks-num_tasks)+j].wcet, alltasks[(total_tasks-num_tasks)+j].deadline, 
			// 	alltasks[(total_tasks-num_tasks)+j].period);
		}
	}
}

/* Function to find the Taskset's first entry in alltasks array */
int find_alltasks_iter(int setno)
{
	int num;
	num = 0;

	while(setno!=alltasks[num].set_no){
		num++;
	}

	return num;
}

/* Function to compute the density function and get the utilisation value for n tasks */
float utilisation_check(int setno, int numtasks)
{
	float utilisation;

	int iter = find_alltasks_iter(setno);

	for(int i=0; i<numtasks; i++)
	{
		utilisation += ((float)alltasks[iter+i].wcet / (float)(min(alltasks[iter+i].deadline, alltasks[iter+i].period)));
	}

	return utilisation;
}

/* Function to verify if any tasks fails utilisation bound of n*(2^(1/n)-1) */
bool RM_DM_util_check(int setno, int numtasks)
{
	float bound, utilisation, exponent;
	//printf("N=%d\n", numtasks);
	for(int n=0; n<numtasks; n++)
	{
		utilisation = utilisation_check(setno, (n+1));
		exponent = (1/((float)(n+1)));
		bound = (pow(2,exponent)-1);
		bound = ((n+1)*(bound));
		//printf("U =%f B=%f\n",utilisation, bound);
		if(utilisation > bound)
		{
			return false;
		}
	}
	return true;
}

/* Function to check if deadline of all tasks equals period */
bool deadline_EQcheck(int setno, int numtasks)
{
	int iter = find_alltasks_iter(setno);

	for(int i=0; i<numtasks; i++)
	{
		if(alltasks[iter+i].deadline != alltasks[iter+i].period)
		{
			return false;
		}
	}

	return true;
}

/* Function to compute a single loading factor Summation of wcet*ceil(l(i-1)/period) */
uint32_t compute_load(uint32_t l0, uint32_t numtasks, uint32_t iter)
{
	uint32_t l1 = 0;

	for(uint32_t i=0; i<numtasks; i++)
	{
		float ceil_value;
		ceil_value = ((float)l0)/((float)alltasks[iter+i].period);
		l1 += ((alltasks[iter+i].wcet)*(ceil(ceil_value)));
	}

	return l1;
}

/* Function to compute the synchronous busy period by checking loading factor iterations */
uint32_t compute_sync_period(int setno)
{
	int numtasks = set_sizes[setno];
	float first_load;
	uint32_t l0, l1;
	int iter = find_alltasks_iter(setno);
	for(int i=0; i<numtasks; i++)
	{
		first_load += alltasks[iter+i].wcet;
	}
	l0 = first_load;
	l1 = compute_load(l0, numtasks, iter);

	//Stop when two consecutive iterations produce same loading factor
	while(l0 != l1)
	{
		l0 = l1;
		l1 = compute_load(l0, numtasks, iter);
	}

	return l0;
}

/* Function to verify utilisation is <= 1 at each time instants till synchrounous busy period */
uint32_t check_loading_factor(int setno, int sync_period)
{
	int numtasks = set_sizes[setno];
	loadFactor.clear();
	int iter = find_alltasks_iter(setno);

	//Iterator for map
	map<float, float>::iterator itr;

	//Populate map for all tasks
	for(int i=0; i<numtasks; i++)
	{
		float time = alltasks[iter+i].deadline;
		float wcet = alltasks[iter+i].wcet;
		float period = alltasks[iter+i].period;

		//Populate entry in map for all arrival times of the task less than sync busy period
		while(time <= sync_period)
		{
			itr = loadFactor.find(time);
			if (itr != loadFactor.end())
			{
				itr->second += wcet;		//Add the wcet to exisiting time instant
			}
			else
			{
				loadFactor.insert( pair<float, float>(time, wcet) );		//Create new entry if time not already present
			}
			
			time += period;
		}
	}

	float load = 0;

	//Verify Utilisation at each time instant is <= 1, if not return time instant
	for (auto& it : loadFactor) {
		float util;
		load += it.second;
		util = (load)/(it.first);
		//printf("Time =%f, H=%f, util=%f\n", it.first, load, util);
		if(util > 1)
		{
			return it.first;
		}
    }
	
	return 0;
}

/* Function to compute single response time except first variable Summation of wcet*ceil(a(i-1)/period) */
uint32_t compute_response(uint32_t a0, uint32_t taskno, uint32_t iter)
{
	uint32_t a1 = 0;

	for(uint32_t i=0; i<(taskno-1); i++)
	{
		float ceil_value;
		ceil_value = ((float)a0)/((float)alltasks[iter+i].period);
		a1 += ((alltasks[iter+i].wcet)*(ceil(ceil_value)));
	}

	return a1;
}

/* Function to compute response time iterations */
uint32_t compute_response_time(int setno, int taskno, int iter)
{
	float first_resp;
	uint32_t a0, a1;
	for(int i=0; i<taskno; i++)
	{
		first_resp += alltasks[iter+i].wcet;
	}
	a0 = first_resp;
	a1 = alltasks[iter+taskno-1].wcet + compute_response(a0, taskno, iter);

	while(1)
	{
		//If Two consecutive response time same or response time exceeds deadline
		if((a0 != a1)&&(a1 < alltasks[iter+taskno-1].deadline))
		{
			a0 = a1;
			a1 = alltasks[iter+taskno-1].wcet + compute_response(a0, taskno, iter);
		}
		else
		{
			fprintf(fout, "Task %d failed utilisation bound, WCRT = %d, Period = %f\n",taskno-1, a1, alltasks[iter+taskno-1].period);
			break;
		}
	}

	return a1;
}


/* Function to perform Response time analysis for Tasks failing Utilisation bound */
void response_time_analysis(int setno)
{
	int numtasks = set_sizes[setno];
	int iter = find_alltasks_iter(setno);
	uint32_t resp_time;
	for(int i=0; i<numtasks; i++)
	{
		if(!RM_DM_util_check(setno, i+1))
		{
			resp_time = compute_response_time(setno, i+1, iter);
			if(resp_time <= alltasks[iter+i].deadline)
			{
				fprintf(fout, "Taskset %d task %d, response_time %d schedulable\n", setno, i, resp_time);
			}
			else
			{
				fprintf(fout, "Taskset %d task %d response time NOT schedulable\n", setno, i);
			}
		}
	}
}

/* EDF Analysis Function  - 
1. Utilisation <= 1 check, yes -> schedulable
2. Utilisation <= 1 check, no - deadline = period -> not schedulable
3. Utilisation <= 1 check, no - deadline < period - loading factor analysis - no deadline missed -> schedulable
4. Utilisation <= 1 check, no - deadline < period - loading factor analysis - deadline missed -> not schedulable
 */
void EDF_analysis()
{
	fprintf(fout, "\n\n---------------- EDF Analysis ----------------\n");
	for(int i=0; i< num_tasksets; i++){
		float utilisation = utilisation_check(i, set_sizes[i]);
		fprintf(fout, "\n~~~~~~~~~~~~~ Taskset %d ~~~~~~~~~~~~~\n", i);
		fprintf(fout, "Utilisation= %f\n", utilisation);
		if(utilisation <= 1)
		{
			fprintf(fout, "Taskset %d schedulable using EDF, \nsince utilisation <= 1 \n", i);
		}
		else
		{
			if(deadline_EQcheck(i, set_sizes[i])){
				fprintf(fout, "Taskset %d NOT schedulable using EDF, \nSince utilisation > 1 and deadline = period \n", i);
			}
			else{
				fprintf(fout, "Taskset %d Deadline < Period, Utilisation > 1 \n", i);
				fprintf(fout, "Taskset %d need to do using Loading factor analysis\n", i);
				int sync_period;
				sync_period = compute_sync_period(i);
				fprintf(fout, "Taskset %d Sync Busy period is %d\n", i, sync_period);
				int deadline_miss;
				deadline_miss = check_loading_factor(i, sync_period);
				//printf("Taskset %d First deadline missed at %d\n", i, deadline_miss);
				if(deadline_miss)
				{
					fprintf(fout, "Taskset %d First deadline missed at %d, Not schedulable!\n", i, deadline_miss);
				}
				else
				{
					fprintf(fout, "Taskset %d No deadline missed, schedulable!\n", i);
				}
			}
		}
	}
}

/* RM/DM Analysis Function  - 
1. Utilisation <= 1 check, no -> RM/DM Fail
2. Utilisation <= 1 check, yes - deadline = period - RM - Utilisation satisfied for each Task -> schedulable
3. Utilisation <= 1 check, yes - deadline = period - RM - Utilisation fail for a Task - response time analysis -
   WCRT <= deadline -> schedulable
4. Utilisation <= 1 check, yes - deadline = period - RM - Utilisation fail for a Task - response time analysis -
   WCRT > deadline -> not schedulable
5. Utilisation <= 1 check, yes - deadline < period - DM - Utilisation satisfied for each Task -> schedulable
6. Utilisation <= 1 check, yes - deadline < period - DM - Utilisation fail for a Task - response time analysis -
   WCRT <= deadline -> schedulable
7. Utilisation <= 1 check, yes - deadline < period - DM - Utilisation fail for a Task - response time analysis -
   WCRT > deadline -> not schedulable
 */
void RM_DM_analysis()
{
	fprintf(fout, "\n\n---------------- RM/DM Analysis ----------------\n");
	for(int i=0; i<num_tasksets; i++){
		float utilisation = utilisation_check(i, set_sizes[i]);
		fprintf(fout, "\n~~~~~~~~~~~~~ Taskset %d ~~~~~~~~~~~~~\n", i);
		fprintf(fout, "Utilisation= %f\n", utilisation);
		if(utilisation <= 1)
		{
			if(deadline_EQcheck(i, set_sizes[i])){
				fprintf(fout, "Taskset %d Utilisation <= 1 and Deadline = Period, performing RM\n", i);
				if(RM_DM_util_check(i, set_sizes[i]))
				{
					fprintf(fout, "Taskset %d schedulable using RM \nSince Utilisation Bound analysis Passed\n", i);
				}
				else
				{
					fprintf(fout, "Taskset %d Utilisation Bound analysis not satisfied\n", i);
					fprintf(fout, "Taskset %d need to do using Response time analysis for RM\n", i);
					response_time_analysis(i);
				}
			}
			else
			{
				fprintf(fout, "Taskset %d Utilisation <= 1 and Deadline < Period, performing DM\n", i);
				if(RM_DM_util_check(i, set_sizes[i]))
				{
					fprintf(fout, "Taskset %d schedulable using DM \nSince Utilisation Bound analysis Passed\n", i);
				}
				else
				{
					fprintf(fout, "Taskset %d Utilisation Bound analysis not satisfied\n", i);
					fprintf(fout, "Taskset %d need to do using Response time analysis for DM\n", i);
					response_time_analysis(i);
				}
			}
		}
		else
		{
			fprintf(fout, "Taskset %d utilisation > 1 -- RM / DM FAIL!\n", i);
		}
	}
}

/* Main Function */
int main(int argc, char** argv)
{
	string inputFile = "";
    string outputFile = "";
    if( argc == 3 ) {
      inputFile = argv[1];
      outputFile = argv[2];
    }
    else {
      cout << "Usage: ./assignment4 inputfile outputfile\n";
      return 1;
    }

    const char* fileIn = inputFile.c_str();
    const char* fileOut = outputFile.c_str();

	FILE *fptr;

	fptr = fopen(fileIn,"r");

   	if(fptr == NULL)
   	{
   		printf("Error! - No file named A4_test.txt");   
      	exit(1);             
   	}

   	num_tasksets = 0;
   	total_tasks = 0;

   	getTasksets(fptr);

   	fclose(fptr);

   	fout = fopen(fileOut,"w");

   	fprintf(fout, "================ CSE 522:RTES - Assignment 4 Output Report ================\n\n");
   	fprintf(fout, "**************** Input Tasksets ****************\n\n");

   	int set0, task;
   	set0 = 0;
   	task = 0;
   	for(int i=0; i<total_tasks; i++)
	{
		if(set0 != alltasks[i].set_no)
		{
			task = 0;
		}
		fprintf(fout, "Taskset %d Task %d: WCET = %f, Deadline = %f, Period = %f\n", alltasks[i].set_no, task,
			alltasks[i].wcet, alltasks[i].deadline, alltasks[i].period);
		task++;
		set0 = alltasks[i].set_no;
	}

   	fprintf(fout, "\n\n**************** Ouput Analysis ****************\n");

	EDF_analysis();
	RM_DM_analysis();

   	fprintf(fout, "\n\n================ END OF REPORT ================\n\n");

	fclose(fout);

   return 0;
}