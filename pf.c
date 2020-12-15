// NAME: KRISHNA PATEL
// ID: 1001273790
// SECTION 003

// MANIZHA NOORI
// ID: 1001520740
// SECTION 001

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE 1024

typedef struct _Frame
{
  int counter;
  int working_set_size;
  int current_Size;
  int value;
  struct _Frame* next;
  struct _Frame* prev;
} frame;


void printList(frame** head)
{
  frame* temp = *head;
  int i =0;
  printf("****PRINTING A MEMORY DUMP******\n");

  while(temp!= NULL)
  {
    printf("Node %i  ==== %i\n",i, temp->value);
    temp = temp->next;
    i++;
  }
}
void updateLRU_Page_Found_PageTable(frame** table,frame** head,frame** tail, int value)
{

    if(((*head)->current_Size) == 1)
    {
        return;
    }
    frame* pageFoundIdx = *table;

    while(pageFoundIdx != NULL)
    {
      if((pageFoundIdx->value) == (value))
      {
        // IS NODE MIDDLE
        // make the NEXT node's PREV value point the node BEFORE the
        // actual page
        if( ((pageFoundIdx->next) != NULL) && ((pageFoundIdx->prev) !=  NULL))
        {
          pageFoundIdx->next->prev = pageFoundIdx->prev;
          pageFoundIdx->prev->next = pageFoundIdx->next;
          pageFoundIdx->prev = (*head)->prev;
          pageFoundIdx->next = (*head);
          (*head)->prev = pageFoundIdx;
          (*head) = pageFoundIdx;
        }
        // IS NODE LAST
        // since this node if was the last node is moving to top
        // tail ptr will end up still pointing at this node
        else if ((pageFoundIdx->next) == NULL)
        {
            pageFoundIdx->prev->next = pageFoundIdx->next;
            *tail = (*tail)->prev;
            pageFoundIdx->prev = (*head)->prev;
            pageFoundIdx->next = (*head);
            (*head)->prev = pageFoundIdx;
            (*head) = pageFoundIdx;
        }
        break;
      }
      pageFoundIdx = pageFoundIdx->next;
    }
}
int pageFound(frame** head, int value)
{
  // traverses the list
  frame* temp = *head;
  while(temp != NULL)
  {
    if((temp->value) == value)
    {
      return 1;
    }
    temp = temp->next;
  }
  return 0;
}

