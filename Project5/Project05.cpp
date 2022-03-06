/*          Eric Smrkovsky--CSci 172--Project5--Fall2021
*       Computer Graphics Project #5: Multiple Object Loader
*
This program uses OpenGL and soil to...
*
*               Instructions to use program
* Zoom In                       : End_Key  (0.2 increment)
* Zoom Out                      : Home_Key (0.2 decrement)
* Rotate Up   (around x - axis) : Up_Key   (5 degree increment)
* Rotate Down (around x - axis) : Down_Key (5 degree increment)
* Rotate Left (around y - axis) : Left_Key (5 degree increment)
* Rotate Right(around y - axis) : Right_Key(5 degree decrement)
* Toggle Wire Frame             : w_Key
*
* Used tutorial "https://www.youtube.com/watch?v=Ya_HESCH1iU" for VS soil setup
*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "ExternalLibraries/SOIL2/include/SOIL2/SOIL2.h"//used soil2 library
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
float PI = 3.14159;

using namespace std;
//const bool DEBUG = true; //This turns debugging on/off to display in window 
const bool DEBUG = false; //This turns debugging on/off to display in window 

//This struct is used to hold values for vertices and normals
typedef struct V {
    float x;
    float y;
    float z;

    //copy constructor
    V(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
}V;

//This struct is used to hold values for texture values
typedef struct VT {
    float x;
    float y;
    //copy constructor
    VT(float x, float y) {
        this->x = x;
        this->y = y;
    }
}VT;


//for drawing this objects
vector<V> v;                //a vector of typedef struct for the vertex indices
vector<V> vn;               //a vector of typedef struct for the vector normal indices
vector<VT> vt;              //a vector of typedef struct for the texture indices

//for cube
vector<V> cube_v;               //a vector of typedef struct for the vertex indices
vector<VT> cube_vt;             //created this so thta I don't haver to make a seperate object loader
vector<V> cube_vn;              //a vector of typedef struct for the vector normal indices
int** F0 = new int* [12];       //a 2D array of ints to hold cube faces

//for ven
vector<V> ven_v;                 //a vector of typedef struct for the vertex indices
vector<VT> ven_vt_not_used;      //created this so thta I don't haver to make a seperate object loader
vector<V> ven_vn;                //a vector of typedef struct for the vector normal indices
int** F1 = new int* [43357];      //a 2D array of ints to hold ven faces

//for ateneam
vector<V> ateneam_v;             //a vector of typedef struct for the vertex indices
vector<V> ateneam_vn;            //a vector of typedef struct for the vector normal indices
vector<VT> ateneam_vt;           //a vector of typedef struct for the texture indices
int** F2 = new int* [22250];      //a 2D array of ints to hold ateneam faces

//for bunny
vector<V> bunny_v;               //a vector of typedef struct for the vertex indices
vector<V> bunny_vn;              //a vector of typedef struct for the vector normal indices
vector<VT> bunny_vt;             //a vector of typedef struct for the texture indices
int** F3 = new int* [30338];      //a 2D array of ints to hold bunny faces

//for changing between stored objects
int objectChoice = 0;

//light settings
GLfloat position[] = { 0.0f, 1.0f, 1.0f, 0.0f };//for my light
const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

//Quaternion rotation variables
// //see https://automaticaddison.com/how-to-describe-the-rotation-of-a-robot-in-3d/
//create the data structure to hold Q (slide19 lec20 slides)
GLfloat matrixX[16];    //a 3 by 3 Q rotation matrix
GLfloat matrixY[16];    //a 3 by 3 Q rotation matrix
GLfloat matrixZ[16];    //a 3 by 3 Q rotation matrix
GLfloat x, y, z, w;     //variables to be used in 
static GLint RotateY = 0; /* model rotation Y index*/
static GLint RotateX = 0; /* model rotation X index*/
static GLint RotateZ = 0; /* model rotation X index*/

//cube position for glTranslatef function
GLfloat cubeX, cubeY, cubeZ;

//rotation angle variables for glRotatef
GLfloat objectAngleX, objectAngleY;
GLfloat skyBoxAngleX, skyBoxAngleY;

//size of teapot and cube for glTranslatef function
GLfloat cubeSize;

//scale multiplier (set to 1 when not in use)
GLfloat scale;
GLfloat sMulC;//for cube and ven
GLfloat sbMulC;//for skyBox

//camera position
GLfloat camX;
GLfloat camY;

//light position
GLfloat lightX;
GLfloat lightY;

//bool to trigger the wireframe functionality
bool WireFrame = false;

//to hold ids of buckets (now we need two one for cube, one for skybox) 
GLuint tex[2];

