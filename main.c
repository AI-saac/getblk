//61113749
//Kazuki Tanaka

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "buf.h"
#include "dlist.h"
#include "state.h"
#include "getblk.h"
#include <assert.h>

#define USER_STRING_LIM 100

void help_proc(int, char *[]);
void init_proc(int, char *[]);
void buf_proc(int, char *[]);
void hash_proc(int, char *[]);
void free_proc(int, char *[]);
void getblk_proc(int, char *[]);
void brelse_proc(int, char *[]);
void set_proc(int, char *[]);
void reset_proc(int, char *[]);
void quit_proc(int, char *[]);
int ParseStatus(char *argv);

//helper function

buf *GetBuf(int index);
void PrintState(buf *p);
void PrintBufferOne(int index);
void PrintBufferAll();
void PrintHashLine(int hkey);
void PrintHashAll();
void PrintFree();
int SearchNum(int blkno);
buf *init();
void SetStatus(buf *h, int stat);
void ResetStatus(buf *h, int stat);
void PrintRoutine(buf *p, int index);
buf *Clone(int blkno);

typedef struct cmd_struct {
  char *cmd;
  void (*func)(int, char *[]);
} cmd_struct_t;

cmd_struct_t cmd_tbl[] =
{
  {"help", help_proc},
  {"init", init_proc},
  {"buf", buf_proc},
  {"hash", hash_proc},
  {"free", free_proc},
  {"getblk", getblk_proc},
  {"brelse", brelse_proc},
  {"set", set_proc},
  {"reset", reset_proc},
  {"quit", quit_proc},
  {NULL, NULL}
};
typedef int checker_t;

checker_t setbit = 1;
checker_t malloced = 0;
checker_t initnum = 0;

int main(int argc, char *argv[]){
  char user_string[USER_STRING_LIM];
  while(setbit){
    printf("$ ");
    if(fgets(user_string, USER_STRING_LIM, stdin) == NULL){
      printf("command not found:%s\n", user_string);
      exit(1);
    }
    cmd_struct_t *p;
    int ac = 0;
    char *av[16];
    ac = parseline(user_string, av);
    if(!ac){
      continue;
    }
    for(p = cmd_tbl; p -> cmd; p++){
      if(strcmp(av[0], p -> cmd) == 0){
        (*p -> func)(ac, av);
        free(av[0]);
        break;
      }
    }
    if(p -> cmd == NULL){
      fprintf(stderr, "command not found: %s\n", av[0]);
    }
  }
}

int parseline(char *cmdline, char **av){
  //char array[100];
   char *buffer = calloc(100, sizeof(char));
  //char *buf = malloc(strlen(cmdline) + 1);
  //int add = buf;
   //char *buffer = array;
  char *delim;
  int argc = 0;
  strcpy(buffer, cmdline);
  buffer[strlen(buffer) - 1] = ' '; // replace null terminator to space
  while(*buffer && (*buffer == ' ')){ //ignore leading space
    buffer++;
  }
  argc = 0;

  delim = strchr(buffer, ' ');

  while(delim){
    *delim = '\0';
    //av[argc++] = calloc(1, sizeof(char *));
    //strcpy(av[argc++], buf);
    av[argc++] = buffer;
    //*delim = '\0';
    buffer = delim + 1;
    while(*buffer && (*buffer == ' ')){
      buffer++;
    }
    delim = strchr(buffer, ' ');
  }
  av[argc] = NULL;
  if(argc == 0){
    printf("No argument is obtained\n");
    return 0;
  }
  else{
    for(int i = 0; i < argc; i++){
      puts(av[i]);
    }
  }
  //*buf = 1;
  return argc;
}