int MFU_Paging(char* line)
{
  int head_node_nullPTR =1; // head hasn't been declared
  frame* head = NULL;
  frame* tail = NULL;
  int i =0;
  int count =0;
  int isfound = 0;

  char * token;
  token = strtok( line, " ");
  int working_set_size = atoi( token );
  // printf("\nWorking set size: %d\n", working_set_size );
  token = strtok( NULL, " " );
  while( token != NULL )
  {
    if(head_node_nullPTR) // checking if the first node has been created
    {
      count=1;
      frame *node = (frame*) malloc(sizeof(frame));
      head = node;
      tail = node;
      head_node_nullPTR =0;

      node->working_set_size = working_set_size;
      node->current_Size = 1;
      node-> value = atoi(token);
      node->prev = NULL;
      node->next = NULL;
      node->counter = 1;
    }
    else
    {
      if((head->current_Size) < (head->working_set_size))
      {
        isfound = 0;
        isfound = pageFound(&head,atoi(token));

        // updates Counter for that page
        if(isfound)
        {
          frame* temp = head;
          while((temp->value) != (atoi(token)))
          {
            temp = temp->next;
          }
          temp->counter =  (temp->counter) +1;
        }
        else
        {
          count++;
          int current_tableSize = (tail->current_Size) +1;
          tail->next = (frame*) malloc(sizeof(frame));
          // Translate data to the next node.
          tail->next->working_set_size = tail->working_set_size;
          tail->next->next= NULL;
          tail->next->value = atoi(token);
          // tail->current_Size =  tail->current_Size +1;
          // tail->next->current_Size = tail->current_Size;

          // update Table size in all nodes
          frame* updateNodes = head;
          while(updateNodes != NULL)
          {
            updateNodes->current_Size = current_tableSize;
            updateNodes = updateNodes->next;
          }
          // set counter to 1;
          tail->next->counter = 1;

          // points to the prev node that was there before this node came in
          // its confusing but draw it
         tail->next->prev = tail;
         tail = tail->next; // move tail to the end
           // printf("%d value of size\n",tail->current_Size );
        }
      }
      else
      {

        isfound = 0;
        isfound = pageFound(&head,atoi(token));

        // updates Counter for that page
        if(isfound)
        {
          frame* temp = head;
          while((temp->value) != (atoi(token)))
          {
            temp = temp->next;
          }
          temp->counter =  (temp->counter) +1;
        }
        else
        {
          count++;
          int maxIdx;
          int loadedPages_counter[working_set_size];
          int i =0;

          frame* temp_fault = head;
          i = 0;
          while(temp_fault != NULL)
          {
            loadedPages_counter[i] = temp_fault->counter;
            temp_fault = temp_fault ->next;
            i++;
          }

          // find the node that is most frequent
          maxIdx =-1;
          frame *evictNode = head;
          frame* temp = head;
          for(i = 0; i < working_set_size; i++ )
          {
            if(maxIdx < loadedPages_counter[i])
            {
              maxIdx = loadedPages_counter[i];
              evictNode = temp;
            }
            temp = temp->next;
          }
          // re connect previous nodes with last node;
          // add free node;
          // checks if the node is in the middle
          if(working_set_size == 1)
          {
            frame* freeThisNode = tail;
            free(freeThisNode);
            frame *node = (frame*) malloc(sizeof(frame));
            head = node;
            tail = node;
            head_node_nullPTR =0;

            node->working_set_size = working_set_size;
            node->current_Size = 1;
            node-> value = atoi(token);
            node->prev = NULL;
            node->next = NULL;
            node->counter = 1;
          }
          else
          {
              if(((evictNode->next) != NULL) && ((evictNode->prev) != NULL))
          {
            evictNode->prev->next = evictNode->next;
            evictNode->next->prev = evictNode->prev;
            free(evictNode);
          }
          // checks last node
          else if((evictNode->next) == NULL)
          {
            evictNode->prev->next = evictNode->next;
            tail = evictNode->prev;
            free(evictNode);
          }
          else if((evictNode->prev) == NULL)
          {
            evictNode->next->prev = evictNode->prev;
            head = evictNode->next;
            free(evictNode);
          }

          // new node
          tail->next = (frame*) malloc(sizeof(frame));
          tail->next->counter = 1;
          // Translate data to the next node.
          tail->next->working_set_size = tail->working_set_size;
          tail->next->current_Size =  tail->current_Size ;
          tail->next->current_Size = tail->current_Size;
          tail->next->value = atoi(token);
          tail->next->next = NULL;

         // points to the prev node that was there before this node came in
         // its confusing but draw it
          tail->next->prev = tail;
          tail = tail->next; // move tail to the end
          }

        }
      }
    }
    if( token != NULL )
    {
//       printf("Request: %d\n", head->value );
//
//        printList(&head);
    }
    token = strtok( NULL, " " );
  }
  // free Nodes to avoid a leak in memory
  head_node_nullPTR =1;
  while(head != NULL)
  {
  frame* temp = head->next;
  free(head);
  head = temp;
  }
  head_node_nullPTR = 1;
  tail = NULL;
  return count;
}
int FIFO_Paging(char* line)
{
  int head_node_nullPTR =1; // head hasn't been declared
  frame* head = NULL;
  frame* tail = NULL;
  int i =0;
  int count =0;
  int isfound = 0;


  char * token;
  token = strtok( line, " ");
  int working_set_size = atoi( token );
  // printf("\nWorking set size: %d\n", working_set_size );
  token = strtok( NULL, " " );
  while( token != NULL )
  {
    if(head_node_nullPTR) // checking if the first node has been created
    {
      count=1;
      frame *node = (frame*) malloc(sizeof(frame));
      head = node;
      tail = node;
      head_node_nullPTR =0;

      node->working_set_size = working_set_size;
      node->current_Size = 1;
      node-> value = atoi(token);
      node->prev = NULL;
      node->next = NULL;
    }
    else
    {
      // this fills the table with nodes until working_set is full
      if((tail->current_Size) < (tail->working_set_size))
      {
        // check table for the page
        isfound = 0;
        isfound = pageFound(&head,atoi(token));
        //Not found Page
        // Page fault has happened
        if(!isfound)
        {
          count++;
          tail->next = (frame*) malloc(sizeof(frame));
          // Translate data to the next node.
          tail->next->working_set_size = tail->working_set_size;
          tail->current_Size =  tail->current_Size +1;
          tail->next->current_Size = tail->current_Size;
          tail->next->value = atoi(token);
          tail->next->next = NULL;

         // points to the prev node that was there before this node came in
         // its confusing but draw it
          tail->next->prev = tail;
          tail = tail->next; // move tail to the end
          // printf("%d value of size\n",tail->current_Size );
        }

      }
      else
      {
        // check table for the page
        isfound = 0;
        isfound = pageFound(&head,atoi(token));


        //Not found Page
        // Page fault has happened
        // NEED TO EVICT A PAGE
        if(!isfound)
        {
            if(working_set_size == 1)
            {
              frame* freeThisNode = tail;
              free(freeThisNode);
              count++;
              frame *node = (frame*) malloc(sizeof(frame));
              head = node;
              tail = node;
              head_node_nullPTR =0;

              node->working_set_size = working_set_size;
              node->current_Size = 1;
              node-> value = atoi(token);
              node->prev = NULL;
              node->next = NULL;
            }
            else
            {
            count++;
            // delete head node because this FIFO
            frame* freeThisNode = head;
            head = head->next; // shift head to next node
            head->prev = NULL;
            free(freeThisNode);

            // add faulted node to the end
            tail->next = (frame*) malloc(sizeof(frame));
            tail->next->working_set_size = tail->working_set_size;
            tail->next->current_Size = tail->current_Size;
            tail->next->value = atoi(token);
            tail->next->next = NULL;
            tail->next->prev = tail;
            tail = tail->next; // move tail to the end
            }


        }
      }
    }
    if( token != NULL )
    {
       // printf("Request: %d\n", tail->value );
       // printf("****PRINTING A MEMORY DUMP******\n");
       // frame* dump = head;
       // i =0;
       // while(dump!= NULL)
       // {
       //   printf("Node %i  ==== %i\n",i, dump->value);
       //   dump = dump->next;
       //   i++;
       // }
    }
    token = strtok( NULL, " " );
  }
  // free Nodes to avoid a leak in memory
  head_node_nullPTR =1;
  while(head != NULL)
  {
    frame* temp = head->next;
    free(head);
    head = temp;
  }
  head_node_nullPTR = 1;
  tail = NULL;

  return count;
}

