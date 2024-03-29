//***** Utilities *****//
void printSeed(const char *c){ printf("%s : seed %u, %d %d %d\n",c,seed_t,rand(),rand(),rand());}

void randInit(const int &argc,char *argv[]){if(argc==3){seed_t=atoi(argv[2]);}else {seed_t = time(NULL);} srand(seed_t);}

int getDistinctColors(const vector<int>&v){
  int distinctColors=0;
  vector<int>ColorCount;
  for(int i = 0 ; i < v.size();++i){
	while(ColorCount.size()<=v.at(i)){
	  ColorCount.push_back(0);
	}
	ColorCount.at(v.at(i))++;

	if(ColorCount.at(v.at(i))==1){//if it's the first time added
	  distinctColors++;
	}
  }

  return distinctColors;
}

int getDistinctColors(const int v[], const int &vSize){
  int distinctColors=0;
  vector<int>ColorCount;
  for(int i = 0 ; i < vSize;++i){
	while(ColorCount.size()<=v[i]){
	  ColorCount.push_back(0);
	}
	ColorCount.at(v[i])++;

	if(ColorCount.at(v[i])==1){//if it's the first time added
	  distinctColors++;
	}
  }

  return distinctColors;
}



int getConflictOfVertex(const vertex *pVertex, const int color[]){
  int nConflicts=0;
  vector<int>adjV = pVertex->adj;
  for (int i = 0 ; i < adjV.size(); ++i){
	if(BB)assert(pVertices[adjV.at(i)]->v_id==adjV.at(i));
	if(BB)assert(getDIMACSBinaryEdgeSwap(pVertex->v_id,adjV.at(i)));

	if(color[pVertex->v_id]==color[adjV.at(i)]){
	  nConflicts++;
	}
  }
  //printf("vertex %d has %d conflict\n",pVertex->v_id,nConflicts);
  return nConflicts;
}

int updateConflictTable(const int color[], int conflict[]){
  int totalConflicts=0;
  for(int i = 0 ; i < nVertices ; ++i){
	conflict[i]=getConflictOfVertex(pVertices[i],color);
	totalConflicts+=conflict[i];
  }
  return totalConflicts;
}



void cleanUp(){
  for (int i=0;i<nVertices;++i){delete pVertices[i];} delete pVertices;
  //  for (int i = 0 ; i < nEdges ; ++i){delete pEdges[i];} delete pEdges;
  for (int i = 0 ; i <vAnts.size();++i){delete vAnts.at(i);}
}


void printSol(const int &MODE){

  int nthreads=1;//temp value, will be change when doing multi-threads


  if(BB)assert(getDistinctColors(bestColorResult)==bestResult);


  printf("nthreads: %d colors: %d vertices: %d edges %d colors_table: () bestIndex: %d seed: %u\n",
		 nthreads,bestResult,nVertices,nEdges,bestCycle,seed_t);
  
  if (MODE==TOCHECK){//write to file
	fstream os("soltest.txt",ios::out);

#ifndef KA  
	for(int i = 0; i < nVertices ; ++i){
	  os << bestColorResult.at(i)+1 << "\n";  //+1  means the index of color starts from 1 instead of 0
	}
#else //write to file using KA's format, just so able to run KA's verifier
#warning "Solution output in KA style"
	os << "c FILENAME: " << inputFile << "\n";
	os << "c bestcolor: " << bestResult << "\n";
	os << "c maxVertices: " << nVertices << "\n";
	os << "c actual Edges: "<< nEdges << "\n";
	os << "p edges " <<nVertices << " " << nEdges << " " << bestResult <<"\n";

	for(int i = 0; i < nVertices ; ++i){
	  os << "v " << i+1 << " " << bestColorResult.at(i)+1 << "\n";  //+1  means the index of color starts from 1 instead of 0
	}

#endif
  }


#ifdef V
  printSeed("End");  //print out the seed and some rand for debugging purpose
#endif

}

int compare_degree (const void *v1, const void *v2 ){
  vertex* a1 = *(vertex**)v1;  vertex* a2 = *(vertex**)v2;
  return a2->adj.size() - a1->adj.size();
}

