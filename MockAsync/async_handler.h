#pragma once
#ifndef ASYNC_HANDLER_H
#define ASYNC_HANDLER_H
#include <Arduino.h>
#include <Chrono.h>
#include "../shared_types.h"

static class AsyncHandler {
public:

	AsyncHandler();

	void addCallback(Chrono::chrono_t delay, void(*callback)());

	void processLoop();

private:

	static constexpr size_t delay_buffer_size = 10;

	static Chrono chrono; // technically will need to be reset every 24 days
	Chrono::chrono_t next_invoke;
	struct DelayedCallback {
		Chrono::chrono_t invoke_at;
		void(*callback_function)();
	} registered_callbacks[delay_buffer_size], pending_callbacks[delay_buffer_size];

} AsyncHandler;

#endif