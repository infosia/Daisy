#ifndef _DAISY_JSEXPORT_HPP_
#define _DAISY_JSEXPORT_HPP_

#include "Daisy/detail/JSBase.hpp"
#include "Daisy/JSExportClass.hpp"
#include "Daisy/JSContext.hpp"
#include "Daisy/JSObject.hpp"
#include <mutex>

namespace Daisy {

	template<typename T>
	using CallNamedFunctionCallback = std::function<JSValue(T&, const std::vector<JSValue>&, JSObject&)>;

	template<typename T>
	class DAISY_EXPORT JSExport {
		
	public:
		static JSExportClass<T> Class();
		virtual ~JSExport() DAISY_NOEXCEPT {}

	protected:
		static JSExportClass<T> js_class__;
		static void AddFunctionProperty(const std::string& name, CallNamedFunctionCallback<T> callback);
		static void SetParent(const JSClass& js_class);
		static void SetClassVersion(const std::uint32_t& class_version);
	};
	
	template<typename T>
	JSExportClass<T> JSExport<T>::js_class__;

	template<typename T>
	void JSExport<T>::AddFunctionProperty(const std::string& name, CallNamedFunctionCallback<T> callback) {
		js_class__.AddFunctionProperty(name, [callback](JSObject function_object, JSObject this_object, const std::vector<JSValue>& arguments){
			auto t = reinterpret_cast<T*>(this_object.GetPrivate());
			if (t) {
				return callback(*t, arguments, this_object);
			}
			// TODO: throw runtime exception
			return this_object.get_context().CreateUndefined();
		});
	}

	template<typename T>
	JSExportClass<T> JSExport<T>::Class() {
		static std::once_flag of;
		std::call_once(of, []() {
			T::JSExportInitialize();
		});
		return js_class__;
	}

	template<typename T>
	void JSExport<T>::SetParent(const JSClass& js_class) {
		js_class__.SetParent(js_class);
	}

	template<typename T>
		void JSExport<T>::SetClassVersion(const std::uint32_t& class_version) {
	}
	
	class DAISY_EXPORT JSExportObject {
	public:
		JSExportObject(const JSContext& js_context) DAISY_NOEXCEPT
			: js_context__(js_context) {
		}

		virtual ~JSExportObject() DAISY_NOEXCEPT {}
		virtual void postInitialize(JSObject& js_object) {}
		virtual void postCallAsConstructor(const JSContext&, const std::vector<JSValue>&) {}
		
		virtual JSObject get_object() DAISY_NOEXCEPT {
			return JSObject::FindJSObjectFromPrivateData(js_context__, reinterpret_cast<std::uintptr_t>(this));
		}

		virtual JSContext get_context() {
			return js_context__;
		}

		static void JSExportInitialize() {

		}
	protected:
		JSContext js_context__;
	};
} // namespace Daisy {

#endif // _DAISY_JSEXPORT_HPP_
