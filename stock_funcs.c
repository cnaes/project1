#include "stock.h"

// PROBLEM 1: Allocate a new stock struct and initialize its fields.
// Integer fields like 'count' and 'min_index' should be initialied to
// -1.  Pointer fields like 'prices' should be initialized to
// NULL. The stock should be heap-allocated using malloc() and
// returned.  Since this is an allocation function, no use of 'free()'
// should appear.
stock_t *stock_new(){ //initializes the stock
  stock_t *stock = malloc(sizeof(stock_t)); //creates space for the stock
  stock->count = -1; //the following lines set all the variables
  stock->min_index = -1;
  stock->max_index = -1;
  stock->best_buy = -1;
  stock->best_sell = -1;
  stock->prices = NULL;
  stock->data_file = NULL;
  return stock; //returns the now initalized stock

}

// PROBLEM 1: Free a stock. Check the 'data_file' and 'prices' fields:
// if they are non-NULL, then free them. Then free the pointer to
// 'stock' itself.
void stock_free(stock_t *stock){ //frees the stock
  if (stock->data_file != NULL){ //if stock has a data file
    free(stock->data_file); //free it from memory
  }

  if (stock->prices != NULL){ //if the stock contains prices
    free(stock->prices); //free it fro memory
  }

  free(stock); //free the whole stock from memory
  return;
}

// PROBLEM 1: Prints data about a stock that is passed in via a
// pointer. Uses the syntax ptr->field to access fields of the struct
// pointed by 'stock'.  Output follows the general convention:
//
// data_file: data/stock-jagged.txt                      
// count: 15
// prices: [103.00, 250.00, 133.00, ...]
// min_index: 8
// max_index: 11
// best_buy:  8
// best_sell: 11
// profit:    232.00
//
// Where each line prints a field of the stock_t struct. In all cases,
// floating point numbers are printed with 2 decimal digits of accuracy.
//
// NULLS FOR FIELDS
// The fields 'data_file' and 'prices' may be NULL in which case they
// will be printed specially as in
// 
// data_file: NULL
// prices: NULL
//
// This requires a manual if/else check for NULL values for these pointers.
// 
// PRICES FIELD
// When printing the 'prices' field, if the 'count' field is 0 to 3,
// print the entire array as in
//
// prices: []                        # count == 0
// prices: [70.00]                   # count == 1
// prices: [50.00, 90.00]            # count == 2
// prices: [59.00, 45.00, 103.00]    # count == 3
//
// Otherwise, print the first 3 elements with a ... at the end as in
//
// prices: [10.00, 20.00, 30.00, ...] # count > 3
//
// PROFIT
// There is no 'profit' field in the struct. Instead, calculate the
// profit as the difference between the price at the 'best_sell' index
// and 'best_buy' index.  If these indices are -1 indicating the best
// buy/sell time is not known or not viable, print a proit of 0.0
void stock_print(stock_t *stock){ 
  if (stock->data_file == NULL){ //if the stock doesn't have a data file
    printf("data_file: NULL\n"); //data file is null
  }
  else{ //otherwise
    printf("data_file: %s\n", stock->data_file); //print out the info of the data file
  }

  printf("count: %d\n", stock->count); //print out the count
  printf("prices: "); //get ready to price the prices
  int count = stock->count; //fetch the count
  if (count == -1){ //if the count hasn't been modified
    printf("NULL\n"); //the count is null
  }
  else if (count == 0){ //if there is nothing inside
    printf("[]\n"); //print this
  }
  else{ //otherwise
  printf("[");
  for (int i = 0; i < count; i++){ //print prices
    if (i <3){ //print the prices up to 3 times
      printf("%.2f", stock->prices[i]);
    if(i != (count-1)){ //add commads as long as it's not going to be the last number
        printf(", ");
      }
    }
    if (i == 3){ //if there's more the 3 already, just leave the rest as ...
      printf(" ...");
    }
  }

  printf("]\n"); //close bracket
  }
  printf("min_index: %d\n", stock->min_index);
  printf("max_index: %d\n", stock->max_index);
  printf("best_buy:  %d\n", stock->best_buy);
  printf("best_sell: %d\n", stock->best_sell);
  if(stock->best_buy == -1 || stock->best_sell == -1){ //if best_buy or best_sell was never updated
    printf("profit:    0.00\n"); //no way to make profit
  }
  else{
   printf("profit:    %.2f\n", (stock->prices[stock->best_sell] - stock->prices[stock->best_buy])); //calculate profit by subtracting prices
  }
  return;
}


