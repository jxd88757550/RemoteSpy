
#pragma once
#include "stdafx.h"

namespace utils
{
	enum { KEYS, WINDOWS, PROCESSES, FILES };

	wstring unique_id(int len);
	wstring get_time();
	wstring get_unique_date_name();
	wstring screen_unique_name();
	path screen_unique_path();
	path log_unique_path();
	void save_logfile(const vector<wstring> & data, int type);
	void save_logfile(const wstringstream & data, int type);
	void save_logfile(const path & file, const vector<wstring> & data, int type);
	void save_logfile(const path & file, const wstringstream & data, int type);
	void execute_cmdline(const vector<wstring> & line);
	void message_box(const vector<wstring> & params);
	wstring resolve_type(int type);

	typedef codecvt<wchar_t, char, mbstate_t> null_wcodecvt_base;

	class null_wcodecvt : public null_wcodecvt_base
	{
	public:
		explicit null_wcodecvt(size_t refs = 0);

	protected:
		virtual result do_out(mbstate_t&,
			const wchar_t* from,
			const wchar_t* from_end,
			const wchar_t*& from_next,
			char* to,
			char* to_end,
			char*& to_next) const;

		virtual result do_in(mbstate_t&,
			const char* from,
			const char* from_end,
			const char*& from_next,
			wchar_t* to,
			wchar_t* to_end,
			wchar_t*& to_next) const;

		virtual result do_unshift(mbstate_t&, char* to, char*, char*& to_next) const;

		virtual int do_length(mbstate_t&, const char* from, const char* end, size_t max) const;

		virtual bool do_always_noconv() const throw();

		virtual int do_encoding() const throw();

		virtual int do_max_length() const throw();
	};

	wostream & wendl(wostream & out);
	string to_ascii(const wstring & wstr, char rep = '_');
}