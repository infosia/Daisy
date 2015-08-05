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

	void JSClass::ConstructorInitializeCallback(const JSContext& js_context, JSObject& this_object) const {
		if (!this_object.HasProperty("prototype")) {
			auto js_object = js_context.CreateObject();
			this_object.SetProperty("prototype", js_object);
		}

		auto proto_object = static_cast<JSObject>(this_object.GetProperty("prototype"));
		for (const auto v : prototype_functions_map__) {
			//
			// NOTE: On HAL, there's no difference between object "static" property and prototype property
			// so add it to both
			//
			this_object.SetProperty(v.first, JSObjectMakeFunctionWithCallback(js_context, v.first, v.second));
			proto_object.SetProperty(v.first, JSObjectMakeFunctionWithCallback(js_context, v.first, v.second));
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

		// callback value should be considered "unmanaged" from Daisy
		callback_result.unmanaged();

		detail::js_jerry_api_value_make_copy(callback_result_value, result_value_ptr);

		return true;
	}

	JSObjectCallAsConstructorCallback JSClass::getCallAsConstructorCallback() const {
		return nullptr;
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