void setGreedyClique(vector<vertex *>&theGreedyClique,const int &nVertices){
  vertex *pVertex ;
  vertex **sv = new vertex *[nVertices];

  int status[nVertices];
  
  //possible_in_output=0
  //not_in_output=-1;
  //in_output=1;
  for (int i=0;  i < nVertices; i++) {
    pVertex = pVertices[i];
    status[i] = 0;
	sv[i] = pVertex;
  }

  qsort((void*)sv, (size_t)nVertices, sizeof(vertex*), compare_degree);

  for (int i = 0; i < nVertices; i++){
	pVertex = sv[i];

    if (status[sv[i]->v_id] == 0){
      status[sv[i]->v_id] = 1;
	  theGreedyClique.push_back(pVertex);

	  for (int j = i+1 ; j < nVertices;++j){
		if (!getDIMACSBinaryEdgeSwap(pVertex->v_id,sv[j]->v_id)){
		  status[sv[j]->v_id]=-1;
		}
	  }//end for j
	}//end     if (pVertex->status == POSSIBLY_IN_OUTPUT){
  }

#ifdef V
  printf("Clique Size %d\n",theGreedyClique.size());
#endif

  delete sv;
}


//***** Initialize *****//

void initAnts(const int &nAnts){
  for(int i = 0 ; i < nAnts ; ++i){
	ant *a = new ant(); a->a_id=i ; a->current=NULL; a->old=NULL;
	vAnts.push_back(a);
  }
}


void initVerticesAndEdges(const int &nVertices, int &nEdges){


  pVertices = new vertex* [nVertices];  
  //pEdges = new edge* [nEdges];

  int edgeCount=0; 
  for (int i = 0 ;  i < nVertices ; ++i){
	vertex *vPtr = new vertex;	vPtr->v_id = i ; vPtr->numAnts = 0;
	pVertices[i]=vPtr;


	//todo: when done, change EdgeSwap to just Edge for performance

	for (int j = 0 ; j < i ;++j){
	  
	  if (getDIMACSBinaryEdgeSwap(i,j)){
		//		printf("e %d %d\n",i,j);


		//edge *edgePtr = new edge; edgePtr->e_id =edgeCount;	edgePtr->pOnEdge=0;

		//		pEdges[edgeCount]=edgePtr;

		pVertices[i]->adj.push_back(j);
		//		pVertices[i]->edgeList.push_back(edgeCount);		

		pVertices[j]->adj.push_back(i);
		//		pVertices[j]->edgeList.push_back(edgeCount);				
				
		edgeCount++;
		
	  }//end if (getDIMACSBinaryEdgeSwap(i,j))
	}//end for j 
  }//end for i


  //there are cases when the Dimacs graphs incorrectly have duplicate edges (i.e., edge i,j then edge j,i)
  if(edgeCount!=nEdges){
	if(BB)printf("WARNING: edgecount %d != nEdges %d , setting nEdges=edgeCount!!!!\n",edgeCount,nEdges);
	nEdges=edgeCount;
  }

}

void copyBestResult(const int color[]){
  if(BB)assert(bestColorResult.size()==nVertices);
  for(int i = 0 ; i < nVertices ; ++i){
	bestColorResult.at(i)=color[i];
  }

}

void initGraph(const int &argc,char *argv[]){

  randInit(argc,argv);

  inputFile=argv[1]; 
  //read_graph_DIMACS_ascii(inputFile,nVertices,nEdges);

  readDIMACSBinaryFormat(inputFile,nVertices,nEdges);


  for(int i = 0 ; i < nVertices; ++i){
	bestColorResult.push_back(i);
  }
  bestResult = nVertices;


  nCycles=nVertices*nCyclesFactor;if(nCycles>4000)nCycles=4000;
  nJolts=(int)(nVertices*nJoltPercent); if(nJolts>300)nJolts=300;
  nAnts=(int)(nVertices*nAntsPercent);if(nAnts>100)nAnts=100;
  breakCycles=int(nCycles/2); if(breakCycles>1500)breakCycles=1500;
  moveLimit=(nVertices>100)?20+(int)(nVertices/nAnts):(int)(moveLimitPercent*nVertices);
  rSizeLimit = (int)(moveLimit/rSizeLimitFactor);if(rSizeLimit<1)rSizeLimit=1;
  nRLFSetLimit=(int)(nVertices*RLFSetPercent);



  if(BB){
	printf("\n*******************\n");
	printf("Graph %s, nVertices %d, nEdges %d\n",inputFile,nVertices,nEdges);
	printf("nCycles %d, ",nCycles);
	printf("nJolts %d, ",nJolts);
	printf("nAnts %d\n",nAnts);
	printf("breakCycles %d, ",breakCycles);
	printf("moveLimit %d, ",moveLimit);
	printf("rSizeLimit %d, ",rSizeLimit);
	printf("nRLFSetLimit %d",nRLFSetLimit);
	printf("\n*******************\n");
  }


  initVerticesAndEdges(nVertices,nEdges);


}


