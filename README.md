# Image Morph

Image Morph is a fast image morphing program.  
It generates an image that mimics morphing two images, by using Image 1 pixels to match the pixel distribution and positions of Image 2.

### How it works:
1. Loads 2 images using stb_image.
2. Both image's pixels are sorted using a 3-pass radix sort, while having their original positions preserved in an index array.
3. Distributes the pixels from the first image based on index array of image 2.
4. Uses stb_image_write to generate the resulting image.

## Requirements:

FFmpeg is required for video generation.


## Usage:

```bash
image_morph <image1> <image2> <output> --optional_arguments
```

### Optional_arguments:
```
-video:         generates a video of morphing 2 images  
    -fps:X      fps of the video generated(X Is the argument, default is 30)  
    -time:X     time of the video generated(X is the argument in seconds, default is 5)  
    -clear      clears the frame buffer before every frame  
    -easein     adds an easein effect to the video generated  
    -easeout    adds an easeout effect to the video generated
```

## Note:

This program uses stb_image and stb_image_write to include and make images.

## Building:

```bash
cmake -B build
cmake --build build
```

### Requirements: Cmake
