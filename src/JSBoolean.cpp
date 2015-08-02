/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSBoolean.hpp"

namespace Daisy {

	jerry_api_value_t JSBoolean::MakeBoolean(bool value) DAISY_NOEXCEPT {
		jerry_api_value_t js_api_value;
		js_api_value.type = JERRY_API_DATA_TYPE_BOOLEAN;
		js_api_value.v_bool = value;
		return js_api_value;
	}

}
