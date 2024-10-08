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
#include <math.h>
#define WIDTH 640
#define HEIGHT 480
#define GRID_SIZE 10  // Defines a 10x10 grid for x and y axes

bool bAnimate = false;
bool bAxis = false;

int iSimulationSwitch = 0;
int iSimulationFrame = 0;
int iCurrentFile = 0;

double rX=0; 
double rY=0;
double rZ = -3.0;
double dEntropy = 0.0;

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

// Global bins for 2D projection (x, y)
int bins[GRID_SIZE][GRID_SIZE] = {0}; 


double fast_log2(double x) {
    int exp;
    double frac = frexp(x, &exp); // Separate the exponent and fraction
    return exp + (frac - 1.0); // Approximation
}

void calculate_2D_bin(struct tPosition pos) {
    // Scale pos.x and pos.y from [-1, 1] to [0, GRID_SIZE - 1] range for binning
    double scale_factor = (double)(GRID_SIZE / 2);
    double grid_x = pos.x * scale_factor;  // Scale from [-1, 1] to [-GRID_SIZE/2, GRID_SIZE/2]
    double grid_y = pos.y * scale_factor;  // Scale from [-1, 1] to [-GRID_SIZE/2, GRID_SIZE/2]

    // Calculate bin index (scale to the range [0, GRID_SIZE-1] from [-GRID_SIZE/2, GRID_SIZE/2])
    int x_bin = (int)((grid_x + (GRID_SIZE / 2.0)));  // Offset by half of GRID_SIZE to shift into [0, GRID_SIZE-1]
    int y_bin = (int)((-grid_y + (GRID_SIZE / 2.0)));  // Invert Y-axis for top-down grid orientation

    // Ensure bin indices are within valid range [0, GRID_SIZE-1]
    if (x_bin >= GRID_SIZE) x_bin = GRID_SIZE - 1;
    if (y_bin >= GRID_SIZE) y_bin = GRID_SIZE - 1;
    if (x_bin < 0) x_bin = 0;
    if (y_bin < 0) y_bin = 0;

    // Increment the count in the corresponding 2D bin
    bins[x_bin][y_bin]++;
}
void CalculateEntropy()
{   
    dEntropy = 0;
    for(int x = 0; x < vPositions.size(); x++)
    {
        calculate_2D_bin(vPositions[x]);
    }

   // Print out the 2D bin counts to the output file with corresponding [-5,5] labeling
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int x_label = i - (GRID_SIZE / 2);  // Convert bin index to the range [-5, 5]
            int y_label = j - (GRID_SIZE / 2);  // Convert bin index to the range [-5, 5]
         
            // Calculate entropy 
            if (bins[i][j] > 0) {
                double p = (double)bins[i][j] / vPositions.size();
                dEntropy -= p * fast_log2(p);
            }            
        }
    } 
}
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

       std::sort(vFiles.begin(), vFiles.end());

    for (const std::string& filename : vFiles) {
        std::cout << "Found: "<< filename << std::endl;
    }
}

