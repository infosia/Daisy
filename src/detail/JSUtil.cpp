/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */
#include "Daisy/detail/JSUtil.hpp"
#include "Daisy/JSValue.hpp"
#include "Daisy/JSContext.hpp"
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

	std::vector<JSValue> to_vector(const JSContext& js_context, const jerry_api_value_t arguments[], const jerry_api_length_t argumentCount) {
		std::vector<JSValue> js_value_vector;
		for (size_t i = 0; i < argumentCount; i++) {
			js_value_vector.push_back(JSValue(js_context, arguments[i]));
		}
		return js_value_vector;
	}

	void js_jerry_api_value_make_copy(const jerry_api_value_t& from, jerry_api_value_t* to) {
		to->type   = from.type;
		if (from.type == JERRY_API_DATA_TYPE_BOOLEAN) {
			to->v_bool = from.v_bool;
		} else if (from.type == JERRY_API_DATA_TYPE_UINT32) {
			to->v_uint32  = from.v_uint32;
		} else if (from.type == JERRY_API_DATA_TYPE_FLOAT32) {
			to->v_float32 = from.v_float32;
		} else if (from.type == JERRY_API_DATA_TYPE_FLOAT64) {
			to->v_float64 = from.v_float64;
		} else if (from.type == JERRY_API_DATA_TYPE_STRING) {
			to->v_string  = from.v_string;
		} else if (from.type == JERRY_API_DATA_TYPE_OBJECT) {
			to->v_object  = from.v_object;
		}
	}

}} // namespace Daisy { namespace detail {