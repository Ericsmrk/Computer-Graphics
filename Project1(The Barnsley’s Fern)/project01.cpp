/* Eric Smrkovsky--CSci 172--Program1--Fall2021
 The Barnsley’s fern is a fractal created by an iterated function system,
 in which a point (the seed) is repeatedly transformed by using 
 one of four transformation functions.
 */
#include <string.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

float a, b, c, d, e, f; //constants for transformations
float xUpdated, yUpdated;//x,y coodinate after transformation
float x, y;             //previous values
int num;                //conditions?
int n;                  //number of iterations    

/********* GLUT callback Handlers **********/
//Resizes the window 
void resize(int width, int height){
    //specifies the affine transformation of x and y from 
    //normalized device coordinates to window coordinates 
    //NOTE: this is different then affine transformations done for the fractal
    if (width <= height)
        glViewport(0, (GLsizei)(height - width) /
            2, (GLsizei)width, (GLsizei)width);
    else
        glViewport((GLsizei)(width - height) / 
            2, 0, (GLsizei)height, (GLsizei)height);
} 

//Initializes the variables needed to draw the fractal  
void init(){
    glMatrixMode(GL_PROJECTION);    //select the matrix to be manipulated
    glLoadIdentity();               //load identity matrix
    glOrtho(-2.5, 2.75, 10.5, 0, -1.0, 1.0); // adjusted for suitable viewport

    //initialize starting values for variables 
    x = 0.5;        
    y = 0.5;
    n = 200000;
}

//Display loop to draw the fractal
void display(void){
    glClear(GL_COLOR_BUFFER_BIT);        // clear display screen
    glColor3f(0, 1, 0);         //sets color of points to green

    //loop to color the screen
    for (int i = 0; i < n; i++) {
        num = rand() % 100;     //find a random value 

        //select the correct inputs for the transformation function...
        //7 % of the times choose coordinate transformation with ƒ3
        if (num < 7) {   
            a = 0.2, b = 0.23, c = -0.26, d = 0.22, e = 0, f = 1.6;
        }
        
        //7 % of the times choose coordinate transformation with ƒ4
        else if (num < 15) {
            a = -0.15, b = 0.26, c = 0.28, d = 0.24, e = 0, f = 0.44;
        }

        //85 % of the times choose coordinate transformation with ƒ2
        else if (num < 99) {
            a = 0.85, b = -0.04, c = 0.04, d = 0.85, e = 0, f = 1.6;
        }
        
        //1 % of the times choose coordinate transformation with ƒ1
        else {              
            a = 0, b = 0, c = 0, d = 0.16, e = 0, f = 0;
        }  

        //update x and y with transformation function
        xUpdated = (a * x) + (c * y) + e;//affine transformation of x coordinat
        yUpdated = (b * x) + (d * y) + f;//affine transformation of y coordinat

        //draw the pixel
        glBegin(GL_POINTS);         //treat each vertex as a single point
        glVertex3f(xUpdated, yUpdated, 0);//map pixel for each point
        glEnd();                    //end gl           

        //update variables for next iteration of the loop
        x = xUpdated;               
        y = yUpdated;
    }

    glFlush();                           // clear buffer
}

//Quit program or redisplay fractal depending on user input  
void key(unsigned char key, int x, int y){
    
    //quit program if esc or 'q' is user input
    switch (key)
    {
    case 27:                       // esc key to exit
    case 'q':
        exit(0);
        break;
    }

    //redisplay the fractal with new values if switch statement is bypassed
    glutPostRedisplay();
}


/******** Program entry point ********/

int main(int argc, char* argv[]){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 800);                //window screen (len,wid)
    glutInitWindowPosition(500, 200);            //window position
    glutCreateWindow("Program1");                //program title
    init();                                   //initialize variables and matrix
    glutDisplayFunc(display);                 //callback function for display
    glutReshapeFunc(resize);                  //callback for reshape
    glutKeyboardFunc(key);                    //callback function for keyboard
    glutMainLoop();                               //loop

    return EXIT_SUCCESS;
}