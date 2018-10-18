#include "async_handler.h"

Chrono AsyncHandler::chrono = Chrono(Chrono::MILLIS);

AsyncHandler::AsyncHandler() : next_invoke(~(Chrono::chrono_t)0) {
	for (size_t i = 0; i < delay_buffer_size; i++) {
		registered_callbacks[i] = { 0, nullptr };
		pending_callbacks[i] = { 0, nullptr };
	}
}

void AsyncHandler::addCallback(Chrono::chrono_t millis_delay, void(*callback)()) {
	if (callback == nullptr) return;
	size_t first_null = delay_buffer_size;
	Chrono::chrono_t delayed_time = chrono.elapsed() + millis_delay;
	if (delayed_time < next_invoke) next_invoke = delayed_time;
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (registered_callbacks[i].callback_function == nullptr) {
			if (first_null > i) first_null = i;
		}
		else if(registered_callbacks[i].callback_function == callback) {
			registered_callbacks[i].invoke_at = delayed_time;
			return;
		}
	}
	registered_callbacks[first_null].callback_function = callback;
	registered_callbacks[first_null].invoke_at = delayed_time;
}

void AsyncHandler::processLoop() {
	memcpy(pending_callbacks, registered_callbacks, sizeof(DelayedCallback)*delay_buffer_size);
	next_invoke = ~(Chrono::chrono_t)0;
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (pending_callbacks[i].callback_function == nullptr) continue;
		else if (chrono.hasPassed(pending_callbacks[i].invoke_at)) pending_callbacks[i].callback_function();
	}
}