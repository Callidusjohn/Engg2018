#pragma once
#ifndef ASYNC_HANDLER_H
#define ASYNC_HANDLER_H
#include <Arduino.h>
#include "shared_types.h"
#include "shared_utils.h"

struct AsyncHandler {
public:

	void addCallback(void(*callback)(), millis_t delay = 0);

	void removeCallback(void(*callback)());

	void processLoop();

private:

	static constexpr size_t delay_buffer_size = 10;

	millis_t next_invoke = millis_t_max;

	struct DelayedCallback {
		millis_t delay_until;
		void (*callback_function)();
	} callbacks[delay_buffer_size];

} extern AsyncHandler;

#endif