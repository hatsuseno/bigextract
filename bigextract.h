#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIND_ONLY 0
#define FIND_AND_EXTRACT 1
#define FIND_AND_EXTRACT_CHRONO 2
#define FIND_AND_SAVE_MATRIX 3

#define FILE_BUFFER_SIZE 65536
#define MAX_FILES 1024
#define MAX_FILENAME 255

typedef struct {
  char APCTag[8];
  char APCVer[4];
  unsigned int APCSamples;
  unsigned int APCSampleRate;
  unsigned long APCSample;
} APCHeader;

char version[] = "0.1-alpha";

void printError(char*);
void printHelp(char*);
void findFiles(char*, int*, int*);
void extractFiles(char*, char*, int*, int*);
void outputMatrix(char*, int*, int*);
APCHeader readHeader(FILE*, int);

void outputMatrix(char *outfile, int *matrix, int *filecounter) {
  FILE* out;
  int i;
  if((out = fopen(outfile, "wb")) == NULL) { 
    printError("error while opening output file"); 
  }
  
  for (i = 0; i < *filecounter; i++) {
    fprintf(out, "Element #%d located on memory address %d\n", i+1, matrix[i]);
  }
  
  fclose(out);
  printf("BigExtract: successfully written %d entries to %s\n", *filecounter, outfile);
}

void extractFiles(char *workfile, char *outfile, int *matrix, int *filecounter) {
  FILE* workfp;
  FILE* out;
  char *filename = malloc(MAX_FILENAME);
  char *data = malloc(sizeof(char));
  int filesize=0;
  
  if((workfp = fopen(workfile, "rb")) == NULL) printError("error while opening file");
  
  int i;
  for(i=0; i < *filecounter; i++) {
    APCHeader header = readHeader(workfp, matrix[i]);
    fseek(workfp, matrix[i], SEEK_SET);
    filesize = (header.APCSamples / 2) + sizeof(APCHeader);
    data = realloc(data, filesize);
    fread(data, filesize, 1, workfp);
    snprintf(filename, MAX_FILENAME, "%s_%d.apc", outfile, matrix[i]);
    if((out = fopen(filename, "wb")) == NULL) printError("error while opening output file");
    fwrite(data, filesize, 1, out);
    fclose(out);
  }    
}

void printError(char *emsg) {
  printf("BigExtract: %s\nBigExtract: calling it a day...", emsg);
  exit(1);
}

void printHelp(char *filename) {
  printf("BigExtract v%s help\n", version);
  printf("Released in public domain, as is\n");
  printf("Rudolf Leermakers <rudolf@hatsuseno.org>\n\n");
  printf(" Usage : %s <options> [filename]\n\n", filename);
	printf(" -f        do a normal search, if possible by index\n");
  printf(" -x        extract files\n");
	printf(" -n name   extract files to name@adress\n");
	printf(" -d name   output matrix of found files to file\n");
	printf(" -h        display this message\n\n");
	exit(1);
}

void findFiles(char *workfile, int *matrix, int *filecounter) {
  FILE* workfp;
  char *filebuffer = malloc(FILE_BUFFER_SIZE);
  char signature[] = "CRYO_APC";
  int sigsize = 8, counter=0;
  unsigned long seekposition=0, filesize=0;
  int i=0;
  printf("BigExtract: searching for collisions in %s\n", workfile);
  printf("BigExtract: signature size is %d bytes\n", sigsize);
  
  if((workfp = fopen(workfile, "rb")) == NULL) { printError("error while opening file"); }
  fseek(workfp, 0, SEEK_END);
  if((filesize=ftell(workfp)) == 0) { printError("file size was zero"); }
  fseek(workfp, 0, SEEK_SET);
  
  while(seekposition < filesize - FILE_BUFFER_SIZE) {
    fseek(workfp, seekposition, SEEK_SET);
    fread(filebuffer, 1, FILE_BUFFER_SIZE, workfp);
    
    for(i=0; i < FILE_BUFFER_SIZE; i++) {
      if(!memcmp(filebuffer+i, signature, sigsize)) {
        matrix[counter] = seekposition+i;
        counter++;
      }
    }
    
    seekposition += FILE_BUFFER_SIZE;
  }
  
  free(filebuffer);
  fclose(workfp);
  
  printf("BigExtract: %d collisions detected\n", counter);
  *filecounter = counter;
}

APCHeader readHeader(FILE* file, int offset) {
  APCHeader header;
 
  fseek(file, offset, SEEK_SET);
  fread(header.APCTag, 8, 1, file);
  fread(header.APCVer, 4, 1, file);
  fread(&header.APCSamples, 4, 1, file);
  fread(&header.APCSampleRate, 4, 1, file);
  fread(&header.APCSample, 8, 1, file);
 
  return header;
}
