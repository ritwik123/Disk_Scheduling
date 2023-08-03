#include<stdio.h>       // For standard funtions
#include<stdlib.h>      // For exit function
#include<pthread.h>     // For thread functions
#include<ctype.h>
#include<string.h>

#define MAX 1000        // For RequestQueue Declaration

struct RQthread
{
   int numberOfRequests;
   int RequestQueue[MAX];
   int initialHeadPosition;
   int diskSize;
   int moveDirection; 
};

FILE *fa;
char ca;
char buf[MAX];
int bufIndex=0;
int HeadMovement[6];
char *algos[] = {"FCFS", "SSTF", "SCAN", "C-SCAN", "LOOK", "C-LOOK"};


/**
 * @brief function to convert a string value read from input file to integer value 
 * 
 * @param *str, string
 */
int str_to_int(char *str)
{
    int num=0;
    for(int i=0;i<strlen(str);i++)
        num = num*10 + (str[i]-'0');
    return num;
}


/**
 * @brief function to obtain the next number from the input file
 */ 
int getNextNum()
{   
    if(isdigit(ca))
    {   
        bzero(buf,sizeof(buf));
        while(isdigit(ca))
        {
            buf[bufIndex++] = ca;
            ca = getc(fa);
        }
        buf[bufIndex] = '\0';
        bufIndex=0;
        int out = str_to_int(buf); 
        return out;
    }
    else
        ca = getc(fa);
}


/**
 * @brief function to print the least head movement among all algorithms
 */ 
void printLeastHeadMovement()
{
    int *min_index = malloc(6*sizeof(int));
    int min = 0;
    int n=0;
    for(int i=1;i<6;i++)
    {
        if(HeadMovement[i]<=HeadMovement[min])
            min = i;
    }

    for(int i=0;i<6;i++)
    {
        if(HeadMovement[i] == HeadMovement[min])
            min_index[n++] = i; 
    }

    printf("\nThe Algorithms with least Head movements is/are: ");
    for(int i=0;i<n;i++)
        printf("%s ",algos[min_index[i]]);
    printf("\n");
}


/**
 * @brief function to interchange the value of two variables 
 * 
 * @param pointer to both variables
 */
void swap(int *x,int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}


/**
 * @brief Sorts in ascending order a given number of requests using bubble sort
 * 
 * @param size
 * @param array 
 * */
void BubbleSort(int size,int array[])
{
    int i,j;
    int temp;
    for(i=0;i<size;i++)
    {
        for(j=0;j<size-i-1;j++)
        {
            if(array[j]>array[j+1])
            {
                swap(&array[j],&array[j+1]);
            }
        }
    }
}


/**
 * @brief Implements FCFS Disc Scheduling Algorithm.
 * 
 * @param *args
 */
void* FCFS(void* args)
{
    int totalHeadMovement = 0;
    int RequestQueue[MAX];
    int i;
    struct RQthread *th = (struct RQthread*)args;

    int numberOfRequests = th->numberOfRequests;
    for(int i=0;i<numberOfRequests;i++)
        RequestQueue[i] = th->RequestQueue[i];
    int initialHeadPosition = th->initialHeadPosition;

    printf("\nIn FCFS:\nCurrent Head Position\n%d\t",initialHeadPosition);

    for(i=0;i<numberOfRequests;i++)
    {
        totalHeadMovement += abs(RequestQueue[i]-initialHeadPosition);
        initialHeadPosition = RequestQueue[i];
        printf("%d\t",initialHeadPosition);
    }

    HeadMovement[0] = totalHeadMovement;
    printf("\nTotal head movement = %d\n",totalHeadMovement);
}


/**
 * @brief Implements SSTF Disc Scheduling Algorithm.
 * 
 * @param *args
 */
