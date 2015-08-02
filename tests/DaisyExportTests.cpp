/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */
#include "gtest/gtest.h"

#define XCTAssertEqual    ASSERT_EQ
#define XCTAssertNotEqual ASSERT_NE
#define XCTAssertTrue     ASSERT_TRUE
#define XCTAssertFalse    ASSERT_FALSE

#include "Daisy/daisy.hpp"
#include "Daisy/detail/JSBase.hpp"
#include <iostream>

using namespace Daisy;

class Widget : public JSExport<Widget> {
public:
	Widget(const JSContext& js_context) DAISY_NOEXCEPT {
		std::cout << "Widget ctor" << std::endl;
	}

	virtual ~Widget() DAISY_NOEXCEPT {
		std::cout << "Widget dtor" << std::endl;
	}

	static void JSExportInitialize() {
		JSExport<Widget>::AddFunctionProperty("test", [](JSObject function_object, JSObject this_object, const std::vector<JSValue>& arguments){
			return this_object.get_context().CreateNull();
		});
	}

	virtual void postInitialize(JSObject& js_object) {
		std::cout << "Widget postInitialize" << std::endl;
	}

};

TEST(DaisyExportTests, CoreInit) {
	JSContextGroup js_context_group;
	auto js_context     = js_context_group.CreateContext();
	auto global_object  = js_context.get_global_object();
  
	auto widget = js_context.CreateObject(JSExport<Widget>::Class());
	XCTAssertTrue(widget.IsObject());
}