//function to calculate variables used in rotation matrix
void CreateFromAxisAngle(GLfloat X, GLfloat Y, GLfloat Z, GLfloat degree)
{
    /* First we want to convert the degrees to radians since the angle is assumed to be in radians */
    GLfloat angle = (GLfloat)((degree / 180.0f) * PI);
    /* Here we calculate the sin( theta / 2) once for optimization */
    GLfloat result = (GLfloat)sin(angle / 2.0f);
    /* Calcualte the w value by cos( theta / 2 ) */
    w = (GLfloat)cos(angle / 2.0f);
    /* Calculate the x, y and z of the quaternion */
    x = (GLfloat)(X * result);
    y = (GLfloat)(Y * result);
    z = (GLfloat)(Z * result);
}

//create the rotation matrix
void CreateMatrix(GLfloat* pMatrix) {
    //visual of matrix 
    //[0][1][2]    | [3]
    //[4][5][6]    | [7]
    //[8][9][10]   | [11]
    //[12][13][14] | [15]

    //column 1
    pMatrix[0] = 1.0f - 2.0f * (y * y + z * z);
    pMatrix[4] = 2.0f * (x * y + z * w);
    pMatrix[8] = 2.0f * (x * z - y * w);
    pMatrix[12] = 0.0f;
    //column 2
    pMatrix[1] = 2.0f * (x * y - z * w);
    pMatrix[5] = 1.0f - 2.0f * (x * x + z * z);
    pMatrix[9] = 2.0f * (z * y + x * w);
    pMatrix[13] = 0.0f;
    //coulmn 3
    pMatrix[2] = 2.0f * (x * z + y * w);
    pMatrix[6] = 2.0f * (y * z - x * w);
    pMatrix[10] = 1.0f - 2.0f * (x * x + y * y);
    pMatrix[14] = 0.0f;
    //final column showing direction
    pMatrix[3] = 0.0f;
    pMatrix[7] = 0.0f;
    pMatrix[11] = 0.0f;
    pMatrix[15] = 1.0f;
}

