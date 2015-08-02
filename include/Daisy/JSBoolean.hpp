/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_JSBOOLEAN_HPP_
#define _DAISY_JSBOOLEAN_HPP_

#include "Daisy/JSValue.hpp"

namespace Daisy {

	class DAISY_EXPORT JSBoolean final : public JSValue {
		
	public:
		JSBoolean& operator=(bool value) {
			return operator=(JSBoolean(get_context(), value));
		}

	private:
		friend JSContext;

		explicit JSBoolean(const JSContext& js_context, bool value = false)
				: JSValue(js_context, MakeBoolean(value)) {
		}
		
		static jerry_api_value_t MakeBoolean(bool value) DAISY_NOEXCEPT;
	};

} // namespace Daisy {

#endif // _DAISY_JSBOOLEAN_HPP_
