/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */
#include "Daisy/detail/JSUtil.hpp"
#include "Daisy/JSValue.hpp"
#include <algorithm>

namespace Daisy { namespace detail {

	std::vector<jerry_api_value_t> to_vector(const std::vector<JSValue>& js_value_vector) {
		std::vector<jerry_api_value_t> js_value_ref_vector;
		std::transform(js_value_vector.begin(),
		               js_value_vector.end(),
		               std::back_inserter(js_value_ref_vector),
		               [](const JSValue& js_value) { return static_cast<jerry_api_value_t>(js_value); });
		return js_value_ref_vector;
	}

}} // namespace Daisy { namespace detail {