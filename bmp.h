//
//  bmp.h
//  
//
//  Created by Alejandro Montoya on 8/17/14.
// http://www.ai.univ-paris8.fr/~jj/Cours/LI2/SI/
//

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/freeglut_ext.h>
#endif  // __APPLE__


struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte *data;
};
typedef struct Image Image;

int ImageLoad(char *filename, Image *image);
int ImageSave(char *filename, Image *image);