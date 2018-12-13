#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>

#include<time.h>

#include<signal.h>
#include<unistd.h>

#define MAXBUF 2048
#define KEY 4859

static int waitting = 1; // wait func

typedef struct{
	int a[4]; // 0~2 index == 0~2 horse_money
	int b[4]; // 3 index == host N money
	int c[4];

	int horse[3];        // horse distance

	int game_start;     // game start flag
	int betting_start; // betting start flag
	int join_flag[3];
} t_message;

void timer(int sig)
{
	waitting = 0;
}

int main()
{
	int round = 0; //round 
	int i;
	int id; //shm id
	int horse_run = 0; // horse distance = horse_distance - horse_run
	void *addr;
	int temp = 0;       //temp

	int a_result; // a_money_result
	int b_result;
	int c_result;
	int random;
	int ranking[3] = {0, 0, 0};
	int ranking_count = 1;
	struct sigaction sa;
	
	// make_shared_memory

	if(-1==(id=shmget((key_t)KEY,MAXBUF,IPC_CREAT|0666)))
	{
		perror("shmget error : ");
		exit(1);
	}

	// shared_memory point

	if((void*)-1 == (addr=shmat(id,NULL,0)))
	{
		perror("shmat error : ");
		exit(1);
	}
	
	// reset_shm_value

	(*(t_message*)addr).a[3] = 100;
	(*(t_message*)addr).b[3] = 100;
	(*(t_message*)addr).c[3] = 100;
	(*(t_message*)addr).game_start = 0;
	(*(t_message*)addr).betting_start = 0;

	for(i = 0; i<3; i++)
	{
			(*(t_message*)addr).horse[i] = 100;
	}
	

	for(i = 0; i<3; i++)
	{
		(*(t_message*)addr).a[i] = 0;
		(*(t_message*)addr).b[i] = 0;
		(*(t_message*)addr).c[i] = 0;
		(*(t_message*)addr).join_flag[i] = 0;
	}	
	


	// start game
	printf("start game wait 5sec\n");
	sleep(5);
	system("clear");
	(*(t_message*)addr).game_start = 1;
	(*(t_message*)addr).betting_start = 2;
		

	for(i = 0; i<3; i++)
	{
		temp += (*(t_message*)addr).join_flag[i];
	}
	if(temp == 0)
	{
		printf("everyone do not joined\n");
		if(shmctl(id, IPC_RMID, 0) == -1)
		{
			perror("shmctl failed");
			exit(-1);
		}
		exit(0);
	}
	if(temp == 1)
	{
		if(shmctl(id, IPC_RMID, 0) == -1)
		{
			perror("shmctl failed");
			exit(-1);
		}
		printf("one man joined\n");
		exit(0);
	}
	while(round <3)
	{
		//10sec wait
		printf("betting time wait 10sec\n");
		sigemptyset(&sa.sa_mask);
		sa.sa_handler = timer;
		if(sigaction(SIGALRM, &sa, NULL) == -1)
		{
			perror("sigaction error : ");
			exit(-1);	
		}
	
		i = 10;
		alarm(10);
		while(waitting != 0)
		{
			printf("%d sec\n",i);
			i = i-1;
			sleep(1);
		}
		waitting = 1;
	
		(*(t_message*)addr).betting_start = 1;

		printf("stop betting time\n");
		sleep(1);
		printf("game start\n");
		sleep(3);
		printf("horse run\n");

		//rand_seed
		srand(time(NULL));
	
		while((*(t_message*)addr).horse[0] != 0 || (*(t_message*)addr).horse[1] != 0 || (*(t_message*)addr).horse[2] || 0 )
		{
			for(i= 0; i<3; i++)
			{
				if((*(t_message*)addr).horse[i] > 0)
				{
					random = rand()%9+1;
					horse_run = (*(t_message*)addr).horse[i] - random;
					
					if(horse_run <=0)
					{
						ranking[i] = ranking_count;
						ranking_count = ranking_count + 1;
						(*(t_message*)addr).horse[i] = 0;
					}
					else
					{
						(*(t_message*)addr).horse[i] = horse_run;
					}
				}
			}
			sleep(1);
		}
		printf("clear game\n");
		sleep(1);
		system("clear");
		for(i = 0; i<3; i++)
		{
			if(ranking[i] == 1)
			{
				printf("horse %d win \n", i+1);
			}
		}

		(*(t_message*)addr).betting_start = 0;

		//result
		a_result = (*(t_message*)addr).a[3]; 
		b_result = (*(t_message*)addr).b[3];
		c_result = (*(t_message*)addr).c[3];

		for(i = 0; i<3; i++)
		{
			if(ranking[i] == 1)
			{
				a_result = a_result +((*(t_message*)addr).a[i] * 2);
				b_result = b_result +((*(t_message*)addr).b[i] * 2);
				c_result = c_result +((*(t_message*)addr).c[i] * 2);
				(*(t_message*)addr).a[3] = a_result;
				(*(t_message*)addr).b[3] = b_result;
				(*(t_message*)addr).c[3] = c_result;
			}
		}
		printf("result sent\n");
		//reset
		for(i = 0; i<3; i++)
		{
			(*(t_message*)addr).a[i] = 0;
			(*(t_message*)addr).b[i] = 0;
			(*(t_message*)addr).c[i] = 0;
		}	
		for(i = 0; i<3; i++)
		{
			(*(t_message*)addr).horse[i] = 100;
		}	
		for(i = 0; i<3; i++)
		{
			ranking[i] = 0;
		}
		ranking_count = 0;
		round = round +1;
	}

	printf("game over\n");
	(*(t_message*)addr).game_start = 0;

	if(shmctl(id, IPC_RMID, 0) == -1)
	{
		perror("shmctl failed");
		exit(-1);
	}
	return 0;
}
