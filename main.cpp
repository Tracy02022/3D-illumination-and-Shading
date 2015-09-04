#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include<vector>
#include <functional> 
#include<math.h>

#include <algorithm>
using namespace std;

/*Switch between different files*/
bool mesh = true;
/*look at point*/
float ex, ey, ez, ox, oy, oz, hx, hy, hz;

int Mode;
/*store each vertex in the file*/
struct Vertex{
	float x;
	float y;
	float z;
	/*reload cross operation*/
	Vertex cross(const Vertex& right)
	{
		Vertex result;
		result.x = y*right.z - z*right.y;
		result.y = z*right.x - x*right.z;
		result.z = x*right.y - y*right.x;
		return result;
	}
	const Vertex operator - (const Vertex& right) const
	{
		Vertex result;
		result.x = x - right.x;
		result.y = y - right.y;
		result.z = z - right.z;
		return result;
	}
	/*Reload + operator*/
	const Vertex operator + (const Vertex& right) const
	{
		Vertex result;
		result.x = x + right.x;
		result.y = y + right.y;
		result.z = z + right.z;
		return result;
	}
	/*Reload * operator to times a number*/
	const Vertex operator * (const float right) const
	{
		Vertex result;
		result.x = x*right;
		result.y = y*right;
		result.z = z*right;
		return result;
	}
	/*For normalized vector dot product use*/
	float dotProduct(const Vertex& right) const
	{	
		float r;
		r = 1.0*x*right.x + 1.0*y*right.y + 1.0*z*right.z;
		
		return r;
	}
};
/*store each triangles in the file*/
struct Triangle{
	int v1;
	int v2;
	int v3;
};
/*color of surface, ambient, phong and light*/
Vertex Cr, Ca, Cl,Cp;
/*light position*/
Vertex l;
struct Mat{
	float elem[4][4];
	float value[4];
	/*Mat times another Mat function*/
	const Mat operator *(const Mat& right) const
	{
		Mat result;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.elem[i][j] = elem[i][0] * right.elem[0][j] + elem[i][1] * right.elem[1][j] + elem[i][2] * right.elem[2][j] + elem[i][3] * right.elem[3][j];
			}
		}
			return result;
	}
	/*Mat times a vertex and return another vertex*/
	const Vertex operator *(const Vertex& v) const
	{
		Vertex result;
		Mat r;
		for (int i = 0; i < 4; i++)
		{
			r.value[i] = elem[i][0] * v.x + elem[i][1] * v.y + elem[i][2] * v.z + elem[i][3];
		}
		for (int i = 0; i < 3; i++)
		{
			r.value[i] = r.value[i] / r.value[3];
		}
		result.x = r.value[0];
		result.y = r.value[1];
		result.z = r.value[2];
		return result;
	}
};
/* Define data */
/*Represent the left mouse key is clicked down*/
int mouse_down = 0;
/*0 means change eye position; 1 means change focus position*/
int change_mode = 0; 
int current_x = 0, current_y = 0;
/*store every vertex in the different lists of each file*/
vector<Vertex> vt_list1,vt_list2;
/*store every triangles in the different lists of each file*/
vector<Triangle> tg_list1,tg_list2;
/*center point in bunny_low file*/
float middle1, middle2,middle3;
/*read bunny_low file*/
void readFile()
{	
	string filename;
	filename = "bunny_high.obj";	
	ifstream file(filename);
	char c;
	float f1, f2, f3;
	while (!file.eof())
	{
		file >> c >> f1 >> f2 >> f3;
		if (c == 'v')
		{			
			Vertex vertex;
			vertex.x = f1;
			vertex.y = f2;
			vertex.z = f3;
			/*compute for center using*/
			middle1 += f1;
			middle2 += f2;
			middle3 += f3;
			vt_list1.push_back(vertex);
		}
		else if (c == 'f')
		{
			Triangle triangle;
			triangle.v1 = f1;
			triangle.v2 = f2;
			triangle.v3 = f3;
			tg_list1.push_back(triangle);
		}
		else
		{
			cout << "ERROR" << endl;
			exit(-1);
		}
	}	
	middle1 = 1.0*middle1 / vt_list1.size();
	middle2 = 1.0*middle2 / vt_list1.size();
	middle3 = 1.0*middle3 / vt_list1.size();
}
void readFile2()
{
	string filename;
	filename = "bunny_low.obj";
	ifstream file(filename);
	char c;
	float f1, f2, f3;
	while (!file.eof())
	{
		file >> c >> f1 >> f2 >> f3;
		if (c == 'v')
		{
			Vertex vertex;
			vertex.x = f1;
			vertex.y = f2;
			vertex.z = f3;
			vt_list2.push_back(vertex);
		}
		else if (c == 'f')
		{
			Triangle triangle;
			triangle.v1 = f1;
			triangle.v2 = f2;
			triangle.v3 = f3;
			tg_list2.push_back(triangle);
		}
		else
		{
			cout << "ERROR" << endl;
			exit(-1);
		}
	}
	
}
/*initialization*/
void initialize()
{	
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 800, 800);
}
/*normal of vector*/
Vertex unifyvector(Vertex v)
{
	Vertex result;
	result = v;
	float r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	result.x = result.x / r;
	result.y = result.y / r;
	result.z = result.z / r;
	return result;
}

