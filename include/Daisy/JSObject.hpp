/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_JSOBJECT_HPP_
#define _DAISY_JSOBJECT_HPP_

#include "Daisy/JSValue.hpp"
#include "Daisy/JSClass.hpp"
#include <vector>
#include <functional>

namespace Daisy {

	class JSObject;

	class DAISY_EXPORT JSObject final : public JSValue {
	public:
		virtual JSObject CallAsConstructor(const std::vector<JSValue>&  arguments) final;
		virtual JSValue  CallAsFunction(const std::vector<JSValue>&  arguments, JSObject this_object) final;
		virtual JSValue  operator()(const std::vector<JSValue>&  arguments, JSObject this_object) final;
		virtual JSValue  operator()(JSObject this_object) final;
		virtual JSValue  operator()() final;

		virtual bool IsFunction() const DAISY_NOEXCEPT final {
			return js_api_value__.type == JERRY_API_DATA_TYPE_OBJECT && jerry_api_is_function(js_api_value__.v_object);
		}

		virtual bool IsConstructor() const DAISY_NOEXCEPT final {
			return js_api_value__.type == JERRY_API_DATA_TYPE_OBJECT && jerry_api_is_constructor(js_api_value__.v_object);
		}

		virtual void SetPrivate(const std::uintptr_t& native_ptr, const JSObjectFinalizeCallback finalize_callback);

		JSObject(const JSContext&)     DAISY_NOEXCEPT;
		JSObject(const JSContext&, const JSClass&)     DAISY_NOEXCEPT;
		virtual ~JSObject()            DAISY_NOEXCEPT;
		JSObject(const JSObject&)      DAISY_NOEXCEPT;
		JSObject(JSObject&&)           DAISY_NOEXCEPT;
		JSObject& operator=(JSObject)  DAISY_NOEXCEPT;
		void swap(JSObject&)           DAISY_NOEXCEPT;

		// Silence 4251 on Windows since private member variables do not
		// need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
		static std::unordered_map<std::uintptr_t, JSObjectFinalizeCallback> js_object_finalizeCallback_map__;
#pragma warning(pop)

	protected:
		friend JSContext;
		friend JSValue;
		JSObject(const JSContext& js_context, const jerry_api_value_t& js_api_value) DAISY_NOEXCEPT;
		JSObject(const JSContext& js_context, jerry_api_object_t* js_api_object) DAISY_NOEXCEPT;

		static jerry_api_value_t MakeObject() DAISY_NOEXCEPT;
		static jerry_api_value_t MakeObject(jerry_api_object_t* js_api_object) DAISY_NOEXCEPT;

		// Prevent heap based objects.
		void* operator new(std::size_t)     = delete; // #1: To prevent allocation of scalar objects
		void* operator new [] (std::size_t) = delete; // #2: To prevent allocation of array of objects
		
#undef  DAISY_JSOBJECT_LOCK_GUARD
#undef  DAISY_JSOBJECT_LOCK_GUARD_STATIC
#ifdef  DAISY_THREAD_SAFE
		std::recursive_mutex mutex__;
		static std::recursive_mutex mutex_static__;
#define DAISY_JSOBJECT_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#define DAISY_JSOBJECT_LOCK_GUARD_STATIC std::lock_guard<std::recursive_mutex> lock_static(JSObject::mutex_static__)
#else
#define DAISY_JSOBJECT_LOCK_GUARD
#define DAISY_JSOBJECT_LOCK_GUARD_STATIC
#endif  // DAISY_THREAD_SAFE	

	};

} // namespace Daisy {

#endif // _DAISY_JSOBJECT_HPP_
