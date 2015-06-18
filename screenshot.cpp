
#include "screenshot.h"
#include "utils.h"

namespace capture
{
	screenshot_t::screenshot_t()
	{
		try
		{
			init();
			capture();
		}
		catch (...)
		{
			release();
		}
	}

	screenshot_t::~screenshot_t()
	{
		release();
	}

	void screenshot_t::save(const path & filename)
	{
		int quality = 30;

		ULONG_PTR gdiplus_token;
		GdiplusStartupInput startup_input;
		GdiplusStartup(&gdiplus_token, &startup_input, NULL);
		HWND hwnd = GetDesktopWindow();
		RECT r;
		int w, h, capture, bpp;
		HDC dc, hdc_capture;
		LPBYTE capture_ptr;
		CLSID image_clsid;
		Bitmap * screen_ptr;

		GetWindowRect(hwnd, &r);
		dc = GetWindowDC(hwnd);
		w = r.right - r.left;
		h = r.bottom - r.top;
		bpp = GetDeviceCaps(dc, BITSPIXEL);
		hdc_capture = CreateCompatibleDC(dc);

		BITMAPINFO bmiCapture = { sizeof(BITMAPINFOHEADER), w,
			-h, 1, bpp, BI_RGB, 0, 0, 0, 0, 0 };

		HBITMAP bmp_cap = CreateDIBSection(dc, &bmiCapture,
			DIB_PAL_COLORS, (LPVOID *)&capture_ptr, NULL, 0);

		if (!bmp_cap)
		{
			DeleteDC(hdc_capture);
			DeleteDC(dc);
			GdiplusShutdown(gdiplus_token);
			return;
		}

		capture = SaveDC(hdc_capture);
		SelectObject(hdc_capture, bmp_cap);
		BitBlt(hdc_capture, 0, 0, w, h, dc, 0, 0, SRCCOPY);
		RestoreDC(hdc_capture, capture);
		DeleteDC(hdc_capture);
		DeleteDC(dc);

		screen_ptr = new Bitmap(bmp_cap, (HPALETTE)NULL);
		EncoderParameters encoder_params;
		encoder_params.Count = 1;
		encoder_params.Parameter[0].NumberOfValues = 1;
		encoder_params.Parameter[0].Guid = EncoderQuality;
		encoder_params.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoder_params.Parameter[0].Value = &quality;
		get_encoder_clsid(L"image/jpeg", &image_clsid);
		screen_ptr->Save(filename.generic_wstring().c_str(), &image_clsid, &encoder_params);

		delete screen_ptr;
		DeleteObject(bmp_cap);
		GdiplusShutdown(gdiplus_token);

		SetFileAttributes(filename.generic_wstring().c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void screenshot_t::init()
	{
		screen_dc = nullptr;
		bitmap = nullptr;
	}

	void screenshot_t::release()
	{
		if (screen_dc)	SelectObject(screen_dc, bitmap_buf);
		if (bitmap)		DeleteObject(bitmap);
		if (screen_dc)	DeleteDC(screen_dc);
	}

	void screenshot_t::capture()
	{
		hwnd = GetDesktopWindow();
		hdc = GetWindowDC(hwnd);

		RECT wnd_rect;
		GetWindowRect(hwnd, &wnd_rect);
		width = wnd_rect.right - wnd_rect.left;
		height = wnd_rect.bottom - wnd_rect.top;

		screen_dc = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, width, height);
		bitmap_buf = SelectObject(screen_dc, bitmap);

		bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmap_info.bmiHeader.biWidth = width;
		bitmap_info.bmiHeader.biHeight = -height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 24;
		bitmap_info.bmiHeader.biCompression = BI_RGB;

		const int color_size = 3;
		const int alignment = 4;
		int step = (int)ceil(width * color_size / (double)alignment) * alignment;

		data.reset(new unsigned char[step * height]);
	}

	int screenshot_t::get_encoder_clsid(WCHAR * format, CLSID * id) const
	{
		unsigned int num = 0, size = 0;
		GetImageEncodersSize(&num, &size);
		if (size == 0) return -1;
		ImageCodecInfo * codec_info = (ImageCodecInfo *)(malloc(size));
		if (codec_info == NULL) return -1;
		GetImageEncoders(num, size, codec_info);

		for (unsigned int j = 0; j < num; ++j)
		{
			if (wcscmp(codec_info[j].MimeType, format) == 0)
			{
				*id = codec_info[j].Clsid;
				free(codec_info);
				return j;
			}
		}

		free(codec_info);
		return -1;
	}

	void make_screenshot()
	{
		path file = utils::screen_unique_path();

		screenshot_t screenshot;
		screenshot.save(file);
	}

	void make_screenshot(const path & file)
	{
		screenshot_t screenshot;
		screenshot.save(file);
	}
}