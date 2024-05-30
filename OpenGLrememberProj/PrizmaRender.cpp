#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"


class PrizmaRender
{
public:
	
	GLuint texId;

	PrizmaRender(const double p1[3], double height, GLuint texId)
	{
		double bot[] = { -10, -10, 0 };
		double top[] = { -10, -10, height };
		glBindTexture(GL_TEXTURE_2D, texId);
		//upperViewRender(top);
		//upperViewRender(bot);
		sides(top, bot);
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
		glBegin(GL_TRIANGLE_STRIP);
		glColor3d(0, 0.7, 0.7);
		glVertex3dv(A);
		glVertex3dv(B);
		glVertex3dv(I);
		glVertex3dv(F);
		glVertex3dv(G);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glColor3d(0, 0.7, 0.7);
		glVertex3dv(B);
		glVertex3dv(C);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex3dv(E);
		glVertex3dv(F);
		glEnd();
		glBegin(GL_TRIANGLE_FAN);
		glColor3d(0, 0.7, 0.7);
		glVertex3dv(B);
		glEnd();
		roundedUpperViewRender(C, D);
		inRoundedUpperViewRender(B, C, D, E, F);
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
		glTexCoord2d(0.825, 0.952
		);
		glVertex3dv(A_bot);
		glTexCoord2d(0.825, 0.924
		);
		glVertex3dv(A_top);
		glTexCoord2d(0.974, 0.924
		);
		glVertex3dv(B_top);
		glTexCoord2d(0.974, 0.952
		);
		glVertex3dv(B_bot);

		glColor3d(0.0, 0.5, 0.5);
		glVertex3dv(B_bot);
		glVertex3dv(B_top);
		glVertex3dv(C_top);
		glVertex3dv(C_bot);
		glColor3d(0.5, 0.5, 0);
		glVertex3dv(E_bot);
		glVertex3dv(E_top);
		glVertex3dv(F_top);
		glVertex3dv(F_bot);
		glColor3d(0.2, 0.8, 0.4);
		glVertex3dv(F_bot);
		glVertex3dv(F_top);
		glVertex3dv(G_top);
		glVertex3dv(G_bot);
		glColor3d(0.8, 0.4, 0.3);
		glVertex3dv(G_bot);
		glVertex3dv(G_top);
		glVertex3dv(I_top);
		glVertex3dv(I_bot);
		glColor3d(0.5, 0.4, 0.3);
		glVertex3dv(I_bot);
		glVertex3dv(I_top);
		glVertex3dv(A_top);
		glVertex3dv(A_bot);
		glEnd();
		std::vector<std::vector<double>> OutCirclePoints = generateSemiCirclePoints(C_bot, D_bot, 32, 1, 1);
		glBegin(GL_QUAD_STRIP);
		glColor3d(0.8, 0.0, 0.5);
		glVertex3dv(C_bot);
		glVertex3dv(C_top);
		for (int i = 0; i < OutCirclePoints.size(); i++) {
			double p[3];
			std::copy(OutCirclePoints[i].begin(), OutCirclePoints[i].end(), p);
			glVertex3dv(p);
			p[2] = C_top[2];
			glVertex3dv(p);
		}
		glVertex3dv(D_top);
		glVertex3dv(D_bot);
		glEnd();

		E_bot[0] += 0.0395;
		std::vector<std::vector<double>> InCirclePoints = generateSemiCirclePoints(E_bot, D_bot, 64, 1, 1);
		E_bot[0] -= 0.0395;
		glBegin(GL_QUAD_STRIP);
		glColor3d(0.0, 0.2, 0.5);
		glVertex3dv(E_bot);
		glVertex3dv(E_top);
		for (int i = 2; i < InCirclePoints.size(); i++) {
			double p[3];
			std::copy(InCirclePoints[i].begin(), InCirclePoints[i].end(), p);
			glVertex3dv(p);
			p[2] = E_top[2];
			glVertex3dv(p);
		}
		glVertex3dv(D_top);
		glVertex3dv(D_bot);
		glEnd();
	}
};