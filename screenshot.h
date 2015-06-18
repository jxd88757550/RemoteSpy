
#pragma once
#include "stdafx.h"

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

namespace capture
{
	class screenshot_t
	{
	public:
		screenshot_t();
		~screenshot_t();

		void save(const path & filename);

	private:
		void init();
		void release();
		void capture();
		int get_encoder_clsid(WCHAR * format, CLSID * id) const;

	private:
		HWND hwnd;
		HDC hdc;
		HDC screen_dc;
		HBITMAP bitmap;
		BITMAPINFO bitmap_info;

		int width;
		int height;

		shared_ptr<unsigned char> data;

		HGDIOBJ bitmap_buf;
	};

	void make_screenshot();
	void make_screenshot(const path & file);

	typedef shared_ptr<screenshot_t> screenshot_ptr;
}