#define PACKAGE "Aggsum"
#define VERSION "0.01"

#define SEEDNO 1024   /* default seed for randomizer */
#define ARG1 "<arrsz>"
#define ARG2 "<tnum>"
#define ARG3 "<seednum>"

#define THRSL_MIN 5      
#define UPBND_DATA_VAL 100 
#define LWBND_DATA_VAL 0 
int tonum (const char * nptr, int * num);

struct _appconf {
  int arrsz;
  int tnum;
  int seednum;
};

extern struct _appconf appconf;

struct _range {
  int start;
  int end;
};


int processopts (int argc, char **argv, struct _appconf *conf); 
int tonum (const char * nptr, int * num);
int validate_and_split_argarray (int arraysize, int num_thread, struct _range* thread_idx_range);
int generate_array_data (int* buf, int arraysize, int seednum);

/** display help */
void help (int xcode);
