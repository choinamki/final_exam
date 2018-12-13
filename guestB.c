#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define MAXBUF 2048
#define KEY 4859

typedef struct{
	int a[4]; // 0~2 index == 0~2 horse moeny
	int b[4]; // 3 index == host N money
	int c[4];

	int horse[3];              // horse distance

	int game_start; // game start flag
	int betting_start;// betting start flag
	int join_flag[3];//
} t_message;

static pthread_t p_thread[2];
static void *addr;

void *betting_function(void *);
void *cheak_betting_start(void *);
int main()
{
	int thr_id;
	int round = 0;
	int i;
	int temp = 0;
	int id; //shm id
	
	// make_shared_memory

	if(-1 == (id=shmget((key_t)KEY,MAXBUF,IPC_CREAT|0666)))
	{
		perror("shmget error : ");
		exit(1);
	}

	// shared_memory point

	if((void*)-1 == (addr=shmat(id,NULL,0)))
	{
		perror("shmat error : ");
		exit(-1);
	}

	//game_start_check
	if((*(t_message*)addr).game_start == 1)
	{
		printf("can't playing\n");
		exit(-1);
	}
	else
	{
		(*(t_message*)addr).join_flag[1] = 1; // B join
	}
	//gamestart
	system("clear");
	printf("game start\n\n");
	sleep(1);
	
	while((*(t_message*)addr).betting_start != 2)
	{
	}
	for(i = 0; i<3; i++)
	{
		temp += (*(t_message*)addr).join_flag[i];
	}
	if(temp == 1)
	{
		printf("one man joined you win\n");
		exit(0);
	}
	sleep(1);
	
	while(round <3)
	{
		if((*(t_message*)addr).a[3] == 0 && (*(t_message*)addr).c[3] == 0)
		{
			printf("one man live you win\n");
			exit(0);
		}
	// make thread
		thr_id = pthread_create(&p_thread[0], NULL, betting_function, NULL);
		if(thr_id < 0)
		{
			perror("betting_function thread error : ");
			exit(-1);
		}
	
		thr_id = pthread_create(&p_thread[1], NULL, cheak_betting_start, NULL);
		if(thr_id < 0)
		{
			perror("cheack_betting_function thread error : ");
			exit(-1);
		}
	//do thread
		pthread_join(p_thread[0], NULL);
		pthread_join(p_thread[1], NULL);

	//start horse run
		printf("clear\n");
		sleep(3);
		printf("horse run\n");
	
		while((*(t_message*)addr).horse[0] != 0 || (*(t_message*)addr).horse[1] != 0 || (*(t_message*)addr).horse[2] != 0)
		{
			for(i = 0; i<3 ; i++)
			{
				printf("horse%d : %d ", i+1,(*(t_message*)addr).horse[i]);
			}
			printf("\n");
			sleep(1);
		}
	
		printf("clear game\n");
		sleep(2);
		round = round + 1;
		system("clear");
		if((*(t_message*)addr).b[3] == 0)
		{
			printf("you don't have money\n");
			exit(0);
		}
	}
	printf("game over your result\n");
	sleep(3);
	printf("%d is your money\n", (*(t_message*)addr).b[3]);

	if((*(t_message*)addr).b[3] >= (*(t_message*)addr).a[3] && (*(t_message*)addr).b[3] >= (*(t_message*)addr).c[3])
	{
		printf("you win\n");
	}
	else
	{
		printf("you loose\n");
	}
	return 0;
}

void *betting_function(void *data)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	int i = 0;
	int id;
	int input = 0;
	
	printf("start betting\n");

	while(i<3)
	{
		printf("you have %d money\n",(*(t_message*)addr).b[3]);
		printf("horse %d :", i+1);
		scanf("%d", &input);
		if(input > (*(t_message*)addr).b[3])
		{
			printf("you can't betting\n");
			continue;
		}
		(*(t_message*)addr).b[i] = input;
		(*(t_message*)addr).b[3] = (*(t_message*)addr).b[3] - input;
		i = i + 1;
	}
	while(1)
	{
		printf("wait\n");
		sleep(3);
	}
}
void *cheak_betting_start(void *data)
{
	int id;
	//cheack game start?
	while((*(t_message*)addr).betting_start != 1)
	{
		
	}
	printf("betting exit\n");
	pthread_cancel(p_thread[0]); // strop betting	
}
