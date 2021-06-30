#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>

#define NUM_ITEMS 6


int pid, msglen, status;

void mergeSort(int numbers[], int temp[], int array_size);
void m_sort(int numbers[], int temp[], int left, int right);
void merge(int numbers[],int numbers2[], int temp[], int left, int mid, int right);

int numbers[NUM_ITEMS];
int numbers2[NUM_ITEMS];
int temp[NUM_ITEMS];
int temp2[NUM_ITEMS];
int fd1[2];



int main()
{
  int i;

  //seed random number generator
  srand(getpid());

  //fill array with random integers
  for (i = 0; i < NUM_ITEMS; i++){
    numbers[i] = rand();
   numbers[i]=numbers[i]%100;
  }

  printf("------------------------------\n");
  for (i = 0; i < NUM_ITEMS; i++){
    printf("%i\n", numbers[i]);
  } 



/*if (pipe(fd1) == -1) {
perror("pipe failed.");
exit(1);
}*/

  //perform merge sort on array
  mergeSort(numbers, temp, NUM_ITEMS);

  

  printf("------------------------------\n");
  printf("Done with sort.\n");
  for (i = 0; i < NUM_ITEMS; i++){
    printf("%i\n", numbers[i]);
  } 
 
  return 0;
}


void mergeSort(int numbers[], int temp[], int array_size)
{
if (pipe(fd1) == -1) {
perror("pipe failed.");
exit(1);
}

if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}

  m_sort(numbers, temp, 0, array_size - 1);
}



void m_sort(int numbers[], int temp[], int left, int right)
{
  int mid,i4;
   mid = (right + left) / 2;

  if (right > left)
  {
     if (pid == 0) { /* Child process */     
       m_sort(numbers, temp, mid+1, right);
         close(fd1[0]);
         
	if (write(fd1[1], numbers, NUM_ITEMS) == -1) {
		perror("child write.");
		exit(1);
	}
      printf("child end\n");
       exit(0);
      
       } else { /* Parent process */
       m_sort(numbers, temp, left, mid);
	
	close(fd1[1]);
	if (read(fd1[0], numbers2, NUM_ITEMS) == -1) {
	perror("pipe read.");
	exit(1);
	}
  printf("----------numbers--------------\n");
  for (i4 = 0; i4 < NUM_ITEMS; i4++){
    printf("%i\n", numbers[i4]);
  } 

  printf("----------numbers2-------------\n");
  for (i4 = 0; i4 < NUM_ITEMS; i4++){
    printf("%i\n", numbers2[i4]);
  }
   printf("                               \n");
   printf("%d,%d,%d\n",left,mid+1,right);
   printf("------------------------------\n");

     merge(numbers, numbers2,temp, left, mid+1, right);
      wait(&status);
      }



  /*  mid = (right + left) / 2;
    m_sort(numbers, temp, left, mid);
    m_sort(numbers, temp, mid+1, right);

    merge(numbers, temp, left, mid+1, right);*/
  }
else{
    printf("aaa\n");
    for (int i3 = 0; i3 < NUM_ITEMS; ++i3) {
         numbers2[i3] = numbers[i3];
    }
}

}


void merge(int numbers[],int numbers2[], int temp[], int left, int mid, int right)
{
  int i, left_end, num_elements, tmp_pos;

  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;

  while ((left <= left_end) && (mid <= right))
  {
    if (numbers[left] <= numbers2[mid])
    {
      temp[tmp_pos] = numbers[left];
      tmp_pos = tmp_pos + 1;
      left = left +1;
    }
    else
    {
      temp[tmp_pos] = numbers2[mid];
      tmp_pos = tmp_pos + 1;
      mid = mid + 1;
    }
  }

  while (left <= left_end)
  {
    temp[tmp_pos] = numbers[left];
    left = left + 1;
    tmp_pos = tmp_pos + 1;
  }
  while (mid <= right)
  {
    temp[tmp_pos] = numbers2[mid];
    mid = mid + 1;
    tmp_pos = tmp_pos + 1;
  }

  for (i=0; i <= num_elements; i++)
  {
    numbers[right] = temp[right];
    right = right - 1;
  }
}
