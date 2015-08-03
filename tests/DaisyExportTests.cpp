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
	}

	virtual ~Widget() DAISY_NOEXCEPT {
	}

	static void JSExportInitialize() {
		JSExport<Widget>::AddFunctionProperty("testString", [](JSObject function_object, JSObject this_object, const std::vector<JSValue>& arguments){
			return this_object.get_context().CreateString("Widget test OK");
		});
		JSExport<Widget>::AddFunctionProperty("testBoolean", [](JSObject function_object, JSObject this_object, const std::vector<JSValue>& arguments){
			return this_object.get_context().CreateBoolean(true);
		});
		JSExport<Widget>::AddFunctionProperty("testNumber", [](JSObject function_object, JSObject this_object, const std::vector<JSValue>& arguments){
			return this_object.get_context().CreateNumber(1234);
		});
		JSExport<Widget>::AddFunctionProperty("testNull", [](JSObject function_object, JSObject this_object, const std::vector<JSValue>& arguments){
			return this_object.get_context().CreateNull();
		});
		JSExport<Widget>::AddFunctionProperty("testUndefined", [](JSObject function_object, JSObject this_object, const std::vector<JSValue>& arguments){
			return this_object.get_context().CreateUndefined();
		});
	}

	virtual void postInitialize(JSObject& js_object) {
	}

};

TEST(DaisyExportTests, FunctionCallback_String) {
	JSContextGroup js_context_group;
	auto js_context     = js_context_group.CreateContext();
	auto global_object  = js_context.get_global_object();
  
	auto widget = js_context.CreateObject(JSExport<Widget>::Class());
	XCTAssertTrue(widget.IsObject());
	XCTAssertTrue(widget.HasProperty("testString"));
	auto test_func_property = widget.GetProperty("testString");
	XCTAssertTrue(!test_func_property.IsUndefined());
	XCTAssertTrue(test_func_property.IsObject());
	auto test_func = static_cast<JSObject>(test_func_property);
	XCTAssertTrue(test_func.IsFunction());
	auto test_result = test_func(widget);
	XCTAssertTrue(test_result.IsString());
	XCTAssertEqual("Widget test OK", static_cast<std::string>(test_result));
}

TEST(DaisyExportTests, FunctionCallback_Boolean) {
	JSContextGroup js_context_group;
	auto js_context     = js_context_group.CreateContext();
	auto global_object  = js_context.get_global_object();
  
	auto widget = js_context.CreateObject(JSExport<Widget>::Class());
	XCTAssertTrue(widget.IsObject());
	XCTAssertTrue(widget.HasProperty("testBoolean"));
	auto test_func_property = widget.GetProperty("testBoolean");
	XCTAssertTrue(!test_func_property.IsUndefined());
	XCTAssertTrue(test_func_property.IsObject());
	auto test_func = static_cast<JSObject>(test_func_property);
	XCTAssertTrue(test_func.IsFunction());
	auto test_result = test_func(widget);
	XCTAssertTrue(test_result.IsBoolean());
	XCTAssertTrue(static_cast<bool>(test_result));
}

TEST(DaisyExportTests, FunctionCallback_Number) {
	JSContextGroup js_context_group;
	auto js_context     = js_context_group.CreateContext();
	auto global_object  = js_context.get_global_object();
  
	auto widget = js_context.CreateObject(JSExport<Widget>::Class());
	XCTAssertTrue(widget.IsObject());
	XCTAssertTrue(widget.HasProperty("testNumber"));
	auto test_func_property = widget.GetProperty("testNumber");
	XCTAssertTrue(!test_func_property.IsUndefined());
	XCTAssertTrue(test_func_property.IsObject());
	auto test_func = static_cast<JSObject>(test_func_property);
	XCTAssertTrue(test_func.IsFunction());
	auto test_result = test_func(widget);
	XCTAssertTrue(test_result.IsNumber());
	XCTAssertEqual(1234, static_cast<std::uint32_t>(test_result));
}

TEST(DaisyExportTests, FunctionCallback_Null) {
	JSContextGroup js_context_group;
	auto js_context     = js_context_group.CreateContext();
	auto global_object  = js_context.get_global_object();
  
	auto widget = js_context.CreateObject(JSExport<Widget>::Class());
	XCTAssertTrue(widget.IsObject());
	XCTAssertTrue(widget.HasProperty("testNull"));
	auto test_func_property = widget.GetProperty("testNull");
	XCTAssertTrue(!test_func_property.IsUndefined());
	XCTAssertTrue(test_func_property.IsObject());
	auto test_func = static_cast<JSObject>(test_func_property);
	XCTAssertTrue(test_func.IsFunction());
	auto test_result = test_func(widget);
	XCTAssertTrue(test_result.IsNull());
}

TEST(DaisyExportTests, FunctionCallback_Undefined) {
	JSContextGroup js_context_group;
	auto js_context     = js_context_group.CreateContext();
	auto global_object  = js_context.get_global_object();
  
	auto widget = js_context.CreateObject(JSExport<Widget>::Class());
	XCTAssertTrue(widget.IsObject());
	XCTAssertTrue(widget.HasProperty("testUndefined"));
	auto test_func_property = widget.GetProperty("testUndefined");
	XCTAssertTrue(!test_func_property.IsUndefined());
	XCTAssertTrue(test_func_property.IsObject());
	auto test_func = static_cast<JSObject>(test_func_property);
	XCTAssertTrue(test_func.IsFunction());
	auto test_result = test_func(widget);
	XCTAssertTrue(test_result.IsUndefined());
}
