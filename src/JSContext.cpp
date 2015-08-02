/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSContextGroup.hpp"
#include "Daisy/JSContext.hpp"
#include "Daisy/JSValue.hpp"
#include "Daisy/JSNumber.hpp"
#include "Daisy/JSBoolean.hpp"
#include "Daisy/JSString.hpp"
#include "Daisy/JSObject.hpp"
#include "Daisy/JSClass.hpp"

namespace Daisy {
	
	JSContext::~JSContext() DAISY_NOEXCEPT {

	}
	
	JSContext::JSContext(const JSContext& rhs) DAISY_NOEXCEPT 
		: js_context_group__(rhs.js_context_group__) {

	}
	
	JSContext::JSContext(JSContext&& rhs) DAISY_NOEXCEPT
		: js_context_group__(rhs.js_context_group__) {

	}

	JSContext::JSContext(const JSContextGroup& js_context_group) DAISY_NOEXCEPT 
		: js_context_group__(js_context_group) {

	}

	JSObject JSContext::get_global_object() const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		return JSObject(*this, jerry_api_get_global());		
	}

	JSValue JSContext::CreateUndefined() const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		jerry_api_value_t js_api_value;
		js_api_value.type = JERRY_API_DATA_TYPE_UNDEFINED;
		return JSValue(*this, js_api_value);
	}

	JSValue JSContext::CreateNull() const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		jerry_api_value_t js_api_value;
		js_api_value.type = JERRY_API_DATA_TYPE_NULL;
		return JSValue(*this, js_api_value);
	}	

	JSNumber JSContext::CreateNumber(const double& number) const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		return JSNumber(*this, number);
	}
	
	JSNumber JSContext::CreateNumber(const std::int32_t& number) const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		return JSNumber(*this, number);
	}
	
	JSNumber JSContext::CreateNumber(const std::uint32_t& number) const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		return JSNumber(*this, number);
	}

	JSBoolean JSContext::CreateBoolean(const bool& value) const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		return JSBoolean(*this, value);
	}

	JSString JSContext::CreateString(const std::string& value) const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		return JSString(*this, value);
	}

	JSObject JSContext::CreateObject() const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		return JSObject(*this);
	}

	JSObject JSContext::CreateObject(const JSClass& js_class) const DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		return JSObject(*this, js_class);
	}

	JSValue JSContext::JSEvaluateScript(const std::string& script) const {
		DAISY_JSCONTEXT_LOCK_GUARD;
		jerry_api_value_t ret_val;
		const auto status = jerry_api_eval(
			reinterpret_cast<const jerry_api_char_t *>(script.c_str()),
			script.size(), false, false, &ret_val);
		if (status == JERRY_COMPLETION_CODE_OK) {
			return JSValue(*this, ret_val);
		} else {
			// TODO: throw runtime exception
		}
		return CreateUndefined();
	}
	
	JSContext& JSContext::operator=(JSContext rhs) DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		swap(rhs);
		return *this;
	}
	
	void JSContext::swap(JSContext& other) DAISY_NOEXCEPT {
		DAISY_JSCONTEXT_LOCK_GUARD;
		std::swap(js_context_group__, other.js_context_group__);
	}
	
}
