#include "async_handler.h"
#include "shared_utils.h"

AsyncHandler::DelayedCallback AsyncHandler::callbacks[delay_buffer_size];// = { { 0, nullptr },{ 0, nullptr }, { 0, nullptr }, { 0, nullptr }, { 0, nullptr }, { 0, nullptr }, { 0, nullptr }, { 0, nullptr }, { 0, nullptr }, { 0, nullptr }, };

AsyncHandler::AsyncHandler() : next_invoke(chrono_t_max) {
	//TODO: make sure that this isn't getting called twice for some reason
	//not required due to default initialization
	//for (size_t i = 0; i < delay_buffer_size; i++) {
	//	callbacks[i] = { 0, nullptr };
	//}
}

void AsyncHandler::addCallback(void(*callback)(), Chrono::chrono_t millis_delay) {
	//Serial.println("Handler adding callback");
	if (callback == nullptr) {
		//Serial.println("This is technically impossible if we've done this well");
		return;
	}
	size_t first_null = delay_buffer_size;
	Chrono::chrono_t delayed_time = millis() + millis_delay;
	if (delayed_time < next_invoke) next_invoke = delayed_time;
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (callbacks[i].callback_function == nullptr) {
			if (first_null > i) {
				first_null = i;
				//Serial.print("First null at ");
				//Serial.println(first_null);
			}
		}
		else if(callbacks[i].callback_function == callback) {
			//Serial.print("Found existing callback, updating time to ");
			//Serial.println(delayed_time);
			callbacks[i].invoke_at = delayed_time;
			return;
		}
	}
	callbacks[first_null].callback_function = callback;
	callbacks[first_null].invoke_at = delayed_time;
	if (callbacks[first_null].callback_function == nullptr) {
		//Serial.println("wtf");
	}
	//Serial.println("Finished adding");
	return;
}

void AsyncHandler::removeCallback(void(*callback)()) {
	//Serial.println("Handler removing callback");
	if (callback == nullptr) return;
	bool update_next_invoke = false;
	Chrono::chrono_t temp_next_invoke = chrono_t_max;
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (callbacks[i].callback_function == callback) {
			if (callbacks[i].invoke_at <= next_invoke) {
				update_next_invoke = true;
			}
			callbacks[i] = { 0, nullptr };
		}
		else if(callbacks[i].callback_function != nullptr && callbacks[i].invoke_at < temp_next_invoke) {
			temp_next_invoke = callbacks[i].invoke_at;
		}
	}
	if (update_next_invoke) {
		next_invoke = temp_next_invoke;
	}
}

void AsyncHandler::processLoop() {
	void(*pending_callbacks[delay_buffer_size])();
	//Serial.println("Handler processing loop");
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (callbacks[i].callback_function != nullptr) {
		//	Serial.println("There is indeed a function");
		}
		if (millis() > callbacks[i].invoke_at) {
			pending_callbacks[i] = callbacks[i].callback_function;
			callbacks[i] = { 0, nullptr };
			next_invoke = chrono_t_max;
		}
		else {
			pending_callbacks[i] = nullptr;
		}
	}
	for (size_t i = 0; i < delay_buffer_size; i++) {
		if (pending_callbacks[i] == nullptr) continue;
		else pending_callbacks[i]();
	}
	//delay(50);
	//auto halt_millis = millis();
	//if (halt_millis < next_invoke) delay(next_invoke - halt_millis);
	//for (size_t i = 0; i < delay_buffer_size; i++) {
	//	if (callbacks[i].callback_function != nullptr) {
	//		//Serial.println("Callback added during processing");
	//	}
	//}
	//Serial.println("Loop processing finished");
}
