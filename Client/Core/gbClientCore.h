#pragma once 
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "event.h"
#include "event2/thread.h"
#include "gbCommon.h"

class gbSendPkg
{
public:

	enum Priority
	{
		low = 1, mid, high
	};
public:
	/**
	@param bCopy if !Copy then use _data directly without memcpy(a feature for big file transfer?)
	*/
	inline gbSendPkg(const unsigned char* _data, const size_t size, const bool bCopy, const Priority priority) :
		_size(size),
		_priority(priority)
	{
		if (bCopy)
		{
			data = new unsigned char[_size];
			memcpy(data, _data, _size);
		}
		else
			data = const_cast<unsigned char*>(_data);
	}
	inline gbSendPkg(gbSendPkg && other):
		_size(other._size),
		_priority(other._priority),
		data(other.data)
	{
		other.data = nullptr;
	}

	inline gbSendPkg& operator=(gbSendPkg && other)
	{
		_size = other._size;
		data = other.data;
		_priority = other._priority;
	}

	inline size_t GetSize()const { return _size; }
	inline ~gbSendPkg()
	{
		gbSAFE_DELETE_ARRAY(data);
	}
	inline bool operator<(const gbSendPkg& other)
	{
		return _priority < other._priority;
	}
	unsigned char* data;
	
private:
	Priority _priority;
	size_t _size;
};

class gbClientCore
{

public:
	gbClientCore();
	~gbClientCore();

	static bool Initialize();

	bool Connect(const char* ip, const unsigned short port);
	bool Send(const unsigned char* msg, const size_t size);
	bool Send(gbSendPkg* pkg);
private:
	event_base* _ev_base;
	bufferevent* _bev;
	std::thread* _loop_thread;

	std::thread* _dedicated_send_thread;

	//note!!!, each number in buffer reverse bytes order individually not reversing the whole buffer, when necessary 
	static bool _is_little_endian;

	static std::mutex _sendPkgMutex;
	static std::mutex _writableMutex;
	static std::condition_variable _sendThread_cv;
	static bool _writable;
	static std::priority_queue<gbSendPkg*> _sendPkgs;
private:
	static void _log_callback(int severity, const char* msg);
	static void _fatal_error_callback(int err);

	static void _bufferevent_readcb(bufferevent* bev, void *ptr);
	static void _bufferevent_writecb(struct bufferevent *bev, void *ctx);
	static void _bufferevent_cb(bufferevent* bev, short events, void *ptr);

	static void _send_thread(bufferevent* bev);
};