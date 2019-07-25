#include "app.h"
#include <openGlEntities.h>
#include <iostream>
#include <tchar.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <shaderManager.h>

DWORD get_time() {
	LARGE_INTEGER Freq, Count;
	QueryPerformanceFrequency(&Freq);	// получаем частоту счётчика
	QueryPerformanceCounter(&Count);	// и количество тактов
	return (DWORD)(Count.QuadPart * 1000L / Freq.QuadPart); // переводим в мс
}

static const GLfloat g_vertex_buffer_data[] = {
   -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f,
   0.0f,  1.0f, 0.0f,
};


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		} break;
		case WM_SIZE:
		{
			Application.set_resolution(LOWORD(lParam), HIWORD(lParam));
			break;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

App::App() : m_deviceContext(0)
			,m_renderingContext(0)
{
	
}

App::~App()
{
}

App& App::get_instance()
{
	static App instance;
	return instance;
}
void App::set_resolution(int width, int height)
{
	m_width = width;
	m_height = height;
}

void App::init_GL() {
	
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;

	int format = ChoosePixelFormat(m_deviceContext, &pfd);
	SetPixelFormat(m_deviceContext, format, &pfd);
	m_renderingContext = wglCreateContext(m_deviceContext);
	if (m_renderingContext == nullptr)
	{
		return;
	}
	int result = wglMakeCurrent(m_deviceContext, m_renderingContext);
	if (result != 1)
	{
		return;
	}
}

void App::free_GL() {
	wglMakeCurrent(0, 0);
	wglDeleteContext(m_renderingContext);
}

void App::free()
{
	DestroyWindow(hWnd);
	free_GL();
	ReleaseDC(hWnd, m_deviceContext);
}

bool App::initialize(int width, int height, HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wc{};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowClass";

	if (!RegisterClassEx(&wc))
		return E_FAIL;

	RECT wr = { 0, 0, m_width, m_height };

	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindowEx(NULL,
		"WindowClass",
		"App",
		WS_OVERLAPPEDWINDOW,
		300,
		150,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd)
	{
		printf("Oh shi- %d\n", GetLastError());
	}
	SetWindowLong(hWnd, GWLP_WNDPROC, (LONG)& WindowProc);
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	m_deviceContext = GetDC(hWnd);
	if (!m_deviceContext)
	{
		return false;
	}

	init_GL();
	
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)& m_pDI, NULL)))
	{
		//SHOWERROR("DirectInput8() - Failed", __FILE__, __LINE__);
		return FALSE;
	}

	if (!m_mouse.Initialize(m_pDI, hWnd, DIT_MOUSE))
	{
		return FALSE;
	}
	if (!m_keyboard.Initialize(m_pDI, hWnd, DIT_KEYBOARD))
	{
		return FALSE;
	}

	return true;
}

GLuint vertexbuffer;


void App::run()
{
	QueryPerformanceCounter((LARGE_INTEGER*)& m_lastTime);
	QueryPerformanceFrequency((LARGE_INTEGER*)& m_ticksPerSecond);
	DWORD time, lastTime = get_time();
	MSG msg;
	msg.message = WM_PAINT;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	// Создадим 1 буфер и поместим в переменную vertexbuffer его идентификатор
	glGenBuffers(1, &vertexbuffer);
	// Сделаем только что созданный буфер текущим
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Передадим информацию о вершинах в OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	pos.x = 4;
	pos.y = 4; 
	pos.z = 4; 
	// Или, для ортокамеры
	GLuint programID = Shader_Manager.get_shader_handle("Shader");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	while (msg.message != WM_QUIT)
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			on_update_frame();
			View = glm::lookAt(
				pos, // Камера находится в мировых координатах (4,3,3)
				glm::vec3(0, 0, 0), // И направлена в начало координат
				glm::vec3(0, 1, 0)  // "Голова" находится сверху
			);
			// Матрица модели : единичная матрица (Модель находится в начале координат)
			glm::mat4 Model = glm::mat4(1.0f);  // Индивидуально для каждой модели

			// Итоговая матрица ModelViewProjection, которая является результатом перемножения наших трех матриц
			glm::mat4 MVP = Projection * View * Model;
			glUseProgram(programID);
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			on_render_frame();
			time = get_time();	// получаем текущее время
			if (time <= lastTime)	// если с предыдущего кадра прошло менее 1 мс
				continue;		// пропускаем кадр

			deltaTime = (time - lastTime) * 0.001f;
			lastTime = time;

			/*glClearColor(0.3f, 0.8f, 1.0f, 1.0f); 
			glClear(GL_COLOR_BUFFER_BIT);*/
			SwapBuffers(m_deviceContext);
		}
}

