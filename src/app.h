#pragma once
#include <stdio.h>
#include <windows.h>
#include <gl/GL.h>
#include <CInputDevice.h>
class App
{
private:
	App();
	~App();
	App(App const&) = delete;
	App& operator= (App const&) = delete;

	int m_width = 800;
	int m_height = 500;
	float m_current_time = 0;
	float m_lastTime = 0;
	float m_ticksPerSecond = 0;
	float m_timeElapsed = 0;
	float deltaTime;
	float r = 0;
	float g = 0;
	float b = 0;
	float a = 1;

	HDC m_deviceContext;
	HWND hWnd;
	HGLRC m_renderingContext;

	CInputDevice    m_mouse;
	CInputDevice    m_keyboard;
	LPDIRECTINPUT8  m_pDI;

	void init_GL();
	void free_GL();
	void process_input(long xDelta, long yDelta, long zDelta, BOOL* pMouseButtons, BOOL* pPressedKeys, float elapsedTime);
	void on_update_frame();
	void on_render_frame();

	void clear(bool color, bool depth);
	void setViewport(int x, int y, int width, int height);

public:
	static App& get_instance();
	void set_resolution(int width, int height);
	bool initialize(int width, int height, HINSTANCE hInstance, int nCmdShow);
	void run();
	void free();
};

#define Application App::get_instance()
