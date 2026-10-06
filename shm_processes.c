#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void DearOldDad(int *);
void PoorStudent(int *);

int main()
{
  int ShmID;
  int *ShmPTR;
  pid_t pid;
  int status;

  srand(time(NULL));

  ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
  if (ShmID < 0) 
  {
      printf("*** shmget error (server) ***\n");
      exit(1);
  }

  ShmPTR = (int *) shmat(ShmID, NULL, 0);
  if (*ShmPTR == -1) 
  {
      printf("*** shmat error (server) ***\n");
      exit(1);
  }

  ShmPTR[0] = 0; // BankAccount = 0
  ShmPTR[1] = 1; // Turn = 0

  pid = fork();
  if (pid < 0) 
  {
      printf("*** fork error ***\n");
      exit(1);
  }
  else if (pid == 0) 
  {
      PoorStudent(ShmPTR);
      exit(0);
  }

  DearOldDad(ShmPTR);

  wait(&status);
  printf("Process has detected the completion of its child...\n");

  shmdt((void *) ShmPTR);
  printf("Process has detached its shared memory...\n");

  shmctl(ShmID, IPC_RMID, NULL);
  printf("Process has removed its shared memory...\n");

  printf("Process exits...\n");
  exit(0);
}

void DearOldDad(int SharedMem[])
{
  int i;
  int account;
  int balance;

  for (i = 0; i < 25; i++) 
  {
    sleep(rand() % 6);
    account = SharedMem[0];

    while (SharedMem[1] != 0);

    if (account <= 100)
    {
      balance = rand() % 101;
      if (balance % 2 == 0)
      {
        account += balance;
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, account);
      } 
      else 
      {
        printf("Dear old Dad: Doesn't have any money to give\n");
      }
    }
    else 
    {
      printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", account);
    }
    
    SharedMem[0] = account;
    SharedMem[1] = 1;
  }
}

void PoorStudent(int SharedMem[])
{
  int i;
  int account;
  int balance;

  for (i = 0; i < 25; i++) 
  {
    sleep(rand() % 6);
    account = SharedMem[0];

    while (SharedMem[1] != 1);

    balance = rand() % 51;
    printf("Poor Student needs $%d\n", balance);

    if (balance <= account)
    {
      account -= balance;
      printf("Poor Student: Withdraws $%d / Balance = $%d\n", balance, account);
    }
    else
    {
      printf("Poor Student: Not Enough Cash ($%d)\n", account);
    }

    SharedMem[0] = account;
    SharedMem[1] = 0;
  }
}