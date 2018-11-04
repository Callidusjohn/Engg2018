#pragma once
#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H
#include <Arduino.h>

enum RemoteError : char {

};



enum CanType : char {
	red, green, blue
};

typedef struct CanQuantities {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t& quantityOf(CanType type) noexcept;
} CanQuantities;
#endif
