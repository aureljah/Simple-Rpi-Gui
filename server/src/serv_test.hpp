#pragma once

#include "OpensslWrapper.hpp"
#include "Socket.hpp"
#include <iostream>
#include <exception>
#include <string>
#include <cstdio>
#include <csignal>
#include <thread>

void test_socket();
void test_second_socket(std::string ip, int port);