//***** XLRF *****//
void updateDegreeB(const vector<int> &adj, const bool W[], int degreeB[]){

  vertex *tV; //temp vertex
  for(int i=0;i<adj.size();++i){
	tV=pVertices[adj.at(i)];
	for(int j=0;j<tV->adj.size();++j){
	  if (W[tV->adj.at(j)]){//if it's uncolored and safe, update its blacklist degree
		degreeB[tV->adj.at(j)]++;
	  }
	}
  }
}

int getLargestDegreeB(const bool W[], const int degreeB[]){
  int chosenV=-1, maxDeg=0;
  for(int i = 0 ; i < nVertices ;++i){
	if(W[i]){
	  if(degreeB[i]>maxDeg){
		maxDeg=degreeB[i]; 
		chosenV=i;
	  }
	}
  }

  if(chosenV==-1){
	int adjCount=0;
	for(int i = 0 ; i < nVertices ;++i){
	  if(W[i]){
		if(chosenV==-1){chosenV=i;} //desperately seeking for a vertex
		
		adjCount=pVertices[i]->adj.size();
		if(adjCount>maxDeg){
		  maxDeg=adjCount;
		  chosenV=i;
		}
	  }
	}
  }


  
  return chosenV;
}

void markW(const vector<int> &adj, bool W[], int &vSize){
  for(int i=0;i<adj.size();++i){
	if (W[adj.at(i)]){ //if it's NOT colored and is considered
	  W[adj.at(i)]=false;//no longer consider it
	  vSize--;
	}
  }
}


void markBlackList(const vector<int> &adj,const bool isColored[], bool B[], int &vSize){
  for(int i=0;i<adj.size();++i){
	if (!isColored[adj.at(i)]){//if it's UNcolored
	  if(!B[adj.at(i)]){//if it's not yet in the blacklist, add it
		B[adj.at(i)]=true;
		vSize++;
	  }
	}
  }//and for 
}



int XRLF(int colorAssigned[], const int &nVertices, const int &nEdges){

  bool isColored[nVertices];//stable set
  bool W[nVertices];//uncolored but can be included in stable set
  bool B[nVertices];//uncolored but can NO longer belong to stable set
  int degreeB[nVertices]; //degree of B that is inside W 

  int wSize=nVertices,bSize ;

  int currentColor=-1;
  int nVerticesWithThisColored=0;
  

  
  for(int i = 0 ; i < nVertices ;++i){
	colorAssigned[i]=-1; 
	isColored[i]=false;
	W[i]=true; //init all can be included
  }
  




  //temp vals

  int maxTest, chosenV, vAdjSize;

  while((nVerticesWithThisColored < nVertices) && (wSize > 0)){
	currentColor++;//current color 
	nVerticesWithThisColored=0; //no vertex has this color yet 

	//reset Blacklist
	for(int i = 0 ; i < nVertices ;++i){B[i]=false;degreeB[i]=0;}bSize=0;

	//reset chosen
	chosenV=-1;

	if(XLRF_METHOD==0){
	  maxTest=0;
	  for(int i = 0;i<nVertices;++i){
		if (W[i]){//only check UNcolored and safe vertices
		  vAdjSize=pVertices[i]->adj.size();
		  if (maxTest<vAdjSize){
			maxTest=vAdjSize;chosenV=i;
		  }
		}

	  }//for (i=0:nVertices)
	}//if (method ==0)
	
	if(BB)assert(chosenV!=-1);

	colorAssigned[chosenV]=currentColor;//incre then assign color

	isColored[chosenV]=true;
	nVerticesWithThisColored++;

	W[chosenV]=false;wSize--; //no longer considered


	markW(pVertices[chosenV]->adj,W,wSize);//marked W[adjV] to false 



	//	printf("coloring %d with color %d, total colored %d\n",
	//		   chosenV,currentColor,howManyColored(W,nVertices));	

	//tvn remove howManyColored() when done 
	//assert(nVerticesWithThisColored==howManyColored(isColored,nVertices));


	while(wSize>0){
	  //mark "UNcolored" adj ones to black list
	  markBlackList(pVertices[chosenV]->adj,isColored,B,bSize);


	  if(nVerticesWithThisColored>nRLFSetLimit){
		bSize=0;
		chosenV=-1;//reset
		for(int i = 0 ; i < nVertices ; ++i){
		  if(!isColored[i]){//if it's not colored,add it to blacklist
			B[i]=true;
			bSize++;
		  }
		}
#ifdef V
		printf("break out of while loop\n");
#endif
		break; //break out of the loop
		
	  }
	  

	  //update degree of B in respect to W
	  updateDegreeB(pVertices[chosenV]->adj,W,degreeB);
	  chosenV=getLargestDegreeB(W,degreeB);

	  colorAssigned[chosenV]=currentColor;//incre then assign color

	  isColored[chosenV]=true;
	  nVerticesWithThisColored++;

	  W[chosenV]=false;wSize--; //no longer considered
	  

	  markW(pVertices[chosenV]->adj,W,wSize);//marked W[adjV] to false 
	  
	  //printf("coloring %d with color %d\n", chosenV,currentColor);
	  
	}//end while(wSize>0)
	
	markBlackList(pVertices[chosenV]->adj,isColored,B,bSize);
	
	wSize = 0;
	for(int i=0; i < nVertices; ++i){
	  W[i] = B[i];
	  //reset the count of 1's in W                                                               
	  if(B[i]) wSize++;
	}

	

  }//while loop


  //save solution
  for(int i = 0; i < nVertices; ++i){
	if (colorAssigned[i]<0){//if vertex is uncolored
	  currentColor++;
	  colorAssigned[i]= currentColor;
	  printf("current color %d\n",currentColor);
	}
  }
  

  int XRLF_Colors=currentColor+1; //since color index starts from 0 ;

  //save the best (lowest) colors
  //this won't even need to be check if this function is called 
  //in the first time (since bestNumColor == nVertices init)
  if (XRLF_Colors<bestResult){
	bestResult=XRLF_Colors;
	copyBestResult(colorAssigned);
  }


  //printSeed("XRLF()");



  return XRLF_Colors;
}


