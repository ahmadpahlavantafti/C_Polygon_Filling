// ***************************************
// CS459-Fundamentals of Computer Graphics
// Project 1: Interactive Polygon-Filling
// Student  : Ahmad Pahlavan Tafti
// ***************************************


#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>


class scrPt {     // Declaring the points for polygon
   public:
      GLint x, y;
};

#define width  450    // Window Size
#define height 450    // Window Size
#define PI 3.14159    // PI Number
#define TWOPI PI*2    // 2PI

//n=number of poly
#define n 4   
scrPt poly[n];
float currentBuffer[width][height][3];

void init (void)
{
    glClearColor (1.0, 1.0, 1.0, 0.0);  // Set display-window color to white.
    glMatrixMode (GL_PROJECTION);       // Set projection parameters.
    gluOrtho2D (0.0, 200.0, 0.0, 150.0);
}


//calculate angle of two point to determine is inside polygon or not:
/* Philippe Reverdy presented a solution to compute the sum 
of the angles made between the test point and each pair of points making 
up the polygon. If this sum is 2pi then the point is an interior point, 
if 0 then the point is an exterior point. This also works for polygons 
with holes given the polygon is defined with a path made up of coincident 
edges into and out of the hole as is common practice in many CAD packages.
Reference: www.eecs.umich.edu/eecs/courses/eecs380/HANDOUTS/PROJ2/InsidePoly.html */

double Angle2D(double x1, double y1, double x2, double y2)
{
   double dtheta,theta1,theta2;

   theta1 = atan2(y1,x1);
   theta2 = atan2(y2,x2);
   dtheta = theta2 - theta1;
   while (dtheta > PI)
      dtheta -= TWOPI;
   while (dtheta < -PI)
      dtheta += TWOPI;
   return(dtheta);
}



//mask1=R(red) of mask table
float mask1[3][3]={
	{1,0,1},
	{1,0,1},
	{1,1,1}};

//mask1=G(green) of mask table
float mask2[3][3]={
	{0,1,1},
	{1,0,1},
	{1,1,1}};

//mask1=B(blue) of mask table
float mask3[3][3]={
	{0,0,1},
	{0,1,1},
	{1,1,1}};

//get the color of pixel according to the mask table
void getColor(int x, int y, float drawColor[3])
{
	drawColor[0]=mask1[x/30-(3*(x/90))][y/30-(3*(y/90))];
	drawColor[1]=mask2[x/30-(3*(x/90))][y/30-(3*(y/90))];
	drawColor[2]=mask3[x/30-(3*(x/90))][y/30-(3*(y/90))];
}

//check a point by Angle2d function whether is in polygon or not. *polygon is Vertices of polygon
int floodFill4 (scrPt *polygon,scrPt p)
{
	 int i;
   double angle=0;

   scrPt p1,p2;
   for (i=0;i<n;i++) {
      p1.x = polygon[i].x - p.x;
      p1.y = polygon[i].y - p.y;
      p2.x = polygon[(i+1)%n].x - p.x;
      p2.y = polygon[(i+1)%n].y - p.y;
      angle += Angle2D(p1.x,p1.y,p2.x,p2.y);
   }

   if (abs(angle) < PI)
      return(FALSE);
   else
      return(TRUE);
}
int minx,miny,maxx,maxy;


//Draw function:
void PolygonFill (void)
{		
		int i,j;
    	glClear (GL_COLOR_BUFFER_BIT);  // Clear display window.
		glColor3f (1.0, 0.0, 0.0); 
		scrPt p1;		
		//First of all:determine the rectangle that polygon inserted in it:
		//purpose: avoiding calculate additional points that we sure dont lie in polygon area.
	minx=width+1;
	for(i=0;i<n;i++)
		if(poly[i].x<minx)minx=poly[i].x;
	miny=height+1;
	for(i=0;i<n;i++)
		if(poly[i].y<miny)miny=poly[i].y;
	maxx=-1;
	for(i=0;i<n;i++)
		if(poly[i].x>maxx)maxx=poly[i].x;
	maxy=-1;
	for(i=0;i<n;i++)
		if(poly[i].y>maxy)maxy=poly[i].y;
	//Second: check pixels of polygon rectangle 
	for(i=0;i<width;i++)
		for(j=0;j<height;j++)
		{
			p1.x=i;
			p1.y=j;
			if((p1.x<=maxx&&p1.x>=minx)&&(p1.y>=miny&&p1.y<=maxy))
			{
			if(floodFill4(poly,p1))
			{
				//if true should determine it's color according to the mask table by getcolor function
				getColor(p1.x,p1.y,currentBuffer[i][j]);
			}
			else
			{
				//else it is black
				currentBuffer[i][j][0]=0;
				currentBuffer[i][j][1]=0;
				currentBuffer[i][j][2]=0;			
			}
			}
			else
			{
				//else not in polygon rect is black:
				currentBuffer[i][j][0]=0;
				currentBuffer[i][j][1]=0;
				currentBuffer[i][j][2]=0;			
			}
		}
		//draw buffer on display
	glDrawPixels(width, height, GL_RGB, GL_FLOAT, currentBuffer);
    	glFlush ( );     // Process all OpenGL routines as quickly as possible.
}


void spec ( int key, int x, int y )
{
	int i;
        switch ( key )
        {
        case GLUT_KEY_UP : 
			printf ("UP arrow pressed\n"); 
			for(i=0;i<n;i++)
				poly[i].x+=10;
			PolygonFill();
			break;
        case GLUT_KEY_DOWN : 
			printf ("DOWN arrow pressed\n"); 
			for(i=0;i<n;i++)
				poly[i].x-=10;
			PolygonFill();
			break;
        case GLUT_KEY_RIGHT : 
			printf ("RIGHT arrow pressed\n"); 
			for(i=0;i<n;i++)
				poly[i].y+=10;
			PolygonFill();
			break;
        case GLUT_KEY_LEFT : 
			printf ("LEFT arrow pressed\n"); 
			for(i=0;i<n;i++)
				poly[i].y-=10;
			PolygonFill();
			break;
    }
}

void winReshapeFcn (int newWidth, int newHeight)
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ( );
    gluOrtho2D (0.0, (GLdouble) newWidth, 0.0, (GLdouble) newHeight);
    glClear (GL_COLOR_BUFFER_BIT);  
}


void main (int argc, char** argv)
{
    glutInit (&argc, argv);                         // Initialize GLUT.
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);   // Set display mode.
    glutInitWindowPosition (100, 100);   // Set top-left display-window position.
    glutInitWindowSize (width, height);      // Set display-window width and height.
    glutCreateWindow ("Project 1: Interactive Polygon-Filling"); // Create display window.
    init ( );                            // Execute initialization procedure.
	poly[0].x = 200;
	poly[0].y = 400;  
	poly[1].x = 100;
	poly[1].y = 300;
	poly[2].x = 100;
	poly[2].y = 200;
	poly[3].x = 200;
	poly[3].y = 100;  
	poly[4].x = 300;
	poly[4].y = 200;
	poly[5].x = 300;
	poly[5].y = 300;	
	
	glutDisplayFunc (PolygonFill);       // Send graphics to display window.
	glutReshapeFunc (winReshapeFcn);
	glutSpecialFunc( spec ) ;
	glutMainLoop ( );                    // Display everything and wait.
}