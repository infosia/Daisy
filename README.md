# The Daisy Library

Daisy is a C++11 wrapper and binding for [JerryScript](https://github.com/Samsung/jerryscript) inspired by [HAL](https://github.com/appcelerator/HAL), which mainly targets mobile platform such as iOS/Android.

## Prerequisites

### OS X

Step 1. Install Xcode 6

Step 2. Install cmake version 3.0 (or higher) using either `sudo port install cmake` or `brew unlink cmake; brew install cmake`.

Step 3. Download our pre-compiled version of Google Test [gtest-1.7.0-osx.zip (3 MB)](http://timobile.appcelerator.com.s3.amazonaws.com/gtest-1.7.0-osx.zip), unzip anywhere (the location doesn't matter) and set the environment variable GTEST_ROOT to where you unzipped it.

Just run the following commands from your bash prompt to setup your development environment for Google Test before proceeding:

```bash
$ curl -O http://timobile.appcelerator.com.s3.amazonaws.com/gtest-1.7.0-osx.zip
$ unzip gtest-1.7.0-osx.zip
```

### iOS

Step 1. Make sure you have all prerequisites described above.

### Android

Daisy is using [android-cmake](https://github.com/taka-no-me/android-cmake). For more information about prerequisites and configurations, see [android-cmake](https://github.com/taka-no-me/android-cmake) for detail.

## How to build for OS X/iOS

By default, `build_ios.sh` build & run unit tests for OS X. To change target platform and architecture make sure change PLATFORM and ARCHTECTURE in `build_ios.sh`.

```bash
$ ./build_ios.sh
```

## How to build for Android

```bash
$ ./build_android.sh
```

## How to use

Daisy implements [HAL](https://github.com/appcelerator/HAL) API. For more information, take a look at [unit tests](tests/).

```c++
JSContextGroup js_context_group;
auto js_context = js_context_group.CreateContext();
auto js_value = js_context.JSEvaluateScript("'Hello, World!';");
XCTAssertTrue(js_value.IsString());
XCTAssertEqual("Hello, World!", static_cast<std::string>(js_value));
```
