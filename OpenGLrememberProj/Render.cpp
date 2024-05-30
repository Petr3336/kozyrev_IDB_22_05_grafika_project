#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool textureMode2 = true;
bool lightMode = true;


class PrizmaRender
{
public:

	GLuint texId;

	PrizmaRender(const double p1[3], double height, GLuint texId)
	{
		double bot[] = { -10, -10, 0 };
		double top[] = { -10, -10, height };
		glBindTexture(GL_TEXTURE_2D, texId);
		sides(top, bot);
		upperViewRender(top);
		upperViewRender(bot);
		this->texId = texId;
	}

	~PrizmaRender() {};

private:

	void midpoint3D(const double p1[3], const double p2[3], double mid[3]) {
		mid[0] = (p1[0] + p2[0]) / 2;
		mid[1] = (p1[1] + p2[1]) / 2;
		mid[2] = (p1[2] + p2[2]) / 2;
	}

	//Функция для вычисления расстояния между двумя точками
	double distance(double point1[3], double point2[3]) {
		return std::sqrt(std::pow(point2[0] - point1[0], 2) +
			std::pow(point2[1] - point1[1], 2) +
			std::pow(point2[2] - point1[2], 2));
	}

	// Функция для определения вектора нормали к плоскости, определяемой двумя точками
	void normalVector(double point1[3], double point2[3], double normal[3]) {
		normal[0] = point2[1] - point1[1];
		normal[1] = point1[0] - point2[0];
		normal[2] = 0; // Предполагаем, что Z-координата нормали равна 0
	}

	// Функция для определения начального угла на основе точки
	double startingAngle(double center[3], double point[3]) {
		return atan2(point[1] - center[1], point[0] - center[0]);
	}

	// Функция для генерации точек полукруга
	std::vector<std::vector<double>> generateSemiCirclePoints(double point1[3], double point2[3], int numberOfPoints, bool half, bool startFromFirstPoint) {
		std::vector<std::vector<double>> semiCirclePoints;
		double center[3];
		midpoint3D(point1, point2, center);
		double r = distance(point1, center);

		// Определяем начальный угол на основе выбранной точки
		double angleOffset = startFromFirstPoint ? startingAngle(center, point1) : startingAngle(center, point2);

		for (int i = 0; i <= numberOfPoints; ++i) {
			double angle = angleOffset + (half ? 1 : -1) * 3.14 * i / numberOfPoints;
			semiCirclePoints.push_back({ center[0] + r * cos(angle), center[1] + r * sin(angle), center[2] });
		}

		return semiCirclePoints;
	}

	std::vector<std::pair<std::vector<double>, double>> GenerateSemiCirclePointsWithAnlge(double point1[3], double point2[3], int numberOfPoints, bool half, bool startFromFirstPoint)
	{
		std::vector<std::pair<std::vector<double>, double>> semiCirclePoints;
		double center[3] = {(point1[0] + point2[0]) / 2, (point1[1] + point2[1]) / 2, (point1[2] + point2[2]) / 2};
		double r = distance(point1, center);

		// Определяем начальный угол на основе выбранной точки
		double angleOffset = startFromFirstPoint ? startingAngle(center, point1) : startingAngle(center, point2);

		for (int i = 0; i <= numberOfPoints; ++i)
		{
			double angle = angleOffset + (half ? 1 : -1) * 3.14 * i / numberOfPoints;
			double arcLength = r * angle; // Вычисляем длину дуги
			std::vector<double> point = { center[0] + r * cos(angle), center[1] + r * sin(angle), center[2] };
			std::pair<std::vector<double>, double> temp(point, angle-angleOffset);
			//std::pair<double[3], double> point = { new double[3] { center[0] + r * cos(angle), center[1] + r * sin(angle), center[2] }, arcLength }
			semiCirclePoints.push_back(temp);
		}

		return semiCirclePoints;
	}

	void roundedUpperViewRender(double C[3], double D[3]) {
		double center[3];
		midpoint3D(C, D, center);
		std::vector<std::vector<double>> CirclePoints = generateSemiCirclePoints(C, D, 32, 1, 1);
		glBegin(GL_TRIANGLE_FAN);
		glVertex3dv(center);
		glVertex3dv(C);
		for (int i = 0; i < CirclePoints.size(); i++) {
			double p[3];
			std::copy(CirclePoints[i].begin(), CirclePoints[i].end(), p);
			glVertex3dv(p);
		}
		glVertex3dv(D);
		glEnd();
	}

