#pragma once

#ifdef _MSC_VER
#define gb_socket_t intptr_t
#else
#define gb_socket_t int
#endif

template<typename T>
struct gb_array
{
    gb_array(){}
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

struct gb_callback
{
    virtual void exe(void*);
};

struct gb_send_pkg
{
    gb_socket_t socket;
    gb_array<unsigned char> data;
    gb_callback cb;
};
