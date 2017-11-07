#pragma once

#include "Theron/Theron.h"
#include <queue>
#include <unordered_map>

#include "Log/gbLog.h"
// class gbActorMsg
// {
//     friend class gbActor;
//     friend class gbActorDispatcher;
// public:
// //    gbActorMsg
//     gbActorMsg(const gbActorMsg& other);
// private:
//     void* _data;
//     bool _processed;
//     Theron::Address _sender;
// private:
//     virtual inline void _process() = 0;

// };

template<typename MsgType>
class gbActor:public Theron::Actor
{
public:
    gbActor(Theron::Framework & framework, const unsigned int idx):
	Theron::Actor(framework),
	_idx(idx)
	{
	    RegisterHandler(this, &gbActor::_handler);
	}

private:
    const unsigned int _idx;
private:
    void _handler(const MsgType & msg, const Theron::Address from)
	{
	    (const_cast<MsgType&>(msg)).Process(_idx);
	    (const_cast<MsgType&>(msg)).SetProcessed();
	    Send(msg, from);
	}
};

template<typename MsgType>
class gbActorDispatcher:public Theron::Actor
{
    typedef gbActor<MsgType> WorkerType;
public:
    gbActorDispatcher(Theron::Framework & framework, const unsigned int num):
	Theron::Actor(framework)
	{
	    for(int i= 0; i < num; i++)
	    {
		WorkerType* a = new WorkerType(framework, i);
		_workers.push(a);
		_freeWorker.push(a->GetAddress());
	    }

	    RegisterHandler(this, &gbActorDispatcher::_handler);
	}
    ~gbActorDispatcher()
	{
	    while(!_workers.empty())
	    {
		delete _workers.front();
		_workers.pop();
	    }
	}
private:
    std::queue<WorkerType*> _workers;
    std::queue<Theron::Address> _freeWorker;
    std::queue<MsgType> _msgs;
private:
    void _handler(const MsgType & msg, const Theron::Address from)
	{
	    if(msg.IsProcessed())
	    {
		_freeWorker.push(from);
	    }
	    else
	    {
		_msgs.push(msg);
	    }
	    while(!_freeWorker.empty() && ! _msgs.empty())
	    {
		Send(_msgs.front(), _freeWorker.front());

		_msgs.pop();
		_freeWorker.pop();
	    }
	}
};

struct gbRefCountAddress
{
    inline gbRefCountAddress(Theron::Address Addr):
	refCount(1),
	addr(Addr)
	{}
    std::uint64_t refCount;
    Theron::Address addr;
};
//for some msg needed be handled sequencially
template<typename MsgType, typename MsgKey>
class gbSerializeActorDispatcher:public Theron::Actor
{
    typedef gbActor<MsgType> WorkerType;
public:
    gbSerializeActorDispatcher(Theron::Framework & framework, const unsigned int num):
	Theron::Actor(framework)
	{
	    for(int i= 0; i < num; i++)
	    {
		WorkerType* a = new WorkerType(framework, i);
		_workers.push(a);
		_freeWorker.push(a->GetAddress());
	    }

	    RegisterHandler(this, &gbSerializeActorDispatcher::_handler);
	}
    ~gbSerializeActorDispatcher()
	{
	    while(!_workers.empty())
	    {
		delete _workers.front();
		_workers.pop();
	    }
	}
private:
    std::queue<WorkerType*> _workers;
    std::queue<Theron::Address> _freeWorker;
    std::unordered_map<MsgKey, gbRefCountAddress*> _mpBusyWorker;
    std::queue<MsgType> _msgs;
private:
    void _handler(const MsgType & msg, const Theron::Address from)
	{
	    if(msg.IsProcessed())
	    {
		typename std::unordered_map<MsgKey, gbRefCountAddress*>::iterator itr = _mpBusyWorker.find(msg.GetKey());
		if(itr != _mpBusyWorker.end())
		{
		    gbRefCountAddress * addr = itr->second;
		    addr->refCount--;
		    if(addr->refCount == 0)
		    {
			_mpBusyWorker.erase(itr);
			_freeWorker.push(addr->addr);
			delete addr;
			addr = nullptr;
		    }
		}
		else
		    gbLog::Instance().Error("impossible error, possible due to msg._processed not initialized with false");//err
	    }
	    else
	    {
		_msgs.push(msg);
	    }
	    
	    while(!_msgs.empty())
	    {
		const MsgType& front = _msgs.front();
		typename std::unordered_map<MsgKey, gbRefCountAddress*>::iterator itr = _mpBusyWorker.find(front.GetKey());
		if(itr != _mpBusyWorker.end())
		{
		    gbRefCountAddress* addr = itr->second;
		    Send(front, addr->addr);

		    //reach max uint64?
		    addr->refCount++;
		    _msgs.pop();
		}
		else
		{
		    if(!_freeWorker.empty())
		    {
			Theron::Address tAddr = _freeWorker.front();
			Send(front, tAddr);
			gbRefCountAddress * addr = new gbRefCountAddress(tAddr);
			_mpBusyWorker.insert(std::pair<MsgKey, gbRefCountAddress*>(front.GetKey(),addr));
			_msgs.pop();
			_freeWorker.pop();
		    }
		    else
			break;
		}
		    
	    }
	}
};
