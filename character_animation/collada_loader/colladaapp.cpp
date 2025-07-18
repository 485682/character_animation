#include "collada.h"

int main(int argc, char * args[]) {
    if(argc == 3){
        collada doc;
        if( doc.loadfile(args[1]) ){ doc.genaratefile(args[2]); }
    }else{
        printf(" collada_loader [source file(.dae)]  [destination file] ");
    }
    return 0;
}