int LRU_Paging(char* line)
{
  int head_node_nullPTR =1; // head hasn't been declared
  frame* head = NULL;
  frame* tail = NULL;
  int i =0;
  int count =0;
  int isfound = 0;

  char * token;
  token = strtok( line, " ");
  int working_set_size = atoi( token );
//  printf("\nWorking set size: %d\n", working_set_size );
  token = strtok( NULL, " " );
  while( token != NULL )
  {
    if(head_node_nullPTR) // checking if the first node has been created
    {
      count=1;
      frame *node = (frame*) malloc(sizeof(frame));
      head = node;
      tail = node;
      head_node_nullPTR =0;

      node->working_set_size = working_set_size;
      node->current_Size = 1;
      node-> value = atoi(token);
      node->prev = NULL;
      node->next = NULL;
    }
    else
    {
      // In LRU we keep pushing the the unused page at the bottom.

      if((head->current_Size) < (head->working_set_size))
      {
        isfound = 0;
        isfound = pageFound(&head,atoi(token));

        //Need to update list
        // if found page
        if(isfound)
        {
          updateLRU_Page_Found_PageTable(&head,&head,&tail,atoi(token));
        }
        // not found
        else
        {
          count++;
          int current_tableSize = (head->current_Size) +1;

          head->prev = (frame*) malloc(sizeof(frame));
          // Translate data to the next head_node
          head->prev->working_set_size = head->working_set_size;
          head->prev->value = atoi(token);
          head->prev->prev = NULL;
          head->prev->next = head;
          head = head->prev; // move headPTR up
          // head->current_Size = head->current_Size +1;
          // head->prev->current_Size = head->current_Size;

          frame* updateNodes_current_Size = head;
          while(updateNodes_current_Size != NULL)
          {
            updateNodes_current_Size->current_Size = current_tableSize;
            updateNodes_current_Size = updateNodes_current_Size->next;
          }



        }
      }
      else
      {
        // check table for the page
        isfound = 0;
        isfound = pageFound(&head,atoi(token));
        //Need to update list
        // if found page
        if(isfound)
        {
          updateLRU_Page_Found_PageTable(&head,&head,&tail,atoi(token));
        }
        else
        {
            if(working_set_size == 1)
            {
                frame* freeThisNode = tail;
                free(freeThisNode);
                count++;
                frame *node = (frame*) malloc(sizeof(frame));
                head = node;
                tail = node;
                head_node_nullPTR =0;

                node->working_set_size = working_set_size;
                node->current_Size = 1;
                node-> value = atoi(token);
                node->prev = NULL;
                node->next = NULL;
            }
            else
            {
              count++;
              // delete head node because this LRU means it is at the tail
              frame* freeThisNode = tail;
              tail = tail->prev; // shift tail to the right
              tail->next = NULL;
              free(freeThisNode);

              head->prev = (frame*) malloc(sizeof(frame));
              head->prev->working_set_size = head->working_set_size;
              head->prev->current_Size = head->current_Size;
              head->prev->value = atoi(token);
              head->prev->prev = NULL;
              head->prev->next = head;
              head = head->prev; // move head up
            }

        }
      }
    }

    if( token != NULL )
    {
      // printf("Request: %d\n", head->value );

      // printList(&head);

    }
    token = strtok( NULL, " " );
  }
  // free Nodes to avoid a leak in memory
  head_node_nullPTR =1;
  while(head != NULL)
  {
    frame* temp = head->next;
    free(head);
    head = temp;
  }
  head_node_nullPTR = 1;
  tail = NULL;

  return count;
}


