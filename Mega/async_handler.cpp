#include "async_handler.h"
#include "shared_types.h"
#include "shared_utils.h"

struct AsyncHandler AsyncHandler = {};

void AsyncHandler::addCallback(void(*func_to_add)(), millis_t millis_delay) {
	if (func_to_add == nullptr) return;
	size_t first_null = delay_buffer_size;
	// determine time to call function
	millis_t delayed_time = millis() + millis_delay;
	// find a slot for the function in the buffer
	for (size_t i = 0; i < delay_buffer_size; i++) {
		auto& callback = callbacks[i];
		// null is a free slot
		if (callback.callback_function == nullptr) {
			if (first_null > i) first_null = i;
		}
		// no duplicate callbacks
		else if (callback.callback_function == func_to_add) {
			return;
		};
	};
	// update next invocation time if it would be lower now
	if (delayed_time < next_invoke) next_invoke = delayed_time;
	// place callback in the empty slot
	callbacks[first_null].callback_function = func_to_add;
	callbacks[first_null].delay_until = delayed_time;
}

// removes callback and updates next_invoke
void AsyncHandler::removeCallback(void(*func_to_remove)()) {
	if (func_to_remove == nullptr) return;
	bool update_next_invoke = false;
	millis_t temp_next_invoke = millis_t_max;
	for (size_t i = 0; i < delay_buffer_size; i++) {
		auto& callback = callbacks[i];
		if (callback.callback_function == func_to_remove) {
			// determine if the next invocation time will change on removal
			update_next_invoke |= callback.delay_until <= next_invoke;
			callback = { 0, nullptr };
		}
		// determine the next-lowest invocation delay
		else if (callback.callback_function != nullptr && callback.delay_until < temp_next_invoke) {
			temp_next_invoke = callback.delay_until;
		};
	};
	if (update_next_invoke) {
		next_invoke = temp_next_invoke;
	};
}

void AsyncHandler::processLoop() {
	auto start_time = millis();
	if (start_time < next_invoke) return;
	// cache to prevent callbacks overwriting eachother in the loop
	void(*pending_callbacks[delay_buffer_size])();
	bool has_any_callbacks = false;
	// populate cache with callbacks to be called this iteration
	for (size_t i = 0; i < delay_buffer_size; i++) {
		auto func = callbacks[i].callback_function;
		has_any_callbacks |= func != nullptr;
		if (func != nullptr && millis() > callbacks[i].delay_until) {
			pending_callbacks[i] = func;
			this->removeCallback(func);
		}
		// ensure this is set to something as we read it later
		else pending_callbacks[i] = nullptr;
	}
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (pending_callbacks[i] != nullptr) {
			pending_callbacks[i]();
		};
	};
	if (!has_any_callbacks) {
		// this indicates a serious issue
		Serial.println("No callbacks registered!");
	}
	else if (next_invoke - start_time > 10000) {
		// this indicates a notable but less serious issue
		Serial.println("More than 10s between async runs!");
	};
	if (millis() - start_time > 2000) {
		// may be problematic
		Serial.println("Loop run took more than 2s!");
	};
	//Serial.println(next_invoke);
}