//Resises the window (given by Dr. Dhanyu)
static void resize(int width, int height){
    double Ratio;

    width = (float)width;
    height = (float)height;

    Ratio = (double)width / (double)height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0f, Ratio, 0.1f, 1500.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

//Draw the ateneam
void createAteneam() {

    //create push/pop matrix for Cube
    glPushMatrix();

    glRotatef(objectAngleX, 1, 0, 0);      //rotates around X axis
    glRotatef(objectAngleY-90, 0, 1, 0);      //rotates around Y axis  
    glScalef(sMulC / scale, sMulC / scale, sMulC / scale);       //zoom in/out depending on user input

    glBindTexture(GL_TEXTURE_2D, tex[0]);//bind cube texture to cube
    int j = 0;//for adding to vn, vt, v values 

    //draw the ateneam
    for (int i = 0; i < 22250; i++) {
        glBegin(GL_TRIANGLES);
        for (int c = 0, j = 0; c <= 2; c++) {
            glNormal3f(ateneam_vn[F2[i][2 + j] - 1].x, ateneam_vn[F2[i][2 + j] - 1].y, ateneam_vn[F2[i][2 + j] - 1].z);
            glTexCoord2f(ateneam_vt[F2[i][1 + j] - 1].x, ateneam_vt[F2[i][1 + j] - 1].y);
            glVertex3f(ateneam_v[F2[i][0 + j] - 1].x, ateneam_v[F2[i][0 + j] - 1].y, ateneam_v[F2[i][0 + j] - 1].z);
            j = j + 3;
        }
        glEnd();
    }
    glPopMatrix();
}

//Calculate missing normals for bunny
//Note: math given in lec 16 calculating normals
void calculateNormals() {
    
    vector<V> surfaceNormalsForBunny;
    vector<vector<int>> vIndexes(bunny_v.size());
    
    //for each face of the bunny CALCULATE SURFACE NORMAL
    for (int i = 0; i < 30338; i++) {
        //preparing to calculate surface normals 
        //need to collect data for cross product math 
        
        //get vertexes of face (p1, p2, p3) and place inside a custom vec struct
        V p1(bunny_v[F3[i][0] - 1].x, bunny_v[F3[i][0] - 1].y, bunny_v[F3[i][0] - 1].z);
        V p2(bunny_v[F3[i][3] - 1].x, bunny_v[F3[i][3] - 1].y, bunny_v[F3[i][3] - 1].z);
        V p3(bunny_v[F3[i][6] - 1].x, bunny_v[F3[i][6] - 1].y, bunny_v[F3[i][6] - 1].z);

        //use vertexes of face (p1, p2, p3) to calculate to vectors (U, V)
        //and place inside a custom vec struct 
        V U(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);//set U to (p2 - p3)
        V V_(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);//set U to (p3 - p1)

        //find vector normals componants by calculating cross product for x, y, z  
        float Nx = (U.y * V_.z) - (U.z * V_.y);
        float Ny = (U.z * V_.x) - (U.x * V_.z);
        float Nz = (U.x * V_.y) - (U.y * V_.x);

        //get the vector length 
        float length = sqrt((Nx * Nx) + (Ny * Ny) + (Nz * Nz));

        //normalize by dividing the componants by the vector length
        float Nvx = (Nx / length);
        float Nvy = (Ny / length);
        float Nvz = (Nz / length);
        V normalized(Nvx, Nvy, Nvz);
        
        ////place in appropriate vector for bunny
        // bunny_vn.push_back(normalized);
        // ran into error here where normals came out rough because     
        // was computed by face, so now have to compute per vertex
        
        //store the surface normals
        surfaceNormalsForBunny.push_back(normalized);

        //when computing normal per vertex must know the faces that 
        //share the vertex, so we need to remember the F3 indexes
        vIndexes[F3[i][0] - 1].push_back(i);
        vIndexes[F3[i][3] - 1].push_back(i);
        vIndexes[F3[i][6] - 1].push_back(i);
    }
    
    //normalized sum of all the unit length normal for each face the vertex shares
    for (int i = 0; i < bunny_v.size(); i++) {
        
        vector<int> vs = vIndexes[i];
        vector<V> surfaceNormals;
        float xTotal = 0;
        float yTotal = 0;
        float zTotal = 0;

        //collect all the surface normals for a total so we can normalize
        for (int i = 0; i < vs.size(); i++) 
            surfaceNormals.push_back(surfaceNormalsForBunny[vs[i]]);
        
        //find values to calculate length
        for (int i = 0; i < surfaceNormals.size(); i++) {
            xTotal += surfaceNormals[i].x;
            yTotal += surfaceNormals[i].y;
            zTotal += surfaceNormals[i].z;
        }

        //get the vector length 
        float length2 = sqrt((xTotal * xTotal) + (yTotal * yTotal) + (zTotal * zTotal));

        //normalize by dividing the componants by the vector length
        float xNormalized = (xTotal / length2);
        float yNormalized = (yTotal / length2);
        float zNormalized = (zTotal / length2);
        V normalized2(xNormalized, yNormalized, zNormalized);
        
        //store vector normals for bunny fixing its face!
        bunny_vn.push_back(normalized2);
    }
}

//Draw the bunny
void createBunny() {

    //create push/pop matrix for Cube
    glPushMatrix();

    glRotatef(objectAngleX, 1, 0, 0);      //rotates around X axis
    glRotatef(objectAngleY, 0, 1, 0);      //rotates around Y axis  
    glScalef(sMulC / 200, sMulC / 200, sMulC / 200);       //zoom in/out depending on user input

    glBindTexture(GL_TEXTURE_2D, tex[0]);//bind cube texture to cube
    int j = 0;//for adding to vn, vt, v values 

    //draw the bunny
    for (int i = 0; i < 30338; i++) {
        glBegin(GL_TRIANGLES);
        for (int c = 0, j = 0; c <= 2; c++) {
            glNormal3f(bunny_vn[F3[i][2 + j] - 1].x, bunny_vn[F3[i][2 + j] - 1].y, bunny_vn[F3[i][2 + j] - 1].z);
            glTexCoord2f(bunny_vt[F3[i][1 + j] - 1].x, bunny_vt[F3[i][1 + j] - 1].y);
            glVertex3f(bunny_v[F3[i][0 + j] - 1].x, bunny_v[F3[i][0 + j] - 1].y, bunny_v[F3[i][0 + j] - 1].z);
            j = j + 3;
        }
        glEnd();
    }
    glPopMatrix();
}

//Draw the venetion lady 
void createVen() {

    //create push/pop matrix for Cube
    glPushMatrix();

    glRotatef(objectAngleX, 1, 0, 0);      //rotates around X axis
    glRotatef(objectAngleY, 0, 1, 0);      //rotates around Y axis  
    glScalef(sMulC / (scale-200), sMulC / (scale - 200), sMulC / (scale - 200));      //zoom in/out depending on user input
    
    int j = 0;//for adding to vn, vt, v values 
    
    //draw the ven
    for (int i = 0; i < 43357; ++i) { //draw each face
        glBegin(GL_TRIANGLES);
        glDisable(GL_TEXTURE_2D);//turn off texturing for drawing faces        

        //same as for cube except only 6 values for faces due to no texture
        glNormal3f(ven_vn[F1[i][1] - 1].x, ven_vn[F1[i][1] - 1].y, ven_vn[F1[i][1] - 1].z);
        glVertex3f(ven_v[F1[i][0] - 1].x, ven_v[F1[i][0] - 1].y, ven_v[F1[i][0] - 1].z);

        glNormal3f(ven_vn[F1[i][3] - 1].x, ven_vn[F1[i][3] - 1].y, ven_vn[F1[i][3] - 1].z);
        glVertex3f(ven_v[F1[i][2] - 1].x, ven_v[F1[i][2] - 1].y, ven_v[F1[i][2] - 1].z);

        glNormal3f(ven_vn[F1[i][5] - 1].x, ven_vn[F1[i][5] - 1].y, ven_vn[F1[i][5] - 1].z);
        glVertex3f(ven_v[F1[i][4] - 1].x, ven_v[F1[i][4] - 1].y, ven_v[F1[i][4] - 1].z);
        glEnable(GL_TEXTURE_2D);//turn on texturing

       glEnd();
    }
    glPopMatrix();
}

//Draw the cube
void createCube() {
    
    //create push/pop matrix for Cube
    glPushMatrix();

    glRotatef(objectAngleX, 1, 0, 0);      //rotates around X axis
    glRotatef(objectAngleY, 0, 1, 0);      //rotates around Y axis  
    glScalef(sMulC, sMulC, sMulC);       //zoom in/out depending on user input

    glBindTexture(GL_TEXTURE_2D, tex[0]);//bind cube texture to cube
    int j = 0;//for adding to vn, vt, v values 
    
    //draw the cube
    for (int i = 0; i < 12; i++) {
        glBegin(GL_TRIANGLES);
        for (int c = 0, j = 0; c <= 2; c++) {
            glNormal3f(cube_vn[F0[i][2 + j] - 1].x, cube_vn[F0[i][2 + j] - 1].y, cube_vn[F0[i][2 + j] - 1].z);
            glTexCoord2f(cube_vt[F0[i][1 + j] - 1].x, cube_vt[F0[i][1 + j] - 1].y);
            glVertex3f(cube_v[F0[i][0 + j] - 1].x, cube_v[F0[i][0 + j] - 1].y, cube_v[F0[i][0 + j] - 1].z);
            j = j + 3;
        }
        glEnd();
    }
    glPopMatrix();
}

//Draw the skyBox
void createSkyBox() {
    //DRAW THE SKYBOX
    glBindTexture(GL_TEXTURE_2D, tex[1]);//bind skyBox texture to skyBox
    glPushMatrix();

    glRotatef(skyBoxAngleX, 1, 0, 0);      //rotates around X axis
    glRotatef(skyBoxAngleY, 0, 1, 0);      //rotates around Y axis  
    glScalef(sbMulC, sbMulC, sbMulC);      //zoom in/out depending on user input

    //had to draw these inverse transpose 
    // 0,0 tex(bottom-left of current face)
    // 1,0 tex(bottom-right of current face)
    // 1,1 tex(top-right of current face)
    // 0,1 tex(top-left of current face)
    // vertex values
    // -1, -1, -1 = v1 = back lower left   = BLL 
    // -1, -1,  1 = v2 = front lower left  = FLL
    // -1,  1, -1 = v3 = back upper left   = BUL
    // -1,  1,  1 = v4 = front upper left  = FUL
    //  1, -1, -1 = v5 = back lower right  = BLR
    //  1, -1,  1 = v6 = front lower right = FLR
    //  1,  1, -1 = v7 = back upper right  = BUR
    //  1,  1,  1 = v8 = front upper right = FUR

    //Draw the back face of the skybox    
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(.75, .667);glVertex3f(-1, -1, -1);
    glTexCoord2f(1, .667); glVertex3f(1, -1, -1);
    glTexCoord2f(1, .333); glVertex3f(1, 1, -1);
    glTexCoord2f(.75, .333); glVertex3f(-1, 1, -1);

    //Draw the left face of the skybox    
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, .667);      glVertex3f(1, -1, -1);
    glTexCoord2f(.25, .667);    glVertex3f(1, -1, 1);
    glTexCoord2f(.25, .333);    glVertex3f(1, 1, 1);
    glTexCoord2f(0, .333);      glVertex3f(1, 1, -1);

    //Draw the top face of the skybox
    glNormal3f(0, -1, 0);
    glTexCoord2f(.25, .333);    glVertex3f(1, 1, 1);
    glTexCoord2f(.5, .333);     glVertex3f(-1, 1, 1);
    glTexCoord2f(.5, 0);        glVertex3f(-1, 1, -1);
    glTexCoord2f(.25, 0);       glVertex3f(1, 1, -1);

    //Draw the right face of the skybox    
    glNormal3f(1, 0, 0);
    glTexCoord2f(.5, .667);    glVertex3f(-1, -1, 1);
    glTexCoord2f(.75, .667);   glVertex3f(-1, -1, -1);
    glTexCoord2f(.75, .333);   glVertex3f(-1, 1, -1);
    glTexCoord2f(.5, .333);    glVertex3f(-1, 1, 1);

    //Draw the bottom face of the skybox
    glNormal3f(0, 1, 0);
    glTexCoord2f(.25, 1);    glVertex3f(1, -1, -1);
    glTexCoord2f(.5, 1);     glVertex3f(-1, -1, -1);
    glTexCoord2f(.5, .667);  glVertex3f(-1, -1, 1);
    glTexCoord2f(.25, .667); glVertex3f(1, -1, 1);

    //Draw the front face of the skybox
    glNormal3f(0, 0, -1);
    glTexCoord2f(.25, .667);    glVertex3f(1, -1, 1);
    glTexCoord2f(.5, .667);     glVertex3f(-1, -1, 1);
    glTexCoord2f(.5, .333);     glVertex3f(-1, 1, 1);
    glTexCoord2f(.25, .333);    glVertex3f(1, 1, 1);

    glEnd();
    glPopMatrix();
}