	void inRoundedUpperViewRender(double B[3], double C[3], double D[3], double E[3], double F[3]) {
		double center[3];
		midpoint3D(E, D, center);
		E[0] += 0.0395;
		std::vector<std::vector<double>> CirclePoints = generateSemiCirclePoints(E, D, 64, 1, 1);
		E[0] -= 0.0395;
		glBegin(GL_TRIANGLE_FAN);
		glVertex3dv(F);
		glVertex3dv(E);
		for (int i = 2; i < CirclePoints.size(); i++) {
			double p[3];
			std::copy(CirclePoints[i].begin(), CirclePoints[i].end(), p);
			glVertex3dv(p);
			if (i == CirclePoints.size() / 2) {
				glEnd();
				glBegin(GL_TRIANGLE_FAN);
				glVertex3dv(C);
				glVertex3dv(p);
			}

		}
		glVertex3dv(D);
		glEnd();
		glBegin(GL_QUADS);
		glVertex3dv(C);
		glVertex3dv(B);
		glVertex3dv(F);
		double point[3];
		std::copy(CirclePoints[CirclePoints.size() / 2].begin(), CirclePoints[CirclePoints.size() / 2].end(), point);
		glVertex3dv(point);
		glEnd();
	}

	void upperViewRender(double coeffVec[3]) {
		double coeffX = coeffVec[0];
		double coeffY = coeffVec[1];
		double coeffZ = coeffVec[2];

		double A[] = { 8 + coeffX, 4 + coeffY, 0 + coeffZ };
		double B[] = { 10 + coeffX, 9 + coeffY, 0 + coeffZ };
		double C[] = { 16 + coeffX, 8 + coeffY, 0 + coeffZ };
		double D[] = { 17 + coeffX, 14 + coeffY, 0 + coeffZ };
		double E[] = { 9 + coeffX, 15 + coeffY, 0 + coeffZ };
		double F[] = { 9 + coeffX, 11 + coeffY, 0 + coeffZ };
		double G[] = { 1 + coeffX, 12 + coeffY, 0 + coeffZ };
		double I[] = { 8 + coeffX, 8 + coeffY, 0 + coeffZ };
		//glBegin(GL_POLYGON);
		//glColor3d(0, 0.7, 0.7);
		//
		//glBegin(GL_TRIANGLES);
		//glTexCoord2f(0.1962890625, 0.47314453125);
		//glVertex3dv(A);
		//glTexCoord2f(0.24560546875, 0.59716796875);
		//glVertex3dv(B);
		//glTexCoord2f(0.19677734375, 0.57180078125);
		//glVertex3dv(I);
		//glEnd();
		glBegin(GL_POLYGON);
		glColor4d(0, 0.7, 0.7, 0.5);
		glTexCoord2f(0.24560546875, 0.59716796875);
		glVertex3dv(B);
		glTexCoord2f(0.22119140625, 0.64599609375);
		glVertex3dv(F);
		glTexCoord2f(0.02392578125, 0.67041015625);
		glVertex3dv(G);
		glTexCoord2f(0.19677734375, 0.57080078125);
		glVertex3dv(I);
		glTexCoord2f(0.1962890625, 0.47314453125);
		glVertex3dv(A);
		glEnd();
		glEnd();

		glBegin(GL_POLYGON);
		glColor4d(0, 0.7, 0.7, 0.5);
		double CenterOut[3];
		midpoint3D(C, D, CenterOut);
		double C_uv[] = { 0.4013671875, 0.56982421875, 0 };
		double D_uv[] = { 0.42041015625, 0.72216796875, 0 };
		std::vector<std::vector<double>> CirclePointsOut = generateSemiCirclePoints(C, D, 32, 1, 1);
		std::vector<std::vector<double>> CirclePointsOutUV = generateSemiCirclePoints(C_uv, D_uv, 32, 1, 1);
		glTexCoord2d(0.4013671875, 0.56982421875);
		glVertex3dv(C);
		for (int i = 0; i < CirclePointsOut.size(); i++) {
			double p[3];
			std::copy(CirclePointsOut[i].begin(), CirclePointsOut[i].end(), p);
			glTexCoord2d(CirclePointsOutUV[i][0], CirclePointsOutUV[i][1]);
			glVertex3dv(p);
		}
		
		
		
		double center[3];
		midpoint3D(E, D, center);
		E[0] += 0.0395;
		double E_uv[] = { 0.22119140625, 0.74853515625, 0 };
		std::vector<std::vector<double>> CirclePoints = generateSemiCirclePoints(E, D, 64, 0, 0);
		std::vector<std::vector<double>> CirclePointsUV = generateSemiCirclePoints(E_uv, D_uv, 64, 0, 0);
		E[0] -= 0.0395;
		
		for (int i = 1; i < CirclePoints.size()-3; i++) {
			double p[3];
			std::copy(CirclePoints[i].begin(), CirclePoints[i].end(), p);
			glTexCoord2f(CirclePointsUV[i][0], CirclePointsUV[i][1]);
			glVertex3dv(p);
			if (i == CirclePoints.size() / 2) {
				//break;
				glTexCoord2f(0.22119140625, 0.64599609375);
				glVertex3dv(F);
				glTexCoord2f(0.24560546875, 0.59716796875);
				glVertex3dv(B);
				glEnd();
				glBegin(GL_TRIANGLE_FAN);
				glTexCoord2f(0.22119140625, 0.64599609375);
				glVertex3dv(F);
				glTexCoord2f(CirclePointsUV[i][0], CirclePointsUV[i][1]);
				glVertex3dv(p);
			}
		
		}
		glTexCoord2d(0.22119140625, 0.74853515625);
		glVertex3dv(E);

		glEnd();

		//glColor3d(0, 0.7, 0.7);
		//glVertex3dv(B);
		//glVertex3dv(C);

		//roundedUpperViewRender(C, D);
		//inRoundedUpperViewRender(B, C, D, E, F);
	}

