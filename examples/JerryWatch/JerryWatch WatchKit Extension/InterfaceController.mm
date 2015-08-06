#import "InterfaceController.h"
#include <iostream>
#include "Daisy/daisy.hpp"
#include "API.h"

using namespace Daisy;

@interface InterfaceController()
@property (weak, nonatomic) IBOutlet WKInterfaceLabel *label;
@property (weak, nonatomic) IBOutlet WKInterfaceButton *button;
@end

@implementation InterfaceController

- (IBAction) onButtonPushed {

	//
	// JavaScript
	//
	std::string script = R"(
	function run() {
		Ti.API.info('Hello, World!');
		return 'JerryScript ' + Math.random();
	}
	run();
	)";

	//
	// HAL API
	//
	JSContextGroup js_context_group;
	JSContext js_context = js_context_group.CreateContext();
	auto global_object = js_context.get_global_object();
	auto Ti = js_context.CreateObject();
	global_object.SetProperty("Ti", Ti);
	Ti.SetProperty("API", js_context.CreateObject(JSExport<TitaniumWatchKit::API>::Class()));
	
	JSValue   js_value   = js_context.JSEvaluateScript(script);

	//
	// WatchKit interface Label
	//
	[self.label setText:[NSString stringWithCString:static_cast<std::string>(js_value).c_str() encoding:NSUTF8StringEncoding]];

}

- (void)awakeWithContext:(id)context {
    [super awakeWithContext:context];
}

- (void)willActivate {
    // This method is called when watch view controller is about to be visible to user
	
    [super willActivate];
}

- (void)didDeactivate {
    // This method is called when watch view controller is no longer visible
    [super didDeactivate];
}

@end
