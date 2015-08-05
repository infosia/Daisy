/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSObject.hpp"
#include "Daisy/detail/JSUtil.hpp"
#include <cassert>
#include <iostream>

namespace Daisy {

	std::unordered_map<const jerry_api_object_t*, JSObjectCallAsConstructorCallback> JSObject::js_object_external_constructors_map__;
	std::unordered_map<const jerry_api_object_t*, JSObjectCallAsFunctionCallback> JSObject::js_object_external_functions_map__;
	std::unordered_map<std::uintptr_t, JSObjectFinalizeCallback> JSObject::js_object_finalizeCallback_map__;
	std::unordered_map<std::uintptr_t, const jerry_api_object_t*> JSObject::js_private_data_to_js_object_ref_map__;

	static bool js_api_object_constructor_function(
				const jerry_api_object_t *function_object_ptr,
				const jerry_api_value_t *this_object_ptr,
				jerry_api_value_t *result_value_ptr,
				const jerry_api_value_t js_api_arguments[],
				const jerry_api_length_t argumentCount) {

		const auto position = JSObject::js_object_external_constructors_map__.find(function_object_ptr);
		assert(position != JSObject::js_object_external_constructors_map__.end());

		auto callback = position->second;

		// TODO: Use current context
		JSContextGroup js_context_group;
		const auto js_context = js_context_group.CreateContext();
		const auto arguments = detail::to_vector(js_context, js_api_arguments, argumentCount);

		JSObject this_object = JSObject(js_context, this_object_ptr->v_object, false);

		callback(js_context, this_object, arguments);

		return true;
	}


	jerry_api_value_t JSObject::MakeObject() DAISY_NOEXCEPT {
		return MakeObject(jerry_api_create_object());
	}

	jerry_api_value_t JSObject::MakeConstructorObject(const JSClass& js_class) DAISY_NOEXCEPT {
		auto js_api_object = MakeObject(jerry_api_create_external_function(js_api_object_constructor_function));

		const auto position = JSObject::js_object_external_constructors_map__.find(js_api_object.v_object);
		assert(position == JSObject::js_object_external_constructors_map__.end());
		JSObject::js_object_external_constructors_map__.emplace(js_api_object.v_object, js_class.getCallAsConstructorCallback());

		return js_api_object;
	}

	jerry_api_value_t JSObject::MakeObject(const jerry_api_object_t* js_api_object) DAISY_NOEXCEPT {
		jerry_api_value_t js_api_value;
		js_api_value.type = JERRY_API_DATA_TYPE_OBJECT;
		js_api_value.v_object = const_cast<jerry_api_object_t*>(js_api_object);
		return js_api_value;
	}

	static void js_object_finalize_callback(std::uintptr_t native_ptr) {
		const auto position = JSObject::js_object_finalizeCallback_map__.find(native_ptr);
		const bool found    = position != JSObject::js_object_finalizeCallback_map__.end();
		assert(found);
		position->second(native_ptr);
		JSObject::js_object_finalizeCallback_map__.erase(native_ptr);
		JSObject::js_private_data_to_js_object_ref_map__.erase(native_ptr);
	}

	std::uintptr_t JSObject::GetPrivate() const {
		std::uintptr_t handle;
		jerry_api_get_object_native_handle(js_api_value__.v_object, &handle);
		return handle;
	}

	void JSObject::SetPrivate(const std::uintptr_t& native_ptr, const JSObjectFinalizeCallback finalize_callback) {
		const auto position = js_object_finalizeCallback_map__.find(native_ptr);
		const bool found    = position != js_object_finalizeCallback_map__.end();

		assert(!found);
		js_object_finalizeCallback_map__.emplace(native_ptr, finalize_callback);

		jerry_api_set_object_native_handle(js_api_value__.v_object, native_ptr, js_object_finalize_callback);

		assert(js_private_data_to_js_object_ref_map__.find(native_ptr) == js_private_data_to_js_object_ref_map__.end());
		js_private_data_to_js_object_ref_map__.emplace(native_ptr, js_api_value__.v_object);
	}

	JSObject JSObject::FindJSObjectFromPrivateData(const JSContext& js_context, const std::uintptr_t& native_ptr) {
		const auto position = js_private_data_to_js_object_ref_map__.find(native_ptr);
		const bool found    = position != js_private_data_to_js_object_ref_map__.end();

		assert(found);

		return JSObject(js_context, position->second);
	}

	bool JSObject::HasProperty(const std::string& name) const {
		jerry_api_value_t js_value;
		if (jerry_api_get_object_field_value(js_api_value__.v_object, reinterpret_cast<const jerry_api_char_t *>(name.c_str()), &js_value)) {
			const auto has_property = (js_value.type != JERRY_API_DATA_TYPE_UNDEFINED);
			jerry_api_release_value(&js_value);
			return has_property;
		}
		return false;
	}

	JSValue JSObject::GetProperty(const std::string& name) const {
		jerry_api_value_t js_value;
		if (jerry_api_get_object_field_value(js_api_value__.v_object, reinterpret_cast<const jerry_api_char_t *>(name.c_str()), &js_value)) {
			jerry_api_release_value(&js_value);
			return JSValue(js_context__, js_value);
		}
		return js_context__.CreateUndefined();
	}

	void JSObject::SetProperty(const std::string& name, JSValue js_value) {
		auto value = static_cast<jerry_api_value_t>(js_value);
		jerry_api_set_object_field_value(js_api_value__.v_object, reinterpret_cast<const jerry_api_char_t *>(name.c_str()), &value);
		if (js_object_properties_map__.find(name) != js_object_properties_map__.end()) {
			js_object_properties_map__.erase(name);
		}
		js_object_properties_map__.emplace(name, js_value); // to prevent from GC
	}

	JSObject::JSObject(const JSContext& js_context) DAISY_NOEXCEPT 
		: JSValue(js_context, MakeObject()) {
	}

	JSObject::JSObject(const JSContext& js_context, const JSClass& js_class) DAISY_NOEXCEPT 
		: JSValue(js_context, MakeConstructorObject(js_class)) {
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

	JSObject::JSObject(const JSContext& js_context, const jerry_api_object_t* js_api_object, const bool& managed) DAISY_NOEXCEPT 
		: JSValue(js_context, MakeObject(js_api_object), managed) {
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
		js_api_value.type = JERRY_API_DATA_TYPE_UNDEFINED;
		if (arguments.empty()) {
			status = jerry_api_call_function(js_api_value__.v_object, static_cast<jerry_api_value_t>(this_object).v_object, &js_api_value, NULL, 0);
		} else {
			const auto arguments_array = detail::to_vector(arguments);
			status = jerry_api_call_function(js_api_value__.v_object, static_cast<jerry_api_value_t>(this_object).v_object, &js_api_value, &arguments_array[0], arguments_array.size());
		}
		if (!status) {
			std::cout << "[ERROR JSObject::CallAsFunction FAILED" << std::endl;
		}
		return JSValue(js_context__, js_api_value);
	}

	std::vector<std::string> JSObject::GetPropertyNames() const DAISY_NOEXCEPT {
		DAISY_JSOBJECT_LOCK_GUARD;
		// TODO:
		return std::vector<std::string>();	
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
}
