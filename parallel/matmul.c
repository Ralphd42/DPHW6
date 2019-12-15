#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<string.h>
#include "timing.h"
#include "mpi.h"
#define INBUFFMAX 25000000
#define MASTER 0
#define bsize   70000  // loading large arrays

void doMatMul( double ** arrA, double ** arrB, double ** arrC ,int rowcolLen, int m);   
struct slicevars
{
    int RwclLn;
    int SliceLn;     
};

int main(int argc, char*argv[])
{
            timing_start() ;
    /*
        1initialize MPI
        setup rank variables etc 
        proc cnt
    */
     
    int num_proc;
    int rank;
    int rc;
    double ** AData;
    double ** BData;
    double ** CData;
    rc =MPI_Init(&argc,&argv);
    if(rc!=MPI_SUCCESS)
    {
        printf("Failed to start MPI");
        MPI_Abort(MPI_COMM_WORLD,rc); 
        exit(1);   
    }
    
    MPI_Comm_size(MPI_COMM_WORLD,&num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Status status;
    struct slicevars sv; 
    if(rank==MASTER)
    {
        if(argc<4)
        {
            perror("\nUsage ./matmul ARRAY DIM <A inputfile.csv> <B inputfile.csv> \n");
            MPI_Abort(MPI_COMM_WORLD,rc);
             exit(1);
        }
        char * Afilename= argv[2];
        char * Bfilename= argv[3];
        int ArrSZ = atoi(argv[1]);


        // remember AData
        if( ArrSZ%num_proc!=0)
        {
            printf(" Arrsize and Proc num must be compatible  ");
             MPI_Abort(MPI_COMM_WORLD,rc);
             exit(1);
        }
        //read and split A
        int splitsz = ArrSZ/num_proc; 
        
        
        FILE * AFile ;
        AFile =fopen("Afilename","r");
        double SplitArr [splitsz][ArrSZ];
        double BArr   [ArrSZ][ArrSZ];
        //fill splitArr  
        int cnt =0;
        int rwcnt =0;
        int rnkCnt =0;
        sv.RwclLn = ArrSZ;
        sv.SliceLn = splitsz;
        for(rnkCnt =1;rnkCnt<num_proc;rnkCnt++ )
        {
            MPI_Send(&sv, sizeof(sv), MPI_BYTE, rnkCnt, 0, MPI_COMM_WORLD);
        }
        printf(" -1 ");
        AData =(double **) malloc( splitsz *sizeof(double *));
        int i=0;
        for (i=0;i<ArrSZ;i++)
        {
            AData[i] = (double *)malloc(ArrSZ * sizeof(double));
        }
        for(rnkCnt=0;rnkCnt<num_proc;rnkCnt++ )
        {
            for(cnt =0; cnt++;cnt<splitsz )
            {
            // get row
                char buf[bsize];
                fgets (buf, bsize,AFile);
                char * cma = ",";
                 char *token;
                token = strtok(buf, cma);
                int arrcnt =0;
            /* walk through other tokens */
            int rowcnt =0;
                while( token != NULL ) 
                {
                    if(rnkCnt==MASTER)
                    {
                        AData[cnt][rowcnt] = atof(token);
                    }
                    else
                    {
                        SplitArr[cnt][rowcnt] = atof(token);
                    }
                    rowcnt++;
                }
            }printf(" -1 ");
            int arrSplitsz = ArrSZ* splitsz;
            if(    rnkCnt!=MASTER           )
            {
                MPI_Send(SplitArr, arrSplitsz,MPI_DOUBLE,  rnkCnt,0,MPI_COMM_WORLD);
            }
        }
        printf(" -2 ");
        // Fill and send Bdata  // need to come up with better plan for this
        BData =(double **) malloc( ArrSZ *sizeof(double *));
        for (i=0;i<ArrSZ;i++)
        {
            BData[i] = (double *)malloc(ArrSZ * sizeof(double));
        }
        printf(" -3 ");
        for(rnkCnt =0;rnkCnt<num_proc;rnkCnt++ )
        {
            for(cnt =0; cnt++;cnt<ArrSZ )
            {
                char buf[bsize];
                fgets (buf, bsize,AFile);
                char * cma = ",";
                char * token;
                token = strtok(buf, cma);
                int arrcnt =0;
                int rowcnt =0;
                while( token != NULL ) 
                {
                    if(rnkCnt==MASTER)
                    {
                        BData[cnt][rowcnt] = atof(token);
                    }
                    else
                    {
                        BArr[cnt][rowcnt] = atof(token);
                    }
                    rowcnt++;
                }
            }
            int arrsz = ArrSZ* ArrSZ;
            if(    rnkCnt!=MASTER           )
            {
                MPI_Send(BArr, arrsz,MPI_DOUBLE,  rnkCnt,0,MPI_COMM_WORLD);
            }

        }
        // send needed info
        
    
    
    
    
    
    }else{
        printf("recv 1 rnk %d", rank);
        MPI_Recv(&sv, sizeof(sv), MPI_BYTE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        int aLen = sv.SliceLn * sv.RwclLn;
        printf("recv 2 rnk %d", rank);
        MPI_Recv(&AData,aLen, MPI_DOUBLE,MASTER,0,MPI_COMM_WORLD, &status );
        int bLen =  sv.RwclLn* sv.RwclLn;
        printf("recv 3 rnk %d", rank);
        MPI_Recv(&BData,bLen, MPI_DOUBLE,MASTER,0,MPI_COMM_WORLD, &status );
    }
    CData =(double **) malloc( sv.SliceLn *sizeof(double *));
    int i=0;
    for (i=0;i<sv.RwclLn;i++)
    {
        CData[i] = (double *)malloc(sv.RwclLn * sizeof(double));
    }


    // do multiplication
    // store in CData
    doMatMul( AData, BData, CData ,sv.RwclLn, sv.SliceLn);
    
    if(rank==MASTER)
    {
        FILE * Output;
        char * outfileName ="outFile.txt";
        Output = fopen(outfileName, "w");
        int colcnter =0;
        int rowcnter =0;
        for(rowcnter=0;rowcnter<sv.SliceLn;rowcnter++  )
        {
            for(  colcnter=0;colcnter<sv.RwclLn;colcnter++)
            {        
                if(colcnter>0)
                {
                    fprintf(Output,",");
                }            
                fprintf(Output, "%f" ,CData[rowcnter ][colcnter]);  
            }
        }
        // get values from 
        int rcc=1;
        for(;rcc<num_proc;rcc++)
        {
            char ** MultData;
            MPI_Recv(&MultData,sv.RwclLn * sv.SliceLn,MPI_DOUBLE, rcc,0,MPI_COMM_WORLD,&status);
            for(rowcnter=0;rowcnter<sv.SliceLn;rowcnter++  )
            {
                for(  colcnter=0;colcnter<sv.RwclLn;colcnter++)
                {        
                    if(colcnter>0)
                    {
                        fprintf(Output,",");
                    }            
                    fprintf(Output, "%f" ,CData[rowcnter ][colcnter]);  
                }
            }
        }
        fclose(Output);
    }
    else
    {
        MPI_Send(CData, ( sv.RwclLn* sv.SliceLn) ,MPI_DOUBLE,  MASTER,0,MPI_COMM_WORLD);
    }
    
    MPI_Finalize(); 
    timing_stop();
    print_timing();
    return 1;
}

void doMatMul( double ** arrA, double ** arrB, double ** arrC ,int rowcolLen, int m)   
{
     
    #pragma omp parallel for
    for ( int rws = 0 ; rws < m ; rws++ )
    {
        int col1;
        double sum =0;
      for ( col1 = 0 ; col1 < rowcolLen ; col1++ )
      {
          int col2;
            for ( col2 = 0 ; col2 < rowcolLen ; col2++ )
            {
                sum = sum + arrA[rws][col2]*arrB[col2][col1];
            }
            arrC[col1][col2] = sum;
            sum = 0;
        }
    }
}






































void DisplayArray( FILE * outLocation, double * arr, int rowcolLen)
{
    int colcnter =0;
    int rowcnter =0;
    for(rowcnter=0;rowcnter<rowcolLen;rowcnter++  ){
        for(  colcnter=0;colcnter<rowcolLen;colcnter++){        
            if(colcnter>0){
                fprintf(outLocation,",");    

            }            
            fprintf(outLocation, "%f" ,arr[ rowcnter*rowcolLen +colcnter]);  
        }
        fprintf(outLocation,"\r\n");
    }
}


/*Fill Arrays    */
void FIllArray( FILE * inFile, double * arr, int rowcolLen)
{
     
    int colcnter =0;
     
    int rowcnter =0;
     
    char line[INBUFFMAX];
     
    while (fgets(line, INBUFFMAX, inFile) && (rowcnter < rowcolLen))
    {
        if(strlen(line)>3){
        char * tok;
        for (tok =strtok(line,","); colcnter<rowcolLen;colcnter++)
        {
            arr[ rowcnter* rowcolLen +colcnter     ] = atof(tok);    

            //printf("ColNum|%d| rowNum |%d| val|%f|tok|%s|\r\n\r\n",colcnter,rowcnter, atof(tok), tok        );
            tok =strtok(NULL,",");

        } 
        colcnter =0;
        rowcnter++;
        }
    }
}

void InitArray(double * arr, int rowcolLen)
{
    for( int rowcnter =0; rowcnter<rowcolLen;rowcnter++)
    {
         
        for( int colcnter =0; colcnter<rowcolLen;colcnter++)
        {
             
            arr[ rowcnter*rowcolLen +colcnter    ] = 0;
             
        }    
    }
}





//https://edoras.sdsu.edu/~mthomas/sp17.605/lectures/MPI-MatMatMult.pdf