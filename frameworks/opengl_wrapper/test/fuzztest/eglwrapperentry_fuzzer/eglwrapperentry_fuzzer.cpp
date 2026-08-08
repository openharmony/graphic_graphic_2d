/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "eglwrapperentry_fuzzer.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <array>
#include <securec.h>
#include <string>

#include "egl_defs.h"
#include "egl_wrapper_entry.h"

namespace OHOS {
namespace {
constexpr size_t NAME_SIZE = 32;
constexpr size_t ENTRY_SCENARIO_COUNT = 6;
constexpr size_t BOOL_OPTION_COUNT = 2;
constexpr size_t CONFIG_COUNT = 2;
constexpr size_t SCENARIO_THREAD_API = 0;
constexpr size_t SCENARIO_CONFIG = 1;
constexpr size_t SCENARIO_CONTEXT_SURFACE = 2;
constexpr size_t SCENARIO_SYNC_IMAGE = 3;
constexpr size_t SCENARIO_KHR = 4;

class FuzzData {
public:
    FuzzData(const uint8_t* data, size_t size) : data_(data), size_(size) {}

    template<class T>
    T Read()
    {
        T value {};
        if (data_ == nullptr || pos_ > size_ || sizeof(T) > size_ - pos_) {
            return value;
        }
        if (memcpy_s(&value, sizeof(T), data_ + pos_, sizeof(T)) != EOK) {
            return {};
        }
        pos_ += sizeof(T);
        return value;
    }

    size_t Select(size_t limit)
    {
        return limit == 0 ? 0 : Read<uint8_t>() % limit;
    }

    std::string ReadName()
    {
        std::array<char, NAME_SIZE> name {};
        size_t length = Select(NAME_SIZE);
        for (size_t index = 0; index < length; ++index) {
            char value = Read<char>();
            name[index] = value == '\0' ? 'x' : value;
        }
        return std::string(name.data());
    }

private:
    const uint8_t* data_ = nullptr;
    size_t size_ = 0;
    size_t pos_ = 0;
};

class ScopedHookLoad {
public:
    ScopedHookLoad() : wasLoaded_(gWrapperHook.isLoad)
    {
        gWrapperHook.isLoad = false;
    }