	void sides(double top[3], double bottom[3]) {
		double A_bot[] = { 8 + bottom[0], 4 + bottom[1], 0 + bottom[2] };
		double B_bot[] = { 10 + bottom[0], 9 + bottom[1], 0 + bottom[2] };
		double C_bot[] = { 16 + bottom[0], 8 + bottom[1], 0 + bottom[2] };
		double D_bot[] = { 17 + bottom[0], 14 + bottom[1], 0 + bottom[2] };
		double E_bot[] = { 9 + bottom[0], 15 + bottom[1], 0 + bottom[2] };
		double F_bot[] = { 9 + bottom[0], 11 + bottom[1], 0 + bottom[2] };
		double G_bot[] = { 1 + bottom[0], 12 + bottom[1], 0 + bottom[2] };
		double I_bot[] = { 8 + bottom[0], 8 + bottom[1], 0 + bottom[2] };
		double A_top[] = { 8 + top[0], 4 + top[1], 0 + top[2] };
		double B_top[] = { 10 + top[0], 9 + top[1], 0 + top[2] };
		double C_top[] = { 16 + top[0], 8 + top[1], 0 + top[2] };
		double D_top[] = { 17 + top[0], 14 + top[1], 0 + top[2] };
		double E_top[] = { 9 + top[0], 15 + top[1], 0 + top[2] };
		double F_top[] = { 9 + top[0], 11 + top[1], 0 + top[2] };
		double G_top[] = { 1 + top[0], 12 + top[1], 0 + top[2] };
		double I_top[] = { 8 + top[0], 8 + top[1], 0 + top[2] };
		glBegin(GL_QUADS);
		glColor3d(0.7, 0.5, 0);
		glTexCoord2d(0.825221, 0.047309);
		glVertex3dv(A_bot);
		glTexCoord2d(0.825221, 0.075087);
		glVertex3dv(A_top);
		glTexCoord2d(0.974809, 0.075087);
		glVertex3dv(B_top);
		glTexCoord2d(0.974809, 0.047309);
		glVertex3dv(B_bot);

		glColor3d(0.0, 0.5, 0.5);
		glTexCoord2d(0.64649, 0.047309);
		glVertex3dv(B_bot);
		glTexCoord2d(0.64649, 0.075087);
		glVertex3dv(B_top);
		glTexCoord2d(0.815455, 0.075087);
		glVertex3dv(C_top);
		glTexCoord2d(0.815455, 0.047309);
		glVertex3dv(C_bot);
		glColor3d(0.5, 0.5, 0);
		glTexCoord2d(0.4772, 0.009766);
		glVertex3dv(E_bot);
		glTexCoord2d(0.4772, 0.037543);
		glVertex3dv(E_top);
		glTexCoord2d(0.588311, 0.037543);
		glVertex3dv(F_top);
		glTexCoord2d(0.588311, 0.009766);
		glVertex3dv(F_bot);
		glColor3d(0.2, 0.8, 0.4);
		glTexCoord2d(0.243483, 0.009766);
		glVertex3dv(F_bot);
		glTexCoord2d(0.243483, 0.037543);
		glVertex3dv(F_top);
		glTexCoord2d(0.467434, 0.037543);
		glVertex3dv(G_top);
		glTexCoord2d(0.467434, 0.009766);
		glVertex3dv(G_bot);
		glColor3d(0.8, 0.4, 0.3);
		glTexCoord2d(0.009766, 0.009766);
		glVertex3dv(G_bot);
		glTexCoord2d(0.009766, 0.037543);
		glVertex3dv(G_top);
		glTexCoord2d(0.233717, 0.037543);
		glVertex3dv(I_top);
		glTexCoord2d(0.233717, 0.009766);
		glVertex3dv(I_bot);
		glColor3d(0.5, 0.4, 0.3);
		glTexCoord2d(0.598077, 0.009766);
		glVertex3dv(I_bot);
		glTexCoord2d(0.598077, 0.037543);
		glVertex3dv(I_top);
		glTexCoord2d(0.70919, 0.037543);
		glVertex3dv(A_top);
		glTexCoord2d(0.70919, 0.009766);
		glVertex3dv(A_bot);
		glEnd();
		std::vector<std::pair<std::vector<double>, double>> OutCirclePoints = GenerateSemiCirclePointsWithAnlge(C_top, D_top, 32, 1, 1);
		float OutTextureWidth = 0.636724 - 0.371314;
		glBegin(GL_QUAD_STRIP);
		glColor3d(0.8, 0.0, 0.5);
		glTexCoord2d(0.371314, 0.047309);
		glVertex3dv(C_bot);
		glTexCoord2d(0.371314, 0.075087);
		glVertex3dv(C_top);
		for (int i = 0; i < OutCirclePoints.size(); i++) {
			double p[3];
			std::copy(OutCirclePoints[i].first.begin(), OutCirclePoints[i].first.end(), p);
			glTexCoord2d(0.371314 + OutTextureWidth * (OutCirclePoints[i].second/3.14), 0.047309);
			glVertex3dv(p);
			p[2] = C_bot[2];
			glTexCoord2d(0.371314 + OutTextureWidth * (OutCirclePoints[i].second /3.14), 0.075087);
			glVertex3dv(p);
		}
		glTexCoord2d(0.636724, 0.075087);
		glVertex3dv(D_top);
		glTexCoord2d(0.636724, 0.047309);
		glVertex3dv(D_bot);
		glEnd();

		float InTextureWidth = 0.009766 - 0.361548;
		E_bot[0] += 0.0395;
		std::vector<std::pair<std::vector<double>, double>> InCirclePoints = GenerateSemiCirclePointsWithAnlge(E_bot, D_bot, 64, 1, 1);
		E_bot[0] -= 0.0395;
		glBegin(GL_QUAD_STRIP);
		glColor3d(0.0, 0.2, 0.5);
		glTexCoord2d(0.009766, 0.047309);
		glVertex3dv(E_bot);
		glTexCoord2d(0.009766, 0.075087);
		glVertex3dv(E_top);
		for (int i = 2; i < InCirclePoints.size(); i++) {
			double p[3];
			std::copy(InCirclePoints[i].first.begin(), InCirclePoints[i].first.end(), p);
			glTexCoord2d(0.009766 - InTextureWidth * (InCirclePoints[i].second / 3.14), 0.047309);
			glVertex3dv(p);
			p[2] = E_top[2];
			glTexCoord2d(0.009766 - InTextureWidth * (InCirclePoints[i].second / 3.14), 0.075087);
			glVertex3dv(p);
		}
		glTexCoord2d(0.361548, 0.075087);
		glVertex3dv(D_top);
		glTexCoord2d(0.361548, 0.047309);
		glVertex3dv(D_bot);
		glEnd();
	}
};

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}
	if (key == 'Z')
	{
		textureMode2 = !textureMode2;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;
GLuint texId2;

//выполняется перед первым рендером
void initRender(OpenGL* ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);


	//массив трехбайтных элементов  (R G B)																						(Н)	Код брался и копировался отсюда
	RGBTRIPLE* texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);					// (Н) Чтоб не мучаться с картинками, я коприовал старую и рисовал на ней
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);


	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);


	//отчистка памяти
	free(texCharArray);
	free(texarray);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);																// (Н) И досюда


	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE* texarray2;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char* texCharArray2;
	int texW2, texH2;
	OpenGL::LoadBMP("texture2.bmp", &texW2, &texH2, &texarray2);
	OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);


	//генерируем ИД для текстуры
	glGenTextures(1, &texId2);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId2);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);


	//отчистка памяти
	free(texCharArray2);
	free(texarray2);

	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH);


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}






void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode || textureMode2)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  


	//Начало рисования квадратика станкина
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	//	glBindTexture(GL_TEXTURE_2D, texId);
	//
	//glColor3d(0.6, 0.6, 0.6);
	//glBegin(GL_QUADS);
	//
	//glNormal3d(0, 0, 1);
	//glTexCoord2d(0, 0);
	//glVertex2dv(A);
	//glTexCoord2d(1, 0);
	//glVertex2dv(B);
	//glTexCoord2d(1, 1);
	//glVertex2dv(C);
	//glTexCoord2d(0, 1);
	//glVertex2dv(D);
	//
	//glEnd();
	//конец рисования квадратика станкина

	double pos[3] = {-10, -10, 0};
	PrizmaRender(pos, 1, (textureMode2 ? texId2 : texId));

   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "Z - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}