// Load DSC completed by tessie_chaos task
bool LoadDSCFile(const std::string& rstrFileName)
{
    // Zero out BINS
    for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
        bins[i][j] = 0;
    }
}

      vValues.clear();
    vPositions.clear();
 
    std::cout << "Loading " << rstrFileName << std::endl;

    std::ifstream file(rstrFileName);
    if (!file.is_open()) {
        std::cout << "Failed to open the file." << std::endl;
        return false;
    }

    std::string line;

    // Variables to store min/max values
    float iMaxValue = std::numeric_limits<float>::min();
    float iMinValue = std::numeric_limits<float>::max();

    // Read the file line by line
    while (std::getline(file, line)) {
        
        if (line.find("Bin") != std::string::npos) {
            continue;  // Skip this line and move to the next
        }
        if (line.find("#") != std::string::npos) {
            continue;  // Skip this line and move to the next
        }

        std::istringstream iss(line);
        tPosition pos;

        // Parse the x, y, z values from the format: x=..., y=..., z=...
        std::string temp;
        
        // Extract the 'x=' part
        std::getline(iss, temp, '=');
        std::getline(iss, temp, ',');  // Read up to the comma
        pos.x = std::stof(temp);       // Convert the extracted string to float

        // Extract the 'y=' part
        std::getline(iss, temp, '=');
        std::getline(iss, temp, ',');
        pos.y = std::stof(temp);

        // Extract the 'z=' part
        std::getline(iss, temp, '=');
        std::getline(iss, temp);
        pos.z = std::stof(temp);

        // Update min/max values
        if (pos.x < iMinValue) iMinValue = pos.x;
        if (pos.x > iMaxValue) iMaxValue = pos.x;

        if (pos.y < iMinValue) iMinValue = pos.y;
        if (pos.y > iMaxValue) iMaxValue = pos.y;

        if (pos.z < iMinValue) iMinValue = pos.z;
        if (pos.z > iMaxValue) iMaxValue = pos.z; 
         
        vPositions.push_back(pos);
    }

    file.close();
    std::cout << "Min: " << iMinValue << std::endl;
    std::cout << "Max: " << iMaxValue << std::endl;

 
    iSimulationFrame = vPositions.size() - 1;
    std::cout << "File loaded, total positions: " << vPositions.size() << std::endl;

    return true;
}
bool LoadFile(const std::string& rstrFileName)
{   
    // Zero out BINS
    for (int i = 0; i < GRID_SIZE; i++) {
    for (int j = 0; j < GRID_SIZE; j++) {
        bins[i][j] = 0;
    }
}

    vValues.clear();
    vPositions.clear();
    std::cout << "Loading " << rstrFileName<<std::endl;

    std::ifstream file(rstrFileName);  
    if (!file.is_open()) {
        std::cout << "Failed to open the file." << std::endl;
        return 1;
    }

    std::string line;

    float iMaxValue = std::numeric_limits<float>::min();
    float iMinValue = std::numeric_limits<float>::max();

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        float number;
        
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
    std::string strFile = "";
    if(!vFiles.empty()) strFile = vFiles[iCurrentFile];
    renderText(-1,0.95,"[s]im: "+std::to_string(iSimulationSwitch)+"/"+std::to_string(iSimulationFrame)+" points: "+std::to_string(vPositions.size())+" rx: "+std::to_string(rX)+" ry: "+std::to_string(rY)+" rz: "+std::to_string(rZ)+" "+strFile+" Entropy: "+std::to_string(dEntropy));
    gluPerspective(45.0, (float)WIDTH / (float)HEIGHT, 0.001, 100.0);
    gluLookAt(0,0,1, 0,0,0, 0,1,0);  
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 
    glTranslatef(0.0f, 0.0f,  rZ);
    glRotatef( rX, 1.0, 0.0, 0.0 );
    glRotatef( rY, 0.0, 1.0, 0.0 );  

    // Render points (DSC positions)
    glPointSize(1.0f); 
    glBegin(GL_POINTS); 
    for(int x = 0; x < iSimulationFrame;  x++)
    {
        if(x==iSimulationFrame-1)   
            glColor4f(1,1,1,1);    
        else 
            glColor4f(1,1,1,0.25); 

        // Flatten out (to match bins)
        if(bAxis)
            glVertex3f(vPositions[x].x, vPositions[x].y, 0); 
        else
            glVertex3f(vPositions[x].x, vPositions[x].y, vPositions[x].z); 
    }
    glEnd();

    // Render axis and boundaries and bins
    if (bAxis) 
    {
        // X, Y, Z axes
        glBegin(GL_LINES);

        // X-axis (red)
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(1.0f, 0.0f, 0.0f);

        // Y-axis (green)
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0.0f, -1.0f, 0.0f);
        glVertex3f(0.0f, 1.0f, 0.0f);

        // Z-axis (blue)
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(0.0f, 0.0f, -1.0f);
        glVertex3f(0.0f, 0.0f, 1.0f);
        glEnd();

        // Render grid with lines to indicate bin divisions for a 10x10 grid
        glColor3f(0.5f, 0.5f, 0.5f);  // Light grey grid
        glBegin(GL_LINES);

        // Horizontal grid lines (parallel to X-axis)
        for (int i = -5; i <= 5; i++) {
            float pos = i / 5.0f;  // Scale to [-1, 1]
            glVertex3f(-1.0f, pos, 0.0f);
            glVertex3f(1.0f, pos, 0.0f);
        }

        // Vertical grid lines (parallel to Y-axis)
        for (int i = -5; i <= 5; i++) {
            float pos = i / 5.0f;  // Scale to [-1, 1]
            glVertex3f(pos, -1.0f, 0.0f);
            glVertex3f(pos, 1.0f, 0.0f);
        }

        glEnd();

        // Optionally, render key points (0,0), (1,1), (-1,-1), etc.
        glPointSize(8.0f);  // Larger points for markers
        glBegin(GL_POINTS);
        glColor3f(1.0f, 0.0f, 0.0f);  // Red marker for (0, 0)
        glVertex3f(0.0f, 0.0f, 0.0f);  // Mark (0, 0)

        glColor3f(0.0f, 1.0f, 0.0f);  // Green marker for (-1, -1)
        glVertex3f(-1.0f, -1.0f, 0.0f);  // Mark (-1, -1)

        glColor3f(0.0f, 0.0f, 1.0f);  // Blue marker for (1, 1)
        glVertex3f(1.0f, 1.0f, 0.0f);  // Mark (1, 1)
        glEnd();
    }


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
                rY += 1.1;
        }
    else if (key == GLUT_KEY_LEFT)
        {
                rY -= 1.1;
        }
    else if (key == GLUT_KEY_DOWN)
        {
                rX -= 1.1;
        }
    else if (key == GLUT_KEY_UP)
        {
                rX += 1.1;
        }
    else if (key == GLUT_KEY_PAGE_UP)
        {
                rZ += 0.01;
        }
    else if (key == GLUT_KEY_PAGE_DOWN)
        {
                rZ -= 0.01;
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

         if(key=='q'){iCurrentFile--;if(iCurrentFile<0)iCurrentFile=vFiles.size()-1;LoadFile(vFiles[iCurrentFile]);CalculateEntropy();}
         if(key=='w'){iCurrentFile++;if(iCurrentFile>vFiles.size()-1)iCurrentFile=0;LoadFile(vFiles[iCurrentFile]);CalculateEntropy();}
         if(key=='e'){iSimulationFrame-=100; if(iSimulationFrame<0)iSimulationFrame=vPositions.size()-1;}
         if(key=='r'){iSimulationFrame+=100;if(iSimulationFrame>vPositions.size()-1)iSimulationFrame=0;}
         if(key=='l'){bAxis=!bAxis;}


}
int main(int argc, char *argv[])
{ 
    if(argc==2)
    { 
        std::cout << "Loading " << argv[1] << std::endl;
        LoadDSCFile(argv[1]);
    }
    else
    {
        LoadFiles();
        if(vFiles.empty())
        {
            std::cout << "Cant find dataset(s) *.txt files"<<std::endl;
            exit(0);
        }
            //
        LoadFile(vFiles[iCurrentFile]);

    }

     
   

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
    glLineWidth(1.0);
    glutDisplayFunc (renderFunction);
    glutReshapeFunc(reshape);
    glutSpecialFunc(keyboard);
    glutKeyboardFunc (keyboard_normal);
    glutMouseFunc(mouse);
    glutTimerFunc(0, timer, 0); 
    glutMainLoop ();

    return 0;
}  