void setUpColorClasses(const int &colorsUsedXRLF, int colorAssigned[], const int colorOrig[]){

  int betaLimit=(int)(colorsUsedXRLF*BETA);
  int vertexRecoloredCounter=0;
  int betaReColorIndex=-1;
  bool toBeRecolor[nVertices];
  vector<int>keepColorsV,unkeepColorsV;
  

  for(int i = 0 ; i < nVertices ;++i){toBeRecolor[i]=true;}
  
  for(int j = 0 ; j< colorsUsedXRLF ;++j){
	//	if(keepColorsV.size()>betaLimit)break;  //just in case it gets too 'lucky' and goes bizzard 

    if(rand()%1000<=BETA*1000){//if choose to reIndex this color
	  keepColorsV.push_back(j);
    }
	else{
	  unkeepColorsV.push_back(j);
	}
	
  }

  //  printf("before, beta limit %d, keep %d, unkeep %d\n",betaLimit,keepColorsV.size(),unkeepColorsV.size());  
  if(BB)assert(keepColorsV.size()+unkeepColorsV.size()==colorsUsedXRLF);


  while(keepColorsV.size()>betaLimit){
	int tIndex=rand()%keepColorsV.size();
	unkeepColorsV.push_back(keepColorsV.at(tIndex));
	keepColorsV.erase(keepColorsV.begin()+tIndex);
  }
  while(keepColorsV.size()+1<=betaLimit){
	int tIndex=rand()%unkeepColorsV.size();
	keepColorsV.push_back(unkeepColorsV.at(tIndex));
	unkeepColorsV.erase(unkeepColorsV.begin()+tIndex);
  }

  //  printf("after, beta limit %d, keep %d, unkeep %d\n",betaLimit,keepColorsV.size(),unkeepColorsV.size());  
  if(BB)assert(keepColorsV.size()+unkeepColorsV.size()==colorsUsedXRLF);



  //recolor & reindex
  for(int j = 0 ; j < keepColorsV.size(); ++j){
	  betaReColorIndex++;
      for(int i = 0 ; i < nVertices ;++i){
        if(colorOrig[i]==keepColorsV.at(j)){
		  //printf("choosing color %d==%d, vertex %d\n",j,colorAssigned[i],i);
          if(BB)assert(toBeRecolor[i]);
		  vertexRecoloredCounter++;
          colorAssigned[i]=betaReColorIndex;
          toBeRecolor[i]=false;
        }
      }
  }
	   


  
  int lg=0;
  if(BB){
	lg=0;
	for(int i=0;i <nVertices ;++i){
	  if(colorAssigned[i]>lg)lg=colorAssigned[i];
	}

#ifdef V
	printf("Q BETA is %d, percent %f, XG %d, recolored vertices %d, largest Color Index %d\n",
		   keepColorsV.size(),(double)keepColorsV.size()/(double)colorsUsedXRLF,colorsUsedXRLF,vertexRecoloredCounter,lg);
#endif

	if(lg==0){
	  printf("lg = %d\n",lg);
	  printf("(QColor)largest color index %d\n",lg);
	  assert(1==0);
	}
  }

  

  vertexRecoloredCounter=0; //reset  , can be removed , only for debug purpose


  //random color distribution                                                                                          
  int deltaNumColors=(int)(colorsUsedXRLF*DELTA); //limit the numbers to 0:availColor  
  //probably I can just pick the colors RANDOMLY
  for(int i = 0 ; i < nVertices ;++i){

    if(toBeRecolor[i]){//if can be colored
	//tvn todo: get rid of toBeRecolor, just check if colorAssigned==-1 or not
	  if(BB)assert(colorAssigned[i]==-1);
      colorAssigned[i]=rand()%(deltaNumColors); //TVN TO FIX,  just to be consistent w/ KA's  ,  take out -1 later

	  vertexRecoloredCounter++;//can be remove later
    }
  }


  

  lg=0;
  if(BB){
	lg=0;
	for(int i=0;i <nVertices ;++i){
	  if(colorAssigned[i]>lg)lg=colorAssigned[i];
	}

#ifdef V
	printf("Q DELTA is %d, percent %f, XG %d, recolored vertices %d, largest Color Index %d\n",
		   deltaNumColors,(double)deltaNumColors/(double)colorsUsedXRLF,colorsUsedXRLF,vertexRecoloredCounter,lg);
#endif

	if(lg==0){
	  printf("lg = %d\n",lg);
	  printf("(QColor)largest color index %d\n",lg);
	  assert(1==0);
	}
  }

  //  if (BB){
	//	printf("colorsKeepPercent %g, totalColors %d, totalColorsKept %d, colorRandom %d\n",
	//		   colorsKeepPercent,colorsUsedXRLF,colorsKeepCounter,availColor);
	//  }


}

