/*
 * Author: SDAM
 *
 */

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <sys/mman.h>
#include <arm_neon.h>
#include <sys/time.h>

int main (void) 
{
	struct timeval start;
	struct timeval end;
	unsigned long diff;
	//举例：uint8x8x2_t代表是两组包含八条向量线的向量元素,每条向量线包含一个无符号 8位整型数据
	uint8x8x2_t tbl;
	uint8x16_t tab;
	
  uint8_t bp[] = { 1,  1,  2,  3,  5,  8, 13, 21,	4,  6,  8,  12,  2,  8, 43, 31 };
  uint8_t cp[] = { 0,  12,  4,  8,  1,  9,  14,  7 };

  tab = vld1q_u8(bp);
  uint8x8_t c = vld1_u8(cp);

	gettimeofday(&start,NULL);
	tbl.val[1] = vget_high_u8(tab);
	tbl.val[0] = vget_low_u8(tab);
  uint8x8_t a = vtbl2_u8(tbl, c);
  uint8_t ap[8];
  vst1_u8(ap,a);
	gettimeofday(&end,NULL);
	diff = 1000000 * (end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec; //计算运算时间
	printf("the difference is %ld\n",diff);

  for (int x = 0 ; x < 8 ; x++)
    printf("%3u ", ap[x]);
  printf("\n");

  return 0;
}

