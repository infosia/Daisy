/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_JSVALUE_HPP_
#define _DAISY_JSVALUE_HPP_

#include "Daisy/detail/JSBase.hpp"
#include "Daisy/JSContext.hpp"
#include "jerry.h"
#include <map>
#include <unordered_map>
#include <tuple>

namespace Daisy {

	class JSObject;
	class JSString;

	class DAISY_EXPORT JSValue {
	public:
		explicit operator bool() const DAISY_NOEXCEPT;
		explicit operator double() const DAISY_NOEXCEPT;
		explicit operator std::int32_t() const DAISY_NOEXCEPT;
		explicit operator std::uint32_t() const DAISY_NOEXCEPT;
		explicit operator std::string() const DAISY_NOEXCEPT;
		explicit operator JSString() const DAISY_NOEXCEPT;
		explicit operator JSObject() const DAISY_NOEXCEPT;

		explicit operator jerry_api_value_t() const DAISY_NOEXCEPT {
			return js_api_value__;
		}

		~JSValue()                       DAISY_NOEXCEPT;
		JSValue(const JSValue&)          DAISY_NOEXCEPT;
		JSValue(JSValue&&)               DAISY_NOEXCEPT;
		JSValue& operator=(JSValue)      DAISY_NOEXCEPT;
		void swap(JSValue&)              DAISY_NOEXCEPT;

		virtual JSContext get_context() const DAISY_NOEXCEPT final {
			return js_context__;
		}

		virtual bool IsUndefined() const DAISY_NOEXCEPT final {
			return js_api_value__.type == JERRY_API_DATA_TYPE_UNDEFINED;
		}

		virtual bool IsNull()      const DAISY_NOEXCEPT final {
			return js_api_value__.type == JERRY_API_DATA_TYPE_NULL;
		}

		virtual bool IsString()    const DAISY_NOEXCEPT final {
			return js_api_value__.type == JERRY_API_DATA_TYPE_STRING;
		}

		virtual bool IsBoolean()   const DAISY_NOEXCEPT final {
			return js_api_value__.type == JERRY_API_DATA_TYPE_BOOLEAN;
		}

		virtual bool IsDouble()    const DAISY_NOEXCEPT final {
			return js_api_value__.type == JERRY_API_DATA_TYPE_FLOAT64 || js_api_value__.type == JERRY_API_DATA_TYPE_FLOAT32 || js_api_value__.type == JERRY_API_DATA_TYPE_UINT32;
		}

		virtual bool IsNumber()    const DAISY_NOEXCEPT final {
			return IsDouble();
		}

		virtual bool IsObject()    const DAISY_NOEXCEPT final {
			return js_api_value__.type == JERRY_API_DATA_TYPE_OBJECT;
		}

		virtual bool IsGlobalObject() const DAISY_NOEXCEPT final;

		virtual void unmanaged() {
			js_value_managed__ = false;
		}

    	JSValue(const JSContext& js_context, const jerry_api_value_t& js_api_value, const bool& managed = true) DAISY_NOEXCEPT;

	protected:

		friend JSContextGroup;

		// Prevent heap based objects.
		void* operator new(std::size_t)     = delete; // #1: To prevent allocation of scalar objects
		void* operator new [] (std::size_t) = delete; // #2: To prevent allocation of array of objects
		
		DAISY_EXPORT friend bool operator==(const JSValue& lhs, const JSValue& rhs);
		
		void retain();
		void release();
		
#undef  DAISY_JSVALUE_LOCK_GUARD
#undef  DAISY_JSVALUE_LOCK_GUARD_STATIC
#ifdef  DAISY_THREAD_SAFE
		std::recursive_mutex mutex__;
		static std::recursive_mutex mutex_static__;
#define DAISY_JSVALUE_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#define DAISY_JSVALUE_LOCK_GUARD_STATIC std::lock_guard<std::recursive_mutex> lock_static(JSString::mutex_static__)
#else
#define DAISY_JSVALUE_LOCK_GUARD
#define DAISY_JSVALUE_LOCK_GUARD_STATIC
#endif  // DAISY_THREAD_SAFE

		JSContext js_context__;

    // Silence 4251 on Windows since private member variables do not
    // need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
		bool js_value_managed__ { true };
		jerry_api_value_t js_api_value__;
		static std::unordered_map<std::intptr_t, std::tuple<jerry_api_data_type_t, std::size_t>> js_api_value_retain_count_map__;
#pragma warning(pop)

	};
	inline
	void swap(JSValue& first, JSValue& second) DAISY_NOEXCEPT {
		first.swap(second);
	}
	
	// Return true if the two JSValues are equal.
	inline
	bool operator==(const JSValue& lhs, const JSValue& rhs) {
		if (lhs.IsNumber() && rhs.IsNumber()) {
			return static_cast<double>(lhs) == static_cast<double>(rhs);	
		} else if (lhs.js_api_value__.type != rhs.js_api_value__.type) {
			return false;
		}
		switch(lhs.js_api_value__.type) { 
		case JERRY_API_DATA_TYPE_BOOLEAN:
			return lhs.js_api_value__.v_bool == rhs.js_api_value__.v_bool;
		case JERRY_API_DATA_TYPE_STRING:
			return lhs.js_api_value__.v_string == rhs.js_api_value__.v_string;
		case JERRY_API_DATA_TYPE_OBJECT:
			return lhs.js_api_value__.v_object == rhs.js_api_value__.v_object;
		case JERRY_API_DATA_TYPE_NULL:
			return true;
		case JERRY_API_DATA_TYPE_UNDEFINED:
			return true;
		default:
			return false; // unknown type
		}
	}
	
	// Return true if the two JSValues are not equal.
	inline
	bool operator!=(const JSValue& lhs, const JSValue& rhs) {
		return ! (lhs == rhs);
	}
}

#endif // _DAISY_JSVALUE_HPP_