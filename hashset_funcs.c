#include "hashset.h"
#include <stdlib.h>
#include <string.h>
// hashset_funcs.c: utility functions for operating on hash sets. Most
// functions are used in the hashset_main.c which provides an
// application to work with the functions.


// PROVIDED: Compute a simple hash code for the given character
// string. The code is "computed" by casting the first 8 characters of
// the string to a numbers and returning their xor. The empty string
// has hash code 0. Longer strings will have numbers which are the
// integer interpretation of up to their first 8 bytes.  ADVANTAGE:
// constant time to compute the hash code. DISADVANTAGE: poor
// distribution for long strings; all strings with same first 8 chars
// hash to the same location.
int hashcode(char key[]){ //built in
  int hc = 0; //calculates the hascode 
  for(int i=0; key[i]!='\0'; i++){
    hc = hc*31 + key[i];
  }
  return hc;
}

void hashset_init(hashset_t *hs, int table_size){
// Initialize the hash set 'hs' to have given size and elem_count
// 0. Ensures that the 'table' field is initialized to an array of
// size 'table_size' and is filled with NULLs. Also ensures that the
// first/last pointers are initialized to NULL
  hs->table = malloc(sizeof(hashset_t)*table_size); //allocate space for the atble
  hs->table_size = table_size;
  hs->elem_count = 0;
  hs->order_first = NULL;
  hs->order_last = NULL;
  for (int i = 0; i< table_size; i++){
    hs->table[i] = NULL; //set each "bucket" to null
  }
  return;
}

int hashset_contains(hashset_t *hs, char elem[]){
// Returns 1 if the parameter `elem` is in the hash set and 0
// otherwise. Uses hashcode() and field `table_size` to determine
// which index in table to search.  Iterates through the list at that
// table index using strcmp() to check for `elem`. NOTE: The
// `hashcode()` function may return positive or negative
// values. Negative values are negated to make them positive. The
// "bucket" (index in hs->table) for `elem` is determined by with
// 'hashcode(key) modulo table_size'.
int size_table = hs->table_size;
int keycode = abs(hashcode(elem)%size_table); //calculate the keycode
hashnode_t *p = hs->table[keycode];
while(p!= NULL){ //while the pointer isn't at the end
  if (strcmp(elem, p->elem) == 0){ //if the element is found
    return 1; //return 1
  }
  p = p->table_next;//keep iterating
  
}
return 0; //none

}

int hashset_add(hashset_t *hs, char elem[]){
// If the element is already present in the hash set, makes no changes
// to the hash set and returns 0. hashset_contains() may be used for
// this. Otherwise determines the bucket to add `elem` at via the same
// process as in hashset_contains() and adds it to the FRONT of the
// list at that table index. Adjusts the `hs->order_last` pointer to
// append the new element to the ordered list of elems. If this is the
// first element added, also adjsuts the `hs->first` pointer. Updates the
// `elem_count` field and returns 1 to indicate a successful addition.
//
// NOTE: Adding elems at the front of each bucket list allows much
// simplified logic that does not need any looping/iteration.
if(hs==NULL){ //if there is no hs put in
  hashset_init(hs, HASHSET_DEFAULT_TABLE_SIZE); //initalize the hs
}
if (hashset_contains(hs, elem) == 1){ //if it already contains the element
  return 0;
}

int keycode = abs(hashcode(elem)%(hs->table_size)); //calcuate which bucket it needs to go into

hashnode_t *newnode = malloc(sizeof(hashnode_t)); 
strcpy(newnode->elem, elem); //copy element into the newnode
if (hs->elem_count ==0){ //if there is nothing inside at all
  hs->table[keycode] = newnode; //set the bucket to the newnode
  hs -> order_first = newnode; //set order_first to the newnode
  newnode->table_next = NULL; //set the newnode next table index to null because there's still nothing
  hs -> order_last = newnode; //set the last node to newnode since there's only one node
  newnode->order_next = NULL; //set next node newnode to null because there's nothing
  hs->elem_count =1;
}
else if(hs->table[keycode] == NULL){ //first one in the bucket
  newnode->table_next = NULL; //set the newnode next table index to null because there's nothing
  newnode->order_next = NULL; //set the newnode next order index to null because there's nothing after
  hs->order_last->order_next = newnode; //set the most recently added node to have the next index of newnode
  hs->table[keycode] = newnode; //set the bucket of the table to newnode
  hs->order_last = newnode; //set the last one to newnode since it's the newest one
  hs->elem_count++; //increase the element count by one
}
else{ //there is something inside
  newnode->table_next = hs->table[keycode]; //points newnode next to the most recently added on in the bucket, since all nodes are interested at the beginning
  hs->table[keycode] = newnode; //set the bucket to the newnode
  newnode->order_next = NULL; //set the next index of the newnode to NULL
  hs->order_last->order_next = newnode; //set the most recently added one (Not this one) to have the next order index of this newly added node
  hs->order_last = newnode; //set the last order to newnode, since it's the most recent one
  hs->elem_count++;
}
return 1;
}