//Displays the object and the skybox
static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //creates viewing matrix derived from an eye point, 
    //a reference point indicating the center of the scene, and an UP vector.
    //(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ)
    gluLookAt(camX, camY, 3, camX, camY, 0.0, 0.0, 1.0, 0.0);

    if (WireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Draw Our Mesh In Wireframe Mesh
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		//Toggle WIRE FRAME

    //CREATE LIGHT
    glPushMatrix();
    glRotatef(lightX, 1, 0, 0);      //rotates around X axis
    glRotatef(lightY, 0, 1, 0);      //rotates around Y axis  
    glTranslatef(1, 0, 0);
    glColor3f(1.0, 0.8, 0.6);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glPopMatrix();

    //CREATE QUATERNION ROTATION
    CreateMatrix(matrixX); /* initial quatonion */
    CreateFromAxisAngle(1, 0, 0, RotateX); /* quatonion for x rotation */
    glMultMatrixf(matrixY);
    CreateMatrix(matrixY); /* initial quatonion */
    CreateFromAxisAngle(0, 1, 0, RotateY); /* quatonion for x rotation */
    glMultMatrixf(matrixY);
    CreateMatrix(matrixZ); /* initial quatonion */
    CreateFromAxisAngle(0, 0, 1, RotateZ); /* quatonion for x rotation */
    glMultMatrixf(matrixZ);

    //CREATE SKYBOX
    createSkyBox();

    //CREATE CUBE
    if(objectChoice == 0) createCube(); 

    //CREATE VEN
    if(objectChoice == 1) createVen(); 
    
    //CREATE ATENEAM
    if (objectChoice == 2) createAteneam();

    //CREATE BUNNY
    if (objectChoice == 3) createBunny();

    glutSwapBuffers();
}

