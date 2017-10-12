#pragma once

#include "Theron/Theron.h"
#include <queue>

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
	    (const_cast<MsgType&>(msg))->Process(_idx);
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
	    if(msg->IsProcessed())
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