int chooseInitialMoveKA(const int &MOVE_METHOD, const int conflictTable[]){
  int movePos=-1;

  if(MOVE_METHOD==RANDOM_MOVE){	movePos=rand()%nVertices; }

  if(MOVE_METHOD==MAX_CONFLICT_MOVE){

	//	assert(1==0);
	movePos=0;

	for(int i=1;i<nVertices;++i){
	  if(BB)assert(conflictTable[i]!=-1);


	  if(conflictTable[i]>conflictTable[movePos]){
		movePos=i;//tvn todo: what if more than 1 has max conflicts ?
	  }
	}//end for

	if(BB){for(int i = 0 ;  i< nVertices;++i)assert(conflictTable[i]<=conflictTable[movePos]);}
  }
  

  return movePos;
}



int chooseInitialMove(const int &curPos, const int &MOVE_METHOD, const int conflictTable[]){
  int movePos=-1;

  if(MOVE_METHOD==RANDOM_MOVE){	do{movePos=rand()%nVertices;}while(movePos==curPos);}


  if(MOVE_METHOD==MAX_CONFLICT_MOVE){

	//movePos=0, if curPos also is 0 , then movePos = 1
	movePos=(curPos==0)?1:0;

	for(int i=0;i<nVertices;++i){if(BB)assert(conflictTable[i]!=-1);
	  
	  if(curPos!=i){
		if(conflictTable[i]>conflictTable[movePos]){
		  movePos=i;//tvn todo: what if more than 1 has max conflicts ?
		}
	  }
	  
	}//end for

	if(BB){for(int i = 0 ;  i< nVertices;++i)assert(conflictTable[i]<=conflictTable[movePos]);}
  }
  
  if(BB)assert(movePos!=-1&&movePos!=curPos);
  return movePos;
}


//move ant to random pos
int chooseNextMoveRANDOM(const int &curPos){
  int nextPos=-1;
  do{
	nextPos=rand()%nVertices;
  }while(nextPos==curPos);
  return nextPos;
}

//move ant to the most conflicted pos
vertex *chooseNextMoveHEURISTIC(vertex *curPos, const int conflicts[]){

  int highestConflictIndex=0;

  if(curPos->v_id==highestConflictIndex){
	highestConflictIndex=1;
  }

  for(int i=0;i<nVertices;++i){
	if(BB)assert(conflicts[i]!=-1);
	if(conflicts[i]>conflicts[highestConflictIndex]&&curPos!=pVertices[i]){
	  highestConflictIndex=i;//tvn todo: what if more than 1 has max conflicts ?
	}
  }
	
  if(BB)assert(highestConflictIndex!=curPos->v_id);

  return pVertices[highestConflictIndex];
}

