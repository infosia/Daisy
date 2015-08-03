/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_DETAIL_JSUTIL_HPP_
#define _DAISY_DETAIL_JSUTIL_HPP_

#include "Daisy/detail/JSBase.hpp"
#include "jerry.h"
#include <vector>

namespace Daisy {
	class JSValue;
	class JSContext;
	namespace detail {
		DAISY_EXPORT std::vector<jerry_api_value_t> to_vector(const std::vector<JSValue>&);
		DAISY_EXPORT std::vector<JSValue> to_vector(const JSContext& js_context, const jerry_api_value_t[], const jerry_api_length_t);
	} // namespace detail {
} // namespace Daisy {

#endif // _DAISY_DETAIL_JSUTIL_HPP_