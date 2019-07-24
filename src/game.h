#pragma once

#ifndef H_GAME
#define H_GAME

#include "core.h"

namespace Game {

	void init() {
		Core::init();
	}

	void free() {
		Core::free();
	}

	void render() {
		Core::setViewport(0, 0, Core::width, Core::height);
		Core::clear(true, true);

		// поворот матрицы вида со скоротсью 90 град/сек
		glRotatef(90.0f * Core::deltaTime, 0, 0, 1);
		// передача вершин треугольника в формате цвет/координата
		glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0); glVertex2f(-0.5f, 0.5f);
		glColor3f(0, 1, 0); glVertex2f(-0.5f, -0.5f);
		glColor3f(0, 0, 1); glVertex2f(0.5f, -0.5f);
		glEnd();
	}
}

#endif
