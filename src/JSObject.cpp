/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSObject.hpp"
#include "Daisy/detail/JSUtil.hpp"
#include <cassert>

namespace Daisy {

	jerry_api_value_t JSObject::MakeObject() DAISY_NOEXCEPT {
		return MakeObject(jerry_api_create_object());
	}

	jerry_api_value_t JSObject::MakeObject(jerry_api_object_t* js_api_object) DAISY_NOEXCEPT {
		jerry_api_value_t js_api_value;
		js_api_value.type = JERRY_API_DATA_TYPE_OBJECT;
		js_api_value.v_object = js_api_object;
		return js_api_value;
	}

	std::unordered_map<std::uintptr_t, JSObjectFinalizeCallback> JSObject::js_object_finalizeCallback_map__;

	static void js_object_finalize_callback(std::uintptr_t native_ptr) {
		const auto position = JSObject::js_object_finalizeCallback_map__.find(native_ptr);
		const bool found    = position != JSObject::js_object_finalizeCallback_map__.end();
		assert(found);
		position->second(native_ptr);
		JSObject::js_object_finalizeCallback_map__.erase(native_ptr);
	}

	void JSObject::SetPrivate(const std::uintptr_t& native_ptr, const JSObjectFinalizeCallback finalize_callback) {
		const auto position = js_object_finalizeCallback_map__.find(native_ptr);
		const bool found    = position != js_object_finalizeCallback_map__.end();

		assert(!found);
		js_object_finalizeCallback_map__.emplace(native_ptr, finalize_callback);

		jerry_api_set_object_native_handle(js_api_value__.v_object, native_ptr, js_object_finalize_callback);
	}

	JSObject::JSObject(const JSContext& js_context) DAISY_NOEXCEPT 
		: JSValue(js_context, MakeObject()) {
	}

	JSObject::JSObject(const JSContext& js_context, const JSClass& js_class) DAISY_NOEXCEPT 
		: JSValue(js_context, MakeObject()) {
		auto js_object = JSObject(js_context__, js_api_value__);
		js_class.JSObjectInitializeCallback(js_context, js_object);
	}

	JSObject::~JSObject() DAISY_NOEXCEPT {
	}
	
	JSObject::JSObject(const JSObject& rhs) DAISY_NOEXCEPT 
		: JSValue(rhs.js_context__, rhs.js_api_value__) {
	}
	
	JSObject::JSObject(JSObject&& rhs) DAISY_NOEXCEPT
		: JSValue(rhs.js_context__, rhs.js_api_value__) {
	}

	JSObject::JSObject(const JSContext& js_context, const jerry_api_value_t& js_api_value) DAISY_NOEXCEPT 
		: JSValue(js_context, js_api_value) {
	}

	JSObject::JSObject(const JSContext& js_context, jerry_api_object_t* js_api_object) DAISY_NOEXCEPT 
		: JSValue(js_context, MakeObject(js_api_object)) {
	}

	JSObject JSObject::CallAsConstructor(const std::vector<JSValue>&  arguments) {
		DAISY_JSOBJECT_LOCK_GUARD;
		assert(IsConstructor());
		bool status = false;
		jerry_api_value_t js_api_value;
		if (arguments.empty()) {
			status = jerry_api_construct_object(js_api_value__.v_object, &js_api_value, NULL, 0);
		} else {
			const auto arguments_array = detail::to_vector(arguments);
			status = jerry_api_construct_object(js_api_value__.v_object, &js_api_value, &arguments_array[0], arguments_array.size());
		}
		if (!status) {
			// TODO: throw runtime exception
		}
		return JSObject(js_context__, js_api_value);
	}

	JSValue JSObject::CallAsFunction(const std::vector<JSValue>&  arguments, JSObject this_object) {
		DAISY_JSOBJECT_LOCK_GUARD;
		assert(this_object.IsObject());
		assert(IsFunction());
		bool status = false;
		jerry_api_value_t js_api_value;
		if (arguments.empty()) {
			status = jerry_api_call_function(static_cast<jerry_api_value_t>(this_object).v_object, js_api_value__.v_object, &js_api_value, NULL, 0);
		} else {
			const auto arguments_array = detail::to_vector(arguments);
			status = jerry_api_call_function(static_cast<jerry_api_value_t>(this_object).v_object, js_api_value__.v_object, &js_api_value, &arguments_array[0], arguments_array.size());
		}
		if (!status) {
			// TODO: throw runtime exception
		}
		return JSValue(js_context__, js_api_value);
	}

	JSValue JSObject::operator()(const std::vector<JSValue>&  arguments, JSObject this_object) {
		return CallAsFunction(arguments, this_object);
	}

	JSValue JSObject::operator()(JSObject this_object) {
		return CallAsFunction(std::vector<JSValue>(), this_object);
	}

	JSValue JSObject::operator()() {
		return CallAsFunction(std::vector<JSValue>(), *this);
	}
	
	JSObject& JSObject::operator=(JSObject rhs) DAISY_NOEXCEPT {
		DAISY_JSOBJECT_LOCK_GUARD;
		swap(rhs);
		return *this;
	}
	
	void JSObject::swap(JSObject& other) DAISY_NOEXCEPT {
		DAISY_JSOBJECT_LOCK_GUARD;
	}
}
