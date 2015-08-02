/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_JSCLASS_HPP_
#define _DAISY_JSCLASS_HPP_

#include "Daisy/detail/JSBase.hpp"
#include <functional>
#include <vector>
#include <unordered_map>

namespace Daisy {

	class JSContext;
	class JSValue;
	class JSObject;
	
	typedef std::function<JSValue(JSObject, JSObject, const std::vector<JSValue>&)> CallAsFunctionCallback;
	typedef std::function<void(const std::uintptr_t&)> JSObjectFinalizeCallback;

	class DAISY_EXPORT JSClass {
	public:
		JSClass() DAISY_NOEXCEPT;
		
		virtual ~JSClass()          DAISY_NOEXCEPT;
		JSClass(const JSClass&)     DAISY_NOEXCEPT;
		JSClass(JSClass&&)          DAISY_NOEXCEPT;
		JSClass& operator=(JSClass) DAISY_NOEXCEPT;
		void swap(JSClass&)         DAISY_NOEXCEPT;
		
		void AddFunctionProperty(const std::string& name, CallAsFunctionCallback callback);

		virtual void JSObjectInitializeCallback(const JSContext& js_context, JSObject& this_object) const {
			
		}

	protected:

		// Prevent heap based objects.
		void* operator new(std::size_t)     = delete; // #1: To prevent allocation of scalar objects
		void* operator new [] (std::size_t) = delete; // #2: To prevent allocation of array of objects
		
#undef  DAISY_JSCLASS_LOCK_GUARD
#ifdef  DAISY_THREAD_SAFE
		std::recursive_mutex mutex__;
#define DAISY_JSCLASS_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#else
#define DAISY_JSCLASS_LOCK_GUARD
#endif  // DAISY_THREAD_SAFE

		// Silence 4251 on Windows since private member variables do not
		// need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
		std::unordered_map<std::string, CallAsFunctionCallback> callAsFunction_names_map__;
#pragma warning(pop)
	};
	
} // namespace Daisy {

#endif // _DAISY_JSCLASS_HPP_
