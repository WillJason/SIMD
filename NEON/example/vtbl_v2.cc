/*
 * 
 * Author: SDAM
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


/*
*discribe:Find the corresponding raw data according to the table
*src		 :origin data
*cp      :table
return 	 :value of table with origin data
*/

uint8_t* vtbl(int src_size,uint32_t *src,int table_size,uint32_t *tab)
{
	static uint8_t ap[16];
	int src_count,table_count;
	src_count   = src_size>>3;																				//计算有多少个带有8条向量线的的向量
	table_count = table_size>>3;
	uint8x8_t src_8x8[src_count];																			//
	uint8x8_t tab_8x8[table_count];
	uint8x8_t rest_value;
	uint8x8_t tab_rest;

	uint16x4_t src16_l[src_count];
	uint16x4_t src16_h[src_count];
	uint32x4_t src32_l[src_count];
	uint32x4_t src32_h[src_count];
	//举例：uint16x8x2_t代表是两组包含八条向量线的向量元素,每条向量线包含一个无符号 16位整型数据
	uint16x4_t tab16_l[table_count];
	uint16x4_t tab16_h[table_count];
	uint32x4_t tab32_l[table_count];
	uint32x4_t tab32_h[table_count];
	
	uint16x8_t s[src_count];
	uint16x8_t t[table_count];
	//uint8_t  ep[16];
	int x,y,z,index;
	char flag = 1;
	
	uint8_t  fix[] = { 8,   8,  8,  8,  8,  8,  8,  8 };
	
  for(x=0;x < src_count ;x++)																				//将32位原始数据转换为uint8x8_t的向量以便于查表
  {
  	src32_l[x] = vld1q_u32(src+8*x);
  	src32_h[x] = vld1q_u32(src+8*x+4);
  	
  	src16_l[x] = vmovn_u32(src32_l[x]);
		src16_h[x] = vmovn_u32(src32_h[x]);
		s[x] = vcombine_u16(src16_l[x],src16_h[x]);
		src_8x8[x] = vmovn_u16(s[x]);
  }
  
  for(y = 0;y < table_count;y++)																		//将32位表转换为uint8x8_t的向量以便于查表
  {
	  tab32_l[y] = vld1q_u32(tab+8*y);
	  tab32_h[y] = vld1q_u32(tab+8*y+4);
	  tab16_l[y] = vmovn_u32(tab32_l[y]);
		tab16_h[y] = vmovn_u32(tab32_h[y]);
		t[y] = vcombine_u16(tab16_l[y],tab16_h[y]);
		tab_8x8[y] = vmovn_u16(t[y]);
	}
	
	for(z = 0;z < table_count;z++)																		//依次在表中按照uint8x8_t的大小一段一段查表
	{
		uint8x8_t  result= vtbl1_u8(src_8x8[0], tab_8x8[z]);						//先在开始的位置建立一个uint8x8_t大小的空间用于保存查找到的值，后面往后查表不断进行填充
		
		uint8x8_t fix_8x8 = vld1_u8(fix);																//取出固定的向量用于之后的减法运算
		tab_rest = vqsub_u8(tab_8x8[z],fix_8x8);												//用现有表里每个向量线的偏移值减去一个8条向量线大小的值，如果表超出索引得到的就是超出索引的偏移值，进一步查表
		index = 0;																											//每次新的查表从原始数据的开头位置查表
	  while((flag != 0)&& (index < (src_count-1)))									 //当查找到的数据里有空值（就是0）并且没有把所有原始数据都查询一遍就继续循环
	  {  
	  		flag=0;
			  
	  
	  		index++;
	  		rest_value = vtbl1_u8(src_8x8[index], tab_rest);						//根据超出索引的偏移值查表找到对应原始数据
	
		  	if(vget_lane_u8(result,0)==0)																//取出向量线0查找到的值并判断是否为0，如果为0代表没有找到对应的数据，就将之后
		  	{																														//超出索引的偏移值查表找到对应原始数据填充到该向量线的位置
		  		result=vset_lane_u8(vget_lane_u8(rest_value,0),result,0);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,1)==0)																//同上，取出向量线1
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,1),result,1);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,2)==0)																//同上，取出向量线2
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,2),result,2);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,3)==0)																//同上，取出向量线3
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,3),result,3);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,4)==0)
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,4),result,4);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,5)==0)
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,5),result,5);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,6)==0)
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,6),result,6);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,7)==0)
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,7),result,7);
		  		flag = 1;
		  	}
		  	
		  	tab_rest = vqsub_u8(tab_rest,fix_8x8);											//若还是超出索引则继续往后查表				
	  }
	  
	  vst1_u8(ap+8*z,result);	  																			//查找完一段表的数据就将其保存下来放到一个存储空间
	}
  
  return ap;
  
}


int main (void) 
{
	struct timeval start;
	struct timeval end;
	unsigned long diff;
	uint8_t *dest;
	int src_size,table_size;
	
  uint32_t 	 src[] = { 1,  1,   2,  3,  5,  8, 13, 21,	  4,  6,  8,  12,  2,  8, 43, 31,   	5,  11,  22,  38,  31,  41, 39, 61,	  9,  17,  19,  27,  37,  29, 28, 36 };
  uint32_t table[] = { 0,  12,  17,  21,  25,  30,  13,  7,    2,  14,  18,  22,  26,  31,  11,  8};
  
  //uint8_t dp[8];
	src_size=32;
	table_size=16;
	gettimeofday(&start,NULL);

  dest=vtbl(src_size,src,table_size,table);

  gettimeofday(&end,NULL);
  diff = 1000000 * (end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec; 
	
  
	printf("the difference is %ld\n",diff);

  for (int x = 0 ; x < 16 ; x++)
    printf("%3u ", dest[x]);
  printf("\n");

  return 0;
}

