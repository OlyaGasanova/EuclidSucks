#pragma once
#ifndef H_CORE
#define H_CORE

#include <stdio.h>
#include <windows.h>
#include <openGlEntities>

namespace Core {
	int width, height;	// размеры клиентской части окна
	float deltaTime;	// время прошедшее с предыдущего кадра в секундах

	void init() {
		//
	}

	void free() {
		//
	}

	// очистка буфера цвета и глубины
	void clear(bool color, bool depth) {
		glClear((color ? GL_COLOR_BUFFER_BIT : 0) |
			(depth ? GL_DEPTH_BUFFER_BIT : 0));
	}

	// задание области вывода
	void setViewport(int x, int y, int width, int height) {
		glViewport(x, y, width, height);
	}
}

#endif