void help_proc(int num, char *name[]){
  //init
  printf("init  \n");
  printf("\tinitialize hash list and free list and make the \n");
  printf("\tinitial state shown in the figure 2.15. \n\n");
  //buf
  printf("buf[n ...] \n");
  printf("\tIf there is no input value, ");
  printf("display all the status of the buffers.\n");
  printf("\tIf there is an input block number, ");
  printf("display the status of the buffer at the block number.\n\n");
  //hash
  printf("hash[n ...] \n");
  printf("\tIf there is no input value, ");
  printf("display all the hash list.\n");
  printf("\tIf there is an input hash key, ");
  printf("display the hash list at the hash key given from user.\n\n");
  //free
  printf("free\n");
  printf("\tDisplay free list\n\n");
  //getblk
  printf("getblk n\n");
  printf("\ttake the blkno from the user, execute getblk(n)\n\n");
  //brelse
  printf("brelse n\n");
  printf("\ttake the blkno from the user, execute brelse(bp), \n");
  printf("\twhere bp is the pointer to buffer header with blkno = n");
  printf("\n\n");
  //set
  printf("set n stat [stat]\n");
  printf("\tset the status of the buffer of blkno n to stat\n\n");
  //reset
  printf("reset n stat [stat]\n");
  printf("\treset the status of the buffer of blkno n to stat\n\n");
  //quit
  printf("exit the software\n\n");
}