void App::process_input(long xDelta, long yDelta, long zDelta, BOOL* pMouseButtons, BOOL* pPressedKeys, float elapsedTime)
{
	float cameraSpeed = 20.0f;
	if (pMouseButtons[0])
	{
		r += xDelta * elapsedTime * 0.5f;
		g+= (yDelta * elapsedTime * 0.5f);
	/*	camera->SetTheta(yDelta * elapsedTime * 0.5f);
		camera->SetPhi(xDelta * elapsedTime * 0.5f);
		camera->Rotate();*/
	}
	if (pMouseButtons[1])
	{
		b += xDelta * elapsedTime * 0.5f;
		a += (yDelta * elapsedTime * 0.5f);
		/*light->SetTheta(yDelta * elapsedTime * 0.5f);
		light->SetPhi(xDelta * elapsedTime * 0.5f);
		light->Rotate();*/
	}
	if (pPressedKeys[DIK_UP])
	{
		//camera->MoveForward(cameraSpeed * elapsedTime * 0.1f);
	}
	if (pPressedKeys[DIK_W])
	{
		pos.z += 10 * elapsedTime * 0.1f;
		//camera->MoveForward(cameraSpeed * elapsedTime*0.1f);
		//light->MoveForward(cameraSpeed * elapsedTime * 0.1f);
	}

	if (pPressedKeys[DIK_S])
	{
		//camera->MoveForward(-cameraSpeed * elapsedTime*0.1f);
		//light->MoveForward(-cameraSpeed * elapsedTime * 0.1f);
	}
	if (pPressedKeys[DIK_DOWN])
	{
		//camera->MoveForward(-cameraSpeed * elapsedTime * 0.1f);
	}

	if (pPressedKeys[DIK_Q])
	{
		pos.x += 10 * elapsedTime * 0.1f;
		//light->SetFOV(light->GetFOV() + 0.05);
		//flag = true;
		/*d3dDeviceContext->VSSetShader(toruspVS, nullptr, 0);
		d3dDeviceContext->PSSetShader(toruspPS, nullptr, 0);*/

	}

	if (pPressedKeys[DIK_E])
	{
		pos.y += 10 * elapsedTime * 0.1f;
		//light->SetFOV(light->GetFOV() - 0.05);
		//flag = false;
		/*d3dDeviceContext->VSSetShader(toruspVS2, nullptr, 0);
		d3dDeviceContext->PSSetShader(toruspPS2, nullptr, 0);*/
	}

	if (pPressedKeys[DIK_Z])
	{
	//	light->SetNearPlane(light->GetNearPlane() + 0.05);
		//flag = false;
		/*d3dDeviceContext->VSSetShader(toruspVS2, nullptr, 0);
		d3dDeviceContext->PSSetShader(toruspPS2, nullptr, 0);*/
	}

	if (pPressedKeys[DIK_C])
	{
	//	light->SetNearPlane(light->GetNearPlane() - 0.05);
		//flag = false;
		/*d3dDeviceContext->VSSetShader(toruspVS2, nullptr, 0);
		d3dDeviceContext->PSSetShader(toruspPS2, nullptr, 0);*/
	}

}

void App::on_update_frame()
{
	//QueryPerformanceFrequency((LARGE_INTEGER*)& m_ticksPerSecond);
	//m_timeElapsed = (m_current_time.QuadPart - m_lastTime.QuadPart) / (float)m_ticksPerSecond.QuadPart;
	//QueryPerformanceCounter((LARGE_INTEGER*)& m_current_time);
//	m_timeElapsed = (m_current_time.QuadPart - m_lastTime.QuadPart) / (float)m_ticksPerSecond.QuadPart;

	m_mouse.Read();
	m_keyboard.Read();

	long xDelta = m_mouse.GetXDelta();
	long yDelta = m_mouse.GetYDelta();
	long zDelta = m_mouse.GetZDelta();

	BOOL* pMouseButtons = m_mouse.GetButtons();
	BOOL* pPressedKeys = m_keyboard.GetKeys();
	process_input(xDelta, yDelta, zDelta, pMouseButtons, pPressedKeys, deltaTime);

	m_lastTime = m_current_time;

}

void App::on_render_frame()
{
	setViewport(0, 0, m_width, m_height);
	clear(true, true);
	glClearColor(r,g,b, 1.0f); 

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // Атрибут 0. Подробнее об этом будет рассказано в части, посвященной шейдерам.
		3,                  // Размер
		GL_FLOAT,           // Тип
		GL_FALSE,           // Указывает, что значения не нормализованы
		0,                  // Шаг
		(void*)0            // Смещение массива в буфере
	);

	// Вывести треугольник!
	glDrawArrays(GL_TRIANGLES, 0, 3); // Начиная с вершины 0, всего 3 вершины -> один треугольник

	glDisableVertexAttribArray(0);

	//glClear(GL_COLOR_BUFFER_BIT);
	//// поворот матрицы вида со скоротсью 90 град/сек
	//glRotatef(90.0f * m_timeElapsed, 0, 0, 1);
	//// передача вершин треугольника в формате цвет/координата
	//glBegin(GL_TRIANGLES);
	//glColor3f(1, 0, 0); glVertex2f(-0.5f, 0.5f);
	//glColor3f(0, 1, 0); glVertex2f(-0.5f, -0.5f);
	//glColor3f(0, 0, 1); glVertex2f(0.5f, -0.5f);
	//glEnd();
}

void App::clear(bool color, bool depth) {
	glClear((color ? GL_COLOR_BUFFER_BIT : 0) |
		(depth ? GL_DEPTH_BUFFER_BIT : 0));
}

void App::setViewport(int x, int y, int width, int height) {
	glViewport(x, y, width, height);
}