int OPTIMAL_Paging(char* line)
{
  // // GETS THE MAX SIZE to know how many idxs in array
  // // since ther is a space -> divide by 2 and add1
  int head_node_nullPTR =1; // head hasn't been declared
  frame* head = NULL;
  frame* tail = NULL;
  int i =0;
  int count =0;
  int isfound = 0;

  int arrSize = 0;
  int refCounter = 0;
  int charCount =0;
  for(charCount =0; line[charCount] != 0;charCount++)
  {
    if((line[charCount] != ' ') && (line[charCount] != '\n'))
    {
      arrSize++;
    }
  }
  arrSize--;
  // printf("SIZE %d\n",arrSize );

  // storing all the page numbers in an array
  int arrPages[arrSize];
  int arrIdx =0;
  for(i = 2; i < (charCount); i++)
  {
    if(line[i] != ' ')
    {
      int value = line[i] - '0';
      arrPages[arrIdx] =  value;
      arrIdx++;
    }
  }

// tokenizing begans
  char * token;
  token = strtok( line, " ");
  int working_set_size = atoi( token );
  // printf("\nWorking set size: %d\n", working_set_size );
  token = strtok( NULL, " " );
  while( token != NULL )
  {
    if(head_node_nullPTR) // checking if the first node has been created
    {
      count=1;
      frame *node = (frame*) malloc(sizeof(frame));
      head = node;
      tail = node;
      head_node_nullPTR =0;

      node->working_set_size = working_set_size;
      node->current_Size = 1;
      node-> value = atoi(token);
      node->prev = NULL;
      node->next = NULL;
      refCounter = 1;
    }
    else
    {
      // this fills the table with nodes until working_set is full
      if((tail->current_Size) < (tail->working_set_size))
      {
        // check table for the page
        isfound = 0;
        isfound = pageFound(&head,atoi(token));

        // Not Found
        // Page fault has happened
        // add node
        if(!isfound)
        {
          count++;
          tail->next = (frame*) malloc(sizeof(frame));
          // Translate data to the next node.
          tail->next->working_set_size = tail->working_set_size;
          tail->current_Size =  tail->current_Size +1;
          tail->next->current_Size = tail->current_Size;
          tail->next->value = atoi(token);
          tail->next->next = NULL;

         // points to the prev node that was there before this node came in
         // its confusing but draw it
          tail->next->prev = tail;
          tail = tail->next; // move tail to the end
        }
      }
      else
      {
        // check table for the page
        isfound = 0;
        isfound = pageFound(&head,atoi(token));
        //Not found Page
        // Page fault has happened
        // NEED TO EVICT A PAGE
        if(!isfound)
        {
            if(working_set_size == 1)
            {
              frame* freeThisNode = tail;
              free(freeThisNode);
              count++;
              frame *node = (frame*) malloc(sizeof(frame));
              head = node;
              tail = node;
              head_node_nullPTR =0;

              node->working_set_size = working_set_size;
              node->current_Size = 1;
              node-> value = atoi(token);
              node->prev = NULL;
              node->next = NULL;
            }
            else
            {
                count++;
                int loadedPages[working_set_size];
                frame* temp = head;
                for(i = 0; i< working_set_size; i++)
                {
                  loadedPages[i] = temp->value;
                  temp = temp->next;
                }
                // this records the next indx the page will be used
                int nextArrivalPages[working_set_size];
                for(i = 0; i < working_set_size; i++)
                {

                  int j = refCounter+1;// starts at the next incoming page
                  for(; j < arrSize; j++)
                  {
                    if(arrPages[j] == loadedPages[i])
                    {
                        nextArrivalPages[i] = j; // records the next index for that page
                        break;
                    }
                  }
                  // means page is no longer needed
                  if(j == arrSize)
                  {
                    nextArrivalPages[i] = -1;
                  }
                }
                int maxIdx = 0;
                int pageNumber_evicting  =0;
                for(i = 0; i< working_set_size; i++)
                {
                  // this page is no longer needed and is evicted
                  if(nextArrivalPages[i] == -1)
                  {
                      pageNumber_evicting = i;
                      break;
                  }
                  if(maxIdx < nextArrivalPages[i])
                  {
                      pageNumber_evicting = i;
                      maxIdx = nextArrivalPages[i];
                  }
                }
                // traverse linked list to node that will be evicted
                temp = head;
                for(i =0; i < pageNumber_evicting;i++ )
                {
                  temp = temp->next;
                }
                // re connect previous nodes with last node;
                // add free node;
                // checks if the node is in the middle
                if(((temp->next) != NULL) && ((temp->prev) != NULL))
                {
                  temp->prev->next = temp->next;
                  temp->next->prev = temp->prev;
                  free(temp);
                }
                // checks last node
                else if((temp->next) == NULL)
                {
                  temp->prev->next = temp->next;
                  tail = temp->prev;
                  free(temp);
                }
                else if((temp->prev) == NULL)
                {
                  temp->next->prev = temp->prev;
                  head = temp->next;
                  free(temp);
                }
                // new node
                tail->next = (frame*) malloc(sizeof(frame));
                // Translate data to the next node.
                tail->next->working_set_size = tail->working_set_size;
                tail->current_Size =  tail->current_Size +1;
                tail->next->current_Size = tail->current_Size;
                tail->next->value = atoi(token);
                tail->next->next = NULL;

               // points to the prev node that was there before this node came in
               // its confusing but draw it
                tail->next->prev = tail;
                tail = tail->next; // move tail to the end
            }
        }

      }
      refCounter++;
    }
    if( token != NULL )
    {
//        printf("Request: %d\n", atoi(token) );
//
//        printList(&head);
    }
    token = strtok( NULL, " " );
  }
  // free Nodes to avoid a leak in memory
  head_node_nullPTR =1;
  while(head != NULL)
  {
    frame* temp = head->next;
    free(head);
    head = temp;
  }
  head_node_nullPTR = 1;
  tail = NULL;

  return count;
}


