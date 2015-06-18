
#include <windows.h>
#include <gdiplus.h>
#include <tlhelp32.h>
#include <wininet.h>

#include <memory>
#include <queue>
#include <functional>
#include <locale>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/format/format_fwd.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp>

#include <gloox/client.h>
#include <gloox/message.h>
#include <gloox/messagehandler.h>
#include <gloox/messagesessionhandler.h>
#include <gloox/connectionlistener.h>
#include <gloox/connectiontcpclient.h>
#include <gloox/presencehandler.h> 

#define WM_HOOKMESSAGE	0x401
#define VK_ENTER		0x0d
#define VK_ESC			0x1b
#define VK_BKSP			0x08

using namespace std;
using namespace boost::filesystem;
using namespace boost::chrono;
using namespace gloox;