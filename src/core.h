#pragma once
#ifndef H_CORE
#define H_CORE

#include <stdio.h>
#include <windows.h>
#include <openGlEntities>

namespace Core {
	int width, height;	// ������� ���������� ����� ����
	float deltaTime;	// ����� ��������� � ����������� ����� � ��������

	void init() {
		//
	}

	void free() {
		//
	}

	// ������� ������ ����� � �������
	void clear(bool color, bool depth) {
		glClear((color ? GL_COLOR_BUFFER_BIT : 0) |
			(depth ? GL_DEPTH_BUFFER_BIT : 0));
	}

	// ������� ������� ������
	void setViewport(int x, int y, int width, int height) {
		glViewport(x, y, width, height);
	}
}

#endif