    ~ScopedHookLoad()
    {
        gWrapperHook.isLoad = wasLoaded_;
    }

private:
    bool wasLoaded_;
};

EGLDisplay GetInitializedDisplay()
{
    static EGLDisplay display = []() {
        EGLDisplay wrapperDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        EGLint major = 0;
        EGLint minor = 0;
        if (wrapperDisplay == EGL_NO_DISPLAY || eglInitialize(wrapperDisplay, &major, &minor) != EGL_TRUE) {
            return EGL_NO_DISPLAY;
        }
        return wrapperDisplay;
    }();
    return display;
}

EGLDisplay SelectDisplay(FuzzData& fuzzData)
{
    EGLDisplay display = GetInitializedDisplay();
    return display != EGL_NO_DISPLAY && fuzzData.Select(BOOL_OPTION_COUNT) != 0 ? display : EGL_NO_DISPLAY;
}

void FuzzApiNames(FuzzData& fuzzData)
{
    constexpr std::array<const char*, 6> API_NAMES = {
        "eglChooseConfig",
        "eglCreateImageKHR",
        "eglPresentationTimeANDROID",
        "eglGetPlatformDisplayEXT",
        "",
        "notAnEglFunction",
    };
    std::string fuzzName = fuzzData.ReadName();
    for (const auto* name : API_NAMES) {
        (void)FindEglWrapperApi(name);
        (void)CheckEglWrapperApi(name);
    }
    (void)FindEglWrapperApi(fuzzName);
    (void)CheckEglWrapperApi(fuzzName);
}

void FuzzThreadAndApiEntries(FuzzData& fuzzData)
{
    ScopedHookLoad hookState;
    (void)gWrapperHook.wrapper.eglGetCurrentDisplay();
    (void)gWrapperHook.wrapper.eglGetCurrentSurface(fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglGetCurrentContext();
    (void)gWrapperHook.wrapper.eglGetError();
    (void)gWrapperHook.wrapper.eglBindAPI(fuzzData.Read<EGLenum>());
    (void)gWrapperHook.wrapper.eglQueryAPI();
    (void)gWrapperHook.wrapper.eglWaitGL();
    (void)gWrapperHook.wrapper.eglWaitNative(fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglWaitClient();
    (void)gWrapperHook.wrapper.eglReleaseThread();
    (void)gWrapperHook.wrapper.eglGetProcAddress(nullptr);
    (void)gWrapperHook.wrapper.eglGetProcAddress("eglCreateImageKHR");
    (void)gWrapperHook.wrapper.eglGetProcAddress("notAnEglFunction");
}

void FuzzConfigEntries(FuzzData& fuzzData)
{
    EGLDisplay display = SelectDisplay(fuzzData);
    ScopedHookLoad hookState;
    EGLConfig configs[CONFIG_COUNT] = { nullptr, nullptr };
    EGLint numConfigs = 0;
    EGLint value = 0;
    EGLint attributes[] = { EGL_SURFACE_TYPE, fuzzData.Read<EGLint>(), EGL_NONE };
    (void)gWrapperHook.wrapper.eglChooseConfig(display, attributes, configs, CONFIG_COUNT, nullptr);
    (void)gWrapperHook.wrapper.eglChooseConfig(display, attributes, configs, CONFIG_COUNT, &numConfigs);
    (void)gWrapperHook.wrapper.eglGetConfigs(display, configs, CONFIG_COUNT, nullptr);
    (void)gWrapperHook.wrapper.eglGetConfigs(display, configs, CONFIG_COUNT, &numConfigs);
    (void)gWrapperHook.wrapper.eglGetConfigAttrib(display, nullptr, fuzzData.Read<EGLint>(), &value);
    (void)gWrapperHook.wrapper.eglQueryString(display, EGL_VENDOR);
    (void)gWrapperHook.wrapper.eglQueryString(display, EGL_VERSION);
    (void)gWrapperHook.wrapper.eglQueryString(display, EGL_EXTENSIONS);
    (void)gWrapperHook.wrapper.eglQueryString(display, fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglSwapInterval(display, fuzzData.Read<EGLint>());
}

void FuzzContextAndSurfaceEntries(FuzzData& fuzzData)
{
    EGLDisplay display = SelectDisplay(fuzzData);
    ScopedHookLoad hookState;
    EGLint value = 0;
    EGLint attributes[] = { EGL_WIDTH, fuzzData.Read<EGLint>(), EGL_NONE };
    (void)gWrapperHook.wrapper.eglCreateContext(display, nullptr, EGL_NO_CONTEXT, attributes);
    (void)gWrapperHook.wrapper.eglCreatePbufferSurface(display, nullptr, attributes);
    (void)gWrapperHook.wrapper.eglCreatePixmapSurface(display, nullptr, nullptr, attributes);
    (void)gWrapperHook.wrapper.eglCreateWindowSurface(display, nullptr, nullptr, attributes);
    (void)gWrapperHook.wrapper.eglDestroyContext(display, EGL_NO_CONTEXT);
    (void)gWrapperHook.wrapper.eglDestroySurface(display, EGL_NO_SURFACE);
    (void)gWrapperHook.wrapper.eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    (void)gWrapperHook.wrapper.eglQueryContext(display, EGL_NO_CONTEXT, fuzzData.Read<EGLint>(), nullptr);
    (void)gWrapperHook.wrapper.eglQueryContext(display, EGL_NO_CONTEXT, fuzzData.Read<EGLint>(), &value);
    (void)gWrapperHook.wrapper.eglQuerySurface(display, EGL_NO_SURFACE, fuzzData.Read<EGLint>(), &value);
    (void)gWrapperHook.wrapper.eglCopyBuffers(display, EGL_NO_SURFACE, nullptr);
    (void)gWrapperHook.wrapper.eglSwapBuffers(display, EGL_NO_SURFACE);
    (void)gWrapperHook.wrapper.eglBindTexImage(display, EGL_NO_SURFACE, fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglReleaseTexImage(display, EGL_NO_SURFACE, fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglSurfaceAttrib(
        display, EGL_NO_SURFACE, fuzzData.Read<EGLint>(), fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglCreatePbufferFromClientBuffer(
        display, fuzzData.Read<EGLenum>(), nullptr, nullptr, attributes);
}

void FuzzSyncAndImageEntries(FuzzData& fuzzData)
{
    EGLDisplay display = SelectDisplay(fuzzData);
    ScopedHookLoad hookState;
    EGLAttrib value = 0;
    EGLAttrib attributes[] = { EGL_NONE };
    EGLSync sync = gWrapperHook.wrapper.eglCreateSync(display, fuzzData.Read<EGLenum>(), attributes);
    (void)gWrapperHook.wrapper.eglClientWaitSync(display, sync, fuzzData.Read<EGLint>(), fuzzData.Read<EGLTime>());
    (void)gWrapperHook.wrapper.eglGetSyncAttrib(display, sync, fuzzData.Read<EGLint>(), nullptr);
    (void)gWrapperHook.wrapper.eglGetSyncAttrib(display, sync, fuzzData.Read<EGLint>(), &value);
    (void)gWrapperHook.wrapper.eglWaitSync(display, sync, fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglDestroySync(display, sync);
    EGLImage image =
        gWrapperHook.wrapper.eglCreateImage(display, EGL_NO_CONTEXT, fuzzData.Read<EGLenum>(), nullptr, attributes);
    (void)gWrapperHook.wrapper.eglDestroyImage(display, image);
    (void)gWrapperHook.wrapper.eglGetPlatformDisplay(fuzzData.Read<EGLenum>(), EGL_DEFAULT_DISPLAY, attributes);
    (void)gWrapperHook.wrapper.eglCreatePlatformWindowSurface(display, nullptr, nullptr, attributes);
    (void)gWrapperHook.wrapper.eglCreatePlatformPixmapSurface(display, nullptr, nullptr, attributes);
}

void FuzzKhrEntries(FuzzData& fuzzData)
{
    EGLDisplay display = SelectDisplay(fuzzData);
    ScopedHookLoad hookState;
    EGLint value = 0;
    EGLuint64KHR value64 = 0;
    EGLTimeKHR time = 0;
    EGLint attributes[] = { EGL_NONE };
    EGLSyncKHR sync = gWrapperHook.wrapper.eglCreateSyncKHR(display, fuzzData.Read<EGLenum>(), attributes);
    (void)gWrapperHook.wrapper.eglClientWaitSyncKHR(display, sync, 0, fuzzData.Read<EGLTimeKHR>());
    (void)gWrapperHook.wrapper.eglGetSyncAttribKHR(display, sync, fuzzData.Read<EGLint>(), &value);
    (void)gWrapperHook.wrapper.eglSignalSyncKHR(display, sync, fuzzData.Read<EGLenum>());
    (void)gWrapperHook.wrapper.eglWaitSyncKHR(display, sync, fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglDestroySyncKHR(display, sync);
    (void)gWrapperHook.wrapper.eglLockSurfaceKHR(display, EGL_NO_SURFACE, attributes);
    (void)gWrapperHook.wrapper.eglUnlockSurfaceKHR(display, EGL_NO_SURFACE);
    EGLImageKHR image =
        gWrapperHook.wrapper.eglCreateImageKHR(display, EGL_NO_CONTEXT, fuzzData.Read<EGLenum>(), nullptr, attributes);
    (void)gWrapperHook.wrapper.eglDestroyImageKHR(display, image);
    EGLStreamKHR stream = gWrapperHook.wrapper.eglCreateStreamKHR(display, attributes);
    (void)gWrapperHook.wrapper.eglStreamAttribKHR(display, stream, fuzzData.Read<EGLenum>(), value);
    (void)gWrapperHook.wrapper.eglQueryStreamKHR(display, stream, fuzzData.Read<EGLenum>(), &value);
    (void)gWrapperHook.wrapper.eglQueryStreamu64KHR(display, stream, fuzzData.Read<EGLenum>(), &value64);
    (void)gWrapperHook.wrapper.eglQueryStreamTimeKHR(display, stream, fuzzData.Read<EGLenum>(), &time);
    (void)gWrapperHook.wrapper.eglStreamConsumerGLTextureExternalKHR(display, stream);
    (void)gWrapperHook.wrapper.eglStreamConsumerAcquireKHR(display, stream);
    (void)gWrapperHook.wrapper.eglStreamConsumerReleaseKHR(display, stream);
    (void)gWrapperHook.wrapper.eglCreateStreamProducerSurfaceKHR(display, nullptr, stream, attributes);
    (void)gWrapperHook.wrapper.eglGetStreamFileDescriptorKHR(display, stream);
    (void)gWrapperHook.wrapper.eglCreateStreamFromFileDescriptorKHR(display, -1);
    (void)gWrapperHook.wrapper.eglDestroyStreamKHR(display, stream);
}

void FuzzExtensionEntries(FuzzData& fuzzData)
{
    EGLDisplay display = SelectDisplay(fuzzData);
    ScopedHookLoad hookState;
    EGLint attributes[] = { EGL_NONE };
    EGLint rects[] = { fuzzData.Read<EGLint>(), fuzzData.Read<EGLint>(), 1, 1 };
    (void)gWrapperHook.wrapper.eglGetPlatformDisplayEXT(fuzzData.Read<EGLenum>(), EGL_DEFAULT_DISPLAY, attributes);
    (void)gWrapperHook.wrapper.eglCreatePlatformWindowSurfaceEXT(display, nullptr, nullptr, attributes);
    (void)gWrapperHook.wrapper.eglCreatePlatformPixmapSurfaceEXT(display, nullptr, nullptr, attributes);
    (void)gWrapperHook.wrapper.eglSwapBuffersWithDamageKHR(display, EGL_NO_SURFACE, rects, 1);
    (void)gWrapperHook.wrapper.eglSetDamageRegionKHR(display, EGL_NO_SURFACE, rects, 1);
    (void)gWrapperHook.wrapper.eglSwapBuffersWithDamageEXT(display, EGL_NO_SURFACE, rects, 1);
    (void)gWrapperHook.wrapper.eglGetCompositorTimingSupportedANDROID(display, EGL_NO_SURFACE, fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglGetFrameTimestampSupportedANDROID(display, EGL_NO_SURFACE, fuzzData.Read<EGLint>());
    (void)gWrapperHook.wrapper.eglPresentationTimeANDROID(display, EGL_NO_SURFACE, fuzzData.Read<EGLnsecsANDROID>());
    (void)gWrapperHook.wrapper.eglDupNativeFenceFDANDROID(display, EGL_NO_SYNC_KHR);
    gWrapperHook.wrapper.eglSetBlobCacheFuncsANDROID(display, nullptr, nullptr);
    (void)gWrapperHook.wrapper.eglGetNativeClientBufferOHOS(nullptr);
    (void)gWrapperHook.wrapper.eglGetNativeClientBufferANDROID(nullptr);
}

void FuzzEntries(FuzzData& fuzzData)
{
    switch (fuzzData.Select(ENTRY_SCENARIO_COUNT)) {
        case SCENARIO_THREAD_API:
            FuzzThreadAndApiEntries(fuzzData);
            break;
        case SCENARIO_CONFIG:
            FuzzConfigEntries(fuzzData);
            break;
        case SCENARIO_CONTEXT_SURFACE:
            FuzzContextAndSurfaceEntries(fuzzData);
            break;
        case SCENARIO_SYNC_IMAGE:
            FuzzSyncAndImageEntries(fuzzData);
            break;
        case SCENARIO_KHR:
            FuzzKhrEntries(fuzzData);
            break;
        default:
            FuzzExtensionEntries(fuzzData);
            break;
    }
}
} // namespace

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }
    FuzzData fuzzData(data, size);
    FuzzApiNames(fuzzData);
    (void)GetInitializedDisplay();
    if (gWrapperHook.wrapper.eglChooseConfig == nullptr ||
        gWrapperHook.wrapper.eglGetNativeClientBufferANDROID == nullptr) {
        return false;
    }
    FuzzEntries(fuzzData);
    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    (void)OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
