#pragma once

#include "Socket.hpp"
#include "OpensslWrapper.hpp"
#include "MainServer.hpp"
#include <iostream>
#include <exception>
#include <string>
#include <cstdio>
#include <csignal>
#include <thread>

void test_socket();
void test_second_socket(std::string ip, int port);