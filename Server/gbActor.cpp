// #include "gbActor.h"

// // gbActorMsg::gbActorMsg(const gbActorMsg& other):
// //     _data(other._data),
// //     _processed(other._processed),
// //     _sender(other._sender)
// // {
    
// // }

// gbActor::gbActor(Theron::Framework & framework):
//     Theron::Actor(framework)
// {
//     RegisterHandler(this, &gbActor::_handler);
// }

// void gbActor::_handler(const gbActorMsg & msg, const Theron::Address from)
// {
//     (const_cast<gbActorMsg&>(msg))._process();
//     Send(msg, from);
// }

// gbActorDispatcher::gbActorDispatcher(Theron::Framework & framework, const unsigned int num):
//     Theron::Actor(framework)
// {
//     for(int i= 0; i < num; i++)
//     {
// 	gbActor* a = new gbActor(framework);
// 	_workers.push(a);
// 	_freeWorker.push(a->GetAddress());
//     }
// }

// gbActorDispatcher::~gbActorDispatcher()
// {
//     while(!_workers.empty())
//     {
// 	delete _workers.front();
// 	_workers.pop();
//     }
// }

// void gbActorDispatcher::_handler(const gbActorMsg& msg, const Theron::Address from)
// {
//     if(msg._processed)
//     {
// 	_freeWorker.push(from);
//     }
//     else
//     {
// 	_msgs.push(msg);
//     }
//     while(!_freeWorker.empty() && ! _msgs.empty())
//     {
// 	Send(_msgs.front(), _freeWorker.front());

// 	_msgs.pop();
// 	_freeWorker.pop();
//     }
// }