void init_proc(int ac, char *av[]){
  //init process
  if(initnum){
    for(int i = 0; i < NHASH; i++){
      //buf *trac = &h_head[i];
      while(!IsEmpty(i)){
        buf *ret = remove_hash_head();
        free(ret);
      }
      /*
      free(h_head[i].hash_fp -> hash_fp -> hash_fp);
      free(h_head[i].hash_fp -> hash_fp);
      free(h_head[i].hash_fp);
      */
    }
    initnum--;
  }
  malloced = 1;
  initnum++;
  for(int i = 0; i < NHASH; i++){
    h_head[i].hash_fp = &h_head[i];
    h_head[i].hash_bp = &h_head[i];
    h_head[i].stat = 0;
    h_head[i].free_fp = &f_head;
    h_head[i].free_bp = &f_head;
    h_head[i].cache_data = NULL;
    //buf *trace = &h_head[i];
    buf *p = malloc(sizeof(buf));
    buf *q = malloc(sizeof(buf));
    buf *r = malloc(sizeof(buf));
    if(p == NULL || q == NULL || r == NULL){
        printf("unable to assign memory\n");
        abort();
      }
      assert(p != NULL && q != NULL && r != NULL);

    insert_list(&h_head[i], p, HASHHEAD);
    insert_list(h_head[i].hash_fp, q, HASHHEAD);
    insert_list(((h_head[i].hash_fp) -> hash_fp), r, HASHHEAD);
  }
  /*
  buf *freebuffer = malloc(sizeof(buf));
  //f_head.free_fp
  freebuffer -> free_fp = freebuffer;
  freebuffer -> free_bp = freebuffer;
  freebuffer -> hash_fp = NULL;
  freebuffer -> hash_bp = NULL;
  freebuffer -> stat = 0;
  freebuffer -> cache_data = NULL;
  //f_head = *freebuffer;
  //f_head = freebuffer;
  //adding buffers
  //mod = 0
  */
  f_head.free_fp = &f_head;
  f_head.free_bp = &f_head;
  buf *trac = h_head[0].hash_fp;
  trac -> blkno = 28;
  SetStatus(trac, STAT_VALID);
  trac = trac -> hash_fp;
  trac -> blkno = 4;
  SetStatus(trac, STAT_VALID);
  trac = trac -> hash_fp;
  trac -> blkno = 64;
  SetStatus(trac, STAT_VALID | STAT_LOCKED);
  //mod = 1
  trac = h_head[1].hash_fp;
  trac -> blkno = 17;
  SetStatus(trac, STAT_VALID | STAT_LOCKED);
  trac = trac -> hash_fp;
  trac -> blkno = 5;
  SetStatus(trac, STAT_VALID);
  trac = trac -> hash_fp;
  trac -> blkno = 97;
  SetStatus(trac, STAT_VALID);
  //mod = 2
  trac = h_head[2].hash_fp;
  trac -> blkno = 98;
  SetStatus(trac, STAT_VALID | STAT_LOCKED);
  trac = trac -> hash_fp;
  trac -> blkno = 50;
  SetStatus(trac, STAT_VALID | STAT_LOCKED);
  trac = trac -> hash_fp;
  trac -> blkno = 10;
  SetStatus(trac, STAT_VALID);
  //mod = 3
  trac = h_head[3].hash_fp;
  trac -> blkno = 3;
  SetStatus(trac, STAT_VALID);
  trac = trac -> hash_fp;
  trac -> blkno = 35;
  SetStatus(trac, STAT_VALID | STAT_LOCKED);
  trac = trac -> hash_fp;
  trac -> blkno = 99;
  SetStatus(trac, STAT_VALID | STAT_LOCKED);

  //adding freelist
  trac = &f_head;
  //trac -> free_fp = NULL;
  /*
  insert_list(trac, h_head[3].hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, (h_head[1].hash_fp) -> hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, (h_head[0].hash_fp) -> hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, h_head[0].hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, (h_head[1].hash_fp) -> hash_fp -> hash_fp, FREEHEAD);
  trac = trac -> free_fp;
  insert_list(trac, (h_head[2].hash_fp) -> hash_fp -> hash_fp, FREEHEAD);
  */
  insert_list(trac, h_head[3].hash_fp, FREETAIL);
  insert_list(trac, h_head[1].hash_fp -> hash_fp, FREETAIL);
  insert_list(trac, h_head[0].hash_fp -> hash_fp, FREETAIL);
  insert_list(trac, h_head[0].hash_fp, FREETAIL);
  insert_list(trac, h_head[1].hash_fp -> hash_fp -> hash_fp, FREETAIL);
  insert_list(trac, h_head[2].hash_fp -> hash_fp -> hash_fp, FREETAIL);
  //remove_free(f_head.free_fp);
}
void buf_proc(int ac, char *av[]){
  if(!initnum){
    printf("Error! Nothing is started\n");
    printf("You should call init first\n");
    return;
  }
  if(ac <= 1){
    PrintBufferAll();
  }
  else{
    for(int i = 1; i < ac; i++){
      char array[100];
      strncpy(array, av[i], 100);
      if(!isalpha((array[0]))){
        if(0 <= atoi(array) && atoi(array) <= 11){
          PrintBufferOne(atoi(array));
          printf("\n");
        }
        else{
          printf("buffer number must be within 0 ~ 11\n");
        }
      }
      else{
        printf("INPUT VALUE MUST BE NUMBER, ASS WHOLE\n");
        printf("NOTHING PRINTED FOR THIS SHITTY REQUEST\n");
      }
    }
  }
}
void hash_proc(int ac, char *av[]){
  if(!initnum){
    printf("Error! Nothing is started\n");
    printf("You should call init first\n");
    return;
  }
  if(ac <= 1){
    PrintHashAll();
  }
  else{
    for(int i = 1; i < ac; i++){
      if(!isalpha((av[i][0]))){
        if(0 <= atoi(av[i]) && atoi(av[i]) <= 3){
          PrintHashLine(atoi(av[i]));
        }
        else{
          printf("INPUT VALUE MUST BE WITHIN 0 ~ 3, DUSHBAG\n");
          printf("NOTHING PRINTED FOR THIS SHITTY REQUEST\n");
        }
      }
      else{
        printf("YOU SHIT NUMBER PLEASE\n");
      }
    }
  }
}
void free_proc(int ac, char *av[]){
  if(!initnum){
    printf("Error! Nothing is started\n");
    printf("You should call init first\n");
    return;
  }
  PrintFree();
}
void getblk_proc(int ac, char *av[]){
  if(!initnum){
    printf("Error! Nothing is started\n");
    printf("You should call init first\n");
    return;
  }
  if(ac <= 1){
    printf("COMMON MAN, WHICH BLOCK ARE YOU TALKING ABOUT\n");
    printf("You Should Specify the block number BY YOURSELF SITTY ASS\n");
  }
  else{
    //char *arr = calloc(strlen(argv[1]) + 1, sizeof(char));
    //strcpy(arr, argv[1]);
    //int t = atoi(argv[1]);
    int t = atoi(av[1]);
    /*
    if(!IsExistBuf(t)){
      printf("You Ass\n");
      printf("There is no such block number in this cache\n");
    }
    */
    buf *blockedbuf = getblk(t);
    /*
    int t = 0;
    t = atoi(argv[1]);
    if(!isalpha((argv[1][0]))){
      struct buf_header *blockedbuf = getblk(t);
    }
    else{
      printf("INPUT VALUE MUST BE NUMBER, ASS WHOLE\n");
    }
    */
  }
}
void brelse_proc(int ac, char *av[]){
  if(!initnum){
    printf("Error! Nothing is started\n");
    printf("You should call init first\n");
    return;
  }
  if(ac <= 1){
    printf("COMMON MAN, WHICH BLOCK ARE YOU TALKING ABOUT\n");
    printf("You Should Specify the block number BY YOURSELF SITTY ASS\n");
  }
  else{
    if(!isalpha(atoi(av[1]))){
      buf *p = Clone(atoi(av[1]));
      brelse(p);
    }
    else{
      printf("INPUT VALUE MUST BE NUMBER, ASS WHOLE\n");
    }
  }
}
void set_proc(int ac, char *av[]){
  if(!initnum){
    printf("Error! Nothing is started\n");
    printf("You should call init first\n");
    return;
  }
  if(ac <= 2){
    printf("JUST DON'T DO THE SHITS\n");
  }
  else{
    int state = 0;
    for(int i = 2; i < ac; i++){
      int num = ParseStatus(av[i]);
      state += num;
    }
    buf *buffer = Clone(atoi((av[1])));
    int blkno = atoi((av[1]));
    if(!isalpha(blkno)){
      if(1 <= blkno && blkno <=12){
        if(buffer == NULL){
          printf("invaid cannot get buffer\n");
        }
        buffer -> stat = buffer -> stat | state;
      }
      else{
        printf("Errror input value for block number should be within 1~12\n");
      }
    }
    else{
      printf("This is not number but alphabet\n");
    }
  }
}

