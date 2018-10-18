#pragma once
#ifndef SHARED_UTILS_H
#define SHARED_UTILS_H
#include <Arduino.h>
#include <Chrono.h>
#include "shared_types.h"
// commonly re-used cross-module code signatures
#include <stdint.h>

static constexpr Chrono::chrono_t chrono_t_max = ~(Chrono::chrono_t)0;

#endif