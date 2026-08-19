#include <stdio.h>
#include <stdint.h>
#include <string.h>
typedef struct args{
    char* img1;
    char* img2;
    char* out;
    struct{
        unsigned type : 1;
        unsigned morph : 1;
        unsigned out : 1;
        unsigned pad1 : 1;
        unsigned pad2 : 1;
        unsigned pad3 : 1;
        unsigned pad4 : 1;
        unsigned pad5 : 1;
    } flags;
} args;
int main(int argc,char** argv){
    args Args={0};
    int i;
    if (argc==1){
        printf("Expected 3 arguments got none, do %s --help for usage",argv[0]);
        return -1;
    }
    if(!strcmp(argv[1],"--help")){
        printf("Usage:\n%s <image1> <image2> <out> <optional_arguments>",argv[0]);
        return 0;
    }
    if(argc<4){
        printf("Expected 3 arguments got %d, do %s --help for usage",argc-1,argv[0]);
        return -1;
    }
    Args.img1=argv[1];
    Args.img2=argv[2];
    Args.out=argv[3];
    for(i=4;i<argc;i++){
        if(!strcmp(argv[i],"-video"))
            Args.flags.type=1;
        else if(!strcmp(argv[i],"-a"))
            Args.flags.morph=1;
        printf("%s\n",argv[i]);
    }
    printf("Flags: Type:%d\tm_type:%d",Args.flags.type,Args.flags.morph);
    return 0;
}