//color the vertex
void colorTheVertex(const ant* theAnt,const int &maxNColor,int colorsTable[], int conflictsTable[]){
  //  printf("Max Color %d\n",maxNColor);

  int currPos=-1, oldPos=-1; 

  if(BB)assert(theAnt->current!=NULL);


  if(theAnt->old!=NULL)oldPos=theAnt->old->v_id;
  if(theAnt->current!=NULL)currPos=theAnt->current->v_id;

  vector<int> adjV=theAnt->current->adj;

  
  //if(BB)assert(currPos!=oldPos);
  
  int oldColor=colorsTable[currPos];
  
  int colorInAdj[maxNColor];for(int i = 0 ; i < maxNColor; ++i){colorInAdj[i]=0;}
  



  for(int i= 0 ; i<adjV.size();++i){

	if(BB){
	  if(colorsTable[adjV.at(i)] >= maxNColor){
		printf("color in vertex %d is %d, maxNColor %d\n",adjV.at(i),colorsTable[adjV.at(i)],maxNColor);	  
		assert(1==0);
	  }

	}

	//	}
	colorInAdj[colorsTable[adjV.at(i)]]++;
  }

  //avoid choosing color from old POS
  if(getDIMACSBinaryEdgeSwap(oldPos,currPos)){
	//printf("Will not choose color %d\n", colorsTable[oldPos]);
	colorInAdj[colorsTable[oldPos]]=2*nVertices;
  }


  vector<int>leftOver;
  for(int i = 0 ; i < maxNColor; ++i){
	if(!colorInAdj[i])leftOver.push_back(i);  //if no adj has this color, then push it to leftOver
  }


  //  if ( getDimacsBinaryEdgeSwap(oldPos,currPos ) && (v != oldPosition) )
  //	colorcountInNeighbors[currentColoring[oldPosition]]= 2* maxV;


  //  printf("Conflict before %d, %d , color %d : ",
  //		 getConflictOfVertex(pVertices[currPos],colorsTable),getTotalConflict(colorsTable),oldColor);

  if(leftOver.size()){
	colorsTable[currPos]=leftOver.at(rand()%leftOver.size());
	conflictsTable[currPos]=0;
	if(BB)assert(getConflictOfVertex(pVertices[currPos],colorsTable)==0);

	//assert(oldColor!=colorsTable[currPos]);
  }
  else{
	int minCount=colorInAdj[0];
	for(int i = 1 ; i < maxNColor; ++i){
	  if(colorInAdj[i]<minCount){
		minCount=colorInAdj[i];
	  }
	}
	


	if(BB){ for(int i = 0 ; i < maxNColor ; ++i){ assert(colorInAdj[i]>=minCount);}}


	vector<int> MinIndexVector;
	for(int i = 0 ; i < maxNColor ; ++i){
	  if(colorInAdj[i]==minCount)MinIndexVector.push_back(i);
	}

	colorsTable[currPos]=MinIndexVector.at(rand()%MinIndexVector.size());
	conflictsTable[currPos]=getConflictOfVertex(pVertices[currPos],colorsTable);

  }

  //  printf("after %d, %d, color %d\n",getConflictOfVertex(pVertices[currPos],colorsTable),getTotalConflict(colorsTable),colorsTable[currPos]);

  
  //update only the conflicts LOCALLY
  if(colorsTable[currPos]!=oldColor){//if changed the Color
	
	//decrement since the colors has changed
	  for(int i= 0 ; i<adjV.size();++i){
		if(colorsTable[adjV.at(i)] == oldColor){
		  conflictsTable[adjV.at(i)]--;
		}
	  }
	
	  //if there's still conflict, and if they are the same color , incre
	if(conflictsTable[currPos]){//if still conflict  , TVN: not sure if this should be done , doesn't make too much sense
	  for(int i= 0 ; i<adjV.size();++i){
		if(colorsTable[adjV.at(i)] == colorsTable[currPos]){
		  conflictsTable[adjV.at(i)]++;
		}
	  }
	}//if still conflict

	
  }

}

