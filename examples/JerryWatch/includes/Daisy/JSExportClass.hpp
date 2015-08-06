#ifndef _DAISY_JSEXPORTCLASS_HPP_
#define _DAISY_JSEXPORTCLASS_HPP_

#include "Daisy/JSClass.hpp"
#include "Daisy/JSObject.hpp"

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

		virtual void ConstructorInitializeCallback(const JSContext& js_context, JSObject& this_object) const override;
		virtual JSObjectCallAsConstructorCallback getCallAsConstructorCallback() const override {
			return js_object_constructor_callback__;
		}

		virtual void SetParent(const JSClass& js_class) {
			js_class_parent__ = js_class;
		}

	protected:
		JSClass js_class_parent__;

		JSObjectFinalizeCallback js_object_finalize_callback__ = [](const std::uintptr_t& native_ptr) {
			delete reinterpret_cast<T*>(native_ptr);
		};
		JSObjectCallAsConstructorCallback js_object_constructor_callback__ = [](const JSContext& js_context, JSObject this_object, const std::vector<JSValue>& arguments) {
			auto native_object_ptr = new T(js_context);
			native_object_ptr->postInitialize(this_object);
			this_object.SetPrivate(reinterpret_cast<std::uintptr_t>(native_object_ptr), [](const std::uintptr_t& native_ptr) {
				delete reinterpret_cast<T*>(native_ptr);
			});
			native_object_ptr->postCallAsConstructor(js_context, arguments);
		};
	};

	template<typename T>
	JSExportClass<T>::JSExportClass() DAISY_NOEXCEPT {
	}

	template<typename T>
	JSExportClass<T>::~JSExportClass() DAISY_NOEXCEPT {
	}

	template<typename T>
	void JSExportClass<T>::ConstructorInitializeCallback(const JSContext& js_context, JSObject& this_object) const {
		js_class_parent__.ConstructorInitializeCallback(js_context, this_object);

		JSClass::ConstructorInitializeCallback(js_context, this_object);
		auto native_object_ptr = new T(js_context);
		native_object_ptr->postInitialize(this_object);
		this_object.SetPrivate(reinterpret_cast<std::uintptr_t>(native_object_ptr), js_object_finalize_callback__);
	}

} // namespace Daisy {

#endif // _DAISY_JSEXPORTCLASS_HPP_
