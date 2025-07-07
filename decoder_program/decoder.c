#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mt19937ar.h"

int load_file(char* filename, char** buffer, unsigned int* buffer_len) {
  FILE *fp;
  struct stat file_info;
  size_t bytes_read = 0;

  fp = fopen(filename, "r");

  if(fp == NULL) {
    fprintf(stderr, "Can't open file: %s\n", filename);
    return 1;
  }

  // Check the size of the file and allocate memory
  if(fstat(fileno(fp), &file_info) != -1) {
    *buffer_len = file_info.st_size;

    if(*buffer_len > 0) {
      *buffer = (char *)calloc(sizeof(char), *buffer_len + 1);

      if(*buffer != NULL) {

        // Read the file into the buffer
        bytes_read = fread(*buffer, sizeof(char), *buffer_len + 1, fp);

        if((bytes_read > 0) && (bytes_read == *buffer_len)) {
          fclose(fp);
          return 0;
        }
      }
    }
  }

  fprintf(stderr, "Failure reading file\n");

  if(*buffer != NULL)
    free(*buffer);

  fclose(fp);
  return 1;
}

int main(int argc, char** argv) {
  char* shader;
  unsigned long* shuffle;
  unsigned int shader_len;
  unsigned long seed_val;
  unsigned int i, j;
  char c;

  if(argc > 2) {
    // Convert seed hex string to value
    if(sscanf(argv[2], "%lx", &seed_val) == 0) {
      fprintf(stderr, "Invalid seed hex value: %s\n", argv[2]);
      return 1;
    }

    // Load the contents of the file
    if(load_file(argv[1], &shader, &shader_len) > 0)
      return 1;

    fprintf(stderr, "File size: %i bytes\n", shader_len);
    fprintf(stderr, "Seed: %lu\n", seed_val);

    init_genrand(seed_val);

    // Generate random numbers
    shuffle = (unsigned long *)calloc(sizeof(unsigned long), shader_len);

    for(i = 0; i < shader_len; i++)
      shuffle[i] = (long)(genrand_int32()) % (shader_len - 2); // Random index from 0 to (bytes - 1)

    for(i = shader_len - 1; i-- > 0;) {
      j = shuffle[i];

      // Swap positions
      c = shader[j];
      shader[j] = shader[i];
      shader[i] = c;
    }

    for(i = 0; i < shader_len - 1; i++)
      printf("%c", shader[i]);
    putchar('\n');

    if(shader != NULL)
      free(shader);
  }
  else {
    fprintf(stderr, "Usage: %s <file> <seed>\n", argv[0]);
    return 1;
  }
  return 0;
}
