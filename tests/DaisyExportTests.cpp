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

class Widget : public JSExportObject, JSExport<Widget> {
public:
	Widget(const JSContext& js_context) DAISY_NOEXCEPT 
		: JSExportObject(js_context) {
	}

	virtual ~Widget() DAISY_NOEXCEPT {
	}

	static void JSExportInitialize() {
		JSExport<Widget>::AddFunctionProperty("testString",    std::mem_fn(&Widget::testString));
		JSExport<Widget>::AddFunctionProperty("testBoolean",   std::mem_fn(&Widget::testBoolean));
		JSExport<Widget>::AddFunctionProperty("testNumber",    std::mem_fn(&Widget::testNumber));
		JSExport<Widget>::AddFunctionProperty("testNull",      std::mem_fn(&Widget::testNull));
		JSExport<Widget>::AddFunctionProperty("testUndefined", std::mem_fn(&Widget::testUndefined));
		JSExport<Widget>::AddFunctionProperty("testCount",     std::mem_fn(&Widget::testCount));
	}

	JSValue testString(const std::vector<JSValue>& arguments, JSObject this_object) {
		return this_object.get_context().CreateString("Widget test OK");
	}

	JSValue testBoolean(const std::vector<JSValue>& arguments, JSObject this_object) {
		return this_object.get_context().CreateBoolean(true);
	}

	JSValue testNumber(const std::vector<JSValue>& arguments, JSObject this_object) {
		return this_object.get_context().CreateNumber(1234);
	}

	JSValue testNull(const std::vector<JSValue>& arguments, JSObject this_object) {
		return this_object.get_context().CreateNull();
	}

	JSValue testUndefined(const std::vector<JSValue>& arguments, JSObject this_object) {
		return this_object.get_context().CreateUndefined();
	}

	JSValue testCount(const std::vector<JSValue>& arguments, JSObject this_object) {
		return this_object.get_context().CreateNumber(count__);
	}

	virtual void postInitialize(JSObject& js_object) {

	}

	virtual std::uint32_t getCount() {
		return count__;
	}

	virtual void countUp() {
		count__++;
	}

private:
	std::uint32_t count__ { 0 };

};

TEST(DaisyExportTests, FunctionCallback_String) {
	JSContextGroup js_context_group;
	auto js_context = js_context_group.CreateContext();
  
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
	auto js_context = js_context_group.CreateContext();
  
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
	auto js_context = js_context_group.CreateContext();
  
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
	auto js_context = js_context_group.CreateContext();
  
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
	auto js_context = js_context_group.CreateContext();
  
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

TEST(DaisyExportTests, GetPrivate) {
	JSContextGroup js_context_group;
	auto js_context = js_context_group.CreateContext();
  
	auto widget = js_context.CreateObject(JSExport<Widget>::Class());
	XCTAssertTrue(widget.HasProperty("testCount"));
	auto test_func_property = widget.GetProperty("testCount");
	XCTAssertTrue(!test_func_property.IsUndefined());
	XCTAssertTrue(test_func_property.IsObject());
	auto test_func = static_cast<JSObject>(test_func_property);
	XCTAssertTrue(test_func.IsFunction());
	auto test_result = test_func(widget);
	XCTAssertEqual(0, static_cast<std::uint32_t>(test_result));

	auto widget_ptr = widget.GetPrivate<Widget>();
	XCTAssertTrue(widget_ptr != nullptr);
	XCTAssertEqual(0, widget_ptr->getCount());
	widget_ptr->countUp();
	XCTAssertEqual(1, widget_ptr->getCount());

	test_result = test_func(widget);
	XCTAssertEqual(1, static_cast<std::uint32_t>(test_result));
}

TEST(DaisyExportTests, GetObject) {
	JSContextGroup js_context_group;
	auto js_context = js_context_group.CreateContext();

	auto widget = js_context.CreateObject(JSExport<Widget>::Class());
	auto widget_ptr = widget.GetPrivate<Widget>();
	XCTAssertTrue(widget_ptr != nullptr);
	auto js_object = widget_ptr->get_object();
	XCTAssertTrue(js_object.IsObject());
	XCTAssertEqual(js_object, widget);
}