int main( int argc, char * argv[] )
{
  int faults = 0;


  char * line = NULL;
  size_t line_length = MAX_LINE;
  char * filename;

  FILE * file;

  if( argc < 2 )
  {
    printf("Error: You must provide a datafile as an argument.\n");
    printf("Example: ./fp datafile.txt\n");
    exit( EXIT_FAILURE );
  }

  filename = ( char * ) malloc( strlen( argv[1] ) + 1 );
  line     = ( char * ) malloc( MAX_LINE );

  memset( filename, 0, strlen( argv[1] + 1 ) );
  strncpy( filename, argv[1], strlen( argv[1] ) );

  printf("Opening file %s\n", filename );
  file = fopen( "datafile.txt" , "r");


  char *FIFO;
  char *LRU;
  char *OPTIMAL;
  char *MFU;

  if ( file )
  {
    while ( fgets( line, line_length, file ) )
    {
      printf("\n" );
      FIFO = (char*) malloc(MAX_LINE);
      LRU = (char*) malloc(MAX_LINE);
      OPTIMAL = (char*) malloc(MAX_LINE);
      MFU = (char*) malloc(MAX_LINE);

      strncpy(FIFO,line,line_length);
      strncpy(LRU,line,line_length);
      strncpy(OPTIMAL,line,line_length);
      strncpy(MFU,line,line_length);

      // printf("***** RUNNING TEST FOR ->\n%s",line );
      faults = FIFO_Paging(FIFO);
      printf("Page faults of FIFO: %d \n",faults );
      faults = LRU_Paging(LRU);
      printf("Page faults of LRU: %d \n",faults );
      faults = MFU_Paging(MFU);
      printf("Page faults of MFU: %d \n",faults );
      faults = OPTIMAL_Paging(OPTIMAL);
      printf("Page faults of Optimal: %d \n",faults );

//      faults = MFU_Paging(MFU);
//      faults = LRU_Paging(LRU);

      free(FIFO);
      free(LRU);
      free(OPTIMAL);
      free(MFU);
      faults = 0;
    }

    free( line );
    fclose(file);
  }

  return 0;
}
