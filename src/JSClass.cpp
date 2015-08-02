/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSClass.hpp"
#include <cassert>

namespace Daisy {
	
	JSClass::JSClass() DAISY_NOEXCEPT {
	}
	
	JSClass::~JSClass() DAISY_NOEXCEPT {
	}
	
	JSClass::JSClass(const JSClass& rhs) DAISY_NOEXCEPT 
		: callAsFunction_names_map__(rhs.callAsFunction_names_map__) {
	}
	
	JSClass::JSClass(JSClass&& rhs) DAISY_NOEXCEPT
		: callAsFunction_names_map__(rhs.callAsFunction_names_map__) {
	}

	void JSClass::AddFunctionProperty(const std::string& name, CallAsFunctionCallback callback) {
		const auto position = callAsFunction_names_map__.find(name);
		assert(position == callAsFunction_names_map__.end());
		callAsFunction_names_map__.emplace(name, callback);
	}
	
	JSClass& JSClass::operator=(JSClass rhs) DAISY_NOEXCEPT {
		DAISY_JSCLASS_LOCK_GUARD;
		swap(rhs);
		return *this;
	}
	
	void JSClass::swap(JSClass& other) DAISY_NOEXCEPT {
		DAISY_JSCLASS_LOCK_GUARD;
		std::swap(callAsFunction_names_map__, other.callAsFunction_names_map__);
	}
	
} // namespace Daisy {
