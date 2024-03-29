#include "everything.h"

int main(int argc, char *argv[]){

  if (argc < 2) {fprintf(stderr,"ERR: Usage is %s Dimacs *Binary* Graph [optional: seed]\n",argv[0]); exit(10);}

#ifdef DB
#warning "Debug Option On"
  BB=true;
#endif

#ifdef V
#warning "VERBOSE ON"
#endif

  initGraph(argc,argv);//initialize graphs
  
  //Recursive Largest First with Xconstraint
  int XRLFColor[nVertices];int nColorsXRLF=XRLF(XRLFColor,nVertices,nEdges);

  int tempColorAssigned[nVertices];for(int i=0;i <nVertices ;++i){tempColorAssigned[i]=-1;}

  setUpColorClasses(nColorsXRLF,tempColorAssigned,XRLFColor);

  AntsOps(nColorsXRLF,tempColorAssigned);//run the ants


  printSol((BB==true)?TOCHECK:NOCHECK);
  
  cleanUp();//clean up mem 
  return 0; 
}
