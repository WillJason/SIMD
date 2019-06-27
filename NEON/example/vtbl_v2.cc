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
	src_count   = src_size>>3;																				//�����ж��ٸ�����8�������ߵĵ�����
	table_count = table_size>>3;
	uint8x8_t src_8x8[src_count];																			//
	uint8x8_t tab_8x8[table_count];
	uint8x8_t rest_value;
	uint8x8_t tab_rest;

	uint16x4_t src16_l[src_count];
	uint16x4_t src16_h[src_count];
	uint32x4_t src32_l[src_count];
	uint32x4_t src32_h[src_count];
	//������uint16x8x2_t����������������������ߵ�����Ԫ��,ÿ�������߰���һ���޷��� 16λ��������
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
	
  for(x=0;x < src_count ;x++)																				//��32λԭʼ����ת��Ϊuint8x8_t�������Ա��ڲ��
  {
  	src32_l[x] = vld1q_u32(src+8*x);
  	src32_h[x] = vld1q_u32(src+8*x+4);
  	
  	src16_l[x] = vmovn_u32(src32_l[x]);
		src16_h[x] = vmovn_u32(src32_h[x]);
		s[x] = vcombine_u16(src16_l[x],src16_h[x]);
		src_8x8[x] = vmovn_u16(s[x]);
  }
  
  for(y = 0;y < table_count;y++)																		//��32λ��ת��Ϊuint8x8_t�������Ա��ڲ��
  {
	  tab32_l[y] = vld1q_u32(tab+8*y);
	  tab32_h[y] = vld1q_u32(tab+8*y+4);
	  tab16_l[y] = vmovn_u32(tab32_l[y]);
		tab16_h[y] = vmovn_u32(tab32_h[y]);
		t[y] = vcombine_u16(tab16_l[y],tab16_h[y]);
		tab_8x8[y] = vmovn_u16(t[y]);
	}
	
	for(z = 0;z < table_count;z++)																		//�����ڱ��а���uint8x8_t�Ĵ�Сһ��һ�β��
	{
		uint8x8_t  result= vtbl1_u8(src_8x8[0], tab_8x8[z]);						//���ڿ�ʼ��λ�ý���һ��uint8x8_t��С�Ŀռ����ڱ�����ҵ���ֵ�������������Ͻ������
		
		uint8x8_t fix_8x8 = vld1_u8(fix);																//ȡ���̶�����������֮��ļ�������
		tab_rest = vqsub_u8(tab_8x8[z],fix_8x8);												//�����б���ÿ�������ߵ�ƫ��ֵ��ȥһ��8�������ߴ�С��ֵ��������������õ��ľ��ǳ���������ƫ��ֵ����һ�����
		index = 0;																											//ÿ���µĲ���ԭʼ���ݵĿ�ͷλ�ò��
	  while((flag != 0)&& (index < (src_count-1)))									 //�����ҵ����������п�ֵ������0������û�а�����ԭʼ���ݶ���ѯһ��ͼ���ѭ��
	  {  
	  		flag=0;
			  
	  
	  		index++;
	  		rest_value = vtbl1_u8(src_8x8[index], tab_rest);						//���ݳ���������ƫ��ֵ����ҵ���Ӧԭʼ����
	
		  	if(vget_lane_u8(result,0)==0)																//ȡ��������0���ҵ���ֵ���ж��Ƿ�Ϊ0�����Ϊ0����û���ҵ���Ӧ�����ݣ��ͽ�֮��
		  	{																														//����������ƫ��ֵ����ҵ���Ӧԭʼ������䵽�������ߵ�λ��
		  		result=vset_lane_u8(vget_lane_u8(rest_value,0),result,0);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,1)==0)																//ͬ�ϣ�ȡ��������1
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,1),result,1);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,2)==0)																//ͬ�ϣ�ȡ��������2
		  	{
		  		result=vset_lane_u8(vget_lane_u8(rest_value,2),result,2);
		  		flag = 1;
		  	}
		  	if(vget_lane_u8(result,3)==0)																//ͬ�ϣ�ȡ��������3
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
		  	
		  	tab_rest = vqsub_u8(tab_rest,fix_8x8);											//�����ǳ������������������				
	  }
	  
	  vst1_u8(ap+8*z,result);	  																			//������һ�α�����ݾͽ��䱣�������ŵ�һ���洢�ռ�
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

