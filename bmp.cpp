#include "bmp.h"

int
ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size;
    unsigned long i;
    unsigned short int planes;
    unsigned short int bpp;
    GLubyte temp;
    
    struct {
        unsigned short int type;                 /* Magic identifier            */
        unsigned int size;                       /* File size in bytes          */
        unsigned short int reserved1, reserved2;
        unsigned int offset;                     /* Offset to image data, bytes */
    } header;
    
    struct {
        unsigned int size;               /* Header size in bytes      */
        int width,height;                /* Width and height of image */
        unsigned short int planes;       /* Number of colour planes   */
        unsigned short int bits;         /* Bits per pixel            */
        unsigned int compression;        /* Compression type          */
        unsigned int imagesize;          /* Image size in bytes       */
        int xresolution,yresolution;     /* Pixels per meter          */
        unsigned int ncolours;           /* Number of colours         */
        unsigned int importantcolours;   /* Important colours         */
    } infoheader;
    
    
    
    if ((file = fopen(filename, "rb"))==NULL) {
        printf("File Not Found : %s\n",filename);
        return -1;
    }
    
    fread(&header, 14, 1, file);
    fread(&infoheader, 40, 1, file);
    
    fseek(file, 18, SEEK_SET);
    
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
        printf("Error reading width from %s.\n", filename);
        return -1;
    }
    
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
        printf("Error reading height from %s.\n", filename);
        return -1;
    }
    
    size = image->sizeX * image->sizeY * 3;
    
    if ((fread(&planes, 2, 1, file)) != 1) {
        printf("Error reading planes from %s.\n", filename);
        return -1;
    }
    
    if (planes != 1) {
        printf("Planes from %s is not 1: %u\n", filename, planes);
        return -1;
    }
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
        printf("Error reading bpp from %s.\n", filename);
        return -1;
    }
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return -1;
    }
    fseek(file, 24, SEEK_CUR);
    image->data = (GLubyte *) malloc(size);
    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return -1;
    }
    
    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return -1;
    }
    
    for (i=0;i<size;i+=3) {
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }
    fclose(file);
    return 0;
}

int
ImageSave(char *filename, Image *image) {
    FILE *file;
    GLubyte *data;
    
    struct {
        unsigned short int type;                 /* Magic identifier            */
        unsigned int size;                       /* File size in bytes          */
        unsigned short int reserved1, reserved2;
        unsigned int offset;                     /* Offset to image data, bytes */
    } header;
    
    struct {
        unsigned int size;               /* Header size in bytes      */
        int width,height;                /* Width and height of image */
        unsigned short int planes;       /* Number of colour planes   */
        unsigned short int bits;         /* Bits per pixel            */
        unsigned int compression;        /* Compression type          */
        unsigned int imagesize;          /* Image size in bytes       */
        int xresolution,yresolution;     /* Pixels per meter          */
        unsigned int ncolours;           /* Number of colours         */
        unsigned int importantcolours;   /* Important colours         */
    } infoheader;
    GLuint size;
    //GLuint temp;
    GLuint i;
    
    if ((file = fopen(filename, "wb"))==NULL) {
        printf("Problem to open file : %s\n",filename);
        return -1;
    }
    
#define UNDEFINED 0
    
    header.type = 0x4d42;
#ifdef __OSX__
    header.type = ((header.type>>8)&0x00ff) | ((header.type & 0xff)<<8);
#endif
    
    header.size = 54+3*image->sizeX+image->sizeY;
    
#ifdef __OSX__
    header.size = (((header.size)>>24)&0xff) | ((((header.size)>>16)&0x00ff)<<8) |
    ((((header.size)>>8)&0x0000ff)<<16) | ((((header.size)>>0)&0x000000ff)<<24);
#endif
    
    header.reserved1 = 0;
#ifdef __OSX__
    header.reserved2 = 0x3600;
#else
    header.reserved2 = 0x36;
#endif
    
#ifdef __OSX__
    header.offset = 0x49d0;
#else
    header.offset = 0;
#endif
    
    fwrite(&header, 14, 1, file);
    
    infoheader.size=40;
#ifdef __OSX__
    infoheader.size = (((infoheader.size)>>24)&0xff) | ((((infoheader.size)>>16)&0x00ff)<<8) |
    ((((infoheader.size)>>8)&0x0000ff)<<16) | ((((infoheader.size)>>0)&0x000000ff)<<24);
#endif
    
    infoheader.width=image->sizeX;
    infoheader.height=image->sizeY;
    
#ifdef __OSX__
    infoheader.width = (((infoheader.width)>>24)&0xff) | ((((infoheader.width)>>16)&0x00ff)<<8) |
    ((((infoheader.width)>>8)&0x0000ff)<<16) | ((((infoheader.width)>>0)&0x000000ff)<<24);
    infoheader.height = (((infoheader.height)>>24)&0xff) | ((((infoheader.height)>>16)&0x00ff)<<8) |
    ((((infoheader.height)>>8)&0x0000ff)<<16) | ((((infoheader.height)>>0)&0x000000ff)<<24);
#endif
    
    
#ifdef __OSX__
    infoheader.planes=256;
#else
    infoheader.planes=1;
#endif
#ifdef __OSX__
    infoheader.bits= 0x1800;
#else
    infoheader.bits= 24;
#endif
    
    infoheader.compression=0;
    
    infoheader.imagesize = 3*image->sizeX*image->sizeY;
    
#ifdef __OSX__
    infoheader.imagesize = (((infoheader.imagesize)>>24)&0xff) | ((((infoheader.imagesize)>>16)&0x00ff)<<8) |
    ((((infoheader.imagesize)>>8)&0x0000ff)<<16) | ((((infoheader.imagesize)>>0)&0x000000ff)<<24);
#endif
    
    infoheader.xresolution = infoheader.yresolution = 0;
    
    infoheader.ncolours = 0;
    infoheader.importantcolours = 0;
    
    fwrite(&infoheader, 40, 1, file);
    
    size = image->sizeX*image->sizeY*3;
    
    //utiliser pour ne pas changer les donnees de image
    data = (GLubyte *) malloc(size);
    
    for (i=0;i<size;i+=3) { 
        data[i] = image->data[i+2];
        data[i+1] = image->data[i+1];
        data[i+2] = image->data[i];
    }  
    fwrite(data, size, 1, file);
    fclose(file);
    free(data);
    return 0;
}