//move the ants, very frequently used operation
int  moveAnt(const int &curPos,const int &MOVE_METHOD, 
			  const vector<int> &recentlyVisited,bool considerVertex[],
			  const int conflictTable[]){

  int movePos=-1;
  vector<int> adjV=pVertices[curPos]->adj;
  vector<int>chosenOne;


  if(!adjV.empty()){//if it has some adj vertices
  
	for(int i=0 ; i < nVertices ;++i)considerVertex[i]=false;//reset	
	for (int i=0 ; i < adjV.size(); ++i)considerVertex[adjV.at(i)]=true; //consider these adj's
	for(int i=0 ; i < recentlyVisited.size(); ++i) {
	  considerVertex[recentlyVisited.at(i)]=false;//don't considered ones recently visited
	}
	
	
	
	if(MOVE_METHOD==RANDOM_MOVE){	
	  for(int i=0 ; i < adjV.size();++i){
		if(considerVertex[adjV.at(i)]){
		  if(BB)assert(getDIMACSBinaryEdgeSwap(curPos,adjV.at(i)));
		  chosenOne.push_back(adjV.at(i));
		}
	  }

	  //if(BB){if(chosenOne.empty())printf("chosenOne is empty");}
	  
	  
	}
	
	if(MOVE_METHOD==MAX_CONFLICT_MOVE){
	  
	  int maxConflict=0;
	  int vid;
	  
	  for(int i = 0 ; i < adjV.size(); ++i){
		vid=adjV.at(i);
		if(considerVertex[vid]){
		  if(conflictTable[vid]>maxConflict){
		  maxConflict=conflictTable[vid];
		  }
		}
	  }//end for
	  
	  
	  if(maxConflict){//if there are conflicts
		
		if(BB)assert(chosenOne.empty());

		for(int i = 0 ; i < adjV.size(); ++i){
		  vid=adjV.at(i);
		  if(considerVertex[vid]){
			if(conflictTable[vid]==maxConflict)chosenOne.push_back(vid);
		  }
		}
		if(BB)assert(!chosenOne.empty()); 
	  }
	  
	}
  }
  else{//if this vertex has no adj vertices

	for(int i=0 ; i < nVertices ;++i)considerVertex[i]=true;//reset	
	for(int i=0 ; i < recentlyVisited.size(); ++i) {
	  considerVertex[recentlyVisited.at(i)]=false;//don't considered ones recently visited
	}
	for(int i=0 ; i < nVertices ;++i){
	  if(considerVertex[i])chosenOne.push_back(i);
	}
	
  }

  movePos=chosenOne.empty()?adjV.at(rand()%adjV.size()):chosenOne.at(rand()%chosenOne.size());  

  if(BB)assert(movePos!=-1);
  return movePos;
}


void reColorMoreThanQ(const int &nColors, int colorTable[], int conflictsTable[]){
  if(BB)assert(nColors>0);//if it's 0 then only 1 color
  

  //possible a bug here, is it >= or just >  ?  remember my color index starts from 0, probably >=
  //>=  since nColors is the number of colors avail, so it starts from 0 to nColors-1 ,  thus the max color index is nColors-1
  for(int i = 0; i < nVertices; ++i){
	if(colorTable[i]>=nColors){ 
	  colorTable[i] = rand()%nColors;
	}
  }

  updateConflictTable(colorTable,conflictsTable);  

  //remember to call update
}


