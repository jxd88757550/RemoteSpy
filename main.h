
#pragma once
#include "stdafx.h"
#include "log.h"

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show);

namespace core
{
	LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, UINT wparam, LONG lparam);
	void init();
	void init_callbacks();
	void init_window(HINSTANCE instance);
	void message_loop();
	void release();
}