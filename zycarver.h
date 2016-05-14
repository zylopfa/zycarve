
struct signatureRecord {
  char shortname[10];
  char description[200];
  char signature[100];
};

struct sigInfo {
  char shortname[10];
  char signature[100];
  long int offsetStart;
  long int offsetEnd;
};

void zycarve_lib_test(const char* filename);
int zycarve_sigsearch(const struct signatureRecord sig,FILE* fp, struct sigInfo* srecords) ;