//store to appropriate data to vectors used in display function
void switchLoadedObject(int choice) {

    //choice is object ID, 0=cube, 1=ven, 2=ateneam, 3=bunny
    if (choice == 0) {//storing for cube
        v = cube_v;
        vn = cube_vn;
        vt = cube_vt;
    }
    if (choice == 1) {//storing for ven
        v = ven_v;
        vn = ven_vn;
        vt = ven_vt_not_used;
    }
    if (choice == 2) {//storing for ateneam
        v = ateneam_v;
        vn = ateneam_vn;
        vt = ateneam_vt;
    }
    if (choice == 3) {//storing for bunny
        v = bunny_v;
        vn = bunny_vn;
        vt = bunny_vt;
    }
}

//Assign functionality for keys 
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        //press ESC or 'q' to terminate program
    case 27:
    case 'q':
        exit(0);
        break;

        //user presses key 'a' and cycles left through objects
    case 'a':
        if (DEBUG) cout << "LEFT TO OBJECT\n";
        if (objectChoice == 0) objectChoice = 3;
        else objectChoice--;
        switchLoadedObject(objectChoice);
        break;

        //user presses key 'd' and cycles right through objects
    case 'd':
        if (DEBUG)  cout << "RIGHT TO OBJECT\n";
        if (objectChoice == 3) objectChoice = 0;
        else objectChoice++;
        switchLoadedObject(objectChoice);
        break;

        //user presses key 'j' and is rotate camera left around the object
    case 'j':
        if (DEBUG) cout << "ROTATE CAMERA LEFT AROUND OBJECT\n";
        RotateY = (RotateY - 5) % 360;
        break;

        //user presses key 'l' and is rotate camera right around the object
    case 'l':
        if (DEBUG) cout << "ROTATE CAMERA RIGHT AROUND OBJECT\n";
        RotateY = (RotateY + 5) % 360;
        break;

        //user presses key 'i' and is rotate camera up around the object
    case 'i':
        if (DEBUG) cout << "ROTATE CAMERA UP AROUND OBJECT\n";
        RotateX = (RotateX - 5) % 360;
        break;

        //user presses key 'k' and is rotate camera down around the object
    case 'k':
        if (DEBUG) cout << "ROTATE CAMERA DOWN AROUND OBJECT\n";
        RotateX = (RotateX + 5) % 360;
        break;

        //user presses key 'o' and resets camera to starting position
    case 'o':
        if (DEBUG) cout << "RESET CAMERA POSITION\n";
        camX = 0;
        camY = 0;
        break;

        //user presses key '1' and is rotate light UP around the X axis
    case '1':
        if (DEBUG) cout << "ROTATE LIGHT UP AROUND X AXIS\n";
        lightY += 5;
        break;

        //user presses key '2' and is rotate light down around the X axis
    case '2':
        if (DEBUG) cout << "ROTATE LIGHT DOWN AROUND X AXIS\n";
        lightX -= 5;
        break;

        //user presses key '3' and is rotate light down around the Y axis
    case '3':
        if (DEBUG) cout << "ROTATE LIGHT UP AROUND Y AXIS\n";
        lightX += 5;
        break;

        //user presses key '4' and is rotate light down around the Y axis
    case '4':
        if (DEBUG) cout << "ROTATE LIGHT DOWN AROUND Y AXIS\n";
        lightY -= 5;
        break;

        //user presses key '0' and resets light to starting position
    case '0':
        if (DEBUG) cout << "RESET LIGHT POSITION\n";
        lightX = 0;
        lightY = 0;
        break;

        //user presses key space and is able to reset the scene
    case ' ':
        if (DEBUG) cout << "RESET SCENE\n";
        //camera position
        camX = 0;
        camY = 0;

        //rotation degree variables for glRotatef
        objectAngleX = 0, objectAngleY = 0;
        skyBoxAngleX = 0, skyBoxAngleY = 180;
        lightX = 0; lightY = 0;

        //scale multiplier (set to 1 when not in use)
        sMulC = .4;  //for objects
        sbMulC = 6;  //for skyBox
        break;

        //user presses key 'w' and is able to display the cube in wireframe mode
    case 'w':
        if (DEBUG) cout << "\nWireframe..\n";
        WireFrame = !WireFrame;//change the wireframe flag back and forth
        break;
    }
}