// PROBLEM 1: Sets the index of 'min_index' and 'max_index' fields of
// the stock to be the positions in 'prices' of the minimum and
// maximum values present in it. Uses a simple loop over the array
// 'prices' which is 'count' elements long to examine each for
// max/min. If 'count' is zero, makes no changes to 'min_index' and
// 'max_index'.
void stock_set_minmax(stock_t *stock){
  if(stock->count == 0){ //if the stock count is 0
    return; //don't make any changes
  }
  else{
    int cur_min = 0; //track minimum
    int cur_max = 0; //track maximum
    for (int i = 0; i < stock->count; i++){ //for each count
      if (stock->prices[cur_min] > stock->prices[i]){  //if minimum is larger than current price
          cur_min = i; //set current to the index of the current price
      }
      else if (stock->prices[cur_max] < stock->prices[i]){ //if maximum is larger than current pric
          cur_max = i; //set current to the index of the current price
      }
    }
    stock->min_index = cur_min; //after for loop, set index of current minimum
    stock->max_index = cur_max;
  }
  return;
}
  
// PROBLEM 2: Sets the 'best_buy' and 'best_sell' fields of 'stock'.
// This corresponds to the pair which produces the best profit. On
// determining the best buy/sell indices which produce a positive
// profit, sets these fields in 'stock' and returns 0. If there is no
// buy/sell point which would result in a positive profit, sets the
// 'best_buy' and 'best_sell' indices to -1 and returns -1. Always
// calculates the earliest buy/sell pair of indices that would get the
// best profit: if 5,8 and 5,9 and 7,10 all give the same, maximal
// profit, the best buy/sell indices are set to 5,7.
// 
// ALGORITHM NOTES
// One intuitive algorithm to compute this is to try every possible
// buy index (outer loop) and every possible sell index after it
// (inner loop) looking for the maximum profit produced in it. This is
// a O(N^2) algorithm with N=count.  Using this algorithm is a good
// start. Some MAKEUP CREDIT will be awarded for implementing a more
// efficient, O(N) algorithm here. See the specification for more details.
int stock_set_best(stock_t *stock){ //finds the best time to buy or sell
  stock->best_buy = -1;
  stock->best_sell = -1;
  float best_profit = 0.0;
  for (int i = 0; i < stock-> count; i++){ //loop through stocks from 0th index
    for(int n = i+1; n < stock->count; n++){ //loop through stocks starting from 1st index
      if ((stock->prices[n] - stock->prices[i]) > best_profit){ //if the difference between the two is larger than the best_profit
        stock->best_buy = i; //set the best buy index
        stock->best_sell = n; //set the best sell index
        best_profit = stock->prices[n] - stock->prices[i]; //set as best possible profits using indicies
    }
  }
  }
  if(best_profit == 0){ //if best profit stays at 0
    return -1; // no positive profit
  
  }
  return 0;
}


// PROBLEM 2: Opens file named 'filename' and counts how many times
// the '\n' newline character appears in it which corresponds to how
// many lines of text are in it.  Makes use of either fscanf() with
// the %c format to read single characters or alternative I/O
// functions like fgetc(). Closes the file before returning a count of
// how many lines are it it.  If for any reason the file cannot be
// opened, prints a message like
//
// Could not open file 'not-there.txt'
//
// and returns -1 to indicate failure.
int count_lines(char *filename){
  FILE *fh;
  fh = fopen(filename, "r"); //open up the filename
  if (fh == NULL){ //if file couldn't be opened
    printf("Could not open file 'data/not-there.txt'\n");
    return -1; //quit out
  }
  int count = 0;
  int ret = fgetc(fh); //get the charter
  while (!feof(fh)){ //while fh not at end of file
    ret = fgetc(fh); //get the next character
    if (ret == '\n'){ //if character is a newline
      count += 1; //add another newline
    }
    }
  fclose(fh); //close file
    return count;
}

