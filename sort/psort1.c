#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/types.h>

#define NUM_ITEMS 10


int pid, msglen, status;

void mergeSort(int numbers[], int temp[], int array_size);
void m_sort(int numbers[], int temp[], int left, int right);
void merge(int numbers[], int temp[], int left, int mid, int right);

int numbers[NUM_ITEMS];

int numbers3[NUM_ITEMS];
int temp[NUM_ITEMS];
int fd1[2];



int main()
{
  int i;

  //seed random number generator
  srand(getpid());

  //fill array with random integers
  for (i = 0; i < NUM_ITEMS; i++){
    numbers[i] = rand();
   numbers[i]=numbers[i];
  }



  printf("-------given numbers----------\n");
  for (i = 0; i < NUM_ITEMS; i++){
    printf("%i\n", numbers[i]);
  } 


  //perform merge sort on array
  mergeSort(numbers, temp, NUM_ITEMS);

  

  printf("--------Done with sort---------\n");
 
  for (i = 0; i < NUM_ITEMS; i++){
    printf("%i\n", numbers[i]);
  } 
 
  return 0;
}


void mergeSort(int numbers[], int temp[], int array_size)
{
int mid,left,right,i,len;


if (pipe(fd1) == -1) {
perror("pipe failed.");
exit(1);
}

if ((pid=fork())== -1) {
perror("fork failed.");
exit(1);
}
mid=(array_size-1)/2;

 


     if (pid == 0) { /* Child process */     
      m_sort(numbers, temp, mid+1, array_size - 1);
        

         close(fd1[0]);        
	if (write(fd1[1], numbers,sizeof(numbers3)) == -1) {
		perror("child write.");
		exit(1);
	}



       exit(0);      
       } else { /* Parent process */
      m_sort(numbers, temp, 0, mid);

       wait(&status);
	close(fd1[1]);
	if (read(fd1[0], numbers3, sizeof(numbers3)+10) == -1) {
	perror("pipe read.");
	exit(1);
	} 


    for(i=mid+1;i<array_size;i++){
 	numbers[i]=numbers3[i];
     }
     merge(numbers,temp, 0, mid+1, array_size-1);

      //wait(&status);
      }


}



void m_sort(int numbers[], int temp[], int left, int right)
{
  int mid,i4;
   mid = (right + left) / 2;

  if (right > left)
  {

   mid = (right + left) / 2;
    m_sort(numbers, temp, left, mid);
    m_sort(numbers, temp, mid+1, right);

    

    merge(numbers,temp, left, mid+1, right);

 }

}


void merge(int numbers[], int temp[], int left, int mid, int right)
{
  int i, left_end, num_elements, tmp_pos;

  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;



  while ((left <= left_end) && (mid <= right))
  {
    if (numbers[left] <= numbers[mid])
    {
      temp[tmp_pos] = numbers[left];
      tmp_pos = tmp_pos + 1;
      left = left +1;
    }
    else
    {
      temp[tmp_pos] = numbers[mid];
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
    temp[tmp_pos] = numbers[mid];
    mid = mid + 1;
    tmp_pos = tmp_pos + 1;
  }

  for (i=0; i <= num_elements; i++)
  {
    numbers[right] = temp[right];
    right = right - 1;
  }
 
}
