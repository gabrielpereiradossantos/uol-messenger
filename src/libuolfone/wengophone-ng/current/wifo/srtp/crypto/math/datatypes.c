/*
 * datatypes.c
 *
 * data types for finite fields and functions for input, output, and
 * manipulation
 *
 * David A. McGrew
 * Cisco Systems, Inc.
 */
/*
 *	
 * Copyright (c) 2001-2005 Cisco Systems, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 * 
 *   Neither the name of the Cisco Systems, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "datatypes.h"

int 
octet_weight[256] = {
  0, 1, 1, 2, 1, 2, 2, 3,
  1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4,
  2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5,
  3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6,
  4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7,
  5, 6, 6, 7, 6, 7, 7, 8
};

int
octet_get_weight(octet_t octet) {
  extern int octet_weight[256];

  return octet_weight[octet];
}  

/*
 * bit_string is a buffer that is used to hold output strings, e.g.
 * for printing.
 */

/* the value MAX_PRINT_STRING_LEN is defined in datatypes.h */

char bit_string[MAX_PRINT_STRING_LEN];

octet_t
nibble_to_hex_char(octet_t nibble) {
  char buf[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
		  '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
  return buf[nibble & 0xF];
}

char *
octet_string_hex_string(const void *s, int length) {
  const octet_t *str = s;
  int i;
  
  /* double length, since one octet takes two hex characters */
  length *= 2;

  /* truncate string if it would be too long */
  if (length > MAX_PRINT_STRING_LEN)
    length = MAX_PRINT_STRING_LEN-1;
  
  for (i=0; i < length; i+=2) {
    bit_string[i]   = nibble_to_hex_char(*str >> 4);
    bit_string[i+1] = nibble_to_hex_char(*str++ & 0xF);
  }
  bit_string[i] = 0; /* null terminate string */
  return bit_string;
}

inline int
hex_char_to_nibble(octet_t c) {
  switch(c) {
  case ('0'): return 0x0;
  case ('1'): return 0x1;
  case ('2'): return 0x2;
  case ('3'): return 0x3;
  case ('4'): return 0x4;
  case ('5'): return 0x5;
  case ('6'): return 0x6;
  case ('7'): return 0x7;
  case ('8'): return 0x8;
  case ('9'): return 0x9;
  case ('a'): return 0xa;
  case ('A'): return 0xa;
  case ('b'): return 0xb;
  case ('B'): return 0xb;
  case ('c'): return 0xc;
  case ('C'): return 0xc;
  case ('d'): return 0xd;
  case ('D'): return 0xd;
  case ('e'): return 0xe;
  case ('E'): return 0xe;
  case ('f'): return 0xf;
  case ('F'): return 0xf;
  default: return -1;   /* this flags an error */
  }
  /* NOTREACHED */
  return -1;  /* this keeps compilers from complaining */
}

int
is_hex_string(char *s) {
  while(*s != 0)
    if (hex_char_to_nibble(*s++) == -1)
      return 0;
  return 1;
}

/*
 * hex_string_to_octet_string converts a hexadecimal string
 * of length 2 * len to a raw octet string of length len
 */

int
hex_string_to_octet_string(char *raw, char *hex, int len) {
  octet_t x;
  int tmp;
  int hex_len;

  hex_len = 0;
  while (hex_len < len) {
    tmp = hex_char_to_nibble(hex[0]);
    if (tmp == -1)
      return hex_len;
    x = (tmp << 4);
    hex_len++;
    tmp = hex_char_to_nibble(hex[1]);
    if (tmp == -1)
      return hex_len;
    x |= (tmp & 0xff);
    hex_len++;
    *raw++ = x;
    hex += 2;
  }
  return hex_len;
}

char *
v128_hex_string(v128_t *x) {
  int i, j;

  for (i=j=0; i < 16; i++) {
    bit_string[j++]  = nibble_to_hex_char(x->octet[i] >> 4);
    bit_string[j++]  = nibble_to_hex_char(x->octet[i] & 0xF);
  }
  
  bit_string[j] = 0; /* null terminate string */
  return bit_string;
}

char *
v128_bit_string(v128_t *x) {
  int j, index;
  uint32_t mask;
  
  for (j=index=0; j < 4; j++) {
    for (mask=0x80000000; mask > 0; mask >>= 1) {
      if (x->v32[j] & mask)
	bit_string[index] = '1';
      else
	bit_string[index] = '0';
      ++index;
    }
  }
  bit_string[128] = 0; /* null terminate string */

  return bit_string;
}

void
v128_copy_octet_string(v128_t *x, const octet_t s[16]) {
#if ALIGNMENT_32BIT_REQUIRED
  if ((((uint32_t) &s[0]) & 0x3) != 0)
#endif
  {
	  x->octet[0]  = s[0];
	  x->octet[1]  = s[1];
	  x->octet[2]  = s[2];
	  x->octet[3]  = s[3];
	  x->octet[4]  = s[4];
	  x->octet[5]  = s[5];
	  x->octet[6]  = s[6];
	  x->octet[7]  = s[7];
	  x->octet[8]  = s[8];
	  x->octet[9]  = s[9];
	  x->octet[10] = s[10];
	  x->octet[11] = s[11];
	  x->octet[12] = s[12];
	  x->octet[13] = s[13];
	  x->octet[14] = s[14];
	  x->octet[15] = s[15];
  }
#if ALIGNMENT_32BIT_REQUIRED
  else 
  {
	  v128_t *v = (v128_t *) &s[0];

	  v128_copy(x,v);
  }
#endif
}

#ifndef DATATYPES_USE_MACROS /* little functions are not macros */

void
v128_set_to_zero(v128_t *x) {
  _v128_set_to_zero(x);
}

void
v128_copy(v128_t *x, const v128_t *y) {
  _v128_copy(x, y);
}

void
v128_xor(v128_t *z, v128_t *x, v128_t *y) {
  _v128_xor(z, x, y);
} 

void
v128_and(v128_t *z, v128_t *x, v128_t *y) {
  _v128_and(z, x, y);
}

void
v128_or(v128_t *z, v128_t *x, v128_t *y) {
  _v128_or(z, x, y);
}

void
v128_complement(v128_t *x) {
  _v128_complement(x);
}

int
v128_is_eq(const v128_t *x, const v128_t *y) {
  return _v128_is_eq(x, y);
}

int
v128_xor_eq(v128_t *x, const v128_t *y) {
  return _v128_xor_eq(x, y);
}

int
v128_get_bit(const v128_t *x, int i) {
  return _v128_get_bit(x, i);
}

void
v128_set_bit(v128_t *x, int i) {
  _v128_set_bit(x, i);
}     

void
v128_clear_bit(v128_t *x, int i){
  _v128_clear_bit(x, i);
}    

void
v128_set_bit_to(v128_t *x, int i, int y){
  _v128_set_bit_to(x, i, y);
}


#endif /* DATATYPES_USE_MACROS */

void
v128_right_shift(v128_t *x, int index) {
  const int base_index = index >> 5;
  const int bit_index = index & 31;
  int i, from;
  uint32_t b;
    
  if (index > 127) {
    v128_set_to_zero(x);
    return;
  }

  if (bit_index == 0) {

    /* copy each word from left size to right side */
    x->v32[4-1] = x->v32[4-1-base_index];
    for (i=4-1; i > base_index; i--) 
      x->v32[i-1] = x->v32[i-1-base_index];

  } else {
    
    /* set each word to the "or" of the two bit-shifted words */
    for (i = 4; i > base_index; i--) {
      from = i-1 - base_index;
      b = x->v32[from] << bit_index;
      if (from > 0)
        b |= x->v32[from-1] >> (32-bit_index);
      x->v32[i-1] = b;
    }
    
  }

  /* now wrap up the final portion */
  for (i=0; i < base_index; i++) 
    x->v32[i] = 0;
  
}

void
v128_left_shift(v128_t *x, int index) {
  int i;
  const int base_index = index >> 5;
  const int bit_index = index & 31;

  if (index > 127) {
    v128_set_to_zero(x);
    return;
  } 
  
  if (bit_index == 0) {
    for (i=0; i < 4 - base_index; i++)
      x->v32[i] = x->v32[i+base_index];
  } else {
    for (i=0; i < 4 - base_index - 1; i++)
      x->v32[i] = (x->v32[i+base_index] >> bit_index) ^
	(x->v32[i+base_index+1] << (32 - bit_index));
    x->v32[4 - base_index-1] = x->v32[4-1] >> bit_index;
  }

  /* now wrap up the final portion */
  for (i = 4 - base_index; i < 4; i++) 
    x->v32[i] = 0;

}


int
octet_string_is_eq(octet_t *a, octet_t *b, int len) {
  octet_t *end = b + len;
  while (b < end)
    if (*a++ != *b++)
      return 1;
  return 0;
}

void
octet_string_set_to_zero(octet_t *s, int len) {
  octet_t *end = s + len;

  do {
    *s = 0;
  } while (++s < end);
  
}

#if 0
/* 
 * bswap_32() is an optimized version of htonl/ntohl
 */

inline uint32_t
bswap_32(uint32_t v) {
#if CPU_CISC                     
#ifndef MIPSEL  /* MIPSEL stands for MIPS Endian Little, but it's not x86,
		 * so ignore the assembly language */
  /* assume that we're on an Intel x86 with x > 3 */
  asm("bswap %0" : "=r" (v) : "0" (v));
#endif
#endif
  /* assume that we're on a big-endian or non-intel machine */
  return v;
}
#endif

inline uint64_t
bswap_64(uint64_t v) {
#if CPU_CISC  /* assume that we're on an Intel x86 with x > 3 */
#ifndef MIPSEL  /* MIPSEL stands for MIPS Endian Little, but it's not x86,
		 * so ignore the assembly language */
  v= (bswap_32(v >> 32)) | ((uint64_t)bswap_32((uint32_t)v)) << 32 ;
#endif
#endif        /* assume that we're on a big-endian or non-intel machine */
  return v;
}




/*
 *  From RFC 1521: The Base64 Alphabet
 *
 *   Value Encoding  Value Encoding  Value Encoding  Value Encoding
 *        0 A            17 R            34 i            51 z
 *        1 B            18 S            35 j            52 0
 *        2 C            19 T            36 k            53 1
 *        3 D            20 U            37 l            54 2
 *        4 E            21 V            38 m            55 3
 *        5 F            22 W            39 n            56 4
 *        6 G            23 X            40 o            57 5
 *        7 H            24 Y            41 p            58 6
 *        8 I            25 Z            42 q            59 7
 *        9 J            26 a            43 r            60 8
 *       10 K            27 b            44 s            61 9
 *       11 L            28 c            45 t            62 +
 *       12 M            29 d            46 u            63 /
 *       13 N            30 e            47 v
 *       14 O            31 f            48 w         (pad) =
 *       15 P            32 g            49 x
 *       16 Q            33 h            50 y
 */

int
base64_char_to_sextet(octet_t c) {
  switch(c) {
  case 'A':
    return 0;
  case 'B':
    return 1;
  case 'C':
    return 2;
  case 'D':
    return 3;
  case 'E':
    return 4;
  case 'F':
    return 5;
  case 'G':
    return 6;
  case 'H':
    return 7;
  case 'I':
    return 8;
  case 'J':
    return 9;
  case 'K':
    return 10;
  case 'L':
    return 11;
  case 'M':
    return 12;
  case 'N':
    return 13;
  case 'O':
    return 14;
  case 'P':
    return 15;
  case 'Q':
    return 16;
  case 'R':
    return 17;
  case 'S':
    return 18;
  case 'T':
    return 19;
  case 'U':
    return 20;
  case 'V':
    return 21;
  case 'W':
    return 22;
  case 'X':
    return 23;
  case 'Y':
    return 24;
  case 'Z':
    return 25;
  case 'a':
    return 26;
  case 'b':
    return 27;
  case 'c':
    return 28;
  case 'd':
    return 29;
  case 'e':
    return 30;
  case 'f':
    return 31;
  case 'g':
    return 32;
  case 'h':
    return 33;
  case 'i':
    return 34;
  case 'j':
    return 35;
  case 'k':
    return 36;
  case 'l':
    return 37;
  case 'm':
    return 38;
  case 'n':
    return 39;
  case 'o':
    return 40;
  case 'p':
    return 41;
  case 'q':
    return 42;
  case 'r':
    return 43;
  case 's':
    return 44;
  case 't':
    return 45;
  case 'u':
    return 46;
  case 'v':
    return 47;
  case 'w':
    return 48;
  case 'x':
    return 49;
  case 'y':
    return 50;
  case 'z':
    return 51;
  case '0':
    return 52;
  case '1':
    return 53;
  case '2':
    return 54;
  case '3':
    return 55;
  case '4':
    return 56;
  case '5':
    return 57;
  case '6':
    return 58;
  case '7':
    return 59;
  case '8':
    return 60;
  case '9':
    return 61;
  case '+':
    return 62;
  case '/':
    return 63;
  case '=':
    return 64;
  default:
    return -1;
 }
 return -1;
}

/*
 * base64_string_to_octet_string converts a hexadecimal string
 * of length 2 * len to a raw octet string of length len
 */

int
base64_string_to_octet_string(char *raw, char *base64, int len) {
  octet_t x;
  int tmp;
  int base64_len;

  base64_len = 0;
  while (base64_len < len) {
    tmp = base64_char_to_sextet(base64[0]);
    if (tmp == -1)
      return base64_len;
    x = (tmp << 6);
    base64_len++;
    tmp = base64_char_to_sextet(base64[1]);
    if (tmp == -1)
      return base64_len;
    x |= (tmp & 0xffff);
    base64_len++;
    *raw++ = x;
    base64 += 2;
  }
  return base64_len;
}
