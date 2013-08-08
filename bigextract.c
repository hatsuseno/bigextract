#include "bigextract.h"

int main(int argc, char **argv) {
  int *matrix = malloc(MAX_FILES*sizeof(int)), fc=0;
  if(argc < 3) { printHelp(argv[0]); }
  if(argv[1][0] == '-') {
    switch(argv[1][1]) {
      case 'f': findFiles(argv[2], matrix, &fc); break;
      case 'x': findFiles(argv[2], matrix, &fc); extractFiles(argv[2], argv[2], matrix, &fc); break;
      case 'n': findFiles(argv[3], matrix, &fc); extractFiles(argv[3], argv[2], matrix, &fc); break;
      case 'd': findFiles(argv[3], matrix, &fc); outputMatrix(argv[2], matrix, &fc); break;
      case 'e': findFiles(argv[2], matrix, &fc); break;
      case 'h': printHelp(argv[0]); break;
      default : printHelp(argv[0]);
    }
  }else {
    printHelp(argv[0]);
  }
  
  return 0;
}
