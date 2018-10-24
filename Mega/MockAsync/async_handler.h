#pragma once
#ifndef ASYNC_HANDLER_H
#define ASYNC_HANDLER_H
#include <Arduino.h>
#include <Chrono.h>
#include "../shared_types.h"
#include "../shared_utils.h"

static class AsyncHandler {
public:

	AsyncHandler();

	void addCallback(void(*callback)(), Chrono::chrono_t delay = 0);

	void processLoop();

private:

	static constexpr size_t delay_buffer_size = 10;

	static Chrono chrono; // technically will need to be reset every 24 days
	Chrono::chrono_t next_invoke;
	struct DelayedCallback {
		Chrono::chrono_t invoke_at;
		void (*callback_function)();
	} callbacks[delay_buffer_size];

} AsyncHandler;

#endif