/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSValue.hpp"
#include "Daisy/JSString.hpp"
#include "Daisy/JSObject.hpp"
#include <cassert>
#include <sstream>

namespace Daisy {

	JSValue::~JSValue() DAISY_NOEXCEPT {
		release();
	}

	JSValue::JSValue(const JSValue& rhs) DAISY_NOEXCEPT
		: js_context__(rhs.js_context__)
		, js_api_value__(rhs.js_api_value__)
		, js_value_managed__(rhs.js_value_managed__) {
		retain();
	}

	JSValue::JSValue(JSValue&& rhs) DAISY_NOEXCEPT
		: js_context__(rhs.js_context__)
		, js_api_value__(rhs.js_api_value__)
		, js_value_managed__(rhs.js_value_managed__) {
		retain();
	}

	JSValue::JSValue(const JSContext& js_context, const jerry_api_value_t& js_api_value, const bool& managed) DAISY_NOEXCEPT
		: js_context__(js_context)
		, js_api_value__(js_api_value)
		, js_value_managed__(managed) {
		retain();
	}


	bool JSValue::IsGlobalObject() const DAISY_NOEXCEPT {
		return (js_api_value__.v_object == JSObject::js_api_global_object__);
	}

	JSValue::operator bool() const DAISY_NOEXCEPT {
		if (IsBoolean()) {
			return js_api_value__.v_bool;
		} else {
			return false;
		}
	}

	JSValue::operator double() const DAISY_NOEXCEPT {
		if (js_api_value__.type == JERRY_API_DATA_TYPE_UINT32) {
			return static_cast<double>(js_api_value__.v_uint32);
		} else if (js_api_value__.type == JERRY_API_DATA_TYPE_FLOAT32) {
			return static_cast<double>(js_api_value__.v_float32);
		} else if (js_api_value__.type == JERRY_API_DATA_TYPE_FLOAT64) {
			return static_cast<double>(js_api_value__.v_float64);
		} else {
			return 0.0;
		}
	}

	JSValue::operator std::int32_t() const DAISY_NOEXCEPT {
		return static_cast<std::int32_t>(operator double());
	}

	JSValue::operator std::uint32_t() const DAISY_NOEXCEPT {
		return static_cast<std::uint32_t>(operator double());
	}

	JSValue::operator std::string() const DAISY_NOEXCEPT {
		if (IsNull()) {
			return "null";
		} else if (IsUndefined()) {
			return "undefined";
		} else if (IsBoolean()) {
			return js_api_value__.v_bool ? "true" : "false";
		} else if (IsNumber()) {
			std::stringstream ss;
			const auto number = static_cast<double>(*this);
			ss << number;
			return ss.str();
		} else if (IsString()) {
			return static_cast<std::string>(JSString(js_context__, js_api_value__));
		}
		return ""; // unknown
	}

	JSValue::operator JSString() const DAISY_NOEXCEPT {
		return JSString(js_context__, js_api_value__);
	}

	JSValue::operator JSObject() const DAISY_NOEXCEPT {
		auto js_object = JSObject(js_context__, js_api_value__);
		js_object.js_value_managed__ = js_value_managed__;
		return js_object;
	}

	JSValue& JSValue::operator=(JSValue rhs) DAISY_NOEXCEPT {
		DAISY_JSVALUE_LOCK_GUARD;
		swap(rhs);
		return *this;
	}

	void JSValue::swap(JSValue& other) DAISY_NOEXCEPT {
		DAISY_JSVALUE_LOCK_GUARD;
		std::swap(js_context__, other.js_context__);
		std::swap(js_api_value__, other.js_api_value__);
		std::swap(js_value_managed__, other.js_value_managed__);
	}

	std::unordered_map<std::intptr_t, std::tuple<jerry_api_data_type_t, std::size_t>> JSValue::js_api_value_retain_count_map__;

	void JSValue::retain() {
		DAISY_JSVALUE_LOCK_GUARD_STATIC;

		std::intptr_t key;
		if (IsString()) {
			key = reinterpret_cast<std::intptr_t>(js_api_value__.v_string);
		} else if (IsObject()) {
			key = reinterpret_cast<std::intptr_t>(js_api_value__.v_object);
		} else {
			return;
		}
		const auto position = js_api_value_retain_count_map__.find(key);
		const bool found    = position != js_api_value_retain_count_map__.end();

		if (found) {
			auto tuple = position->second;
			++std::get<1>(tuple);
			js_api_value_retain_count_map__[key] = tuple;
		} else {
			const auto insert_result = js_api_value_retain_count_map__.emplace(key, std::make_tuple(js_api_value__.type, 1));
			const bool inserted      = insert_result.second;

			assert(inserted);
		}
	}

	void JSValue::release() {
		DAISY_JSVALUE_LOCK_GUARD_STATIC;

		std::intptr_t key;
		if (IsString()) {
			key = reinterpret_cast<std::intptr_t>(js_api_value__.v_string);
		} else if (IsObject()) {
			key = reinterpret_cast<std::intptr_t>(js_api_value__.v_object);
		} else {
			return;
		}

		const auto position = js_api_value_retain_count_map__.find(key);
		const bool found    = position != js_api_value_retain_count_map__.end();

		assert(found);

		if (found) {
			auto tuple = position->second;
			--std::get<1>(tuple);
			if (std::get<1>(tuple) == 0) {
				js_api_value_retain_count_map__.erase(key);
				const auto js_value_type = std::get<0>(tuple);
				if (js_value_type == JERRY_API_DATA_TYPE_STRING) {
					if (js_value_managed__) {
						jerry_api_release_string(reinterpret_cast<jerry_api_string_t*>(key));
					}
				} else if (js_value_type == JERRY_API_DATA_TYPE_OBJECT) {
					if (!IsGlobalObject()) {
						const auto api_object_ptr = reinterpret_cast<jerry_api_object_t*>(key);
						JSObject::js_object_external_functions_map__.erase(api_object_ptr);
						JSObject::js_object_external_constructors_map__.erase(api_object_ptr);
						JSObject::js_object_properties_map__.erase(api_object_ptr);
						if (js_value_managed__) {
							std::uintptr_t private_ptr;
							jerry_api_get_object_native_handle(api_object_ptr, &private_ptr);
							JSObject::FinalizePrivateData(private_ptr);
							jerry_api_release_object(api_object_ptr);
						}
					}
				}
			} else {
				js_api_value_retain_count_map__[key] = tuple;
			}
		}
	}
}
