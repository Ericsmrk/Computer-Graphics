/* Eric Smrkovsky--CSci 172--Project2--Fall2021
* This program uses OpenGL to manipulate 3 different shapes
* TEAPOT CUBE SPHERE
* Instructions to use program
* Zoom In : Up_Key(0.5 increment)
* Zoom Out : Down_Key(0.5 decrement)
* Rotate Left(around y - axis) : Left_key(by 5 degrees increment)
* Rotate Right(around y - axis) : Right_key(by5 degrees decrement)
* Ex ... c + Up_key : cube zoom in but the rest remains the same
* Ex ... t + Up_key : cube teapot in but the rest remains the same
* Ex ... s + Up_key : cube sphere in but the rest remains the same 
*/

#include <string.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>

#include <math.h>
using namespace std;

bool WireFrame = false;

//light settings
const GLfloat light_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

//variables for inputs to functions
//Sphere position for glTranslatef function
GLfloat sphereX, sphereY, sphereZ;

//Cube position for glTranslatef function
GLfloat cubeX, cubeY, cubeZ;

//Teapot position for glTranslatef function
GLfloat teapotX, teapotY, teapotZ; 

//rotation angle variables for glRotatef
GLfloat sphereAngle, teapotAngle, cubeAngle;

//size of teapot and cube for glTranslatef function
GLfloat teapotSize, cubeSize;

//radius of sphere for glTranslatef function
GLfloat sphereRad;

//shape selection variable
unsigned char shapeSelected;

//scale multiplier (set to 1 when not in use)
GLfloat sMulC;//for cube
GLfloat sMulS;//for sphere
GLfloat sMulT;//for teapot


/* GLUT callback Handlers */

//Resises the window
static void resize(int width, int height)
{
    double Ratio;

    //projection settings
    //specifies the affine transformation of x and y from 
    //normalized device coordinates to window coordinates 
    if (width <= height)
        glViewport(0, (GLsizei)(height - width) / 2, (GLsizei)width, (GLsizei)width);
    else
        glViewport((GLsizei)(width - height) / 2, 0, (GLsizei)height, (GLsizei)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0f, 1, 0.1f, 100.0f);//(focal point, ratio, near, far)
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //creates viewing matrix derived from an eye point, 
    //a reference point indicating the center of the scene, and an UP vector.
    //(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ)
    gluLookAt(0, 5, 10, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    if (WireFrame)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//Draw Our Mesh In Wireframe Mesh
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);		//Toggle WIRE FRAME

    //create push/pop matrix for each shape
    //Note glRotatef(cubeAngle, cubeX, cubeY, cubeZ);//rotates and stays in place this way
    //SPHERE
    glPushMatrix();                 
    glColor3f(0, 0, 1);                     //sets color of points to blue
    //glRotatef(sphereAngle, 0, 1, 0);      //rotates around y axis per instructions 
    //glRotatef(sphereAngle, sphereX, sphereY, sphereZ);//rotates and stays in place this way
    glScalef(sMulS, sMulS, sMulS);          //zoom in/out depending on user input
    //set position of object, must divide by scale factor or shape moves during scaling
    glTranslatef(sphereX / sMulS, sphereY / sMulS, sphereZ / sMulS);
    glRotatef(sphereAngle, 0, 1, 0);      //rotates around y axis per instructions 
    glutSolidSphere(sphereRad, 10, 10);   //(radius, slices, stacks)
    glPopMatrix();

    //Cube
    glPushMatrix();
    glColor3f(1, 0, 0);                 //sets color of points to green
    glScalef(sMulC, sMulC, sMulC);      //zoom in/out depending on user input
    //set position of object, must divide by scale factor or shape moves during scaling
    glTranslatef(cubeX / sMulC, cubeY / sMulC, cubeZ / sMulC);  //set position of object
    glRotatef(cubeAngle, 0, 1, 0);      //rotates around y axis per instructions 
    glutSolidCube(cubeSize);            //(size is 2)
    glPopMatrix();
    
    //TEAPOT
    glPushMatrix();
    glColor3f(0, 1, 0);                     //sets color of points to red
    glScalef(sMulT, sMulT, sMulT);          //zoom in/out depending on user input
    glTranslatef(teapotX, teapotY, teapotZ);//set position of object
    glRotatef(teapotAngle, 0, 1, 0);      //rotates around y axis per instructions
    glutSolidTeapot(teapotSize);            //(create Glut object and set to certain size)
    glPopMatrix();

    glutSwapBuffers();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    //press ESC or 'q' to terminate program
    case 27:    
    case 'q':
        exit(0);
        break;

    //User selects cube for alterations
    case 'c':
        shapeSelected = 'c';
        break;

    //User selects sphere for alterations
    case 's':
        shapeSelected = 's';
        break;

    //User selects teapot for alterations
    case 't':
        shapeSelected = 't';
        break;
    }
}

