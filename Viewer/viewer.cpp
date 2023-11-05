// g++ viewer.cpp -lglut -lGL
// invpe Attracthor viewer https://github.com/invpe/Attracthor/
#include <vector>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <limits>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream> 
#include <filesystem>
#define WIDTH 640
#define HEIGHT 480
bool bAnimate = false;
double rX=0; 
double rY=0;
double rZ = -3.0;
struct tPosition
{
    double x;
    double y;
    double z;
};  
std::vector<double> vValues;
std::vector<tPosition> vPositions; 
std::vector<std::string> vFiles;
std::string strFilename;
int iSimulationSwitch = 0;
int iSimulationFrame = 0;
int iCurrentFile = 0;

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
void Capture()
{
    std::string strSS = vFiles[iCurrentFile]+".ppm";
    captureScreenshot(strSS.c_str());

    std::cout << "Captured to "<<strSS<<std::endl;
}
void LoadFiles()
{ 
    std::filesystem::path directoryPath = "./";
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            vFiles.push_back(entry.path().filename().string());
        }
    }
    for (const std::string& filename : vFiles) {
        std::cout << "Found: "<< filename << std::endl;
    }
}
bool LoadFile(const std::string& rstrFileName)
{   
    vValues.clear();
    vPositions.clear();
    std::cout << "Loading " << rstrFileName<<std::endl;

    std::ifstream file(rstrFileName);  
    if (!file.is_open()) {
        std::cout << "Failed to open the file." << std::endl;
        return 1;
    }

    std::string line;

    int iMaxValue = std::numeric_limits<int>::min();
    int iMinValue = std::numeric_limits<int>::max();

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int number;
        
        while (iss >> number) {

        if(number < iMinValue)iMinValue = number;
        if(number > iMaxValue)iMaxValue = number;

        vValues.push_back(number);
                    
        }
    }

    file.close();
    std::cout << "Min "<<iMinValue<<std::endl;
    std::cout << "Max "<<iMaxValue<<std::endl; 
 
    // Calculate delayed coordinates and normalize to the range [0, 1]
    for (int a = 3; a < vValues.size(); a++) {  // Start from a=3 to ensure you have enough history
        tPosition _Position;
        
        // Calculate delayed space coordinates (DSC)
        _Position.x = vValues[a] - vValues[a - 1];
        _Position.y = vValues[a - 1] - vValues[a - 2];
        _Position.z = vValues[a - 2] - vValues[a - 3];
        
        // Normalize to the range [0, 1]
        _Position.x = (_Position.x - iMinValue) / (iMaxValue - iMinValue);
        _Position.y = (_Position.y - iMinValue) / (iMaxValue - iMinValue);
        _Position.z = (_Position.z - iMinValue) / (iMaxValue - iMinValue);
        
        vPositions.push_back(_Position);
    }
    iSimulationFrame =vPositions.size()-1;

    std::cout << "File loaded "<<std::endl;    
    return 0;
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
    renderText(-1,0.95,"[s]im: "+std::to_string(iSimulationSwitch)+"/"+std::to_string(iSimulationFrame)+" points: "+std::to_string(vPositions.size())+" rx: "+std::to_string(rX)+" ry: "+std::to_string(rY)+" rz: "+std::to_string(rZ)+" "+vFiles[iCurrentFile]);
    gluPerspective(45.0, (float)WIDTH / (float)HEIGHT, 0.001, 100.0);
    gluLookAt(0,0,1, 0,0,0, 0,1,0);  
    
 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
    glTranslatef(0.0f, 0.0f,  rZ);
    glRotatef( rX, 1.0, 0.0, 0.0 );
    glRotatef( rY, 0.0, 1.0, 0.0 ); 

    //  

    glPointSize(2.0f); 
    glBegin(GL_POINTS); 
    for(int x = 0; x < iSimulationFrame;  x++)
    {
        if(x==iSimulationFrame-1)   
            glColor4f(1,1,1,1);    
        else 
            glColor4f(1,1,1,0.25); 
        glVertex3f(vPositions[x].x, vPositions[x].y, vPositions[x].z); 
    }

    glEnd();
    // This is rendering the space boundary <-1,1>
    /*
    glPointSize(6.0f);  
    glBegin(GL_POINTS); 

        glVertex3f(-1,-1,-1);
        glVertex3f(-1,1,-1);

        glVertex3f(1,-1,-1);
        glVertex3f(1,1,-1);


        glVertex3f(-1,-1,1);
        glVertex3f(-1,1,1);

        glVertex3f(1,-1,1);
        glVertex3f(1,1,1);
        
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
	//if(state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) printf("LMB\n"); 
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
        }
         if (key == 'z'  ) {Capture();}

         if(key=='q'){iCurrentFile--;if(iCurrentFile<0)iCurrentFile=vFiles.size()-1;LoadFile(vFiles[iCurrentFile]);}
         if(key=='w'){iCurrentFile++;if(iCurrentFile>vFiles.size()-1)iCurrentFile=0;LoadFile(vFiles[iCurrentFile]);}
}
int main(int argc, char *argv[])
{ 

    LoadFiles();
    if(vFiles.empty())
    {
        std::cout << "Cant find dataset(s) *.txt files"<<std::endl;
        exit(0);
    }

    //
    LoadFile(vFiles[iCurrentFile]);


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