/*display and refresh the bunny*/
void onDisplay()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*glMatrixMode(GL_PROJECTION);*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.4, 0.4, -0.4, 0.4, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(0, 0, 3, 0, 0, 0, 0, 1, 0);
	/*show mesh image*/
	if (mesh)
	{
		/*draw the bunny in the window with mesh style*/
		for (int i = 0; i < tg_list2.size(); i++)
		{
			glColor3f(0.5, 0.5, 0.5);
			glBegin(GL_LINE_LOOP);
			glVertex3f(vt_list2[tg_list2[i].v1 - 1].x, vt_list2[tg_list2[i].v1 - 1].y, vt_list2[tg_list2[i].v1 - 1].z); //point 1
			glVertex3f(vt_list2[tg_list2[i].v2 - 1].x, vt_list2[tg_list2[i].v2 - 1].y, vt_list2[tg_list2[i].v2 - 1].z); //point 2
			glVertex3f(vt_list2[tg_list2[i].v3 - 1].x, vt_list2[tg_list2[i].v3 - 1].y, vt_list2[tg_list2[i].v3 - 1].z); //point 3
			glEnd();
		}	
	}
	else
	{
		if (Mode == 0)
		{
			for (int i = 0; i < tg_list1.size(); i++)
			{
				glColor3f(Cr.x, Cr.y, Cr.z);
				glBegin(GL_TRIANGLES);
				glVertex3f(vt_list1[tg_list1[i].v1 - 1].x, vt_list1[tg_list1[i].v1 - 1].y, vt_list1[tg_list1[i].v1 - 1].z); //point 1
				glVertex3f(vt_list1[tg_list1[i].v2 - 1].x, vt_list1[tg_list1[i].v2 - 1].y, vt_list1[tg_list1[i].v2 - 1].z); //point 2
				glVertex3f(vt_list1[tg_list1[i].v3 - 1].x, vt_list1[tg_list1[i].v3 - 1].y, vt_list1[tg_list1[i].v3 - 1].z); //point 3
				glEnd();
			}
			for (int i = 0; i < tg_list2.size(); i++)
			{
				glColor3f(1.0, 1.0, 1.0);
				glBegin(GL_LINE_LOOP);
				glVertex3f(vt_list2[tg_list2[i].v1 - 1].x, vt_list2[tg_list2[i].v1 - 1].y, vt_list2[tg_list2[i].v1 - 1].z); //point 1
				glVertex3f(vt_list2[tg_list2[i].v2 - 1].x, vt_list2[tg_list2[i].v2 - 1].y, vt_list2[tg_list2[i].v2 - 1].z); //point 2
				glVertex3f(vt_list2[tg_list2[i].v3 - 1].x, vt_list2[tg_list2[i].v3 - 1].y, vt_list2[tg_list2[i].v3 - 1].z); //point 3
				glEnd();
			}
		}
		/*plastic meterial*/
		else if (Mode == 1)
		{
			for (int i = 0; i < tg_list1.size(); i++)
			{
				/*normal*/
				Vertex n;
				Vertex n1,n2;
				n1 = vt_list1[tg_list1[i].v2 - 1] - vt_list1[tg_list1[i].v1 - 1];
				n2 = vt_list1[tg_list1[i].v3 - 1] - vt_list1[tg_list1[i].v1 - 1];
				n1 = unifyvector(n1);
				n2 = unifyvector(n2);
				n = n2.cross(n1);
				/*unify normal*/
				n = unifyvector(n);

				Vertex center;
				center = vt_list1[tg_list1[i].v2 - 1] + vt_list1[tg_list1[i].v1 - 1] + vt_list1[tg_list1[i].v3 - 1];
				center.x = center.x / 3.0;
				center.y = center.y / 3.0;
				center.z = center.z / 3.0;
				/*light vector*/
				Vertex light;
				light =  center - l;
			
				Vertex c;
				c = Cr;
				float COSIN = 0.0;
				light = unifyvector(light);

				if (light.dotProduct(n) > 0)
					COSIN = light.dotProduct(n);
				c.x = Cr.x*Ca.x + Cl.x*Cr.x*COSIN;
				c.y = Cr.y*Ca.y + Cl.y*Cr.y*COSIN;
				c.z = Cr.z*Ca.z + Cl.z*Cr.z*COSIN;
				/*final color*/
				glColor3f(c.x, c.y, c.z);
				/*color triangles*/
				glBegin(GL_TRIANGLES);
				glVertex3f(vt_list1[tg_list1[i].v1 - 1].x, vt_list1[tg_list1[i].v1 - 1].y, vt_list1[tg_list1[i].v1 - 1].z); //point 1
				glVertex3f(vt_list1[tg_list1[i].v2 - 1].x, vt_list1[tg_list1[i].v2 - 1].y, vt_list1[tg_list1[i].v2 - 1].z); //point 2
				glVertex3f(vt_list1[tg_list1[i].v3 - 1].x, vt_list1[tg_list1[i].v3 - 1].y, vt_list1[tg_list1[i].v3 - 1].z); //point 3
				glEnd();
			}
		}
		else if(Mode == 2)
		{	
			for (int i = 0; i < tg_list1.size(); i++)
			{
				Vertex n;
				Vertex n1, n2;
				Vertex v;
				Vertex r;
				/*view vector*/
				v.x = 0;
				v.y = 0;
				v.z = 3;
				n1 = vt_list1[tg_list1[i].v2 - 1] - vt_list1[tg_list1[i].v1 - 1];
				n2 = vt_list1[tg_list1[i].v3 - 1] - vt_list1[tg_list1[i].v1 - 1];
				n1 = unifyvector(n1);
				n2 = unifyvector(n2);
				/*normal vector*/
				n = n1.cross(n2);
				n = unifyvector(n);
				Vertex center;
				center = vt_list1[tg_list1[i].v2 - 1] + vt_list1[tg_list1[i].v1 - 1] + vt_list1[tg_list1[i].v3 - 1];
				center.x = 1.0*center.x / 3.0;
				center.y = 1.0*center.y / 3.0;
				center.z = 1.0*center.z / 3.0;
				Vertex light;
				light = l - center;
				/*light vector*/
				light = unifyvector(light);
				Vertex c;
				c = Cr;
				float cosin;
				cosin = n.dotProduct(light);
				/*reflect vector*/
				r = n*cosin*2 - light;
				float COSIN1 = 0.0;
				if (light.dotProduct(n) > 0)
					COSIN1 = light.dotProduct(n);
				float COSIN2 = 0.0;
				if (v.dotProduct(r) > 0)
					COSIN2 = v.dotProduct(r);
				COSIN2 = pow(COSIN2, 0.5);
				c.x = Cr.x*Ca.x + Cl.x*Cr.x*COSIN1 + Cl.x*Cp.x*COSIN2;
				c.y = Cr.y*Ca.y + Cl.y*Cr.y*COSIN1 + Cl.y*Cp.y*COSIN2;
				c.z = Cr.z*Ca.z + Cl.z*Cr.z*COSIN1 + Cl.z*Cp.z*COSIN2;
				/*final color after adding phong term*/
				glColor3f(c.x, c.y, c.z);
				glBegin(GL_TRIANGLES);
				glVertex3f(vt_list1[tg_list1[i].v1 - 1].x, vt_list1[tg_list1[i].v1 - 1].y, vt_list1[tg_list1[i].v1 - 1].z); //point 1
				glVertex3f(vt_list1[tg_list1[i].v2 - 1].x, vt_list1[tg_list1[i].v2 - 1].y, vt_list1[tg_list1[i].v2 - 1].z); //point 2
				glVertex3f(vt_list1[tg_list1[i].v3 - 1].x, vt_list1[tg_list1[i].v3 - 1].y, vt_list1[tg_list1[i].v3 - 1].z); //point 3
				glEnd();
			}
		}
	}
	glFlush();
	glutSwapBuffers();//display the buffer
}
void onMouse(int button, int state, int x, int y)
{
	GLint specialKey = glutGetModifiers();
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) {
			/*if the button is down change the variable's value*/
			mouse_down = 1;
			current_x = x;
			current_y = y;
			if (specialKey == GLUT_ACTIVE_SHIFT)
			{
				/*translate the bunny*/
				change_mode = 1;
			}
			else if (specialKey == GLUT_ACTIVE_CTRL)
			{
				/*Rotate around the center original*/
				change_mode = 2;
				/*compute the center point each time after refresh the window*/
				middle1 = 0;
				middle2 = 0;
				middle3 = 0;
				for (int i = 0; i < vt_list1.size(); i++)
				{
					middle1 += vt_list1[i].x;
					middle2 += vt_list1[i].y;
					middle3 += vt_list1[i].z;
				}
				middle1 = 1.0*middle1 / vt_list1.size();
				middle2 = 1.0*middle2 / vt_list1.size();
				middle3 = 1.0*middle3 / vt_list1.size();
				
			}
			else
			{
				/*Rotation the bunny*/
				change_mode = 0;
			}
		}
		else if (state == GLUT_UP)
		{
			mouse_down = 0;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
			break;
	default:
		break;
	}
}
void onMouseMotion(int x, int y)
{
	if (mouse_down == 1)
	{
		if (change_mode == 0)
		{	
			/*rotate the bunny around Rx and Ry*/
			Mat Rx,Ry;
			Mat RXRY;
			float theta;
			/*rotate angle*/
			theta = (x - current_x) / 100.f;
			float thetay = (y - current_y) / 100.f;
			float elemy[4][4] = { { cos(theta), 0, sin(theta), 0 }, { 0, 1, 0, 0 }, { -sin(theta), 0, cos(theta), 0 }, { 0, 0, 0, 1 } };	
			float elemx[4][4] = { { 1, 0, 0, 0 }, { 0, cos(thetay), -sin(thetay), 0 }, { 0, sin(thetay), cos(thetay), 0 }, { 0, 0, 0, 1 } };
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					Rx.elem[i][j] = elemy[i][j];
					Ry.elem[i][j] = elemx[i][j];
				}
			}
			/*rotation matrix*/
			RXRY = Rx*Ry;
			/*rotate with different situations*/		
			for (int i = 0; i < vt_list1.size(); i++)
			{		
				vt_list1[i] =RXRY*vt_list1[i];
			}
			for (int i = 0; i < vt_list2.size(); i++)
			{
				vt_list2[i] = RXRY*vt_list2[i];
			}
						
		}
		else if (change_mode == 1)
		{
			Mat Tx;
			float pos1,pos2;
			pos1 = 1.0*(x - current_x) / 400.f;
			pos2 = 1.0*(y - current_y) / 400.f;
			float trans1[4][4] = { { 1, 0, 0, pos1 }, { 0, 1, 0, -pos2 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 } };
			/*translation matrix*/
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					Tx.elem[i][j] = trans1[i][j];
				}
			}
			/*translate with different situations*/
			for (int i = 0; i < vt_list1.size(); i++)
			{	
				vt_list1[i] = Tx*vt_list1[i];
			}
			for (int i = 0; i < vt_list2.size(); i++)
			{
				vt_list2[i] = Tx*vt_list2[i];
			}
		}
		
		else
		{
			/*rotate with center origin*/
			Mat T1,T2;
			Mat Ry,Rx;
			Mat P;	
			float theta;
			theta = (x - current_x) / 100.f;
			float thetax = (y - current_y) / 100.f;
			float elemy[4][4] = { { cos(theta), 0, sin(theta), 0 }, { 0, 1, 0, 0 }, { -sin(theta), 0, cos(theta), 0 }, { 0, 0, 0, 1 } };
			float elemx[4][4] = { { 1, 0, 0, 0 }, { 0, cos(thetax), -sin(thetax), 0 }, { 0, sin(thetax), cos(thetax), 0 }, { 0, 0, 0, 1 } };
			float trans1[4][4] = { { 1, 0, 0, middle1 }, { 0, 1, 0, middle2 }, { 0, 0, 1, middle3 }, { 0, 0, 0, 1 } };
			float trans2[4][4] = { { 1, 0, 0, -middle1 }, { 0, 1, 0, -middle2 }, { 0, 0, 1, -middle3 }, { 0, 0, 0, 1 } };
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					T1.elem[i][j] = trans1[i][j];
					T2.elem[i][j] = trans2[i][j];
					Ry.elem[i][j] = elemy[i][j];
					Rx.elem[i][j] = elemx[i][j];
				}
			}
			/*rotation matrix with center origin*/
			P = T1*(Rx*Ry)*T2;
			for (int i = 0; i < vt_list1.size(); i++)
			{
				vt_list1[i] = P*vt_list1[i];
			}	
			for (int i = 0; i < vt_list2.size(); i++)
			{
				vt_list2[i] = P*vt_list2[i];
			}
		}
		current_x = x;
		current_y = y;
	}
	glutPostRedisplay();
}
void onKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27:
			exit(1);
			break;
		case 'm':
			mesh = true;
			glutPostRedisplay();
			break;
		case'M':
			mesh = true;
			glutPostRedisplay();
			break;
		case '0':
			Mode = 0;
			mesh = false;
			glutPostRedisplay();
			break;
		case '1':
			Mode = 1;
			mesh = false;
			glutPostRedisplay();
			break;
		case '2':
			Mode = 2;
			mesh = false;
			glutPostRedisplay();
			break;
		default:
			break;
	}
}
int main(int argc, char** argv)
{
	middle1 = 0.0;
	middle2 = 0.0;
	middle3 = 0.0;
	Mode = -1;
	Cr.x = 0.9;
	Cr.y = 0.7;
	Cr.z = 0.1;
	Ca.x = 0.2;
	Ca.y = 0.2;
	Ca.z = 0.2;
	Cl.x = 0.9;
	Cl.y = 0.9;
	Cl.z = 0.9;
	Cp.x = Cp.y = Cp.z = 0.8;
	l.x = 0;
	l.y = 0;
	l.z = 5;
	/*Initialization functions*/
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Bunny");
	initialize();
	/*read files*/
	readFile();
	readFile2();
	glutDisplayFunc(onDisplay);
	glutKeyboardFunc(onKeyboard);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMouseMotion);
	glutMainLoop();
	return 1;
}




