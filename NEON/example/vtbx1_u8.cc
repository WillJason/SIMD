
  uint8_t  srctbx[] = { 5,  1,  7,  0,  3,  0,  0 , 0};
  uint8_t  tabtbx[] = { 1,   0,  2,  4,  8,  8,  8,  8 };
  uint8x8_t bevc = vld1_u8(srctbx);
  uint8x8_t tabc = vld1_u8(tabtbx);
  uint8_t tab[] = {0,2,4,6,1,3,5,7};
  uint8x8_t t = vld1_u8(tab);
  uint8x8_t  res;
  res = vtbx1_u8(t,bevc,tabc);
  vst1_u8(dest,res);
  for (int x = 0 ; x < 8 ; x++)
    printf("%#d ", dest[x]);
  printf("\n");

//out£º1 5 7 3 1 3 5 7 