void* SSTF(void* args)
{
    int totalHeadMovement = 0;
    int RequestQueue[MAX];
    int count = 0;
    int i;
    struct RQthread *th = (struct RQthread*)args;
    
    int numberOfRequests = th->numberOfRequests;
    for(int i=0;i<numberOfRequests;i++)
        RequestQueue[i] = th->RequestQueue[i];
    int initialHeadPosition = th->initialHeadPosition;

    printf("\nIn SSTF:\nCurrent Head Position\n%d\t",initialHeadPosition);
   
    // loop will execute until all process is completed
    while(count!=numberOfRequests)
    {
        int min=10000;                                       // 10000 is for max
        int diff;
        int index;
        for(i=0;i<numberOfRequests;i++)
        {
           diff=abs(RequestQueue[i]-initialHeadPosition);
           if(min>diff)
           {
               min=diff;
               index=i;
           } 
        }

        totalHeadMovement += min;
        initialHeadPosition=RequestQueue[index];
        printf("%d\t",initialHeadPosition);
        RequestQueue[index]=10000;
        count++;
    } 

    HeadMovement[1] = totalHeadMovement;
    printf("\nTotal head movement = %d\n",totalHeadMovement);
}


/** @brief Implements SCAN Disc Scheduling Algorithm.
 * SCAN Algorithm scans all the cylinders of the disk starting from one end to the other end even if there are no requests at the ends. 
 * 
 * @param *args
 */
void* SCAN(void* args)
{
    int totalHeadMovement=0;
    int RequestQueue[MAX];
    int index;
    int i,j;
    struct RQthread *th = (struct RQthread*)args;
    
    int numberOfRequests = th->numberOfRequests;
    for(int i=0;i<numberOfRequests;i++)
        RequestQueue[i] = th->RequestQueue[i];
    int initialHeadPosition = th->initialHeadPosition;
    int diskSize = th->diskSize;
    int moveDirection = th->moveDirection;

    printf("\nIn SCAN:\nCurrent Head Position\n%d\t",initialHeadPosition);
    
    BubbleSort(numberOfRequests,RequestQueue);
    for(i=0;i<numberOfRequests;i++)
    {
        if(initialHeadPosition<RequestQueue[i])     // first value greater than initialHeadPosition
        {
            index=i;
            break;
        }
    }
   
    // if movement is towards high value
    if(moveDirection==1)
    {
        for(i=index;i<numberOfRequests;i++)
        {
            totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i];
            printf("%d\t",initialHeadPosition);
        }
        
        totalHeadMovement+=abs(diskSize-RequestQueue[i-1]-1);   // movement required to get to the end of disk from the last request 
        initialHeadPosition = diskSize-1;                       // at rightmost end of the disc
        printf("%d\t",initialHeadPosition);

        for(i=index-1;i>=0;i--)
        {
             totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
             initialHeadPosition=RequestQueue[i];
             printf("%d\t",initialHeadPosition);
        }
    }
    // if movement is towards low value
    else
    {
        for(i=index-1;i>=0;i--)
        {
            totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i];
            printf("%d\t",initialHeadPosition);
        }
        
        //  last movement for min size 
        totalHeadMovement+=abs(RequestQueue[i+1]-0);          // movement required to get to the start of disk from the first request
        initialHeadPosition = 0;                              // at leftmost end of the disc
        printf("%d\t",initialHeadPosition);

        for(i=index;i<numberOfRequests;i++)
        {
             totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
             initialHeadPosition=RequestQueue[i];
             printf("%d\t",initialHeadPosition);    
        }
    }

    HeadMovement[2] = totalHeadMovement;
    printf("\nTotal head movement = %d\n",totalHeadMovement);
}


/**
 * @brief Implements C-SCAN Disc Scheduling Algorithm.
 * C-SCAN stands for Circular SCAN
 * 
 * @param *args
 */
