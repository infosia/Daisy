/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Daisy/JSContextGroup.hpp"
#include "Daisy/JSContext.hpp"
#include "Daisy/JSObject.hpp"
#include "Daisy/JSValue.hpp"
#include "jerry.h"
#include <cassert>

namespace Daisy {

	std::size_t JSContextGroup::retainCount { 0 };

	void JSContextGroup::EnsureJerryInit() {
		DAISY_JSCONTEXTGROUP_LOCK_GUARD;
		if (retainCount == 0) {
			jerry_init(JERRY_FLAG_EMPTY);
		}
		++retainCount;
	}
	
	JSContextGroup::JSContextGroup() DAISY_NOEXCEPT {
		EnsureJerryInit();
	}
	
	JSContext JSContextGroup::CreateContext() const DAISY_NOEXCEPT {
		return JSContext(*this);
	}

	JSContextGroup::~JSContextGroup() DAISY_NOEXCEPT {
		--retainCount;
		if (retainCount == 0) {
			// make sure we have properly cleaned up resources
			assert(JSObject::js_private_data_to_js_object_ref_map__.empty());
			assert(JSObject::js_object_external_functions_map__.empty());
			assert(JSObject::js_object_external_constructors_map__.empty());
			assert(JSObject::js_object_finalizeCallback_map__.empty());
			assert(JSValue::js_api_value_retain_count_map__.empty());
			jerry_cleanup();
		}
	}
	
	JSContextGroup::JSContextGroup(const JSContextGroup& rhs) DAISY_NOEXCEPT {
		EnsureJerryInit();
	}
	
	JSContextGroup::JSContextGroup(JSContextGroup&& rhs) DAISY_NOEXCEPT {
		EnsureJerryInit();
	}
	
	JSContextGroup& JSContextGroup::operator=(JSContextGroup rhs) DAISY_NOEXCEPT {
		DAISY_JSCONTEXTGROUP_LOCK_GUARD;
		swap(rhs);
		return *this;
	}
	
	void JSContextGroup::swap(JSContextGroup& other) DAISY_NOEXCEPT {
		// DAISY_JSCONTEXTGROUP_LOCK_GUARD;
		// std::swap(js_context_group_ref__, other.js_context_group_ref__);
	}
	
}
