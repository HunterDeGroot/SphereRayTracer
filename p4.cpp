#include <cmath>
#include <fstream>
#include <vector>
#include "p4.h"
#include "bmp.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#ifdef __linux__
#else
// Windows doesn't define these values by default, Linux does
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif

// leave at 500x500
#define WIDTH 500
#define HEIGHT 500

#define NLIGHTS 1

// intensities
#define AMB .6
#define SPEC .1
#define DIFF .4

using namespace std;

float data[WIDTH][HEIGHT][3];
Image *bg = (Image *) malloc(sizeof(Image));

Vec3<float> trace(const Vec3<float> &eyeOrigin, const Vec3<float> &eyeDir,
              const std::vector<Sphere<float> *> &spheres, const int &light)
{
    float near = INFINITY;
    const Sphere<float> *sphere = NULL;
    const int firstLightPos = spheres.size() - NLIGHTS;
    
    // find intersection with sphere
    for (unsigned i = 0; i < spheres.size() - (NLIGHTS*(1-light)); ++i) {
        float t0 = INFINITY, t1 = INFINITY;
        
        // calculating light skip ahead
        if (light && !i) i += firstLightPos;
        
        if (spheres[i]->intersect(eyeOrigin, eyeDir, &t0, &t1)) {
            if (t0 < 0) t0 = t1;
            if (t0 < near) {
                near = t0;
                sphere = spheres[i];
            }
        }
    }
    
    // if there's no intersection return signal to draw bg
    if (!sphere) return Vec3<float>(-1);
    
    Vec3<float> surfaceColor = 0;
    Vec3<float> phit = eyeOrigin + eyeDir * near; // intersection point
    Vec3<float> nhit = phit - sphere->center; // phit normal
    nhit.normalize();
    if (eyeDir.dot(nhit) > 0) nhit = -nhit;

    // for each light calculate intensity and apply if any to pixel
    for (unsigned i = 0; i < spheres.size(); ++i) {
        if (spheres[i]->emissionColor.x > 0 || spheres[i]->emissionColor.y > 0 || spheres[i]->emissionColor.z > 0) {
            
            Vec3<float> transmission = 1, lightDirection = spheres[i]->center - phit;
            lightDirection.normalize();
            for (unsigned j = 0; j < spheres.size(); ++j) {
                if (i != j) {
                    
                    // see if a sphere casts a shadow
                    float t0, t1;
                    if (spheres[j]->intersect(phit + nhit, lightDirection, &t0, &t1)) {
                        transmission = 0;
                        break;
                    }
                }
            }
            
            surfaceColor += (sphere->surfaceColor)*AMB * transmission *
            std::max(float(0), nhit.dot(lightDirection)) * spheres[i]->emissionColor + sphere->surfaceColor * DIFF;
        }
    }
    
    // calculate specular light to the camera
    Vec3<float> refldir = eyeDir - nhit * 2 * eyeDir.dot(nhit);
    refldir.normalize();
    if (!light)
    {
        Vec3<float> r = trace(phit + nhit, refldir, spheres, 1);
        if(r.x || r.y || r.z)
        {
            float r1 = r.dot(eyeDir);
            surfaceColor += -SPEC/(r1*r1*r1);
        }
    }
    
    // if the light then return its direction for calculation
    else return eyeDir;

    return surfaceColor;
}

// Traces a line thru each pixel from the eye and returns the color of the sphere it intersects with first
void render(const std::vector<Sphere<float> *> &spheres)
{
    Vec3<float> *image = new Vec3<float>[WIDTH * HEIGHT], *pixel = image;
    
    float invW = 1 / float(WIDTH);
    float invH = 1 / float(HEIGHT);
    
    float xPerY = float(WIDTH) / HEIGHT;
    float fov = 40;
    float angle = tan(M_PI/2 * fov / float(180));
    
    for (unsigned y = 0; y < HEIGHT; ++y) {
        for (unsigned x = 0; x < WIDTH; ++x, ++pixel) {
            float X = xPerY * angle * (2 * ((x + 0.5) * invW) - 1);
            float Y = angle * (1 - 2 * ((y + 0.5) * invH));
            
            // eye will look down -Z axis thru each pixel
            Vec3<float> eyeDir(X, Y, -1);
            Vec3<float> eyeOrigin(0,0,0);
            
            eyeDir.normalize();
            *pixel = trace(eyeOrigin, eyeDir*Vec3<float>(1), spheres, 0);
        }
    }
    
    for (int x = 0; x < WIDTH*3; x+=3) {
        for (int y = 0; y < HEIGHT*3; y+=3) {
            
            // if the ray didn't intersect draw a kickass background that took
            // me a suprising amount of time to implement else draw the sphere
            if(image[x/3+WIDTH*y/3].x == -1){
                int i = x*bg->sizeX+y;
                data[x/3][y/3][0] = float(bg->data[i])/255;
                data[x/3][y/3][1] = float(bg->data[i+1])/255;
                data[x/3][y/3][2] = float(bg->data[i+2])/255;
            } else {
                data[x/3][y/3][0] = image[x/3+WIDTH*y/3].x;
                data[x/3][y/3][1] = image[x/3+WIDTH*y/3].y;
                data[x/3][y/3][2] = image[x/3+WIDTH*y/3].z;
            }
        }
    }
    delete [] image;
}

void display(void) {
    glClearColor( 0, 0, 0, 1 );
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels( WIDTH, HEIGHT, GL_RGB, GL_FLOAT, data);
    glutSwapBuffers();
}

// Somehow fixes memory management bug
void mem(void) {}
void man(void) {}

int main(int argc, char **argv)
{
    
    // load the background
    char *inputFileName = argv[1];
    if (bg == NULL) {
        fprintf(stderr, "Out of memory\n");
        return(-1);
    }
    
    if (ImageLoad(inputFileName, bg)==-1) {
        printf ("Can't read the image\n");
        return -1;
    }
    
    std::vector<Sphere<float> *> spheres;
    
    // hole (secretely a sphere.. shhh)
    spheres.push_back(new Sphere<float>(Vec3<float>(-22, 0, -15), 20, Vec3<float>(0), 0, 0.0));
    
    // blueish golfball
    spheres.push_back(new Sphere<float>(Vec3<float>(0, -1, -10), 1, Vec3<float>(0.4, 0.7, 0.9), 0, 0.0));
    
    // ground (secretely a sphere.. shhh)
    spheres.push_back(new Sphere<float>(Vec3<float>(-100,0,-10),98, Vec3<float>(0.1, 0.9, 0.1), 0, 0.0));
    
    // light last
    spheres.push_back(new Sphere<float>(Vec3<float>(10, 0, 0), 5, Vec3<float>(0), 0, 0, Vec3<float>(3)));
    
    render(spheres);
    while (!spheres.empty()) {
        Sphere<float> *sph = spheres.back();
        spheres.pop_back();
        delete sph;
    }
    
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Part 4");
    glutDisplayFunc(display);
    glutMainLoop();
    free(bg);
    return 0;
}