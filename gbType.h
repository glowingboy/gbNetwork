#pragma once
#include "gbCommon.h"
#ifdef _MSC_VER
#define gb_socket_t intptr_t
#else
#define gb_socket_t int
#endif

typedef unsigned int tcpPkgLen;

template<typename T>
struct gb_array
{
    gb_array():
	data(nullptr),
	length(0)
	{}
    gb_array(T* _data, const unsigned int len):
	data(_data),
	length(len)
	{}
    ~gb_array()
	{
	    gbSAFE_DELETE_ARRAY(data);
	}
    gb_array(const gb_array& other):
	data(other.data),
	length(other.length)
	{}
    void operator=(const gb_array& other)
	{
	    data = other.data;
	    length = other.length;
	}
    T* data;
    unsigned int length;
};

