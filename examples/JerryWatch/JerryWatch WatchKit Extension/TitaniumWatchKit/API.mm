/**
 * Titanium.API for Windows
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "API.h"
#include <iostream>
#import <WatchKit/WatchKit.h>
#import <Foundation/Foundation.h>

namespace TitaniumWatchKit {
	
	API::API(const JSContext& js_context) TITANIUM_NOEXCEPT
	: Titanium::API(js_context) {
		NSLog(@"TitaniumWatchKit::API ctor");
	}
	
	API::~API() TITANIUM_NOEXCEPT
	{
		NSLog(@"TitaniumWatchKit::API dtor");
	}
	
	void API::log(const std::string& message) const TITANIUM_NOEXCEPT
	{
		NSLog([NSString stringWithUTF8String:message.c_str()]);
	}
	
	void API::JSExportInitialize()
	{
		JSExport<API>::SetParent(JSExport<Titanium::API>::Class());
	}
}