void hashset_free_fields(hashset_t *hs){
// De-allocates nodes/table for `hs`. Iterates through the ordered
// list of the hash set starting at the `order_first` field and
// de-allocates all nodes in the list. Also free's the `table`
// field. Sets all relevant fields to 0 or NULL as appropriate to
// indicate that the hash set has no more usable space. Does NOT
// attempt to de-allocate the `hs` itself as it may not be
// heap-allocated (e.g. in the stack or a global).
// int size_table = hs->table_size;
// for (int i = 0; i < size_table; size_table++){
//   hs->table[i] = NULL;
//   free(hs -> table[i]);
// }

hashnode_t *curNode = hs->order_first; //set the currentNode to the first one added
while (curNode != NULL){ 
  hashnode_t *var = curNode->order_next; //set the current node to the nex
  free(curNode); //free up the current node
  curNode = var;
}
 free(hs->table); //free everything up
}


void hashset_show_structure(hashset_t *hs){
// Displays detailed structure of the hash set. Shows stats for the
// hash set as below including the load factor (element count divided
// by table_size) to 4 digits of accuracy.  Then shows each table
// array index ("bucket") on its own line with the linked list of
// elems in the bucket on the same line. 
// 
// EXAMPLE:
// elem_count: 4
// table_size: 5
// order_first: Rick
// order_last : Tinyrick
// load_factor: 0.8000
// [ 0] : {7738144525137111380 Tinyrick >>NULL} 
// [ 1] : 
// [ 2] : 
// [ 3] : {125779953153363 Summer >>Tinyrick} {1801677138 Rick >>Morty} 
// [ 4] : {521644699469 Morty >>Summer} 
//
// NOTES:
// - Uses format specifier "[%2d] : " to print the table indices
// - Nodes in buckets have the following format:
//   {1415930697 IceT >>Goldenfold}
//    |          |       |        
//    |          |       +-> order_next->elem OR NULL if last node
//    |          +->`elem` string     
//    +-> hashcode("IceT"), print using format "%ld" for 64-bit longs

printf("elem_count: %d\n", hs->elem_count);
printf("table_size: %d\n", hs->table_size);
if (hs->order_first->elem == NULL){ //if there is nothing in order first
  printf("order_first: NULL\n"); //print null
}
else{
  printf("order_first: %s\n", hs->order_first->elem); //if there is something print order_first
}
if (hs->order_last->elem == NULL){ //if there is nothing in order last
  printf("order_last : NULL\n");  //print null
}
else{
  printf("order_last : %s\n", hs->order_last->elem); //if there is something print order_last
}
printf("load_factor: %.4f\n", ((float)hs->elem_count/hs->table_size));
for (int i = 0; i< hs->table_size; i++){ //interate thru each bucket
  printf("[%2d] : ", i);
  hashnode_t *p = hs->table[i];
  while(p!= NULL){
  int hash = hashcode(p->elem);
  printf("{%d ", hash); //print out the hash number
  printf("%s", p->elem); //print out the element
  if (p->order_next->elem == NULL){ //if the next in the order is null, print null
     printf(" >>NULL} ");
  }
  else{ //otherwise
  printf(" >>%s} ", p->order_next->elem); //print out what's next
  }
  p = p->table_next;
}
printf("\n");

}

}


void hashset_write_elems_ordered(hashset_t *hs, FILE *out){
// Outputs all elements of the hash set according to the order they
// were added. Starts at the `order_first` field and iterates through
// the list defined there. Each element is printed on its own line
// preceded by its add position with 1 for the first elem, 2 for the
// second, etc. Prints output to `FILE *out` which should be an open
// handle. NOTE: the output can be printed to the terminal screen by
// passing in the `stdout` file handle for `out`.
int count = 1;
hashnode_t *p = hs->order_first; //first element
while(p!= NULL){
  fprintf(out,"  %d %s\n", count ,p->elem); //print out elements, in format number element
  p = p->order_next; //iterate more
  count++;
  
}
}