void* C_SCAN(void* args)
{
    int totalHeadMovement=0;
    int RequestQueue[MAX];
    int index;
    int i,j;
    struct RQthread *th = (struct RQthread*)args;
    
    int numberOfRequests = th->numberOfRequests;
    for(int i=0;i<numberOfRequests;i++)
        RequestQueue[i] = th->RequestQueue[i];
    int initialHeadPosition = th->initialHeadPosition;
    int diskSize = th->diskSize;
    int moveDirection = th->moveDirection;

    printf("\nIn C-SCAN:\nCurrent Head Position\n%d\t",initialHeadPosition);
  
    BubbleSort(numberOfRequests,RequestQueue);

    for(i=0;i<numberOfRequests;i++)
    {
        if(initialHeadPosition<RequestQueue[i])             // first value greater than initialHeadPosition
        {
            index=i;
            break;
        }
    }
   
    // if movement is towards high value
    if(moveDirection==1)
    {
        for(i=index;i<numberOfRequests;i++)
        {
            totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i];
            printf("%d\t",initialHeadPosition);    

        }
        
        initialHeadPosition=diskSize-1;
        printf("%d\t",initialHeadPosition); 
        totalHeadMovement+=abs(diskSize-RequestQueue[i-1]-1);   //  last movement for max size 
        totalHeadMovement+=abs(diskSize-1-0);                   //movement max to min disk 
        initialHeadPosition=0;                                  // moving initialHeadPosition to the start 
        printf("%d\t",initialHeadPosition);    

        for( i=0;i<index;i++)
        {
             totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
             initialHeadPosition=RequestQueue[i];
             printf("%d\t",initialHeadPosition);       
        }
    }
    // if movement is towards low value
    else
    {
        for(i=index-1;i>=0;i--)
        {
            totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i];
            printf("%d\t",initialHeadPosition);    
        }

        initialHeadPosition=0;
        printf("%d\t",initialHeadPosition); 
        totalHeadMovement+=abs(RequestQueue[i+1]-0);        //  last movement for min size 
        totalHeadMovement+=abs(diskSize-1-0);               //movement min to max disk    
        initialHeadPosition=diskSize-1;                     // moving initialHeadPosition to the end   
        printf("%d\t",initialHeadPosition);    

        for(i=numberOfRequests-1;i>=index;i--)
        {
             totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
             initialHeadPosition=RequestQueue[i];
             printf("%d\t",initialHeadPosition);         
        }
    }

    HeadMovement[3] = totalHeadMovement;
    printf("\nTotal head movement = %d\n",totalHeadMovement);
}


/**
 * @brief Implements LOOK Disc Scheduling Algorithm.
 * LOOK Algorithm scans all the cylinders of the disk starting from the first request at one end to the last request at the other end.
 * 
 * @param *args
 */
void* LOOK(void* args)
{
    int totalHeadMovement=0;
    int RequestQueue[MAX];
    int index;
    int i,j;
    struct RQthread *th = (struct RQthread*)args;
    
    int numberOfRequests = th->numberOfRequests;
    for(int i=0;i<numberOfRequests;i++)
        RequestQueue[i] = th->RequestQueue[i];
    int initialHeadPosition = th->initialHeadPosition;
    int diskSize = th->diskSize;
    int moveDirection = th->moveDirection;

    printf("\nIn LOOK:\nCurrent Head Position\n%d\t",initialHeadPosition);
    
    BubbleSort(numberOfRequests,RequestQueue);

    for(i=0;i<numberOfRequests;i++)
    {
        if(initialHeadPosition<RequestQueue[i])                       // first value greater than initialHeadPosition
        {
            index=i;
            break;
        }
    }

    // if movement is towards high value
    if(moveDirection==1)
    {
        for(i=index;i<numberOfRequests;i++)
        {
            totalHeadMovement += abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i];
            printf("%d\t",initialHeadPosition);
        }
        
        for(i=index-1;i>=0;i--)
        {
             totalHeadMovement += abs(RequestQueue[i]-initialHeadPosition);
             initialHeadPosition=RequestQueue[i];
             printf("%d\t",initialHeadPosition);
            
        }
    }

    // if movement is towards low value
    else
    {
        for(i=index-1;i>=0;i--)
        {
            totalHeadMovement += abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i];
            printf("%d\t",initialHeadPosition);
        }
        
        for(i=index;i<numberOfRequests;i++)
        {
             totalHeadMovement += abs(RequestQueue[i]-initialHeadPosition);
             initialHeadPosition=RequestQueue[i];
             printf("%d\t",initialHeadPosition);   
        }
    }

    HeadMovement[4] = totalHeadMovement; 
    printf("\nTotal head movement = %d\n",totalHeadMovement);
}


/**
 * @brief Implements C-LOOK Disc Scheduling Algorithm.
 * C-LOOK stands for Circular LOOK
 * @param *args
 */

