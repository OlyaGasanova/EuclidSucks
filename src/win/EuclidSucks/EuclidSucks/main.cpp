// NonEuclid.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma once
#include <iostream>
#include <tchar.h>
#include <game.h>
#include <app.h>

//
//HGLRC initGL(HDC hDC) {
//	PIXELFORMATDESCRIPTOR pfd; // структура-описатель формата
//	memset(&pfd, 0, sizeof(pfd));
//	pfd.nSize = sizeof(pfd);
//	pfd.nVersion = 1;
//	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//	pfd.cColorBits = 32;
//	pfd.cDepthBits = 24;
//	
//	int format = ChoosePixelFormat(hDC, &pfd); // выбор наиболее подходящего формата пикселя
//	SetPixelFormat(hDC, format, &pfd); // установка формата
//	HGLRC hRC = wglCreateContext(hDC); // создаём контекст
//	wglMakeCurrent(hDC, hRC); // устанавливаем контекст
//	return hRC; // возвращаем идентификатор контекта
//}

void freeGL(HGLRC hRC) {
	wglMakeCurrent(0, 0); // отвязываемся от контекста
	wglDeleteContext(hRC); // и уничтожаем его
}

DWORD getTime() {
	LARGE_INTEGER Freq, Count;
	QueryPerformanceFrequency(&Freq);	// получаем частоту счётчика
	QueryPerformanceCounter(&Count);	// и количество тактов
	return (DWORD)(Count.QuadPart * 1000L / Freq.QuadPart); // переводим в мс
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	App& app= Application.get_instance();
	app.initialize(100, 100, hInstance, nCmdShow);
	app.run();
	app.free();

	// получаем размеры окна с учётом рамок для нужного нам размера клиентской области
//	WNDCLASSEX wc{};
//
//	wc.cbSize = sizeof(WNDCLASSEX);
//	wc.style = CS_HREDRAW | CS_VREDRAW;
//	wc.lpfnWndProc = WindowProc;
//	wc.hInstance = hInstance;
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
//	wc.lpszClassName = "WindowClass";
//
//	if (!RegisterClassEx(&wc))
//		return E_FAIL;
//
//	RECT wr = { 0, 0, 50, 50 };
//
//	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
//	HWND hWnd = CreateWindowEx(NULL,
//		"WindowClass",
//		"App",
//		WS_OVERLAPPEDWINDOW,
//		300,
//		150,
//		wr.right - wr.left,
//		wr.bottom - wr.top,
//		NULL,
//		NULL,
//		hInstance,
//		NULL);
//
//	if (!hWnd)
//	{
//		printf("Oh shi- %d\n", GetLastError());
//	}
//	// инициализация
//	HDC hDC = GetDC(hWnd);
//	HGLRC hRC = initGL(hDC);
//
//	Game::init();
//
//	// назначаем функцию-обработчик сообщений
//	SetWindowLong(hWnd, GWLP_WNDPROC, (LONG)& WindowProc);
//	// показываем окно
//	ShowWindow(hWnd, SW_SHOWDEFAULT);
//
//	DWORD time, lastTime = getTime();
//
//	MSG msg;
//	msg.message = WM_PAINT; // инициализируем, для входа в цикл
////// цикл обработки сообщений
//	while (msg.message != WM_QUIT)
//		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//		else {
//			
//			glClearColor(0.3f, 0.8f, 1.0f, 1.0f); // выставляем цвет в формате RGBA
//			glClear(GL_COLOR_BUFFER_BIT);
//			SwapBuffers(hDC);
//		}
//
	// освобождение ресурсов

	// уничтожаем окно
	


	return 0;
}