void hashset_save(hashset_t *hs, char *filename)
// Writes the given hash set to the given `filename` so that it can be
// loaded later.  Opens the file and writes its 'table_size' and
// 'elem_count' to the file. Then uses the hashset_write_elems_ordered()
// function to output all elems in the hash set into the file.
// EXAMPLE FILE:
// 
// 5 6
//   1 Rick
//   2 Morty
//   3 Summer
//   4 Jerry
//   5 Beth
//   6 Tinyrick
// 
// First two numbers are the 'table_size' and 'elem_count' field and
// remaining text is the output of hashset_write_elems_ordered();
// e.g. insertion position and element.
{
  FILE *fp = fopen(filename, "w"); //open up file
  if (fp == NULL){//file coulnd't be opened
    printf("ERROR: could not open file '%s'\n", filename);
    return;
  }
  fprintf(fp,"%d %d\n", hs->table_size, hs->elem_count); //save table size and elemnt into file
  hashset_write_elems_ordered(hs, fp); //print elements into file
  fclose(fp);
}


int hashset_load(hashset_t *hs, char *filename)
{

// Loads a hash set file created with hashset_save(). If the file
// cannot be opened, prints the message
// 
// ERROR: could not open file 'somefile.hs'
//
// and returns 0 without changing anything. Otherwise clears out the
// current hash set `hs`, initializes a new one based on the size
// present in the file, and adds all elems from the file into the new
// hash set. Ignores the indices at the start of each line and uses
// hashset_add() to insert elems in the order they appear in the
// file. Returns 1 on successful loading (FIXED: previously indicated
// a different return value on success) . This function does no error
// checking of the contents of the file so if they are corrupted, it
// may cause an application to crash or loop infinitely.
  FILE *fp = fopen(filename, "r"); //open up file
  if (fp == NULL){
    printf("ERROR: could not open file '%s'\n", filename);
    return 0;
  }
  char buffer[128];
  int new_table_size;
  int new_elem_count;

  fscanf(fp, "%d %d",&new_table_size, &new_elem_count); //scan table size and elem count
  hashset_free_fields(hs);
  hashset_init(hs, new_table_size); //initalize the new hashset
  for (int i= 0; i < new_elem_count; i++){
   fscanf(fp, "%*d %s", buffer);
    hashset_add(hs, buffer); //add into hashset
  }
  fclose(fp); //close file
  return 1;
}


int next_prime(int num){
// If 'num' is a prime number, returns 'num'. Otherwise, returns the
// first prime that is larger than 'num'. Uses a simple algorithm to
// calculate primeness: check if any number between 2 and (num/2)
// divide num. If none do, it is prime. If not, tries next odd number
// above num. Loops this approach until a prime number is located and
// returns this. Used to ensure that hash table_size stays prime which
// theoretically distributes elements better among the array indices
// of the table.
int half_num = num/2;
  for (int i = 2; i< half_num; i++){
  if (num%i == 0){ //if number isn't prime
    if (num%2 ==0){ //if number is multiple of two
      num++;
  }
  else{
      num+= 2; //iterate to next odd num
  }
  }
  }
return num;
}




void hashset_expand(hashset_t *hs){

// Allocates a new, larger area of memory for the `table` field and
// re-adds all current elems to it. The size of the new table is
// next_prime(2*table_size+1) which keeps the size prime.  After
// allocating the new table, all table entries are initialized to NULL
// then the old table is iterated through and all elems are added to
// the new table according to their hash code. The memory for the old
// table is de-allocated and the new table assigned to the hash set
// fields "table" and "table_size".  This function increases
// "table_size" while keeping "elem_count" the same thereby reducing
// the load of the hash table. Ensures that the memory associated with
// the old table is free()'d. Makes NO special effort to preserve old
// nodes: re-adds everything into the new table and then frees the old
// one along with its nodes. Uses functions such as hashset_init(),
// hashset_add(), hashset_free_fields() to accomplish the transfer.
  int new_size = next_prime(2*hs->table_size+1);
  hashset_t *temphash = malloc(sizeof(hashset_t)); //temp hashset
  hashset_init(temphash, new_size); //make temporary hashet

  hashnode_t *temp = hs->order_first; //adding the values of OG hashset
  while(temp != NULL){
    hashset_add(temphash,temp->elem);//add stuff to temp hashet
    temp = temp->order_next; //increment to next thing added
  }

  hashset_free_fields(hs); //freeing original hashset
  *hs = *temphash; //put temp hashset into hashset
  free(temphash);

}
