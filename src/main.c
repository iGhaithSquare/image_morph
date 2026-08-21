#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include "stb_image_write.h"
#include "stb_image.h"
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
typedef struct image{
    int width;
    int height;
    int channels;
    stbi_uc *data;
    int* index_array;
} image;
void radix_sort(stbi_uc* data,int* index_array,int size,stbi_uc* tmp_data,int* tmp_iarr){
    stbi_uc* o_data=data;
    int* o_iarr=index_array;
    int i;
    int pass;
    for(pass=2;pass>=0;pass--){
        int count_table[256]={0};
        int position[256];
        int sum=0;
        for(i=0;i<size;i++)
            count_table[data[i*4+pass]]++;
        for(i=0;i<256;i++){
            position[i]=sum;
            sum+=count_table[i];
        }
        for(i=0;i<size;i++){
            int key=data[i*4+pass];
            int dst=position[key]++;
            memcpy(tmp_data+dst*4,data+i*4,4);
            if(index_array)
                tmp_iarr[dst]=index_array[i];
        }
        stbi_uc *tmp_data_ptr =data;
        data=tmp_data;
        tmp_data=tmp_data_ptr;
        if(index_array){
            int *tmp_index_ptr =index_array;
            index_array=tmp_iarr;
            tmp_iarr=tmp_index_ptr;
        }
    }
    memcpy(o_data,data,(size_t)size*4);
    if(index_array)
        memcpy(o_iarr,index_array,(size_t)size*sizeof(int));
}
int main(int argc,char** argv){
    image Img1,Img2;
    stbi_uc* out_data=NULL;
    args Args={0};
    int i,img2_size;
    stbi_uc* tmp_data=NULL;
    int* tmp_array=NULL;
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
    }
    Img1.data=stbi_load(Args.img1,&Img1.width,&Img1.height,&Img1.channels,4);
    if(!Img1.data){
        printf("Failed to load \"%s\"\nError: %s",Args.img1,stbi_failure_reason());
        return -1;
    }
    Img2.index_array=NULL;
    Img2.data=stbi_load(Args.img2,&Img2.width,&Img2.height,&Img2.channels,4);
    if(!Img2.data){
        printf("Failed to load \"%s\"\nError: %s",Args.img2,stbi_failure_reason());
        stbi_image_free(Img1.data);
        return -1;
    }
    if(Img1.height!=Img2.height||Img1.width!=Img2.width){
        printf("The 2 images dont have the same dimensions.");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        return -1;
    }
    img2_size=Img2.height*Img2.width;
    Img2.index_array=(int*)malloc(sizeof(int)*img2_size);
    if(!Img2.index_array){
        printf("Failed to allocate memory to image 2 index array");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        return -1;
    }
    out_data=(stbi_uc*)malloc(img2_size*4);
    if(!out_data){
        printf("Failed to allocate memory to output data");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        free(Img2.index_array);
        return -1;
    }
    tmp_data=(stbi_uc*)malloc(img2_size*4);
    if(!tmp_data){
        printf("Failed to allocate memory to tmp data");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        free(Img2.index_array);
        free(out_data);
        return -1;
    }
    tmp_array=(int*)malloc(sizeof(int)*img2_size);
    if(!tmp_array){
        printf("Failed to allocate memory to tmp index array");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        free(Img2.index_array);
        free(out_data);
        free(tmp_data);
        return -1;
    }
    radix_sort(Img1.data,NULL,img2_size,tmp_data,tmp_array);
    for(i=0;i<img2_size;i++){
        Img2.index_array[i]=i;
    }
    radix_sort(Img2.data,Img2.index_array,img2_size,tmp_data,tmp_array);
    free(tmp_data);
    free(tmp_array);
    for(i=0;i<img2_size;i++){
        memcpy(out_data+Img2.index_array[i]*4,Img1.data+i*4,4);
    }
    stbi_image_free(Img1.data);
    stbi_image_free(Img2.data);
    if(!stbi_write_png(Args.out,Img2.width,Img2.height,4,out_data,Img2.width*4)){
        printf("Failed to write \"%s\"",Args.out);
        free(out_data);
        return -1;
    };
    free(out_data);
    return 0;
}