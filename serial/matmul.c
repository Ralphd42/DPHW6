#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "timing.h"

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

void doMatMul( double * arrA, double * arrB, double * arrC ,int rowcolLen)   
{
     
    timing_start();
    
    for (int i = 0;i<rowcolLen;++i)
    {
        for(int  j = 0;j<rowcolLen;++j)
        {
            for(int k=0;k<rowcolLen;++k)
            {
                 
                *(arrC +i*rowcolLen + j)  = 
                    *(arrC + i * rowcolLen + j)  +   
                    *(arrA + i * rowcolLen + k) *  
                    *(arrB + k * rowcolLen + j);
                 
            }
        }
    }
     
    timing_stop();
    print_timing();
}

/*Fill Arrays    */
void FIllArray( FILE * inFile, double * arr, int rowcolLen)
{
     
    int colcnter =0;
     
    int rowcnter =0;
     
    char line[4098];
     
    while (fgets(line, 4098, inFile) && (rowcnter < rowcolLen))
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



int main(int argc, char*argv[])
{
    int arrdim =0;
    if(argc<2)
    {
        perror("\nUsage ./matmul <inputfile.csv>\n");
        exit(-1);
    }
    char * filename= argv[1];
    FILE *q;
    q = fopen(filename, "r");
    fscanf(q,"%d", &arrdim);
    printf("Arr dim %d\r\n",arrdim);
    int numElements = arrdim*arrdim;
    // matrixes
    double * matA   =((double *) malloc(sizeof(double)*numElements));
    double * matB   =((double *) malloc(sizeof(double)*numElements));
    double * matC   =((double *) malloc(sizeof(double)*numElements));
    FIllArray(q,matA,arrdim);
    FIllArray(q,matB,arrdim);
    InitArray(matC,arrdim);
    doMatMul( matA, matB, matC ,arrdim);
    FILE * Output;
    char * outfileName ="outFile.txt";
    Output = fopen(outfileName, "w");
    DisplayArray(Output,matC,arrdim);
    free(matA  );
    free(matB  );
    free(matC  );
    fclose(q);
    fclose(Output);
    return 1;
}








    //printf("----------------------------------------------------\n");
    //DisplayArray(stdout,matA,arrdim);
    //printf("----------------------------------------------------\n");
    //DisplayArray(stdout,matB,arrdim);
    //printf("----------------------------------------------------\n");
   // DisplayArray(stdout,matC,arrdim);
    // deallocate the three
    