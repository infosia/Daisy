#ifndef _DAISY_JSEXPORT_HPP_
#define _DAISY_JSEXPORT_HPP_

#include "Daisy/detail/JSBase.hpp"
#include "Daisy/JSExportClass.hpp"
#include <mutex>

namespace Daisy {

	template<typename T>
	class DAISY_EXPORT JSExport {
		
	public:
		static JSExportClass<T> Class();
		virtual ~JSExport() DAISY_NOEXCEPT {

		}
	protected:
	    virtual void postInitialize(JSObject& js_object) {

	    }
		static JSExportClass<T> js_class__;
		static void AddFunctionProperty(const std::string& name, CallAsFunctionCallback callback);

	};
	
	template<typename T>
	JSExportClass<T> JSExport<T>::js_class__;

	template<typename T>
	void JSExport<T>::AddFunctionProperty(const std::string& name, CallAsFunctionCallback callback) {
		js_class__.AddFunctionProperty(name, callback);
	}

	template<typename T>
	JSExportClass<T> JSExport<T>::Class() {
		static std::once_flag of;
		std::call_once(of, []() {
			T::JSExportInitialize();
		});
		return js_class__;
	}
	
} // namespace Daisy {

#endif // _DAISY_JSEXPORT_HPP_