void* C_LOOK(void* args)
{  
    int totalHeadMovement=0;
    int RequestQueue[MAX];
    int index;
    int i,j;
    struct RQthread *th = (struct RQthread*)args;
    
    int numberOfRequests = th->numberOfRequests;
    for(int i=0;i<numberOfRequests;i++)
        RequestQueue[i] = th->RequestQueue[i];
    int initialHeadPosition = th->initialHeadPosition;
    int diskSize = th->diskSize;
    int moveDirection = th->moveDirection;

    printf("\nIn C-LOOK:\nCurrent Head Position\n%d\t",initialHeadPosition);
    
    BubbleSort(numberOfRequests,RequestQueue);

    for(i=0;i<numberOfRequests;i++)                     
    {
        if(initialHeadPosition<RequestQueue[i])               // first value greater than initialHeadPosition
        {
            index=i;
            break;
        }
    }
   
    // if movement is towards high value
    if(moveDirection==1)
    {
        for(i=index;i<numberOfRequests;i++)
        {
            totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i];
            printf("%d\t",initialHeadPosition);
        }

        totalHeadMovement+=abs(RequestQueue[0]-RequestQueue[i-1]);    //  last movement for max size   
         initialHeadPosition=RequestQueue[0];
         printf("%d\t",initialHeadPosition);

        for( i=1;i<index;i++)
        {
            totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i]; 
            printf("%d\t",initialHeadPosition);
        }
    }

    // if movement is towards low value
    else
    {
        for(i=index-1;i>=0;i--)
        {
            totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
            initialHeadPosition=RequestQueue[i];
             printf("%d\t",initialHeadPosition);
        }
        
        totalHeadMovement+=abs(RequestQueue[i+1]-RequestQueue[numberOfRequests-1]);    //  last movement for min size 
        initialHeadPosition =RequestQueue[numberOfRequests-1];
        printf("%d\t",initialHeadPosition);

        for(i=numberOfRequests-2;i>=index;i--)
        {
             totalHeadMovement+=abs(RequestQueue[i]-initialHeadPosition);
             initialHeadPosition=RequestQueue[i]; 
             printf("%d\t",initialHeadPosition);
        }
    }

    HeadMovement[5] = totalHeadMovement;
    printf("\nTotal head movement = %d\n",totalHeadMovement);
}


/**
 * @brief main funtion 
 * 
 * @return int
 */