//Assign functionality for keys UP, DOWN, RIGHT, LEFT, HOME, END
void Specialkeys(int key, int x, int y)
{
    switch (key)
    {
        //user presses key LEFT and object is able to preform a rotation left around the Y axis
    case GLUT_KEY_LEFT:
        if (DEBUG) cout << "ROTATE OBJECT LEFT AROUND Y AXIS\n";
        objectAngleY -= 5; //set the angle to be rotated to 5 degrees
        break;

        //user presses key RIGHT and object is able to preform a rotation right around the Y axis
    case GLUT_KEY_RIGHT:
        if (DEBUG)  cout << "ROTATE OBJECT RIGHT AROUND Y AXIS\n";
        objectAngleY += 5; //set the angle to be rotated to 5 degrees
        break;

        //user presses key UP and is object able to preform a rotation up around the X axis
    case GLUT_KEY_UP:
        if (DEBUG) cout << "ROTATE OBJECT UP AROUND X AXIS\n";
        objectAngleX -= 5; //set the angle to be rotated to 5 degrees
        break;

        //user presses key DOWN and object is able to preform a rotation down around the X axis
    case GLUT_KEY_DOWN:
        if (DEBUG) cout << "ROTATE OBJECT DOWN AROUND X AXIS\n";
        objectAngleX += 5; //set the angle to be rotated to 5 degrees
        break;

        //user presses key END and is able to preform a zoom in
    case GLUT_KEY_END:
        if (DEBUG) cout << "ZOOM IN\n";
        sMulC += 0.2; //increments by .2 in scaling function (above 1.0 stretches)
        sbMulC += 0.2; //increments by .2 in scaling function (above 1.0 stretches)
        break;

        //user presses key HOME and is able to preform a zoom out
    case GLUT_KEY_HOME:
        if (DEBUG) cout << "ZOOM OUT\n";
        sMulC -= 0.2; //decrements by .2 in scaling function (below 1.0 shrinks)
        sbMulC -= 0.2; //decrements by .2 in scaling function (below 1.0 shrinks)
        break;
    }
    glutPostRedisplay();
}

//prints the file that is being loaded (used for debugging)
void printLines(string& filename) {
    int i = 0;
    //open the file
    fstream myfile;
    myfile.open(filename);

    string temp;//to hold each line while we sort
    while (getline(myfile, temp)) {
        cout << "Line" << i++ << "          " << temp << "          " << endl;
    }

    myfile.close();
}

//auto updater
static void idle(void){
    lightX -= .8;//cause light to rotate around object without doing anything
    glutPostRedisplay();
}

//load images for cube and skybox
void myImageLoad() {

    glEnable(GL_TEXTURE_2D);        //enable texture 2d bucket
    glGenTextures(2, tex);         //generate id for bucket

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, tex[0]);//this is the texture that we are going to use for cube

    int width, height;          //width and height of image
    unsigned char* image;       //loaded image 
    //loads bucket
    image = SOIL_load_image("images/crate.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (!image) cout << "image not found" << endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    SOIL_free_image_data(image);//free the image data cause we are done with it
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //loads bucket2
    glBindTexture(GL_TEXTURE_2D, tex[1]);//this is the texture that we are going to use for skybox
    //image = SOIL_load_image("images/aiden.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    image = SOIL_load_image("images/sample_skybox.jpg", &width, &height, 0, SOIL_LOAD_RGB);
    if (!image) cout << "image not found" << endl;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    SOIL_free_image_data(image);//free the image data cause we are done with it
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_CLAMP, GL_LINEAR);//helps the image blend
}

