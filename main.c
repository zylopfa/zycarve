#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdarg.h>
#include <signal.h>
#include <stdlib.h>


#include "zycarver.h"


int main(int argc, char** argv) {

  zycarve_lib_test(argv[1]);


  return 0;
}