void reset_proc(int ac, char *av[]){
  if(!initnum){
    printf("Error! Nothing is started\n");
    printf("You should call init first\n");
    return;
  }
  if(ac <= 2){
    printf("JUST DON'T DO THE SHITS\n");
  }
  else{
    int state = 0;
    for(int i = 2; i < ac; i++){
      int num = ParseStatus(av[i]);
      state += num;
    }
    buf *buffer = Clone(atoi((av[1])));
    //int blkno = atoi((av[1]));
    if(!isalpha(av[1][0])){
      //if(1 <= blkno && blkno <=12){
      if(buffer == NULL){
        printf("invaid cannot get buffer\n");
      }
        //int midstate = (buffer -> stat) & state;
        //buffer -> stat = buffer -> stat & midstate;
      buffer -> stat = buffer -> stat ^ state;

      //  else{
      //printf("Errror input value for block number should be within 1~12\n");
      //}
    }
    else{
      printf("This is not number but alphabet\n");
    }
  }
}
void quit_proc(int ac, char *av[]){
  //free(av);
  if(malloced){
    for(int i = 0; i < NHASH; i++){
      //buf *trac = &h_head[i];
      while(!IsEmpty(i)){
        buf *ret = remove_hash_head();
        free(ret);
      }
      //free(h_head[i].hash_fp -> hash_fp -> hash_fp);
      //free(h_head[i].hash_fp -> hash_fp);
      //free(h_head[i].hash_fp);
    }
  }
  setbit = 0;
  return;
}

void PrintState(buf *p){
  int state = p -> stat;
  if(state & 0x20)  printf("O");
  else  printf("-");
  state = state << 1;

  if(state & 0x20)  printf("W");
  else  printf("-");
  state = state << 1;

  if(state & 0x20)  printf("K");
  else  printf("-");
  state = state << 1;

  if(state & 0x20)  printf("D");
  else  printf("-");
  state = state << 1;

  if(state & 0x20)  printf("V");
  else  printf("-");
  state = state << 1;

  if(state & 0x20)  printf("L");
  else  printf("-");

}
void PrintBufferOne(int index){
  int hkey = index / 3;
  buf *p = &h_head[hkey];
  for(int i = index % 3; i >= 0; i--){
    p = p -> hash_fp;
  }
  PrintRoutine(p, index);
  //printf("[ %d : %d ", index, p -> blkno);
  PrintState(p);
  printf("]\n");
}

