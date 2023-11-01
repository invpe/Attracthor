// g++ viewer.cpp -lglut -lGL
// invpe Attracthor viewer https://github.com/invpe/Attracthor/
#include <vector>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 

#define WIDTH 640
#define HEIGHT 480
 
double rX=0; 
double rY=0;
double rZ = 0;
struct tPosition
{
    double x;
    double y;
    double z;
};
int iSimulationCounter = 0;
int iSimulationFrame = 0;
std::vector<double> vValues;
std::vector<tPosition> vPositions; 
 
void renderText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);  
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);   
    }
}
void reshape(int width, int height) 
{ 
    glViewport(0, 0, width,height); 
}

void renderFunction ()
{ 
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();      
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); 
    glColor3f(1.0, 1.0, 1.0); 
    renderText(-1,0.95,"[s]imulation: "+std::to_string(iSimulationCounter)+"/"+std::to_string(iSimulationFrame)+" points: "+std::to_string(vPositions.size()));
    gluPerspective(45.0, (float)WIDTH / (float)HEIGHT, 0.001, 100.0);
    gluLookAt(0,0,1, 0,0,0, 0,1,0);  
    

     
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
    glTranslatef(0.0f, 0.0f,  rZ);
    glRotatef( rX, 1.0, 0.0, 0.0 );
    glRotatef( rY, 0.0, 1.0, 0.0 ); 

    //  
    glBegin(GL_POINTS); 
    glColor4f(1,1,1,0.5);
 
    for(int x = 0; x < iSimulationFrame;  x++)
        glVertex3f(vPositions[x].x, vPositions[x].y, vPositions[x].z);
               
    glEnd();
    glBegin(GL_POINTS);
    
    for(int x=0; x < 10; x++)
    {
        glColor4f(1,0,0,1);
        glVertex3f(-5+x,0,0);

    }
    glEnd();
    
    glBegin(GL_POINTS);
    for(int x=0; x < 10; x++)
    {
        glColor4f(0,1,0,1);
        glVertex3f(0,-5+x,0);

    }
    glEnd();
 
    
    glutSwapBuffers();  
} 
void timer(int value) { 


    if(iSimulationCounter == 1)
    {
        iSimulationFrame++;
        if(iSimulationFrame>vPositions.size())
        iSimulationFrame=0; 
    }
  
    glutPostRedisplay(); 
    glutTimerFunc(16, timer, 0);
}
void mouse(int button, int state, int x, int y) {
	if(state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) printf("LMB\n");
       glutPostRedisplay();
}
void keyboard (int key, int x, int y)
{
	int modifiers = glutGetModifiers();
if (key == GLUT_KEY_RIGHT)
        {
                rY += 0.1;
        }
    else if (key == GLUT_KEY_LEFT)
        {
                rY -= 0.1;
        }
    else if (key == GLUT_KEY_DOWN)
        {
                rX -= 0.1;
        }
    else if (key == GLUT_KEY_UP)
        {
                rX += 0.1;
        }
    else if (key == GLUT_KEY_PAGE_UP)
        {
                rZ += 0.1;
        }
    else if (key == GLUT_KEY_PAGE_DOWN)
        {
                rZ -= 0.1;
        } 
}
 void keyboard_normal (unsigned char key, int x, int y)
{
        if (key == 's')
        {
            if(iSimulationCounter == 0)
                iSimulationCounter = 1,iSimulationFrame = 0;
            else iSimulationCounter = 0,iSimulationFrame = vValues.size();

            printf("Simulation: %i\n", iSimulationCounter); 
        }
}
int main(int argc, char *argv[])
{ 
    std::ifstream file("input.txt");  
    if (!file.is_open()) {
        std::cout << "Failed to open the file." << std::endl;
        return 1;
    }

    std::string line;

    double iMaxValue = -9999;
    double iMinValue = 9999;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double number;
        
        while (iss >> number) {

        if(number < iMinValue)iMinValue = number;
        if(number > iMaxValue)iMaxValue = number;

        vValues.push_back(number);
                    
        }
    }

    file.close();

    printf("Min: %i\n", iMinValue);
    printf("Max: %i\n", iMaxValue);
 
    // Calculate delayed coordinates 
    for(int a = 1; a < vValues.size(); a++)
    {  

        tPosition _Position;
        _Position.x = vValues[a] - vValues[a-1];
        _Position.y = vValues[a-1] - vValues[a-2];
        _Position.z = vValues[a-2] - vValues[a-3];

        // Normalize to <0,1>
//        normalized_value = (x - a) / (b - a)
        _Position.x = (_Position.x-iMinValue)/(iMaxValue-iMinValue);
        _Position.y = (_Position.y-iMinValue)/(iMaxValue-iMinValue);
        _Position.z = (_Position.z-iMinValue)/(iMaxValue-iMinValue);
        vPositions.push_back(_Position);  
    }
    printf("Done loading\n");

    glutInit (&argc, argv);    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); 
    glutInitWindowSize (WIDTH,HEIGHT); 
    glutInitWindowPosition (100,100); 
    glutCreateWindow ("invpe Attractor Viewer");


    // Enable Z-buffer depth test
    //glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPointSize(1.0f);  
    glutDisplayFunc (renderFunction);
    glutReshapeFunc(reshape);
    glutSpecialFunc(keyboard);
    glutKeyboardFunc (keyboard_normal);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0);
    glutMainLoop ();

    return 0;
}  
