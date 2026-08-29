#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
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
    int fps;
    int time;
    struct{
        unsigned type : 1;
        unsigned easein : 1;
        unsigned easeout : 1;
        unsigned clear : 1;
        unsigned pad1 : 1;
        unsigned pad2 : 1;
        unsigned pad3 : 1;
        unsigned pad4 : 1;
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
            tmp_iarr[dst]=index_array[i];
        }
        stbi_uc *tmp_data_ptr =data;
        int *tmp_index_ptr =index_array;
        data=tmp_data;
        tmp_data=tmp_data_ptr;
        index_array=tmp_iarr;
        tmp_iarr=tmp_index_ptr;
    }
    memcpy(o_data,data,(size_t)size*4);
    memcpy(o_iarr,index_array,(size_t)size*sizeof(int));
}
int main(int argc,char** argv){
    image Img1,Img2;
    stbi_uc* out_data=NULL;
    args Args={0};
    int i,j,img2_size,frame_count;
    stbi_uc* tmp_data=NULL;
    int* tmp_array=NULL;
    char file_name[512],ffmpeg[1024];
    FILE *pipe;
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
        else if(!strcmp(argv[i],"-easein"))
            Args.flags.easein=1;
        else if(!strcmp(argv[i],"-easeout"))
            Args.flags.easeout=1;
        else if(!strcmp(argv[i],"-clear"))
            Args.flags.clear=1;
        else if(!strncmp(argv[i],"-fps:",5))
            Args.fps=atoi(argv[i]+5);
        else if(!strncmp(argv[i],"-time:",6))
            Args.time=atoi(argv[i]+6);
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
    Img1.index_array=(int*)malloc(sizeof(int)*img2_size);
    if(!Img1.index_array){
        printf("Failed to allocate memory to image 1 index array");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        free(Img2.index_array);
        return -1;
    }
    out_data=(stbi_uc*)malloc(img2_size*4);
    if(!out_data){
        printf("Failed to allocate memory to output data");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        free(Img2.index_array);
        free(Img1.index_array);
        return -1;
    }
    tmp_data=(stbi_uc*)malloc(img2_size*4);
    if(!tmp_data){
        printf("Failed to allocate memory to tmp data");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        free(Img2.index_array);
        free(Img1.index_array);
        free(out_data);
        return -1;
    }
    tmp_array=(int*)malloc(sizeof(int)*img2_size);
    if(!tmp_array){
        printf("Failed to allocate memory to tmp index array");
        stbi_image_free(Img1.data);
        stbi_image_free(Img2.data);
        free(Img2.index_array);
        free(Img1.index_array);
        free(out_data);
        free(tmp_data);
        return -1;
    }
    for(i=0;i<img2_size;i++){
        Img1.index_array[i]=i;
        Img2.index_array[i]=i;
    }
    radix_sort(Img1.data,Img1.index_array,img2_size,tmp_data,tmp_array);
    radix_sort(Img2.data,Img2.index_array,img2_size,tmp_data,tmp_array);
    free(tmp_array);
    for(i=0;i<img2_size;i++){
        memcpy(out_data+Img2.index_array[i]*4,Img1.data+i*4,4);
    }
    if(Args.flags.type){
        Args.fps=Args.fps?Args.fps:30;
        Args.time=Args.time?Args.time:5;
        frame_count=Args.fps*Args.time;
        snprintf(ffmpeg,sizeof(ffmpeg),"ffmpeg -y -f rawvideo -pixel_format rgba -video_size %dx%d -framerate %d -i - -c:v libx264 -pix_fmt yuv420p \"%s.mp4\"",Img2.width,Img2.height,Args.fps,Args.out);
        #ifdef _WIN32
        pipe=_popen(ffmpeg,"wb");
        #else
        pipe=popen(ffmpeg,"w");
        #endif
        if(!pipe){
            printf("Failed to open ffmpeg. This algorithm uses ffmpeg to produce videos");
            free(tmp_data);
            stbi_image_free(Img1.data);
            stbi_image_free(Img2.data);
            free(out_data);
            free(Img1.index_array);
            free(Img2.index_array);
            return -1;
        }
        for(i=0;i<frame_count-1;i++){
            if(Args.flags.clear)
                memset(tmp_data,0,img2_size*4);
            float t=(float)i/(float)(frame_count-1);
            if(Args.flags.easein)
                t=Args.flags.easeout?t*t*(3.0f-2.0f*t):t*t;
            else if (Args.flags.easeout)
                t=1.0f-(1.0f-t)*(1.0f-t);
            for(j=0;j<img2_size;j++){
                int start=Img1.index_array[j];
                int end=Img2.index_array[j];
                int x1=start%Img2.width;
                int y1=start/Img2.width;
                int x2=end%Img2.width;
                int y2=end/Img2.width;
                int x=x1+(x2-x1)*t;
                int y=y1+(y2-y1)*t;
                int position=(y*Img2.width+x)*4;
                memcpy(tmp_data+position,Img1.data+j*4,4);
            }    
            fwrite(tmp_data,1,img2_size*4,pipe);
        }
        fwrite(out_data,1,img2_size*4,pipe);    
        #ifdef _WIN32
        int result =_pclose(pipe);
        #else
        int result= pclose(pipe);
        #endif
        
        if(result){
            printf("FFmpeg failed with error code %d",result);
            free(tmp_data);
            stbi_image_free(Img1.data);
            stbi_image_free(Img2.data);
            free(out_data);
            free(Img1.index_array);
            free(Img2.index_array);
            return -1;
        }
    }
    else{
        snprintf(file_name,sizeof(file_name),"%s.png",Args.out);
        if(!stbi_write_png(file_name,Img2.width,Img2.height,4,out_data,Img2.width*4)){
            printf("Failed to write \"%s\"",Args.out);
            free(tmp_data);
            stbi_image_free(Img1.data);
            stbi_image_free(Img2.data);
            free(out_data);
            free(Img1.index_array);
            free(Img2.index_array);
            return -1;
        };

    }
    free(tmp_data);
    stbi_image_free(Img1.data);
    stbi_image_free(Img2.data);
    free(out_data);
    free(Img1.index_array);
    free(Img2.index_array);
    return 0;
}