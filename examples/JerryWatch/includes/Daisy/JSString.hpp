/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_JSSTRING_HPP_
#define _DAISY_JSSTRING_HPP_

#include "Daisy/detail/JSBase.hpp"
#include "Daisy/JSValue.hpp"
#include "jerry.h"

namespace Daisy {

	class JSContext;

	class DAISY_EXPORT JSString  final : public JSValue {
	public:

		const std::size_t length() const DAISY_NOEXCEPT;
		const std::size_t size() const DAISY_NOEXCEPT;
		const bool empty() const DAISY_NOEXCEPT;
		operator std::string() const DAISY_NOEXCEPT;
		std::size_t hash_value() const;

		~JSString()                      DAISY_NOEXCEPT;
		JSString(const JSString&)        DAISY_NOEXCEPT;
		JSString(JSString&&)             DAISY_NOEXCEPT;
		JSString& operator=(JSString)    DAISY_NOEXCEPT;
		void swap(JSString&)             DAISY_NOEXCEPT;

	protected:
		friend JSContext;
		friend JSValue;
		friend JSObject;

		JSString(const JSContext& js_context, const jerry_api_value_t& js_api_value) DAISY_NOEXCEPT;
		JSString(const JSContext& js_context, const std::string& value) DAISY_NOEXCEPT;

		static jerry_api_value_t MakeString(const std::string& value) DAISY_NOEXCEPT;
		static std::string ToString(const jerry_api_value_t& value) DAISY_NOEXCEPT;

		// Prevent heap based objects.
		void* operator new(std::size_t)     = delete; // #1: To prevent allocation of scalar objects
		void* operator new [] (std::size_t) = delete; // #2: To prevent allocation of array of objects
		
		DAISY_EXPORT friend bool operator==(const JSString& lhs, const JSString& rhs);
		
#undef  DAISY_JSSTRING_LOCK_GUARD
#undef  DAISY_JSSTRING_LOCK_GUARD_STATIC
#ifdef  DAISY_THREAD_SAFE
		std::recursive_mutex mutex__;
		static std::recursive_mutex mutex_static__;
#define DAISY_JSSTRING_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#define DAISY_JSSTRING_LOCK_GUARD_STATIC std::lock_guard<std::recursive_mutex> lock_static(JSString::mutex_static__)
#else
#define DAISY_JSSTRING_LOCK_GUARD
#define DAISY_JSSTRING_LOCK_GUARD_STATIC
#endif  // DAISY_THREAD_SAFE

		// Silence 4251 on Windows since private member variables do not
		// need to be exported from a DLL.
#pragma warning(push)
#pragma warning(disable: 4251)
		std::string string__;
		std::size_t hash_value__;
#pragma warning(pop)

	};
	
	inline
	std::string to_string(const JSString& js_string) {
		return static_cast<std::string>(js_string);
	}
	
	// Return true if the two JSStrings are equal.
	DAISY_EXPORT bool operator==(const JSString& lhs, const JSString& rhs);
	
	// Return true if the two JSStrings are not equal.
	inline
	DAISY_EXPORT bool operator!=(const JSString& lhs, const JSString& rhs) {
		return ! (lhs == rhs);
	}
	
	// Define a strict weak ordering for two JSStrings.
	inline
	bool operator<(const JSString& lhs, const JSString& rhs) {
		return to_string(lhs) < to_string(rhs);
	}
	
	inline
	bool operator>(const JSString& lhs, const JSString& rhs) {
		return rhs < lhs;
	}
	
	inline
	bool operator<=(const JSString& lhs, const JSString& rhs) {
		return !(lhs > rhs);
	}
	
	inline
	bool operator>=(const JSString& lhs, const JSString& rhs) {
		return !(lhs < rhs);
	}
	
	inline
	std::ostream& operator << (std::ostream& ostream, const JSString& js_string) {
		ostream << to_string(js_string);
		return ostream;
	}
	
	inline
	void swap(JSString& first, JSString& second) DAISY_NOEXCEPT {
		first.swap(second);
	}

}

namespace std {
	template<>
	struct hash<Daisy::JSString> {
		using argument_type = Daisy::JSString;
		using result_type   = std::size_t;
		
		result_type operator()(const argument_type& js_string) const {
			return js_string.hash_value();
		}
	};
}  // namespace std

#endif // _DAISY_JSSTRING_HPP_