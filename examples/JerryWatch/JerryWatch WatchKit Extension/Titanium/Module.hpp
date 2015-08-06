/**
 * TitaniumKit
 *
 * Copyright (c) 2014 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _TITANIUM_MODULE_HPP_
#define _TITANIUM_MODULE_HPP_

#include "Daisy/daisy.hpp"
#include <unordered_map>
#include <vector>

#define TITANIUMKIT_EXPORT DAISY_EXPORT
#define TITANIUM_NOEXCEPT DAISY_NOEXCEPT
#define TITANIUM_LOG_DEBUG(...)
#define TITANIUM_LOG_INFO(...)
#define TITANIUM_LOG_WARN(...)
#define TITANIUM_LOG_ERROR(...)
#define TITANIUM_ASSERT(...)
#define TITANIUM_ASSERT_AND_THROW(...)

#define TITANIUM_FUNCTION_DEF(NAME) \
JSValue js_##NAME(const std::vector<JSValue>& arguments, JSObject& this_object)

#define TITANIUM_ADD_FUNCTION(MODULE, NAME) \
JSExport<MODULE>::AddFunctionProperty(#NAME, std::mem_fn(&MODULE::js_##NAME))

#define TITANIUM_FUNCTION(MODULE, NAME) \
JSValue MODULE::js_##NAME(const std::vector<JSValue>& arguments, JSObject& this_object)

#define ENSURE_ARGUMENT_BOUNDS(...)

#define ENSURE_STRING_AT_INDEX(OUT,INDEX) \
ENSURE_ARGUMENT_BOUNDS(INDEX); \
const auto _##INDEX = arguments.at(INDEX); \
std::string OUT; \
if (!_##INDEX.IsUndefined() && !_##INDEX.IsNull()) { \
OUT = static_cast<std::string>(_##INDEX); \
}

#define ENSURE_VALUE_AT_INDEX(OUT,INDEX) \
ENSURE_ARGUMENT_BOUNDS(INDEX); \
auto OUT = arguments.at(INDEX);

#define ENSURE_OBJECT_AT_INDEX(OUT,INDEX) \
ENSURE_ARGUMENT_BOUNDS(INDEX); \
const auto _##INDEX = arguments.at(INDEX); \
TITANIUM_ASSERT_AND_THROW(_##INDEX.IsObject(), "Expected Object"); \
auto OUT = static_cast<JSObject>(_##INDEX);

#define ENSURE_OPTIONAL_OBJECT_AT_INDEX(OUT,INDEX) \
auto OUT = this_object.get_context().CreateObject(); \
if (arguments.size() >= INDEX + 1) { \
const auto _##INDEX = arguments.at(INDEX); \
TITANIUM_ASSERT_AND_THROW(_##INDEX.IsObject(), "Expected Object"); \
OUT = static_cast<JSObject>(_##INDEX);\
}

namespace Titanium
{
	using namespace Daisy;

	/*!
	  @class

	  @discussion This is the Titanium Module.

	  See http://docs.appcelerator.com/titanium/latest/#!/api/Titanium
	 */
	class TITANIUMKIT_EXPORT Module : public JSExportObject, public JSExport<Module>
	{
	public:
		/*!
		  @method

		  @abstract addEventListener( name, callback ) : void

		  @discussion Adds the specified callback as an event listener for
		  the named event.

		  @param name Name of the event (String).

		  @param callback Callback function to invoke when the event is
		  fired (Function).

		  @result void
		*/
		virtual void addEventListener(const std::string& name, JSObject& callback, JSObject& this_object) TITANIUM_NOEXCEPT final;

		/*!
		  @method

		  @abstract addEventListener( name, callback ) : void

		  @discussion Removes the specified callback as an event listener
		  for the named event.

		  Multiple listeners can be registered for the same event, so the
		  callback parameter is used to determine which listener to remove.

		  When adding a listener, you must save a reference to the callback
		  function in order to remove the listener later:

		  @param name Name of the event (String).

		  @param callback Callback function to invoke when the event is
		  fired (Function).

		  @result void
		*/
		virtual void removeEventListener(const std::string& name, JSObject& callback, JSObject& this_object) TITANIUM_NOEXCEPT final;

		/*!
		  @method

		  @abstract applyProperties( props ) : void

		  @discussion Copy the enumerable properties from the given
		  JavaScript object to this JavaScript object.

		  @param props The JavaScript object to copy the properties from
		  (Dictionary).

		  @result void
		*/
		static void applyProperties(const JSObject& props, JSObject& this_object) TITANIUM_NOEXCEPT;

		/*!
		  @method

		  @abstract fireEvent( name, event ) : void

		  @discussion Fires a synthesized event to any registered
		  listeners.

		  @param name Name of the event (String).

		  @param event A dictionary of keys and values to add to the
		  Titanium.Event object sent to the listeners. (Dictionary).

		  @result void
		*/
		virtual void fireEvent(const std::string& name) TITANIUM_NOEXCEPT final;
		virtual void fireEvent(const std::string& name, const JSObject& event) TITANIUM_NOEXCEPT final;

		virtual void showRedScreenOfDeath(const std::string& message) TITANIUM_NOEXCEPT final;

		Module(const JSContext&) TITANIUM_NOEXCEPT;
		virtual void postCallAsConstructor(const JSContext& js_context, const std::vector<JSValue>& arguments) override;

		virtual ~Module() TITANIUM_NOEXCEPT;  //= default;
		Module(const Module&) = default;
		Module& operator=(const Module&) = default;
#ifdef TITANIUM_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
		Module(Module&&) = default;
		Module& operator=(Module&&) = default;
#endif

		// TODO: The following functions can automatically be generated from
		// the YAML API docs.
		static void JSExportInitialize();

		TITANIUM_FUNCTION_DEF(addEventListener);
		TITANIUM_FUNCTION_DEF(removeEventListener);
		TITANIUM_FUNCTION_DEF(applyProperties);
		TITANIUM_FUNCTION_DEF(fireEvent);

	protected:
		/*!
		  @method

		  @abstract Notify Module subclasses that there are event listeners
		  for the event with the given name.

		  @param event_name Name of the event for which there are event
		  listeners.

		  @result void
		*/
		virtual void enableEvent(const std::string& event_name) TITANIUM_NOEXCEPT;

		/*!
		  @method

		  @abstract Notify Module subclasses that there are no longer any
		  event listeners for the event with the given name. It is
		  guaranteed that the method enableEvent will have been called
		  before disableEvent.

		  @param event_name Name of the event for which there are no longer
		  any event listeners.

		  @result void
		*/
		virtual void disableEvent(const std::string& event_name) TITANIUM_NOEXCEPT;

		/*
		 * Stop firing all events, especially used when module is closed/hidden.
		 */
		virtual void disableEvents() TITANIUM_NOEXCEPT
		{
			enableEvents__ = false;
		}

		virtual void enableEvents() TITANIUM_NOEXCEPT
		{
			enableEvents__ = true;
		}

	private:
		static unsigned eventListenerIndex(const std::vector<JSObject>& event_listener_list, const std::string& name, JSObject& callback) TITANIUM_NOEXCEPT;
#pragma warning(push)
#pragma warning(disable : 4251)
		std::unordered_map<std::string, std::vector<JSObject>> event_listener_map__;
		bool enableEvents__ { true };
#pragma warning(pop)
	};
}  // namespace Titanium

#endif  // _TITANIUM_MODULE_HPP_
