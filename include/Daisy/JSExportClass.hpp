#ifndef _DAISY_JSEXPORTCLASS_HPP_
#define _DAISY_JSEXPORTCLASS_HPP_

#include "Daisy/JSClass.hpp"

namespace Daisy {

	template<typename T>
	class DAISY_EXPORT JSExportClass final : public JSClass {
	public:
		JSExportClass() DAISY_NOEXCEPT;
		virtual ~JSExportClass() DAISY_NOEXCEPT;
		JSExportClass(const JSExportClass&)            = default;
		JSExportClass& operator=(const JSExportClass&) = default;
		JSExportClass(JSExportClass&&)                 = default;
		JSExportClass& operator=(JSExportClass&&)      = default;

		virtual void JSObjectInitializeCallback(const JSContext& js_context, JSObject& this_object) const override;
	protected:
		JSObjectFinalizeCallback js_object_finalize_callback = [](const std::uintptr_t& native_ptr) {
			delete reinterpret_cast<T*>(native_ptr);
		};
	};

	template<typename T>
	JSExportClass<T>::JSExportClass() DAISY_NOEXCEPT {
	}

	template<typename T>
	JSExportClass<T>::~JSExportClass() DAISY_NOEXCEPT {
	}

	template<typename T>
	void JSExportClass<T>::JSObjectInitializeCallback(const JSContext& js_context, JSObject& this_object) const {
		JSClass::JSObjectInitializeCallback(js_context, this_object);
		auto native_object_ptr = new T(js_context);
		native_object_ptr->postInitialize(this_object);
		this_object.SetPrivate(reinterpret_cast<std::uintptr_t>(native_object_ptr), js_object_finalize_callback);
	}

} // namespace Daisy {

#endif // _DAISY_JSEXPORTCLASS_HPP_
