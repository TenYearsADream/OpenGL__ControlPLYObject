/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <io.h>
#include <process.h>
#include "./GL/glut.h"

#include "inputModule.h"
#include "ply.h"
#include "FreeImage.h"
#include "hsv.h"
#include "bitmap.h"

#pragma warning( disable : 4996 )

extern "C" {
#define IMAGE_WIDTH  800
#define IMAGE_HEIGHT 600
#define GL_PI 3.1415962
    typedef struct{
        float fieldOfView;
        float aspect;
        float nearPlane;
        float farPlane;
    }perspectiveData;
}

extern GLfloat current_pos_bunny[];
extern GLfloat current_rot_bunny[];
extern GLfloat current_sca_bunny[];
extern int record;

unsigned char image[256][256][3];

int window;
int updateFlag;

PLYObject *ply = NULL;
GLuint texture_index;
perspectiveData pD;

int window2;
extern int path[256];
extern int iKeyT;
extern int iKeyX;
extern int iKeyY;
extern int iKeyZ;
extern float fKeyA;
extern float fKeyS;
extern float fKeyD;
extern int iKeyF1;
extern int iKeyF2;
extern int iKeyF3;
static GLuint texName;
// Shading
float light_pos[4] = {0.0f, 4.0f, 0.0f, 1.0f};
float ambient_light[4] = {0.3f, 0.3f, 0.3f, 1.0f};
float light_color[4] = {0.8f, 0.8f, 0.8f, 1.0f};
float black_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
extern int light;
extern int ambient;
extern int diffuse;
extern int specular;
//
static GLuint texName2;

