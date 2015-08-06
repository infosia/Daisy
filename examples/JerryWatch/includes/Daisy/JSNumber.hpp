/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_JSNUMBER_HPP_
#define _DAISY_JSNUMBER_HPP_

#include "Daisy/JSValue.hpp"

namespace Daisy {
	class DAISY_EXPORT JSNumber final : public JSValue {
		
	public:
		
		JSNumber& operator=(double number) {
			return operator=(JSNumber(get_context(), number));
		}

		JSNumber& operator=(std::int32_t number) {
			return operator=(JSNumber(get_context(), number));
		}

		JSNumber& operator=(std::uint32_t number) {
			return operator=(JSNumber(get_context(), number));
		}

	private:
		friend JSContext;

		explicit JSNumber(const JSContext& js_context, double number = 0)
				: JSValue(js_context, MakeNumber(number)) {
		}
		
		JSNumber(const JSContext& js_context, std::int32_t number)
				: JSNumber(js_context, static_cast<double>(number)) {
		}
		
		JSNumber(const JSContext& js_context, std::uint32_t number)
				: JSNumber(js_context, static_cast<double>(number)) {
		}
		
		static jerry_api_value_t MakeNumber(double number) DAISY_NOEXCEPT;
	};

} // namespace Daisy {

#endif // _DAISY_JSNUMBER_HPP_
