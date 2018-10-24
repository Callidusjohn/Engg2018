#include "shared_types.h"


uint8_t& CanQuantities::quantityOf(CanType type) noexcept {
	switch (type) {
	case CanType::red: return this->red;
	case CanType::green: return this->green;
	case CanType::blue: return this->blue;
		//definite error otherwise
	};
}