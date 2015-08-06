/**
 * Copyright (c) 2015 by Kota Iguchi. All Rights Reserved.
 * Licensed under the terms of the Apache Public License.
 * Please see the LICENSE included with this distribution for details.
 */

#ifndef _DAISY_JSCONTEXTGROUP_HPP_
#define _DAISY_JSCONTEXTGROUP_HPP_

#include "Daisy/detail/JSBase.hpp"

namespace Daisy {

	class JSContext;
	
	class DAISY_EXPORT JSContextGroup {
	public:
		JSContextGroup() DAISY_NOEXCEPT;
		JSContext CreateContext() const DAISY_NOEXCEPT;
		
		~JSContextGroup()                         DAISY_NOEXCEPT;
		JSContextGroup(const JSContextGroup&)     DAISY_NOEXCEPT;
		JSContextGroup(JSContextGroup&&)          DAISY_NOEXCEPT;
		JSContextGroup& operator=(JSContextGroup) DAISY_NOEXCEPT;
		void swap(JSContextGroup&)                DAISY_NOEXCEPT;

	private:
		// Prevent heap based objects.
		void* operator new(std::size_t)     = delete; // #1: To prevent allocation of scalar objects
		void* operator new [] (std::size_t) = delete; // #2: To prevent allocation of array of objects
		
		DAISY_EXPORT friend bool operator==(const JSContextGroup& lhs, const JSContextGroup& rhs);
		
#undef DAISY_JSCONTEXTGROUP_LOCK_GUARD
#ifdef DAISY_THREAD_SAFE
		std::recursive_mutex mutex__;
#define DAISY_JSCONTEXTGROUP_LOCK_GUARD std::lock_guard<std::recursive_mutex> lock(mutex__)
#else
#define DAISY_JSCONTEXTGROUP_LOCK_GUARD
#endif  // DAISY_THREAD_SAFE

		void EnsureJerryInit();
		static std::size_t retainCount__;
	};

  inline
  void swap(JSContextGroup& first, JSContextGroup& second) DAISY_NOEXCEPT {
    first.swap(second);
  }
  
  // Return true if the two JSContextGroups are equal.
  inline
  bool operator==(const JSContextGroup& lhs, const JSContextGroup& rhs) {
    return true; // There's only one JSContextGroup on Daisy
  }
  
  // Return true if the two JSContextGroups are not equal.
  inline
  bool operator!=(const JSContextGroup& lhs, const JSContextGroup& rhs) {
    return false; // There's only one JSContextGroup on Daisy
  }
}

#endif // _DAISY_JSCONTEXTGROUP_HPP_
