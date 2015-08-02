/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */
#include "Daisy/JSString.hpp"
#include <cassert>

namespace Daisy {

	jerry_api_value_t JSString::MakeString(const std::string& value) DAISY_NOEXCEPT {
		jerry_api_value_t js_api_value;
		js_api_value.type = JERRY_API_DATA_TYPE_STRING;
		js_api_value.v_string = jerry_api_create_string(reinterpret_cast<const jerry_api_char_t *>(value.c_str()));
		return js_api_value;
	}

	std::string JSString::ToString(const jerry_api_value_t& js_api_value) DAISY_NOEXCEPT {
		assert(js_api_value.type == JERRY_API_DATA_TYPE_STRING);
		ssize_t neg_size = jerry_api_string_to_char_buffer(js_api_value.v_string, NULL, 0);
		char*   buffer = static_cast<char*>(calloc(-neg_size, sizeof(char)));
		jerry_api_string_to_char_buffer(js_api_value.v_string, reinterpret_cast<jerry_api_char_t *>(buffer), -neg_size);
		const std::string result = buffer;
		free(buffer);
		return result;
	}

	JSString::JSString(const JSContext& js_context, const jerry_api_value_t& js_api_value) DAISY_NOEXCEPT
		: JSValue(js_context, js_api_value)
		, string__(ToString(js_api_value)) {
		std::hash<std::string> hash_function = std::hash<std::string>();
		hash_value__ = hash_function(static_cast<std::string>(string__));
	}

	JSString::JSString(const JSContext& js_context, const std::string& value) DAISY_NOEXCEPT
		: JSValue(js_context, MakeString(value))
		, string__(value) {
		std::hash<std::string> hash_function = std::hash<std::string>();
		hash_value__ = hash_function(static_cast<std::string>(string__));
	}

	const std::size_t JSString::length() const  DAISY_NOEXCEPT {
		DAISY_JSSTRING_LOCK_GUARD;
		return -jerry_api_string_to_char_buffer(js_api_value__.v_string, NULL, 0);
	}

	const std::size_t JSString::size() const DAISY_NOEXCEPT {
		return length();
	}

	const bool JSString::empty() const DAISY_NOEXCEPT {
		return length() == 0;
	}

	JSString::operator std::string() const DAISY_NOEXCEPT {
		return string__;
	}

	std::size_t JSString::hash_value() const {
		return hash_value__;
	}

	JSString::~JSString() DAISY_NOEXCEPT {
	}

	JSString::JSString(const JSString& rhs) DAISY_NOEXCEPT
		: JSValue(rhs.js_context__, rhs.js_api_value__)
		, string__(rhs.string__)
		, hash_value__(rhs.hash_value__) {
	}

	JSString::JSString(JSString&& rhs) DAISY_NOEXCEPT
		: JSValue(rhs.js_context__, rhs.js_api_value__)
		, string__(std::move(rhs.string__))
		, hash_value__(std::move(rhs.hash_value__)) {
	}

	JSString& JSString::operator=(JSString rhs) DAISY_NOEXCEPT {
		DAISY_JSSTRING_LOCK_GUARD;
		swap(rhs);
		return *this;
	}

	void JSString::swap(JSString& other) DAISY_NOEXCEPT {
		DAISY_JSSTRING_LOCK_GUARD;
		std::swap(js_api_value__,  other.js_api_value__);
		std::swap(string__       , other.string__);
		std::swap(hash_value__   , other.hash_value__);
	}

	bool operator==(const JSString& lhs, const JSString& rhs) {
		return (lhs.string__ == rhs.string__);
	}

} // namespace Daisy {
