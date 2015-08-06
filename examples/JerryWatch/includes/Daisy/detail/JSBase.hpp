/**
 * The Daisy Library
 *
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_DETAIL_JSBASE_HPP_
#define _DAISY_DETAIL_JSBASE_HPP_

#define DAISY_NOEXCEPT_ENABLE
#define DAISY_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE

#if defined(_MSC_VER) && _MSC_VER <= 1800
#undef DAISY_NOEXCEPT_ENABLE
#undef DAISY_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
#endif // #defined(_MSC_VER) && _MSC_VER <= 1800

#ifdef DAISY_NOEXCEPT_ENABLE
#define DAISY_NOEXCEPT noexcept
#else
#define DAISY_NOEXCEPT
#endif

#ifdef DAISY_THREAD_SAFE
#include <mutex>
#endif

#include "DAISY_EXPORT.h"
#include <utility>
#include <cstdint>
#include <string>

#endif  // _DAISY_DETAIL_JSBASE_HPP_