//Initialize all variables with starting values 
static void init(void)
{
    //camera position
    camX = 0;
    camY = 0;

    //cube position for glTranslatef function
    cubeX = 0.0, cubeY = 0.0, cubeZ = -2.0;

    //rotation degree variables for glRotatef
    objectAngleX = 0, objectAngleY = 0;
    skyBoxAngleX = 0, skyBoxAngleY = 180;

    //scale multiplier (set to 1 when not in use)
    scale = 1500;//makes a good starting size for ven
    sMulC = .4;  //for cube, ven
    sbMulC = 6;  //for skyBox

    //light position
    lightX = 0;
    lightY = 0;

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

    myImageLoad();//loads images
}

//load the object file in to the appropriate vectors
//objNum is object, 0=cube, 1=ven, 2=ateneam, 3=bunny
void myLoader(bool debug, string filename, int objNum, int** F, vector<V>& v, vector<V>& vn, vector<VT>& vt) {
    //debug = 0;//to control debugging local to this function rather then globally

    // print the file if debugging is on
    //if (debug) {
    //    //string filename = "objFiles/cube.obj";
    //    printLines(filename);
    //}

    //open the object file for cube
    fstream myfile;
    myfile.open(filename);

    //check that file was opened succsessfully
    if (debug && myfile.is_open()) {
        cout << "File opened for parsing...\n";
    }

    string temp;//creat a string var to hold lines from file 
    string token;//creat a string var to hold token from line

    int vi = 0;//a standard counter to keep track of where we are at in the file
    int vni = 0;//a standard counter to keep track of where we are at in the file
    int vti = 0;//a standard counter to keep track of where we are at in the file
    int fi = 0;//a standard counter to keep track of where we are at in the file
    int lineNum = 1;//keeps tack of what line we are on for debugging
    string x, y, z;
    bool done = false;//set flags
    int tableSizes[4];//hold the 'd' values given in the file. 
    
    //main while loop that collects v, vn, vt and f values and stores them 
    //in correct data structures all vectors except faces are in 2d array
    while (getline(myfile, temp) && !done) {
        
        //Constructs a stringstream object with a copy of temp as content.
        stringstream ss(temp);

        //display skipped lines and keep track of line count and table sizes
        if (temp.empty()) { //skip blank lines
            if (debug) { 
                cout << "I just hit a blank line\n"; 
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
            lineNum++;
            continue; 
        } 
        else if (temp.front() == '#') {//skip lines beggining with #
            if (debug) {
                cout << "I just hit a #\n";
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
            lineNum++;
            continue;
        }
        else if (temp.front() == 'd') {//skip lines beggining with d
            if (debug) {
                cout << "I just hit a d\n";
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
            //get table sizes 
            getline(ss, token, ' ');
            getline(ss, token, ' ');
            tableSizes[0] = atoi(token.c_str());
            //debug = 1;
            if (debug) cout << "Table sizes-->   v: " << tableSizes[0] << "  ";
            getline(ss, token, ' ');
            tableSizes[1] = atoi(token.c_str());
            if (debug) cout << "vn: " << tableSizes[1] << "  ";
            getline(ss, token, ' ');
            tableSizes[2] = atoi(token.c_str());
            if (debug) cout << "vt: " << tableSizes[2] << "  ";
            getline(ss, token, ' ');
            tableSizes[3] = atoi(token.c_str());
            if (debug) cout << "f: " << tableSizes[3] << endl;
            lineNum++;
            //debug = 0;
            continue;
        }
        else if (temp.front() == 'g') {//skip lines beggining with g
            if (debug) {
                cout << "I just hit a g\n";
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
            lineNum++;
            continue;
        }
        else if (temp.front() == 's') {//skip lines beggining with s
            if (debug) {
                cout << "I just hit a s\n";
                this_thread::sleep_for(chrono::milliseconds(1000));
            }
            lineNum++;
            continue;
        }
        else if  (debug) {
            cout << "Parsing line" << lineNum++ << ": " << temp << "    ---->    ";
        }

        //place tokens in correct struct
        while (getline(ss, token, ' ')) {//read identifier token

            //get the v tokens, convert to float and add to vector
            if (token == "v") {
                //capture x
                getline(ss, token, ' ');
                if (token.empty()) { getline(ss, token, ' '); }
                x = token;

                //capture y
                getline(ss, token, ' ');
                if (token.empty()) { getline(ss, token, ' '); }
                y = token;

                //capture z
                getline(ss, token, ' ');
                if (token.empty()) { getline(ss, token, ' '); }
                z = token;

                //push x, y, z into struct
                v.push_back(V(atof(x.c_str()), atof(y.c_str()), atof(z.c_str())));

                if (debug) cout << "tokens: " << x << "  " << y << "  " << z << "   inVec--->  ";
                if (debug) cout << " x: " << v[vi].x << "  y: " << v[vi].y << "  z: " << v[vi].z << "   " << endl;

                vi++; //increment counter for vector index
            }

            //get the vn tokens, convert to float and add to vector
            if (token == "vn") {
                //capture x
                getline(ss, token, ' ');
                if (token.empty()) { getline(ss, token, ' '); }
                x = token;

                //capture y
                getline(ss, token, ' ');
                if (token.empty()) { getline(ss, token, ' '); }
                y = token;

                //capture z
                getline(ss, token, ' ');
                if (token.empty()) { getline(ss, token, ' '); }
                z = token;

                //push x, y, z into struct
                vn.push_back(V(atof(x.c_str()), atof(y.c_str()), atof(z.c_str())));

                if (debug) cout << "tokens: " << x << "  " << y << "  " << z << "   inVec--->  ";
                if (debug) cout << " x: " << vn[vni].x << "  y: " << vn[vni].y << "  z: " << vn[vni].z << "   " << endl;

                vni++; //increment counter for vector index
            }

            //get the vt tokens, convert to float and add to vector
            if (token == "vt") {
                //capture x
                getline(ss, token, ',');
                //if (token.empty()) { getline(ss, token, ' '); }
                x = token;

                //capture y
                getline(ss, token, ',');
                if (token.empty()) { getline(ss, token, ' '); }
                y = token;

                //push x, y into struct
                vt.push_back(VT(atof(x.c_str()), atof(y.c_str())));

                if (debug) cout << "tokens: " << x << "  " << y << "   inVec--->  ";
                if (debug) cout << " x: " << vt[vti].x << "  y: " << vt[vti].y << "   " << endl;

                vti++; //increment counter for vector index
            }

            //get the faces 
            if (token == "f") {
                int venNumFaces = 43357;
                int numSlots = 9;//9 slots for each index per row for cube, ateneam, bunny
                if(tableSizes[3] == venNumFaces)//catch the one obj file that has 6 face vals
                    numSlots = 6;       //6 slots for each index per row for ven

                char separators[] = " /s,\t\n";//values we skip in the string 
                char* token, * nextToken, * char_arr;//needed for tokenization process

                //set a flag
                string str_obj(temp);//convert string to char array
                char_arr = &str_obj[0];//convert string to char array (a v, vn, or vt)
                token = strtok_s(char_arr, separators, &nextToken);//get first token
                F[fi] = new int[numSlots];  //allocate mem for ind number of columns	
                
                for (int col = 0; col < numSlots; col++) {

                    F[fi][col] = atoi(strtok_s(NULL, separators, &nextToken));//insert
                    if (debug) cout << "inserted " << F[fi][col] << " at F[" << fi << "][" << col << "]\n";
                }
                fi++;
            }
        }
    }
    if (debug) cout << "\n\nQuit!";
    myfile.close(); //close the object file
        
    //if at the cube set drawing vectors for first object
    if (objNum == 0) {//storing for cube
        v = cube_v;
        vn = cube_vn;
        vt = cube_vt;
    }

    //this is the case where bunny has no vertex normals
    if (vn.size() == 0) {
        calculateNormals();
    }
}

/* Program entry point */

int main(int argc, char* argv[])
{
    //objNum is object, 0=cube, 1=ven, 2=ateneam, 3=bunny
    //debug on/off == true/false set at top of program
    
    //load up the cube
    myLoader(DEBUG, "objFiles/cube.obj", 0, F0, cube_v, cube_vn, cube_vt);   

    //load up the venitial lady statue
    myLoader(DEBUG, "objFiles/ven.obj", 1, F1, ven_v, ven_vn, ven_vt_not_used);

    //load up the ateneam
    myLoader(DEBUG, "objFiles/ateneam.obj", 2, F2, ateneam_v, ateneam_vn, ateneam_vt); 

    //load up the fluffy bunny
    myLoader(DEBUG, "objFiles/bunny.obj", 3, F3, bunny_v, bunny_vn, bunny_vt);   

    //initialize Glut variables
    glutInit(&argc, argv);

    //set window size
    glutInitWindowSize(1000, 900);//starting size of window

    //set window position
    glutInitWindowPosition(1100, 100);//start window pos 

    //set the modes for the display process
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    //open the debug console 
    glutCreateWindow("Project 5: Multiple Model Loader");

    //initialize my variables
    init();

    //resize the view in the window when resizing window manually
    glutReshapeFunc(resize);

    //display what we have loaded
    glutDisplayFunc(display);

    //keyboard functions that take in user inputs
    glutKeyboardFunc(key);
    glutSpecialFunc(Specialkeys);

    //continue to update when no user input is happening (for the light)
    glutIdleFunc(idle);

    //run glut handler constantly
    glutMainLoop();

    return EXIT_SUCCESS;
}//end main program