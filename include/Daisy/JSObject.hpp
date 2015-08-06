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
#include <memory>

namespace Daisy {

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

		template<typename T>
		std::shared_ptr<T> GetPrivate() const DAISY_NOEXCEPT;

		virtual std::uintptr_t GetPrivate() const;
		virtual void SetPrivate(const std::uintptr_t& native_ptr, const JSObjectFinalizeCallback finalize_callback);

		virtual bool HasProperty(const std::string& name) const;
		virtual JSValue GetProperty(const std::string& name) const;
		virtual void SetProperty(const std::string& name, JSValue js_value);
		virtual std::vector<std::string> GetPropertyNames() const DAISY_NOEXCEPT;

		JSObject(const JSContext&)     DAISY_NOEXCEPT;
		JSObject(const JSContext&, const JSClass&)     DAISY_NOEXCEPT;
		virtual ~JSObject()            DAISY_NOEXCEPT;
		JSObject(const JSObject&)      DAISY_NOEXCEPT;
		JSObject(JSObject&&)           DAISY_NOEXCEPT;
		JSObject& operator=(JSObject)  DAISY_NOEXCEPT;

		JSObject(const JSContext& js_context, const jerry_api_value_t& js_api_value) DAISY_NOEXCEPT;
		JSObject(const JSContext& js_context, const jerry_api_object_t* js_api_object, const bool& managed = true) DAISY_NOEXCEPT;

		static void FinalizePrivateData(std::uintptr_t native_ptr);
		static JSObject FindJSObjectFromPrivateData(const JSContext& js_context, const std::uintptr_t& native_ptr);

		// Silence 4251 on Windows since private member variables do not
		// need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
		static std::unordered_map<std::uintptr_t, JSObjectFinalizeCallback> js_object_finalizeCallback_map__;
		static std::unordered_map<const jerry_api_object_t*, JSObjectCallAsFunctionCallback> js_object_external_functions_map__;
		static std::unordered_map<const jerry_api_object_t*, JSObjectCallAsConstructorCallback> js_object_external_constructors_map__;
		static std::unordered_map<std::uintptr_t, const jerry_api_object_t*> js_private_data_to_js_object_ref_map__;
		static std::unordered_map<const jerry_api_object_t*, std::unordered_map<std::string, JSValue>> js_object_properties_map__;
		static jerry_api_object_t* js_api_global_object__;
#pragma warning(pop)

	protected:
		static jerry_api_value_t MakeObject() DAISY_NOEXCEPT;
		static jerry_api_value_t MakeObject(const jerry_api_object_t* js_api_object) DAISY_NOEXCEPT;
		static jerry_api_value_t MakeConstructorObject(const JSClass& js_class) DAISY_NOEXCEPT;

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

	template<typename T>
	std::shared_ptr<T> JSObject::GetPrivate() const DAISY_NOEXCEPT {
		return std::shared_ptr<T>(std::make_shared<JSObject>(*this), reinterpret_cast<T*>(GetPrivate()));
	}

} // namespace Daisy {

#endif // _DAISY_JSOBJECT_HPP_