void PrintBufferAll(){
  //printf("buf[n ...]\n");
  int index = 0;
  for(int i = 0; i < NHASH; i++){
    for(buf *p = h_head[i].hash_fp; p != &h_head[i]; p = p -> hash_fp){
      PrintRoutine(p, index);
      /*
      if(index >= 10){
        if(p -> blkno >= 10){
          printf("[%d : %d ", index, p -> blkno);
        }
        else{
          printf("[%d :  %d ", index, p -> blkno);
        }
      }
      else{
        if(p -> blkno >= 10){
          printf("[ %d : %d ", index, p -> blkno);
        }
        else{
          printf("[ %d :  %d ", index, p -> blkno);
        }
      }
      */
      PrintState(p);
      printf("]\n");
      index++;
    }
  }
}

void PrintHashLine(int hkey){
  int index = hkey * 3;
  for(buf *p = h_head[hkey].hash_fp; p != &h_head[hkey]; p = p -> hash_fp){
    //printf("\t[ %d : %d ", index, p -> blkno);
    PrintRoutine(p, index);
    PrintState(p);
    printf("]  ");
    index++;
  }
  printf("\n");
}

void PrintHashAll(){
  printf("Hash[n ...]\n");
  int index = 0;
  for(int i = 0; i < NHASH; i++){
    printf("%d :", i);
    for(buf *p = h_head[i].hash_fp; p != &h_head[i]; p = p -> hash_fp){
      //printf("\t[ %d : %d ", index, p -> blkno);
      PrintRoutine(p, index);
      PrintState(p);
      printf("]");
      index++;
    }
    printf("\n");
  }
}

void PrintFree(){
  int index = 0;
  for(buf *p = f_head.free_fp; p != &f_head; p = p -> free_fp){
    index = SearchNum(p -> blkno);
    PrintRoutine(p, index);
    //printf("\t[ %d : %d ", index, p -> blkno);
    PrintState(p);
    printf("]");
  }
  printf("\n");
}

int SearchNum(int blkno){
  buf *buffer = Search(blkno);
  int index = 0;
  for(int i = 0; i < NHASH; i++){
    for(buf *p = h_head[i].hash_fp; p != &h_head[i]; p = p -> hash_fp){
      if(p == buffer) return index;
      index++;
    }
  }
  return index;
}

buf *init(){
  buf *p = malloc(sizeof(buf));
  p -> hash_fp = NULL;
  p -> hash_bp = NULL;
  p -> stat = 0;
  p -> free_fp = NULL;
  p -> free_bp = NULL;
  p -> cache_data = NULL;

  return p;
}

void SetStatus(buf *h, int state){
  h -> stat = state;
}

void ResetStatus(buf *h, int stat){
  h -> stat = h -> stat ^ stat;
}

buf *GetBuf(int index){
  int quotient = index / 4;
  int remainder = index % 4;
  buf *p = h_head[quotient].hash_fp;
  for(int i = 0; i < remainder; i++){
    p = p -> hash_fp;
  }
  return p;
}

int ParseStatus(char *av){
  char val = av[0];
  switch(val){
  case('L'):
    return 1;
  case('V'):
    return 2;
  case('D'):
    return 4;
  case('K'):
    return 8;
  case('W'):
    return 16;
  case('O'):
    return 32;
  default:
    return 0;
  }
}

void PrintRoutine(buf *p, int index){
  if(index >= 10){
    if(p -> blkno >= 100){
      printf("[%d : %d ", index, p -> blkno);
    }
    else{
      if(p -> blkno >= 10){
        printf("[%d :  %d ", index, p -> blkno);
      }
      else{
        printf("[%d :   %d ", index, p -> blkno);
      }
    }
  }
  else{
    if(p -> blkno >= 10){
      if(p -> blkno >= 100){
        printf("[ %d : %d ", index, p -> blkno);
      }
      else{
        printf("[ %d :  %d ", index, p -> blkno);
      }
    }
    else{
      printf("[ %d :   %d ", index, p -> blkno);
    }
  }
}

buf *Clone(int blkno){
  int hkey = blkno % 4;
  buf *p;
  for(p = h_head[hkey].hash_fp; p != &h_head[hkey];p = p -> hash_fp){
    if(p -> blkno == blkno){
      return p;
    }
  }
  if(p == NULL){
    printf("Invalid block number, There is no such block number in cache\n");
  }
  return NULL;
}