void Specialkeys(int key, int x, int y)
{
    switch (key)
    {

    //user presses key up and is able to preform a zoom in
    case GLUT_KEY_UP:
        cout << "UP\n";
        if (shapeSelected == 's')
            sMulS += 0.5; //increments by .5 in scaling function (above 1.0 stretches)
        if (shapeSelected == 't')
            sMulT += 0.5; //increments by .5 in scaling function (above 1.0 stretches)
        if (shapeSelected == 'c')
            sMulC += 0.5; //increments by .5 in scaling function (above 1.0 stretches)
        break;

    //user presses key down and is able to preform a zoom out
    case GLUT_KEY_DOWN:
        cout << "DOWN\n";
        if (shapeSelected == 's')
            sMulS -= 0.5; //decrements by .5 in scaling function (below 1.0 shrinks)
        if (shapeSelected == 't')
            sMulT -= 0.5; //decrements by .5 in scaling function (below 1.0 shrinks)
        if (shapeSelected == 'c')
            sMulC -= 0.5; //decrements by .5 in scaling function (below 1.0 shrinks)
        break;

    //user presses key left and is able to preform a left rotation around the y axis
    case GLUT_KEY_LEFT:
        cout << "LEFT\n";
        if (shapeSelected == 's')
            sphereAngle += 5; //set the angle to be rotated to 5 degrees
        if (shapeSelected == 't')
            teapotAngle += 5; //set the angle to be rotated to 5 degrees
        if (shapeSelected == 'c')
            cubeAngle += 5; //set the angle to be rotated to 5 degrees
        break;

    //user presses key right and is able to preform a right rotation around the y axis
    case GLUT_KEY_RIGHT:
        cout << "RIGHT\n";
        if (shapeSelected == 's')
            sphereAngle -= 5; //set the angle to be rotated to 5 degrees
        if (shapeSelected == 't')
            teapotAngle -= 5; //set the angle to be rotated to 5 degrees
        if (shapeSelected == 'c')
            cubeAngle -= 5; //set the angle to be rotated to 5 degrees
        break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

static void init(void)
{
    //initialize all variables with starting values 
    //Sphere position for glTranslatef function
    sphereX = -3.5, sphereY = 0.0, sphereZ = -1.0;

    //Cube position for glTranslatef function
    cubeX = 4.0, cubeY = 0.0, cubeZ = -2.0;

    //Teapot position for glTranslatef function
    teapotX = 0.0, teapotY = 0.0, teapotZ = -3.0;

    //rotation degree variables for glRotatef
    sphereAngle = 0, teapotAngle = 0, cubeAngle = 0;

    //set initial size of teapot and cube for glTranslatef function
    teapotSize = 1.5, cubeSize = 2;

    //set radius of sphere for glTranslatef function
    sphereRad = 1.3;

    //shape selection variable
    shapeSelected = 'n';

    //scale multiplier (set to 1 when not in use)
    sMulC = 1;  //for cube
    sMulS = 1;  //for sphere
    sMulT = 1;  //for teapot


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
}


/* Program entry point */

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

    glutInitWindowSize(800, 600);
    glutInitWindowPosition(10, 10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("GLUT Shapes");
    init();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(Specialkeys);

    glutIdleFunc(idle);
    glutMainLoop();

    return EXIT_SUCCESS;
}