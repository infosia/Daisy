/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_JSCONTEXT_HPP_
#define _DAISY_JSCONTEXT_HPP_

#include "Daisy/detail/JSBase.hpp"
#include "Daisy/JSContextGroup.hpp"

namespace Daisy {

	class JSValue;
	class JSNumber;
	class JSBoolean;
	class JSString;
	class JSObject;
	class JSClass;

	class DAISY_EXPORT JSContext {
	public:
		~JSContext()                         DAISY_NOEXCEPT;
		JSContext(const JSContext&)          DAISY_NOEXCEPT;
		JSContext(JSContext&&)               DAISY_NOEXCEPT;
		JSContext& operator=(JSContext)      DAISY_NOEXCEPT;
		void swap(JSContext&)                DAISY_NOEXCEPT;

		JSObject get_global_object() const DAISY_NOEXCEPT;

		JSValue CreateUndefined() const DAISY_NOEXCEPT;
		JSValue CreateNull() const DAISY_NOEXCEPT;
		JSNumber CreateNumber(const double& number) const DAISY_NOEXCEPT;
		JSNumber CreateNumber(const std::int32_t& number) const DAISY_NOEXCEPT;
		JSNumber CreateNumber(const std::uint32_t& number) const DAISY_NOEXCEPT;
		JSBoolean CreateBoolean(const bool& value) const DAISY_NOEXCEPT;
		JSString CreateString(const std::string& value) const DAISY_NOEXCEPT;
		JSObject CreateObject() const DAISY_NOEXCEPT;
		JSObject CreateObject(const JSClass&) const DAISY_NOEXCEPT;

		JSValue JSEvaluateScript(const std::string& script) const;

	private:
    friend class JSContextGroup;
    
		JSContext(const JSContextGroup& js_context_group) DAISY_NOEXCEPT;

		// Prevent heap based objects.
		void* operator new(std::size_t)     = delete; // #1: To prevent allocation of scalar objects
		void* operator new [] (std::size_t) = delete; // #2: To prevent allocation of array of objects
		
		DAISY_EXPORT friend bool operator==(const JSContext& lhs, const JSContext& rhs);
		
#undef DAISY_JSCONTEXT_LOCK_GUARD
#ifdef DAISY_THREAD_SAFE
		std::recursive_mutex mutex__;
#define DAISY_JSCONTEXT_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#else
#define DAISY_JSCONTEXT_LOCK_GUARD
#endif  // DAISY_THREAD_SAFE

    // Silence 4251 on Windows since private member variables do not
    // need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
    JSContextGroup     js_context_group__;
#pragma warning(pop)

	};
	inline
	void swap(JSContext& first, JSContext& second) DAISY_NOEXCEPT {
		first.swap(second);
	}
	
	// Return true if the two JSContexts are equal.
	inline
	bool operator==(const JSContext& lhs, const JSContext& rhs) {
		return true; // There's only one JSContext on Daisy
	}
	
	// Return true if the two JSContexts are not equal.
	inline
	bool operator!=(const JSContext& lhs, const JSContext& rhs) {
		return false; // There's only one JSContext on Daisy
	}
}

#endif // _DAISY_JSCONTEXT_HPP_