// PROBLEM 2: Loads a stock from file 'filename' into 'stock' filling
// its 'prices' and 'count' fields in. Makes use of the count_lines()
// function to determine how many lines are in the file which will
// dictate 'count' and the length of 'prices'. Allocates space in the
// heap for the stock's 'prices' array, opens the 'filename' and
// iterates through reading prices into the array. The data format for
// prices files is
//
// time_03 133.00
// time_04 143.00
// time_05 168.00
// time_06 91.00
// 
// where each line has a time as as single string and a price which is
// floating point number. The times can be ignored which can be
// accomplished with a fscanf() call with format "%*s" to read a
// string but ignore/discard it.
// 
// Assigns the 'datafile' field to be a duplicated string of
// 'filename' for which 'strdup()' is extremely useful. This string
// must be free()'d later likely in 'stock_free()'
// 
// On successfully loading the stock, returns 0.
//
// If 'filename' cannot be opened, prints the message 
// 
// Unable to open stock file 'some-stock.txt', bailing out
//
// with 'filename' substituted in for the name of the stock and
// returns -1.
int stock_load(stock_t *stock, char *filename){
  FILE *fh;
  int line_cnt = count_lines(filename); //count how many lines in filename
  fh = fopen(filename, "r"); //read filename
  if (fh == NULL){ //file was not able to be opened
    printf("Unable to open stock file '%s', bailing out\n", filename);
    return -1;
  }
  stock->count = line_cnt; //set stock line count section
  stock->prices = malloc(sizeof(double)*line_cnt); //set space for stock prices
  stock -> data_file = strdup(filename); //duplicate filename to data file
  for (int i = 0; i < line_cnt; i++){
    fscanf(fh, "%*s %lf\n", &stock->prices[i]); //scan everything in to the file
  }
  fclose(fh);
  return 0;
}

// PROBLEM 2: Plots a graphical representation of stock
// information. First calculates and prints plot which is in the
// following format:
//
// max_width: 25
// range:     40.00
// plot step: 1.60
//
// The prints a header line and iterates through each stock data on it
// along with a bar which varies in width with the stock price. A
// sample format is as follows with some annotations.
// 
//....20 spaces.......+ ...max_width dashes.....
//01234567890123456789
//                    +-------------------------
//  0:         223.00 |################
//  1:         292.00 |######################
//  2: B        27.00 |#
//  3:          92.00 |######
//  4: S MAX   323.00 |#########################
//  5:         189.00 |##############
//  6:         207.00 |###############
//  7:         142.00 |##########
//  8:         321.00 |########################
//  9:          89.00 |######
// 10:   MIN    14.00 |
// 11:         182.00 |#############
// 12:         164.00 |############
// 13:         156.00 |###########
// 14:         169.00 |############
//01234567890123456789
//|    | |     |       |
//|    | |     |       +-> Each bar is (price-min)/plot_step hashes wide
//|    | |     +-> Stock price printed with format %8.2f
//|    | +-> Print MIN or MAX if the stock is at min_index/max_index
//|    +-> Print B or S if the stock is at the best_buy/best_sell index
//+--> Index in the array printed with format %3d
void stock_plot(stock_t *stock, int max_width){
  double range = stock->prices[stock->max_index]-stock->prices[stock->min_index]; //range of stocks, from highs price to lowest price
  double plot_step = range/max_width; //calculate the steps
  double min = stock->prices[stock->min_index]; //minimum stock prices

  printf("max_width: %d\n", max_width); //print width
  printf("range:     %.2f\n", (stock->prices[stock->max_index]-stock->prices[stock->min_index])); //print range
  printf("plot step: %.2f\n", range/max_width);
  printf("                   +"); //add spaces
  for (int i = 0; i < max_width; i++){
    printf("-"); //add dashes for everything
  }
  printf("\n"); //add newline after the -
  for (int n = 0; n < stock -> count; n++){
    printf("%3d:", n); 
    if (n == stock->best_buy){ //indicate best buy
      printf(" B ");
    }
    else if (n == stock->best_sell){ //indicate the best sell
      printf(" S ");
    }
    else{
      printf("   ");
    }

    if (n == stock->max_index){//print maximum index
      printf("MAX");
    }
    else if(n == stock->min_index){ //print minimum index
      printf("MIN");
    }
    else{
      printf("   ");
  }
  printf(" ");
  printf("%8.2f", stock->prices[n]); //print out stock prices to 2 floats
  printf(" |");
  int width = (stock->prices[n] - min) / plot_step;

  for (int x = 0; x < width; x++){//for each # print
    printf("#");
  } 
  printf(" \n");
}
}
