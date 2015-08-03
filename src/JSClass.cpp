/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSClass.hpp"
#include "Daisy/JSObject.hpp"
#include "Daisy/detail/JSUtil.hpp"
#include <cassert>
#include <iostream>

namespace Daisy {
	
	JSClass::JSClass() DAISY_NOEXCEPT {
	}
	
	JSClass::~JSClass() DAISY_NOEXCEPT {
	}
	
	JSClass::JSClass(const JSClass& rhs) DAISY_NOEXCEPT 
		: prototype_functions_map__(rhs.prototype_functions_map__) {
	}
	
	JSClass::JSClass(JSClass&& rhs) DAISY_NOEXCEPT
		: prototype_functions_map__(rhs.prototype_functions_map__) {
	}

	void JSClass::JSObjectInitializeCallback(const JSContext& js_context, JSObject& this_object) const {
		// TODO: set static property here, because "prototype" is not defined
		for (const auto v : prototype_functions_map__) {
			this_object.SetProperty(v.first, JSObjectMakeFunctionWithCallback(js_context, v.first, v.second));
		}
	}

	static void js_jerry_api_value_make_copy(const jerry_api_value_t& from, jerry_api_value_t* to) {
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

	static bool js_object_external_function_callback(
				const jerry_api_object_t *function_object_ptr,
				const jerry_api_value_t *this_object_ptr,
				jerry_api_value_t *result_value_ptr,
				const jerry_api_value_t js_api_arguments[],
				const jerry_api_length_t argumentCount) {

		const auto position = JSObject::js_object_external_functions_map__.find(function_object_ptr);
		assert(position != JSObject::js_object_external_functions_map__.end());

		auto callback = position->second;

		// TODO: Use current context
		JSContextGroup js_context_group;
		const auto js_context = js_context_group.CreateContext();
		auto function_object = JSObject(js_context, function_object_ptr);
		auto this_object     = JSObject(js_context, *this_object_ptr);
		const auto arguments = detail::to_vector(js_context, js_api_arguments, argumentCount);

		auto callback_result = callback(function_object, this_object, arguments);
		auto callback_result_value = static_cast<jerry_api_value_t>(callback_result);

		// mark it temporary to avoid release at dtor.
		callback_result.temporary();

		js_jerry_api_value_make_copy(callback_result_value, result_value_ptr);

		return true;
	}

	JSObject JSClass::JSObjectMakeFunctionWithCallback(const JSContext& js_context, const std::string& name, JSObjectCallAsFunctionCallback callback) const {
		auto js_api_object = jerry_api_create_external_function(js_object_external_function_callback);

		auto function_object = JSObject(js_context, js_api_object);

		const auto position = JSObject::js_object_external_functions_map__.find(js_api_object);
		assert(position == JSObject::js_object_external_functions_map__.end());
		JSObject::js_object_external_functions_map__.emplace(js_api_object, callback);

		return function_object;
	}

	void JSClass::AddFunctionProperty(const std::string& name, JSObjectCallAsFunctionCallback callback) {
		const auto position = prototype_functions_map__.find(name);
		assert(position == prototype_functions_map__.end());
		prototype_functions_map__.emplace(name, callback);
	}
	
	JSClass& JSClass::operator=(JSClass rhs) DAISY_NOEXCEPT {
		DAISY_JSCLASS_LOCK_GUARD;
		swap(rhs);
		return *this;
	}
	
	void JSClass::swap(JSClass& other) DAISY_NOEXCEPT {
		DAISY_JSCLASS_LOCK_GUARD;
		std::swap(prototype_functions_map__, other.prototype_functions_map__);
	}
	
} // namespace Daisy {