void drawCube ()
{
    // x axis
    glColor3f(1,0,0);
    glBegin(GL_LINES);
    glVertex3f(-1,-1, 1);
    glVertex3f( 1,-1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f( 1, 1, 1);

    glVertex3f(-1,-1,-1);
    glVertex3f( 1,-1,-1);
    glVertex3f(-1, 1,-1);
    glVertex3f( 1, 1,-1);
    glEnd();

    // y axis
    glColor3f(0,1,0);
    glBegin(GL_LINES);
    glVertex3f(-1,-1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f( 1,-1, 1);
    glVertex3f( 1, 1, 1);

    glVertex3f(-1,-1,-1);
    glVertex3f(-1, 1,-1);
    glVertex3f( 1,-1,-1);
    glVertex3f( 1, 1,-1);
    glEnd();

    // z axis
    glColor3f(0,1,0);
    glBegin(GL_LINES);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1,-1, 1);
    glVertex3f(-1, 1,-1);
    glVertex3f(-1, 1, 1);

    glVertex3f( 1,-1,-1);
    glVertex3f( 1,-1, 1);
    glVertex3f( 1, 1,-1);
    glVertex3f( 1, 1, 1);
    glEnd();
}

int loadtexture ( const char* fileName )
{
    BITMAPFILEHEADER1 texInfo;
    GLubyte* bits;
    GLuint id;

    bits = LoadBitmapFile(fileName,&texInfo);
    glGenTextures(1,&id);
    printf("id=%d......\n",id);

    glBindTexture(GL_TEXTURE_2D,id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST

    glTexImage2D(GL_TEXTURE_2D, 0, 4, texInfo.biWidth, texInfo.biHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bits );

    return id;
}

void initTexture2()
{
    texName2 = loadtexture(".\\Erath-Map.bmp");
    glBindTexture(GL_TEXTURE_2D, texName2);
}

void drawGlobe()
{
    glDisable(GL_LIGHTING);
    //glColor4f(1,1,1,1);
    glColor4f(0,0,0,1);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texName2);

    float d = 128.0f; //32
    float radius = 1.0f;
    float DTOR = 3.141592653587f / 180.0f;

    int x, y;
    float theta, phi;
    
    //GL_FRONT, GL_BACK, GL_FRONT_AND_BACK
    //GL_POINT, GL_LINE, GL_FILL
    // void glPolygonMode (GLenum face, GLenum mode );

    // glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    for ( y = -2048.0f; y <= 2048.0f; y+=d )
    {
        glBegin(GL_TRIANGLE_STRIP);
        //glBegin(GL_LINE_STRIP);
        for ( x = - 2048.0f; x <= 2048.0f; x+=d )
        {
            theta = (x+2048.0f) / (2*2048.0f) * 360.0f - 90.0f;
            phi = (y - 2048.0f) / ( 2* 2048.0f) * 180.0f;
            glTexCoord2f(1.0-(x+2048.0f)/4096.0f, (y+2048.0f)/4096.0f);
            
            glVertex3f(
                radius * sin(phi*DTOR) * cos(DTOR*theta),
                radius * cos(phi*DTOR),
                radius * sin(phi*DTOR) * sin(DTOR*theta)
                );
            //
            phi = (y+d-2048.0f)/(2*2048.0f) * 180.0f;
            glTexCoord2f(1.0-(x+2048.0f)/4096.0f,(y+d+2048.0f)/4096.0f);
            glVertex3f(
                radius * sin(phi*DTOR) * cos(DTOR*theta),
                radius * cos(phi*DTOR),
                radius * sin(phi*DTOR) * sin(DTOR*theta)
                );

        }
        glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}

unsigned char* readPPM(char* filename, int* width, int* height)
{
    FILE* fp;
    int i, w, h, d;
    unsigned char* image;
    char head[70];
    fp = fopen(filename, "rb");
    if(!fp)
    {
        perror(filename);
        return NULL;
    }
    fgets(head,70,fp);
    if(strncmp(head,"P6",2))
    {
        fprintf(stderr,"%s: Not a raw PPM file\n", filename);
        return NULL;
    }
    i = 0;
    while(i<3)
    {
        fgets(head,70,fp);
        if(head[0]=='#')
        {
            continue;
        }
        if(i==0)
        {
            i+=sscanf(head,"%d,%d,%d", &w, &h, &d);
        }
        else if (i==1)
        {
            i+=sscanf(head, "%d,%d", &h, &d);
        }
        else if ( i == 2 )
        {
            i+=sscanf(head, "%d", &d );
        }
    }
    image = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
    fread(image,sizeof(unsigned char), w*h*3, fp);
    fclose(fp);
    *width = w;
    *height = h;
    return image;
}

/*
**
*/
void initLighting()
{
	// setup lights
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black_color);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


/*
**
*/
void gl_ortho_begin(unsigned size_x, unsigned size_y)
{
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.375f, 0.375f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, size_x, 0, size_y);
    glMatrixMode(GL_MODELVIEW);
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}


/*
**
*/
void gl_ortho_end(void)
{
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


/*
**  cleanup
*/
void cleanup(int sig)
{
    delete(ply);
    exit(0);
}


/*
**  drawPlane
*/
void drawPlane()
{
    // floor
    glColor3f(0.5f, 0.5f, 0.9f);
    glBegin(GL_POLYGON);
    glVertex3f(-5,-1, 5);
    glVertex3f( 5,-1, 5);
    glVertex3f( 5,-1,-5);
    glVertex3f(-5,-1,-5);
    glEnd();

    // wire
    float i;
    glColor3f(0.5,0,0);
    glBegin(GL_LINES);
    for (i=-5.0f; i<=5.0f; i+=1.0f)
    {
        glVertex3f( i,-0.98f,  5);
        glVertex3f( i,-0.98f, -5);
        glVertex3f( 5,-0.98f,  i);
        glVertex3f(-5,-0.98f,  i);
    }
    glEnd();

    // axis
    glBegin(GL_LINES);
    for (i=-5.0f; i<=5.0f; i+=1.0f)
    {
        glColor3f(1,0,0);
        glVertex3f(0,0,0);
        glVertex3f(5,0,0);
        glColor3f(0,1,0);
        glVertex3f(0,0,0);
        glVertex3f(0,5,0);
        glColor3f(0,0,1);
        glVertex3f(0,0,0);
        glVertex3f(0,0,5);
    }
    glEnd();


    //
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texName2);
    static float s = 0.0f;
    s+=0.0001f;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f+s, 0.0f+s); glVertex3f(-5.0f,-0.99f, 5.0f);
    glTexCoord2f(0.0f+s, 1.0f+s); glVertex3f( 5.0f,-0.99f, 5.0f);
    glTexCoord2f(1.0f+s, 1.0f+s); glVertex3f( 5.0f,-0.99f,-5.0f);
    glTexCoord2f(1.0f+s, 0.0f+s); glVertex3f(-5.0f,-0.99f,-5.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

}


/*
**  string render function
*/
void string_render(const char *string, GLint x, GLint y)
{
    unsigned i;
    glRasterPos2i(x + 1, y - 1);
    for (i = 0; string[i]; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
    }
}


/*
**  screenshot
*/
void screenshot(int x, int y, int w, int h)
{
    FIBITMAP *image = FreeImage_Allocate(w, h, 24);

    unsigned char *data = new unsigned char[w*h*3];
    unsigned char *temp = new unsigned char[w*3];

    if(!data || !temp)
    {
        fprintf(stderr, "no more memory!\n");
    }

    glReadPixels(x,y,w,h, GL_RGB, GL_UNSIGNED_BYTE, data);

    int i;
    unsigned char tempData;
    for(i=0; i<w*h*3; i+=3){
        tempData = data[i+2];
        data[i+2] = data[i];
        data[i]   = tempData;
    }

    // Save image
    image=FreeImage_ConvertFromRawBits(data, w,h,w*3, 24, 0,0,0);
    if(!image)
    {
        fprintf(stderr, "Failed to convert bits!\n");
    }


    static int count = 0;
    char filename[50];

    sprintf(filename,"%05d.png", count);

    if(FreeImage_Save(FIF_PNG, image, filename))
        fprintf(stderr, "Wrote %s!\n", filename);
    else
        fprintf(stderr, "Failed!\n");

    count++;

    FreeImage_Unload(image);

    delete [] temp;
    delete [] data;
}


void drawPlaneTexture()
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texName);
    static float s = 0.0f;
    s+=0.005f;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f+s, 0.0f+s); glVertex3f(-5.0f, 10.0f, -5.0f);
    glTexCoord2f(0.0f+s, 1.0f+s); glVertex3f( 5.0f, 10.0f, -5.0f);
    glTexCoord2f(1.0f+s, 1.0f+s); glVertex3f( 5.0f,  0.0f, -5.0f);
    glTexCoord2f(1.0f+s, 0.0f+s); glVertex3f(-5.0f,  0.0f, -5.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
}

