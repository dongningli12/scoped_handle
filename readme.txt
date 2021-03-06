﻿// The code following this comment block illustrates how to use the library. It is also my test code.
//

#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include "scoped_handle.h"

#pragma comment(lib,"Ws2_32.lib")

using file_handle_t = dnl::scoped_handle<decltype(fclose), fclose, NULL>;
using kernel_handle_h = dnl::scoped_handle<decltype(CloseHandle), CloseHandle, NULL>;
using socket_handle_h = dnl::scoped_handle<decltype(closesocket), closesocket, INVALID_SOCKET>;

void scoped_handle_usage_examples()
{
	file_handle_t f(fopen("test_file", "wb"));
	char msg[] = "Test scoped_handle";
	fwrite(msg, 1, sizeof(msg), f);

	f.reset(fopen("non_exist_file", "rb"));
	if (!f)
		std::cout << "calling fopen() failed\n";

	kernel_handle_h h(CreateEvent(NULL, TRUE, FALSE, NULL));
	if (h)
	{
		SetEvent(h);
		std::cout << "handle value: " << h << std::dec << std::endl;
	}
	else
		std::cout << "CreateEvent() failed";

	h.reset(CreateSemaphore(NULL, 0, 4, NULL));
	if (h)
		ReleaseSemaphore(h, 1, NULL);
	else
		std::cout << "CreateSemaphore() failed";

	kernel_handle_h h2(CreateEvent(NULL, TRUE, FALSE, NULL));

	std::cout << "h == h2: " << (h == h2) << ", h: " << h << ", h2: " << h2 << std::endl;
	h = std::move(h2);

	std::cout << "Test socket_handle_t\n";
	WSADATA wsaData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	socket_handle_h s(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
	if (!s)
		std::cout << "calling socket failed\n";

	std::cout << "To leave the scope\n";
}

int main()
{
	scoped_handle_usage_examples();
}
