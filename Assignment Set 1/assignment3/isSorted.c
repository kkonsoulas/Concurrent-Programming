#include <stdio.h>

int main(int argc,char *argv[]){
    if(argc != 2){
        printf("Invalid number of args, insert name of file containing the ints");
        return 0;
    }

    int x,y;
    FILE* fp = fopen(argv[1],"rb");
    fread(&x,sizeof(int),1,fp);
    while(1){
        fread(&y,sizeof(int),1,fp);
        if(feof(fp)){
            printf("File Sorted!\n");
            return 0;
        }
        if(x > y){
            printf("File not sorted !!\n%d > %d\n",x,y);
            return 0;
        }
        x=y;

    }

    return 0;
}