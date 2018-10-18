#include "async_handler.h"

Chrono AsyncHandler::chrono = Chrono(Chrono::MILLIS);

AsyncHandler::AsyncHandler() : next_invoke(chrono_t_max) {
	for (size_t i = 0; i < delay_buffer_size; i++) {
		callbacks[i] = { 0, nullptr };
	}
}

void AsyncHandler::addCallback(void(*callback)(), Chrono::chrono_t millis_delay) {
	if (callback == nullptr) return;
	size_t first_null = delay_buffer_size;
	Chrono::chrono_t delayed_time = chrono.elapsed() + millis_delay;
	if (delayed_time < next_invoke) next_invoke = delayed_time;
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (callbacks[i].callback_function == nullptr) {
			if (first_null > i) first_null = i;
		}
		else if(callbacks[i].callback_function == callback) {
			callbacks[i].invoke_at = delayed_time;
			return;
		}
	}
	callbacks[first_null].callback_function = callback;
	callbacks[first_null].invoke_at = delayed_time;
}

void AsyncHandler::processLoop() {
	static void(*pending_callbacks[delay_buffer_size])();
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (chrono.hasPassed(callbacks[i].invoke_at)) {
			pending_callbacks[i] = callbacks[i].callback_function;
			callbacks[i] = { 0, nullptr };
		}
		else {
			pending_callbacks[i] = nullptr;
		}
	}
	next_invoke = chrono_t_max;
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (pending_callbacks[i] == nullptr) continue;
		else pending_callbacks[i]();
	}
}