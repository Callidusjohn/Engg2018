#pragma once
#ifndef ASYNC_HANDLER_H
#define ASYNC_HANDLER_H
#include <Arduino.h>
#include "shared_types.h"
#include "shared_utils.h"

struct AsyncHandler {
public:

	AsyncHandler();

	void addCallback(void(*callback)(), millis_t delay = 0);

	void removeCallback(void(*callback)());

	void processLoop();

private:

	static constexpr size_t delay_buffer_size = 10;

	//static Chrono chrono; // technically will need to be reset every 24 days
	millis_t next_invoke;
	struct DelayedCallback {
		millis_t invoke_at;
		void (*callback_function)();
	} callbacks[delay_buffer_size];

} extern AsyncHandler;
//AsyncHandler AsyncHandler;

#endif