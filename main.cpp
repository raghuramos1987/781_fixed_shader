#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <unistd.h>

#include "globals.h"
#include "main.h"

using namespace std;


int main(int argc, char *argv[])
{
    FILE *ifp = fopen(argv[1], "r");
    if(!strcmp(argv[1], "temp.ply"))
    {
        cout<<endl<<"Cannot use "<<argv[1]<<
            " as input. Please rename it"<< endl;
        exit(-1);
    }
    FILE *ofp = fopen("temp.ply", "w");
    normal_calc(ifp, ofp, 0);
    FILE *ifp1 = fopen("temp.ply", "r");
    plyread(ifp1); 
    OpenGLInit(argc, argv);
    return 0;
}
