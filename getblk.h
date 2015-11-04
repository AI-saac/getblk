#ifndef GETBLK_H_
#define GETBLK_H_

//#include <stdio.h>
//#include <stdlib.h>
#include "buf.h"
//#include "dlist.h"
//#include "state.h"

buf *getblk(int blk_num);
void brelse(buf *buffer);
buf *Search(int blkno);
int isStatus(buf *buffer, int state);
void AddStatus(buf *buffer, int state);
void RemFromFreeList(buf *buf);
void AddToHash(buf *elem);
int IsInFreeList(buf *buffer);
int CheckStatus(buf *buffer, int state);
void RemStatus(buf *buffer, int state);
void MakeStatus(buf *buffer, int state);

#endif
