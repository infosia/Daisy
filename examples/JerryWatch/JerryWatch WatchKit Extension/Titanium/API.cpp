/**
 * TitaniumKit
 *
 * Copyright (c) 2014-2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#include "Titanium/API.hpp"
#include <unordered_map>
#include <sstream>
#include <iostream>

namespace Titanium
{
	API::API(const JSContext& js_context) TITANIUM_NOEXCEPT
	    : Module(js_context)
	{
		TITANIUM_LOG_DEBUG("API:: ctor ", this);
	}

	void API::postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments) {
		TITANIUM_LOG_DEBUG("API:: postCallAsConstructor ", this);
	}

	API::~API() TITANIUM_NOEXCEPT
	{
		TITANIUM_LOG_DEBUG("API:: dtor ", this);
	}

	void API::info(const std::string& message) const TITANIUM_NOEXCEPT
	{
		log(LogSeverityLevel::API_INFO, message);
	}

	void API::warn(const std::string& message) const TITANIUM_NOEXCEPT
	{
		log(LogSeverityLevel::API_WARN, message);
	}

	void API::error(const std::string& message) const TITANIUM_NOEXCEPT
	{
		log(LogSeverityLevel::API_ERROR, message);
	}

	void API::debug(const std::string& message) const TITANIUM_NOEXCEPT
	{
		log(LogSeverityLevel::API_DEBUG, message);
	}

	void API::trace(const std::string& message) const TITANIUM_NOEXCEPT
	{
		log(LogSeverityLevel::API_TRACE, message);
	}

	void API::log(const std::string& level, const std::string& message) const TITANIUM_NOEXCEPT
	{
		log(ToLogLevel(level), message);
	}

	void API::log(LogSeverityLevel log_severity_level, const std::string& message) const TITANIUM_NOEXCEPT
	{
		std::ostringstream os;
		switch (log_severity_level) {
			case LogSeverityLevel::API_INFO:
				os << "[INFO] ";
				break;

			case LogSeverityLevel::API_WARN:
				os << "[WARN] ";
				break;

			case LogSeverityLevel::API_ERROR:
				os << "[ERROR] ";
				break;

			case LogSeverityLevel::API_DEBUG:
				os << "[DEBUG] ";
				break;

			case LogSeverityLevel::API_TRACE:
				os << "[TRACE] ";
				break;

			default:
				os << "[INFO] ";
				break;
		}

		os << message;
		log(os.str());
	}

	void API::log(const std::string& message) const TITANIUM_NOEXCEPT
	{
		std::cout << message << std::endl;
	}

	API::LogSeverityLevel API::ToLogLevel(const std::string& level)
	{
		static std::unordered_map<std::string, LogSeverityLevel> log_severity_level_map;
		static std::once_flag of;
		std::call_once(of, []() {
      log_severity_level_map.emplace("info" , LogSeverityLevel::API_INFO);
      log_severity_level_map.emplace("warn" , LogSeverityLevel::API_WARN);
      log_severity_level_map.emplace("error", LogSeverityLevel::API_ERROR);
      log_severity_level_map.emplace("debug", LogSeverityLevel::API_DEBUG);
      log_severity_level_map.emplace("trace", LogSeverityLevel::API_TRACE);
		});

		LogSeverityLevel log_severity_level = LogSeverityLevel::API_INFO;
		const auto position = log_severity_level_map.find(level);

		if (position != log_severity_level_map.end()) {
			log_severity_level = position->second;
		}

		return log_severity_level;
	}

	// TODO: The following functions can automatically be generated from
	// the YAML API docs.

	void API::JSExportInitialize()
	{
		JSExport<API>::SetParent(JSExport<Module>::Class());
		
		TITANIUM_ADD_FUNCTION(API, info);
		TITANIUM_ADD_FUNCTION(API, warn);
		TITANIUM_ADD_FUNCTION(API, error);
		TITANIUM_ADD_FUNCTION(API, debug);
		TITANIUM_ADD_FUNCTION(API, trace);
		TITANIUM_ADD_FUNCTION(API, log);
	}

	JSObject API::GetStaticObject(const JSContext& js_context) TITANIUM_NOEXCEPT
	{
		JSValue Ti_property = js_context.get_global_object().GetProperty("Ti");
		TITANIUM_ASSERT(Ti_property.IsObject());  // precondition
		JSObject Ti = static_cast<JSObject>(Ti_property);
		
		JSValue Object_property = Ti.GetProperty("API");
		TITANIUM_ASSERT(Object_property.IsObject());  // precondition
		return static_cast<JSObject>(Object_property);
	}

	TITANIUM_FUNCTION(API, info)
	{
		ENSURE_STRING_AT_INDEX(message, 0);
		
		const auto js_context = this_object.get_context();
		GetStaticObject(js_context).GetPrivate<API>()->info(message);
		return js_context.CreateUndefined();
	}

	TITANIUM_FUNCTION(API, warn)
	{
		ENSURE_STRING_AT_INDEX(message, 0);
		
		const auto js_context = this_object.get_context();
		GetStaticObject(js_context).GetPrivate<API>()->warn(message);
		return js_context.CreateUndefined();
	}

	TITANIUM_FUNCTION(API, error)
	{
		ENSURE_STRING_AT_INDEX(message, 0);
		
		const auto js_context = this_object.get_context();
		GetStaticObject(js_context).GetPrivate<API>()->error(message);
		return js_context.CreateUndefined();
	}

	TITANIUM_FUNCTION(API, debug)
	{
		ENSURE_STRING_AT_INDEX(message, 0);
		
		const auto js_context = this_object.get_context();
		GetStaticObject(js_context).GetPrivate<API>()->debug(message);
		return js_context.CreateUndefined();
	}

	TITANIUM_FUNCTION(API, trace)
	{
		ENSURE_STRING_AT_INDEX(message, 0);
		
		const auto js_context = this_object.get_context();
		GetStaticObject(js_context).GetPrivate<API>()->trace(message);
		return js_context.CreateUndefined();
	}

	TITANIUM_FUNCTION(API, log)
	{
		ENSURE_STRING_AT_INDEX(level, 0);
		ENSURE_STRING_AT_INDEX(message, 1);
		
		const auto js_context = this_object.get_context();
		GetStaticObject(js_context).GetPrivate<API>()->log(message);
		return js_context.CreateUndefined();
	}

}  // namespace Titanium
