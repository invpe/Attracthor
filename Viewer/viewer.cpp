// g++ viewer.cpp -lglut -lGL
// invpe Attracthor viewer https://github.com/invpe/Attracthor/
#include <vector>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream> 

#define WIDTH 640
#define HEIGHT 480
bool bBreak = false;
bool bAnimate = false;
double rX=0; 
double rY=0;
double rZ = -6.0;
struct tPosition
{
    double x;
    double y;
    double z;
};  
std::string strFilename;
int iSimulationSwitch = 0;
int iSimulationFrame = 0;
std::vector<double> vValues;
std::vector<tPosition> vPositions; 
void captureScreenshot(const char* filename) {
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport); // Get the current viewport dimensions

    int width = viewport[2]; // Get the width of the viewport
    int height = viewport[3]; // Get the height of the viewport

    unsigned char* pixels = new unsigned char[width * height * 3];

    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    std::ofstream screenshotFile(filename, std::ios::out | std::ios::binary);

    if (!screenshotFile.is_open()) {
        std::cerr << "Failed to open the file for writing." << std::endl;
        delete[] pixels;
        return;
    }

    screenshotFile << "P6\n" << width << " " << height << "\n255\n";

    // Write the pixel data in reverse order to account for the upside-down orientation
    for (int row = height - 1; row >= 0; row--) {
        for (int col = 0; col < width; col++) {
            int index = (row * width + col) * 3;
            screenshotFile.put(pixels[index + 0]);
            screenshotFile.put(pixels[index + 1]);
            screenshotFile.put(pixels[index + 2]);
        }
    }

    screenshotFile.close();
    delete[] pixels;
    std::cout << "Screenshot saved to " << filename << std::endl;
}
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
    renderText(-1,0.95,"[s]imulation: "+std::to_string(iSimulationSwitch)+" points: "+std::to_string(vPositions.size())+" rx: "+std::to_string(rX)+" ry: "+std::to_string(rY)+" rz: "+std::to_string(rZ));
    gluPerspective(45.0, (float)WIDTH / (float)HEIGHT, 0.001, 100.0);
    gluLookAt(0,0,1, 0,0,0, 0,1,0);  
    
 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
    glTranslatef(0.0f, 0.0f,  rZ);
    glRotatef( rX, 1.0, 0.0, 0.0 );
    glRotatef( rY, 0.0, 1.0, 0.0 ); 

    //  
    for(int x = 0; x < iSimulationFrame;  x++)
    {
        if(x==iSimulationFrame-1)   
            glColor4f(1,1,1,1);    
        else 
            glColor4f(1,1,1,0.25); 


        glBegin(GL_POINTS); 
        glVertex3f(vPositions[x].x, vPositions[x].y, vPositions[x].z); 
        glEnd();
    }

 
    /* AXIS
    glPointSize(2.0f);  
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
 */
    
    glutSwapBuffers();  
} 
void Tick()
{ 
    if(iSimulationSwitch == 1)
    {
        iSimulationFrame++;
        if(iSimulationFrame>vPositions.size())
        iSimulationFrame=0; 
    }

    if(bAnimate)
    {
        rX+=0.1;
        rY+=0.1; 
    }
  
}
void timer(int value) { 
    Tick();
    glutPostRedisplay(); 
    glutTimerFunc(16, timer, 0);
}
void mouse(int button, int state, int x, int y) {
	if(state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) printf("LMB\n"); 
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
                rZ += 0.05;
        }
    else if (key == GLUT_KEY_PAGE_DOWN)
        {
                rZ -= 0.05;
        } 
}
void Capture()
{

            std::string strSS = strFilename+".ppm";
        captureScreenshot(strSS.c_str());
}
 void keyboard_normal (unsigned char key, int x, int y)
{
        if(key=='a')
        {
            bAnimate = !bAnimate;
        }
        if (key == 's')
        {
            if(iSimulationSwitch == 0)
                iSimulationSwitch = 1,iSimulationFrame = 0;
            else iSimulationSwitch = 0,iSimulationFrame = vValues.size();

            printf("Simulation: %i\n", iSimulationSwitch); 
        }
         if (key == 'z'  ) {Capture();}
}
 
int main(int argc, char *argv[])
{ 
    if(argc<2)
    {
        printf("./viewer <filename>\n");
        exit(0);
    }

    if(argc==3)
        bBreak=true;

    // Add automate option to render,take screenshot and exit


    strFilename=argv[1];
    std::ifstream file(strFilename);  
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
        _Position.x = (_Position.x-iMinValue)/(iMaxValue-iMinValue);
        _Position.y = (_Position.y-iMinValue)/(iMaxValue-iMinValue);
        _Position.z = (_Position.z-iMinValue)/(iMaxValue-iMinValue);
        vPositions.push_back(_Position);  
    }
 
    iSimulationFrame =vPositions.size()-1;
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
    glPointSize(2.0f);   
    glLineWidth(2.0);
    glutDisplayFunc (renderFunction);
    glutReshapeFunc(reshape);
    glutSpecialFunc(keyboard);
    glutKeyboardFunc (keyboard_normal);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0); 
    glutMainLoop ();

    return 0;
}  