int main()
{
    int i,choice;
    int *RequestQueue;
    struct RQthread th;
    pthread_t thread;
    char filename[20];

    while(1)
    {
        printf("\nMenu for disk scheduling algorithm:\n");
        printf("1: FCFS\n");
        printf("2: SSTF\n");
        printf("3: SCAN:\n");
        printf("4: C_SCAN\n");
        printf("5: LOOK\n");
        printf("6: C_LOOK\n");
        printf("7: Run all Algorithms\n");
        printf("8: EXIT\n");

        printf("Enter your choice:\n");
        scanf("%d",&choice);


        switch(choice)
        {
            case 1:                                                 
                printf("Enter the number of Requests\n");
                scanf("%d",&(th.numberOfRequests));
                
                printf("Enter the Requests sequence\n");
                for(i=0;i<th.numberOfRequests;i++)
                    scanf("%d",&(th.RequestQueue[i]));
                
                printf("Enter initial head position\n");
                scanf("%d",&(th.initialHeadPosition));

                pthread_create(&thread,0,&FCFS,(void*)&th);
                pthread_join(thread,0);
                printf("\n");
            break;

            case 2: 
                printf("Enter the number of Requests\n");
                scanf("%d",&(th.numberOfRequests));
                
                printf("Enter the Requests sequence\n");
                for(i=0;i<th.numberOfRequests;i++)
                    scanf("%d",&(th.RequestQueue[i]));
                
                printf("Enter initial head position\n");
                scanf("%d",&(th.initialHeadPosition));

                pthread_create(&thread,0,&SSTF,(void*)&th);
                pthread_join(thread,0);
                printf("\n");
            break;

            case 3:
                printf("Enter the number of Requests\n");
                scanf("%d",&(th.numberOfRequests));
                
                printf("Enter the Requests sequence\n");
                for(i=0;i<th.numberOfRequests;i++)
                    scanf("%d",&(th.RequestQueue[i]));
                
                printf("Enter initial head position\n");
                scanf("%d",&(th.initialHeadPosition));

                printf("Enter total disk size\n");
                scanf("%d",&(th.diskSize));
                
                printf("Enter the head movement direction for high 1 and for low 0\n");
                scanf("%d",&(th.moveDirection));

                pthread_create(&thread,0,&SCAN,(void*)&th);
                pthread_join(thread,0);
                printf("\n");

            break;

            case 4:
                printf("Enter the number of Requests\n");
                scanf("%d",&(th.numberOfRequests));
                
                printf("Enter the Requests sequence\n");
                for(i=0;i<th.numberOfRequests;i++)
                    scanf("%d",&(th.RequestQueue[i]));
                
                printf("Enter initial head position\n");
                scanf("%d",&(th.initialHeadPosition));

                printf("Enter total disk size\n");
                scanf("%d",&(th.diskSize));
                
                printf("Enter the head movement direction for high 1 and for low 0\n");
                scanf("%d",&(th.moveDirection));

                pthread_create(&thread,0,&C_SCAN,(void*)&th);
                pthread_join(thread,0);
                printf("\n");
            break;

            case 5:
                printf("Enter the number of Requests\n");
                scanf("%d",&(th.numberOfRequests));
                
                printf("Enter the Requests sequence\n");
                for(i=0;i<th.numberOfRequests;i++)
                    scanf("%d",&(th.RequestQueue[i]));
                
                printf("Enter initial head position\n");
                scanf("%d",&(th.initialHeadPosition));

                printf("Enter total disk size\n");
                scanf("%d",&(th.diskSize));
                
                printf("Enter the head movement direction for high 1 and for low 0\n");
                scanf("%d",&(th.moveDirection));

                pthread_create(&thread,0,&LOOK,(void*)&th);
                pthread_join(thread,0);
                printf("\n");
            break;

            case 6:
                printf("Enter the number of Requests\n");
                scanf("%d",&(th.numberOfRequests));
                
                printf("Enter the Requests sequence\n");
                for(i=0;i<th.numberOfRequests;i++)
                    scanf("%d",&(th.RequestQueue[i]));
                
                printf("Enter initial head position\n");
                scanf("%d",&(th.initialHeadPosition));

                printf("Enter total disk size\n");
                scanf("%d",&(th.diskSize));
                
                printf("Enter the head movement direction for high 1 and for low 0\n");
                scanf("%d",&(th.moveDirection));

                pthread_create(&thread,0,&C_LOOK,(void*)&th);
                pthread_join(thread,0);
                printf("\n");
            break;

            case 7:
                printf("Enter name of file to read: ");
                scanf("%s",filename);
                fa = fopen(filename, "r");
                if(fa==NULL)
                {
                    printf("Cannot open file\n");
                    exit(0);
                }
                
                ca = getc(fa);
                th.numberOfRequests = getNextNum();
                ca = getc(fa);

                for(int i=0;i<th.numberOfRequests;i++)
                {    
                    th.RequestQueue[i]=getNextNum();
                    ca = getc(fa);
                }

                th.initialHeadPosition = getNextNum();
                ca = getc(fa);
                
                th.diskSize = getNextNum();
                ca = getc(fa);
                
                th.moveDirection = getNextNum();
                ca = getc(fa);
                
                pthread_t threads[6];
                pthread_create(&threads[0],0,&FCFS,(void*)&th);
                pthread_join(threads[0],0);

                pthread_create(&threads[1],0,&SSTF,(void*)&th);
                pthread_join(threads[1],0);

                pthread_create(&threads[2],0,&SCAN,(void*)&th);
                pthread_join(threads[2],0);
                
                pthread_create(&threads[3],0,&C_SCAN,(void*)&th);
                pthread_join(threads[3],0);

                pthread_create(&threads[4],0,&LOOK,(void*)&th);
                pthread_join(threads[4],0);

                pthread_create(&threads[5],0,&C_LOOK,(void*)&th);
                pthread_join(threads[5],0);

                printLeastHeadMovement();
            break;

            case 8:
                printf("Program finished\n");
                exit(0);

            default:
                printf("Invalid choice\n");
        }
    }
    return 0;
}