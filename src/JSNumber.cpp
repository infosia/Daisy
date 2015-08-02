/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSNumber.hpp"

namespace Daisy {

	jerry_api_value_t JSNumber::MakeNumber(double number) DAISY_NOEXCEPT {
		jerry_api_value_t js_api_value;
		js_api_value.type = JERRY_API_DATA_TYPE_FLOAT64;
		js_api_value.v_float64 = number;	
		return js_api_value;
	}

}
