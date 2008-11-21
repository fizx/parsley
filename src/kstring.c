#include <string.h>
#include <stdlib.h>
#include "kstring.h"

char* aastrcat(int count, char** strings) {
  size_t size = 1;
  for(int i = 0; i < count; i++){
    size += sizeof(char) * strlen(strings[i]);
  }
  char* output = (char*) malloc(size);
  for(int i = 0; i < count; i++){
    strcat(output, strings[i]);
  }
  return output;
}

char* astrcat(char* a, char* b) {
  char* arr[2] = {a, b};
  return aastrcat(2, arr);
}

char* astrcat3(char* a, char* b, char* c) {
  char* arr[3] = {a, b, c};
  return aastrcat(3, arr);
}

char* astrcat4(char* a, char* b, char* c, char* d) {
  char* arr[4] = {a, b, c, d};
  return aastrcat(4, arr);
}

char* astrcat5(char* a, char* b, char* c, char* d, char* e) {
  char* arr[5] = {a, b, c, d, e};
  return aastrcat(5, arr);
}

char* astrcat9(char* a, char* b, char* c, char* d, char* e, char* f, char* g, char* h, char* i) {
  char* arr[9] = {a, b, c, d, e, f, g, h, i};
  return aastrcat(9, arr);
}

void* memdup(void* input, int size){
  void* out = malloc(size);
  memcpy(out, input, size);
  return out;
}

char* esc(char* input) { 
  int size = sizeof(char) * 2 * strlen(input);
  char* buffer = malloc(size);
  char* bufptr = buffer;
  char c;
  while(c = *(input++)){
    switch(c){
      case '\n': *(bufptr++) = '\\'; *(bufptr++) = 'n'; break;
      case '\t': *(bufptr++) = '\\'; *(bufptr++) = 't'; break;
      case '\\': *(bufptr++) = '\\'; *(bufptr++) = '\\'; break;
      default:   *(bufptr++) = c;
    }
  }
  char* out = malloc(sizeof(char) * strlen(buffer));
  strcpy(out, buffer);
  free(buffer);
  return buffer;
}