void AntsOps(int &colorsUsed,int currentColor[]){

  initAnts(nAnts);//add the ants

  int alphaNumColors=(int)(colorsUsed*ALPHA); //tvn todo: no need to use ceil, get rid of it and also #include<cmath>

  int lg=0;
  if(BB){
	lg=0;
	for(int i=0;i <nVertices ;++i){
	  if(currentColor[i]>lg)lg=currentColor[i];
	}

#ifdef V
	printf("Q ALPHA is %d, percent %f, XG %d, largest Color Index %d\n",
		   alphaNumColors,(double)alphaNumColors/(double)colorsUsed,colorsUsed,lg);
#endif

	if(lg==0){
	  printf("lg = %d\n",lg);
	  printf("(QColor)largest color index %d\n",lg);
	  assert(1==0);
	}
  }




  int conflictsTable[nVertices];
  int totalConflicts = updateConflictTable(currentColor,conflictsTable);  
  //  printf("Total Conflict %d, %d\n",totalConflicts, getTotalConflict(conflictsTable));


  vector<int> recentlyVisited ;
  bool considerVertices[nVertices];


  int changedCycle=0;
  int moveSoFar;

  for (int iCycles=0 ; iCycles < nCycles; ++iCycles) {
	for(int iAnts=0; iAnts<nAnts; ++iAnts){
	  ant *theAnt=vAnts.at(iAnts);
	  moveSoFar=0;

	  if(!recentlyVisited.empty())recentlyVisited.clear();//resest



	  //TVN Todo :  there's no probablilty for activation etc ... add this 
	  //there's no factor to reduce exploration and increase exploitation 

	  //assert(theAnt->current==NULL);

	  
	  theAnt->current=pVertices[chooseInitialMoveKA(MOVE_METHOD,conflictsTable)];
	  //	  theAnt->current=pVertices[chooseInitialMoveKA(MAX_CONFLICT_MOVE,conflictsTable)];


	  moveSoFar++; if(BB)assert(moveSoFar==1);
	  //if(theAnt->current!=NULL)theAnt->old=theAnt->current;


	  colorTheVertex(theAnt,alphaNumColors,currentColor,conflictsTable);



	  while(moveSoFar<moveLimit){ 
		if(BB)assert(theAnt->current!=NULL); 

		theAnt->old=theAnt->current;
		
		for(int distI=0;distI<HOW_FAR;++distI){

		  ///move to Max Conflict adj lastly
		  theAnt->current=pVertices[moveAnt(theAnt->current->v_id,
											 (distI==HOW_FAR-1)?MAX_CONFLICT_MOVE:RANDOM_MOVE,
											 recentlyVisited,considerVertices,conflictsTable)];

		  //printf("-> [%d] old %d, current %d\n",iAnts,theAnt->old->v_id,theAnt->current->v_id);



		  //if reach maximum then remove from the beginning, todo, use a diff structure
		  if(recentlyVisited.size()==rSizeLimit)recentlyVisited.erase(recentlyVisited.begin());
		  recentlyVisited.push_back(theAnt->current->v_id);  



		}//HOW_FAR

		
		colorTheVertex(theAnt,alphaNumColors,currentColor,conflictsTable);
		moveSoFar++;


	  }//while(moveSoFar<moveLimit)
	}//for ant loop


	totalConflicts=	updateConflictTable(currentColor,conflictsTable);  

	if(!totalConflicts){
	  if(alphaNumColors<bestResult){

		//		lg=0;
		//		for(int i=0;i <nVertices ;++i){
		//		  if(currentColor[i]>lg)lg=currentColor[i];
		//		}
		//		printf ("Cycle %d, current color is %d, prev BEST color is %d, nAnts %d, movelimit %d, howFar %d, lg %d\n",
		//				iCycles,alphaNumColors,bestResult,nAnts,moveLimit,HOW_FAR,lg);
		
		bestResult=alphaNumColors;
		copyBestResult(currentColor);
		bestCycle=iCycles;

#ifdef V
		printf("*********** Achieve %d colors at cycle %d ************\n",bestResult,iCycles);
#endif

		if(alphaNumColors>1){
		  alphaNumColors--;
#ifdef V
		  printf("Cycle [last changed %d, current %d], Q_CHANGE_CYCLE %d, decrease nColors from %d to %d\n",
			   changedCycle,iCycles,Q_CHANGE_CYCLE,alphaNumColors+1,alphaNumColors);
#endif
		  changedCycle = iCycles;
		  reColorMoreThanQ(alphaNumColors,currentColor,conflictsTable);
		}
		else{
		  fprintf(stderr,"ERROR: attempting to have only 1 color, only possible if there's no edge .. haha\n");assert(1==0);
		}
	  }//if better than best result
	}//if no conflict


	//	if (iCycles-changedCycle == (int)(nCycles/2)){
	if (iCycles-changedCycle == Q_CHANGE_CYCLE){
	  int tIncr = (int)(bestResult-alphaNumColors/4);

	  if(tIncr==0||alphaNumColors+tIncr>=bestResult){
		tIncr=1;
	  }
	  
	  if(alphaNumColors+tIncr<bestResult){
		alphaNumColors+=tIncr;
		changedCycle=iCycles;
#ifdef V
		printf("Cycle [last changed %d, current %d], Q_CHANGE_CYCLE %d, increasing nColors from %d to %d\n",
			   changedCycle,iCycles,Q_CHANGE_CYCLE,alphaNumColors-tIncr,alphaNumColors);
#endif
	  }
	}

	if(iCycles-changedCycle==breakCycles){
#ifdef V
		printf("Break at cycle %d, last changed cycle %d, breakCycle %d\n",iCycles,changedCycle,breakCycles);
#endif
		break;
	}
	
	
  }//cycle loop
}