/*
**
*/
void display(void)
{  
    glutSetWindow(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glEnable( GL_LIGHTING );

    setUserView();

    
    if ( light )
    {
        glEnable(GL_LIGHTING);
    }
    else
    {
        glDisable(GL_LIGHTING);
    }
    if (ambient)
    {
        glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    }
    else
    {
        glLightfv(GL_LIGHT0, GL_AMBIENT, black_color);
    }
    if (diffuse)
    {
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    }
    else
    {
        glLightfv(GL_LIGHT0, GL_DIFFUSE, black_color);
    }
    if (specular)
    {
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    }
    else
    {
        glLightfv(GL_LIGHT0, GL_SPECULAR, black_color);
    }

    
    GLfloat mat_specular[] = {1.0f,1.0f,1.0f,1.0f};
    GLfloat low_shininess[] = {5.0f};
    GLfloat mat_emission[] = {0.3f, 0.2f, 0.2f, 0.0f };
    GLfloat no_mat[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_ambient_color[] = { 0.8f, 0.8f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };
    //GLfloat mat_specular[] = { 1.0f,1.0f,1.0f,1.0f };
    GLfloat no_shininess[] = { 0.0f };
    //GLfloat low_shininess[] = { 5.0f };
    GLfloat high_shininess[] = { 100.0f };
    //GLfloat mat_emission[] = { 0.3f, 0.2f, 0.2f, 0.0f };

    glMaterialfv ( GL_FRONT, GL_AMBIENT, no_mat );
    glMaterialfv ( GL_FRONT, GL_DIFFUSE, mat_diffuse );
    glMaterialfv ( GL_FRONT, GL_SPECULAR, no_mat );
    glMaterialfv ( GL_FRONT, GL_SHININESS, no_shininess );
    glMaterialfv ( GL_FRONT, GL_EMISSION, no_mat );

    drawPlane();

    
    if ( iKeyT )
    {
        drawPlaneTexture();
    }

    
    // µe­y¸ñ²y
    int i = 0;
    for ( i = 0; i < 256; ++i )
    {
        glPushMatrix();
        glTranslatef(-5.0f+(i/256.0f)*10, -0.95f, 5.0f - (path[i]/256.0) * 10 );
        glutSolidSphere(0.1f,5, 5);
        glPopMatrix();
    }

    
    // draw Cube
    drawCube ();
    // draw Erath
    drawGlobe();

    //// primitives
    //glColor3f(1,0,0);
    //glBegin(GL_TRIANGLES);
    //// fill_me...
    //glEnd();

    //// primitives
    //glColor3f(1,1,0);
    //glBegin(GL_POLYGON);
    //// fill_me...
    //glEnd();

    //glColor3f(0,1,0);
    //glBegin(GL_LINES);
    //// fill_me...
    //glEnd();


    
    static int index = 0;
    if ( index >= 256 )
    {
        index = 0;
    }
    glPushMatrix();
    // fill_me...
    glTranslatef (
        current_pos_bunny[0],
        current_pos_bunny[1],
        current_pos_bunny[2] );
    glTranslatef
        (
        -5.0f+(index/256.0f)*10,
        0, 
        5.0f - (path[index]/256.0)*10
        );
    
    glRotatef((GLfloat)iKeyX,1.0f,0.0f,0.0f);
    glRotatef((GLfloat)iKeyY,0.0f,1.0f,0.0f);
    glRotatef((GLfloat)iKeyZ,0.0f,0.0f,1.0f);
    glScalef(fKeyA+1.0f,1.0f,1.0f);
    glScalef(1.0f,fKeyS+1.0f,1.0f);
    glScalef(1.0f,1.0f,fKeyD+1.0f);
    ply->draw();  
    glPopMatrix();
    ++index;

    // string render
    gl_ortho_begin(IMAGE_WIDTH, IMAGE_HEIGHT);
    glColor3f(0.0f, 0.0f, 0.0f);
    string_render("OpenGL: How to control PLY Object Using OpenGL.", 20, 580); // fill_me...
    string_render("v: start/end screenshot", 20, 550);
    gl_ortho_end();


    // QR-Code kamon
    
    glDisable(GL_LIGHTING);
    glBegin(GL_POINTS);
    for (int i=0; i<256; i++)
    {
        for (int j=0; j<256; j++)
        {
            glColor3ubv(image[i][j]);
            glVertex3f(2.5f + j/200.0f, -0.99, 2.5f + i/200.0f);
        }
    }   
    glEnd();
    glEnable(GL_LIGHTING);
    

    // screenshot
    if (record)
    {
        screenshot(0,0,IMAGE_WIDTH,IMAGE_HEIGHT);
    }

    glutSwapBuffers();
    
    glDisable(GL_LIGHTING);

}


/*
**
*/
void reshape(int width, int height)
{
    int screenWidth  = width;
    int screenHeight = height;

    if (screenWidth <= 0 || screenHeight <=0)
        return;

    glViewport(0, 0, screenWidth, screenHeight);
    glutReshapeWindow(screenWidth, screenHeight);

    pD.aspect = (float)screenWidth / screenHeight;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(pD.fieldOfView, pD.aspect, pD.nearPlane, pD.farPlane);

    // set basic matrix mode
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


/*
**
*/
void initDisplay()
{
    // Perspective projection parameters
    pD.fieldOfView = 45.0;
    pD.aspect      = (float)IMAGE_WIDTH/IMAGE_HEIGHT;
    pD.nearPlane   = 0.1;
    pD.farPlane    = 50.0;

    // setup context
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(pD.fieldOfView, pD.aspect, pD.nearPlane, pD.farPlane);

    // set basic matrix mode
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClearIndex(0);
    glClearDepth(1);

    //-------------------------------------------------------
    // Kamon (image.raw) must be 256x256 (color image) pixels
    FILE *p;
    p = fopen("qr.raw", "rb");  // fill_me, change it to your student ID number
    fread(image, sizeof(unsigned char), 256*256*3, p);
    fclose(p);
    //-------------------------------------------------------
}


/*
**
*/
void idle_callback(void){
    
    glutSetWindow(window);
    glutPostRedisplay();
    glutSetWindow(window2);
    glutPostRedisplay();
}

void init_ply(void)
{
    FILE *in;
    char *filename = "bunny.ply"; // "dragon.ply", "happy.ply"

    if (!(in = fopen(filename, "r"))) {
        fprintf(stderr, "Cannot open input file %s.\n", filename);
        exit(1);
    }
    ply = new PLYObject(in);
    ply->resize();
}


void initTexture()
{
    //read a ppm image
    unsigned char* image;
    int width, height;
    image = readPPM("face.ppm", &width, &height );
    //generate a texture id
    glGenTextures(1, &texName);
    glBindTexture(GL_TEXTURE_2D,texName);
    //set texture mapping parameters
    //float env_color[]={1.0,1.0,1.0,1.0};
    //glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, env_color);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE); //GL_DECAL, GL_MODULATE, GL_BLEND
    //generate a texture
    glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image);
}



void display2(void)
{
    //glutSetWindow(2)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //string render
    gl_ortho_begin(450,650);

	//¦â½ü
	drawColorWheel();

    //arrow
    glColor3f(1,1,1);
    glBegin(GL_TRIANGLES);
    //up
    glVertex2i(62,650);
    glVertex2i(75,625);
    glVertex2i(50,625);
    //right
    glVertex2i(75,625);
    glVertex2i(100,612);
    glVertex2i(75,600);
    //down
    glVertex2i(50,600);
    glVertex2i(75,600);
    glVertex2i(62,575);
    //left
    glVertex2i(25,612);
    glVertex2i(50,625);
    glVertex2i(50,600);
    glEnd();
    //words
    //glColor3f(1.0f, 0.0f, 0.0f);
    //string_render("UP",55,630); // fill_me...
    //string_render("RIGHT",75,610);
    //string_render("DOWN",40,585);
    //string_render("LEFT",20,610);
    //block
    glColor3f(1,1,0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(100,300);
    glVertex2i(100+256,300);
    glVertex2i(100+256,300+256);
    glVertex2i(100,300+256);
    glEnd();
    //line
    glColor3f(1,1,1);
    glBegin(GL_LINE_STRIP);
    int i = 0;
    for( i=0; i<256; ++i )
    {
        glVertex2i(100+i,300+path[i]);
    }
    glEnd();
    //button
    GLfloat x,y,z,angle;
    z = 0;
    //button 1
    glPushMatrix();
    glTranslatef(212,612,0);
    glColor3f(0.0f,0.5f,0.5f);
    glBegin(GL_POINTS);
    for(angle=0.0f;angle<2*GL_PI;angle+=0.01f)
    {
        x=12.0f*sin(angle);
        y=12.0f*cos(angle);
        glVertex2f(x,y);
    }
    glEnd();
    glPopMatrix();
    //button 2
    glPushMatrix();
    glTranslatef(262,612,0);
    glColor3f(0.0f,0.5f,0.5f);
    glBegin(GL_POINTS);
    for(angle=0.0f;angle<2*GL_PI;angle+=0.01f)
    {
        x=12.0f*sin(angle);
        y=12.0f*cos(angle);
        glVertex2f(x,y);
    }
    glEnd();
    glPopMatrix();
    //button 3
    glPushMatrix();
    glTranslatef(312,612,0);
    glColor3f(0.0f,0.5f,0.5f);
    glBegin(GL_POINTS);
    for(angle=0.0f;angle<2*GL_PI;angle+=0.01f)
    {
        x=12.0f*sin(angle);
        y=12.0f*cos(angle);
        glVertex2f(x,y);
    }
    glEnd();
    glPopMatrix();
    //button 4
    glPushMatrix();
    glTranslatef(362,612,0);
    glColor3f(0.0f,0.5f,0.5f);
    glBegin(GL_POINTS);
    for(angle=0.0f;angle<2*GL_PI;angle+=0.01f)
    {
        x=12.0f*sin(angle);
        y=12.0f*cos(angle);
        glVertex2f(x,y);
    }
    glEnd();
    glPopMatrix();
    //arrow
    glColor3f(0,1,1);
    glBegin(GL_TRIANGLES);
    glVertex2i(125,625);
    glVertex2i(150,612);
    glVertex2i(125,600);
    glEnd();


    gl_ortho_end();
    glutSwapBuffers();
}
/*
**
*/
int main(int argc, char **argv)
{
    
    iKeyT = 1;
    iKeyX = 0;
    iKeyY = 0;
    iKeyZ = 0;
    fKeyA = 0.0f;
    fKeyS = 0.0f;
    fKeyD = 0.0f;
    iKeyF1 = 0;
    iKeyF2 = 0;
    iKeyF3 = 0;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(IMAGE_WIDTH,IMAGE_HEIGHT);

    
    window = glutCreateWindow("OpenGL: How to control PLY Object Using OpenGL. (rotate:a,s,d; scale:z,x,c) "); 

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(readKeyboard);
    glutMouseFunc(mouseButtHandler);
    glutMotionFunc(mouseMoveHandler);
    glutPassiveMotionFunc(mouseMoveHandler);
    glutSpecialFunc(special_callback);
    glutIdleFunc(idle_callback);
    
    glutSpecialUpFunc(F123Keyboard);

    
    init_ply();
    initDisplay();
    initTexture();
	initLighting();
    initTexture2();

    
    // window 2
    glutInitWindowPosition(820,0);
    glutInitWindowSize(450,650);
    window2 = glutCreateWindow ("control panel");
    printf("window2=%d\n",window2);

    glutDisplayFunc(display2);
    glutMouseFunc(mouseButtHandler2);
    glutPassiveMotionFunc(mouseMoveHandler2);

    glutMainLoop();

    return 0;             /* ANSI C requires main to return int. */
}
