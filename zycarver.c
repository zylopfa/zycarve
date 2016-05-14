#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>
#include <signal.h>
#include <stdlib.h>
#include "zycarver.h"

#define MAX_RECORDS 3000

int count_lines(FILE* fp);
void usage();

void zycarve_lib_test(const char* filename) {

  FILE *sigfp;
  FILE *carvefp;
  int sig_total;
  int i;
  struct signatureRecord *sigrecords;
  const char* SIGNATUREFILE = "./signatures.dat";

  carvefp = fopen(filename,"r");
  if (!carvefp) {
    usage();
    return;
  }

  sigfp = fopen(SIGNATUREFILE,"r");

  if ( !sigfp) {
   fprintf(stderr,"\nError opening file for reading: %s\n",SIGNATUREFILE);
   usage();
   return;
  }

  sig_total = count_lines(sigfp);
  sigrecords = malloc(sig_total * sizeof(struct signatureRecord));

  sig_total = 0;
  while ( 3 == fscanf(sigfp,"%9[^|]|%199[^|]|%99[ABCDEF0123456789 ]\n",sigrecords[sig_total].shortname,sigrecords[sig_total].description,sigrecords[sig_total].signature)) {
    sig_total++;
  }

  if ( sig_total < 1 ) {
    fprintf(stderr,"\nPlease make sure %s contains atleast 1 valid signature!\n",SIGNATUREFILE);
    usage();
    return;
  }

  struct sigInfo* foundSignatures;
  foundSignatures = malloc(5000 * sizeof(struct sigInfo));
  int currentIndex = 0;
  for (i=0;i<sig_total;i++) {
    struct sigInfo *siginforecords;
    siginforecords = malloc(2000 * sizeof(struct sigInfo));

    int totalSig = zycarve_sigsearch(sigrecords[i],carvefp,siginforecords);
    int i;
    for ( i=0;i<totalSig;i++) {
      memcpy(&foundSignatures[currentIndex], &siginforecords[i],sizeof(struct sigInfo));
      currentIndex++;
    }

    free(siginforecords);
  }

  int sigTotal=currentIndex;
  printf("Found signatures total: %d\n",sigTotal);

  for (i=0;i<sigTotal;i++) {
    printf("Signature: %s, offset: %lx\n",foundSignatures[i].shortname,foundSignatures[i].offsetStart);
  }


  free(foundSignatures);
  free(sigrecords);
  fclose(sigfp);
  return;
}


int zycarve_sigsearch(const struct signatureRecord sig,FILE* fp, struct sigInfo* siginforecords) {

  char sigbuf[sizeof(sig.signature)];
  int sigLength = 0;

  strncpy(sigbuf,sig.signature,sizeof(sig.signature));

  char sigbytes[25];
  char* p = strtok(sigbuf," ");
  int i = 0;
  while (p != NULL && i < 25) {
    char number = (int)strtol(p, NULL, 16);
    sigbytes[i++] = number;
    p=strtok(NULL," ");
  }

  sigLength=i;

  char lastBytes[sigLength];
  long int  pos = 0; int curSearch = 0;

  fseek(fp, 0L, SEEK_END);
  long int lSize = ftell(fp);
  rewind(fp);

  int recordNr = 0;
  while(pos <= lSize && recordNr < 2000) {
    char curChar = getc(fp); pos++;

    if ( curChar == sigbytes[curSearch]) {
      curSearch++; 
      if ( curSearch > sigLength-1 ) {

        strncpy(siginforecords[recordNr].signature,sig.signature,sizeof(sig.signature));
        strncpy(siginforecords[recordNr].shortname,sig.shortname,sizeof(sig.shortname));
	siginforecords[recordNr].offsetStart =  ftell(fp)-sigLength;
	siginforecords[recordNr].offsetEnd   = 0;

	curSearch = 0;
	recordNr++;
	int read = fread(lastBytes,1,sigLength+1,fp);
	pos += read;
      }
    }
    else {
      curSearch = 0;
    }
  }
  return recordNr;
}



int count_lines(FILE* fp) {

  int lines;
  char ch;
  lines = 0;
  while(!feof(fp))
  {
    ch = fgetc(fp);
    if(ch == '\n')
    {
     lines++;
    }
  }

  rewind(fp);
  return lines;
}

void usage() {
  
  printf("\n");
  printf("Usage: \n");
  printf("\t./zycarver <file_to_be_carved>\n");
  printf("\n");
  printf("We expect the file signatures signatures.dat to be located in the same directory\n");
  printf("as the zycarver binary.\n");
  printf("\n"); 
  printf("signatures.dat format, 1 pr line :\n"); 
  printf("\n"); 
  printf("<short-name>|short description|hex values\n"); 
  printf("jpg|JPEG raw or in the JFIF or Exif file format|FF D8 FF \n"); 
  printf("\n"); 
  printf("\n"); 

  return;
}
