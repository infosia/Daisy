/**
 * Titanium.API for WatchKit
 *
 * Copyright (c) 2015 by Appcelerator, Inc. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef __JerryWatch__API__
#define __JerryWatch__API__

#include "Titanium/API.hpp"

namespace TitaniumWatchKit {
	
	using namespace Daisy;
	
	class TITANIUMKIT_EXPORT API final : public Titanium::API, public JSExport<API>
	{
	public:
		API(const JSContext&) TITANIUM_NOEXCEPT;
		
		virtual ~API() TITANIUM_NOEXCEPT;
		API(const API&) = default;
		API& operator=(const API&) = default;
#ifdef TITANIUM_MOVE_CTOR_AND_ASSIGN_DEFAULT_ENABLE
		API(API&&) = default;
		API& operator=(API&&) = default;
#endif
		
		static void JSExportInitialize();
		
	protected:
		virtual void log(const std::string& message) const TITANIUM_NOEXCEPT override final;
	};
}

#endif /* defined(__JerryWatch__API__) */
