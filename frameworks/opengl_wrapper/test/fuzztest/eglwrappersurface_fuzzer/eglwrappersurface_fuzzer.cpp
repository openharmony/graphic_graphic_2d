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

#include "eglwrappersurface_fuzzer.h"

#include <EGL/egl.h>
#include <array>
#include <securec.h>

#include "egl_wrapper_display.h"
#include "egl_wrapper_surface.h"
#include "iconsumer_surface.h"
#include "window.h"

namespace OHOS {
namespace {
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

private:
    const uint8_t* data_ = nullptr;
    size_t size_ = 0;
    size_t pos_ = 0;
};

EglWrapperDisplay* GetInitializedDisplay()
{
    static EglWrapperDisplay* display = []() {
        EGLDisplay eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        auto* wrapperDisplay = EglWrapperDisplay::GetWrapperDisplay(eglDisplay);
        if (wrapperDisplay != nullptr && !wrapperDisplay->IsReady()) {
            EGLint major = 0;
            EGLint minor = 0;
            (void)wrapperDisplay->Init(&major, &minor);
        }
        return wrapperDisplay;
    }();
    return display;
}

EGLint SelectColorSpace(FuzzData& fuzzData)
{
    constexpr std::array<EGLint, 3> COLOR_SPACES = {
        EGL_UNKNOWN,
        EGL_GL_COLORSPACE_LINEAR_KHR,
        EGL_GL_COLORSPACE_SRGB_KHR,
    };
    return COLOR_SPACES[fuzzData.Select(COLOR_SPACES.size())];
}

void ExerciseSurface(EglWrapperSurface* surface, FuzzData& fuzzData)
{
    if (surface == nullptr) {
        return;
    }
    EGLSurface handle = reinterpret_cast<EGLSurface>(surface);
    EGLint value = 0;
    (void)EglWrapperSurface::GetWrapperSurface(handle);
    (void)surface->GetEglSurface();
    (void)surface->GetNativeWindow();
    (void)surface->GetColorSpace();
    (void)surface->GetColorSpaceAttribute(EGL_GL_COLORSPACE_KHR, &value);
    (void)surface->GetColorSpaceAttribute(fuzzData.Read<EGLint>(), &value);
}

void FuzzSurfaceWithoutWindow(FuzzData& fuzzData)
{
    auto* surface = new EglWrapperSurface(GetInitializedDisplay(), EGL_NO_SURFACE, nullptr, SelectColorSpace(fuzzData));
    ExerciseSurface(surface, fuzzData);
    EglWrapperSurface::Disconnect(nullptr);
    surface->Destroy();
}

void FuzzSurfaceWithWindow(FuzzData& fuzzData)
{
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create();
    if (consumer == nullptr || consumer->GetProducer() == nullptr) {
        FuzzSurfaceWithoutWindow(fuzzData);
        return;
    }
    sptr<Surface> producer = Surface::CreateSurfaceAsProducer(consumer->GetProducer());
    if (producer == nullptr) {
        FuzzSurfaceWithoutWindow(fuzzData);
        return;
    }
    OHNativeWindow* window = CreateNativeWindowFromSurface(&producer);
    if (window == nullptr) {
        FuzzSurfaceWithoutWindow(fuzzData);
        return;
    }
    auto* surface = new EglWrapperSurface(GetInitializedDisplay(), EGL_NO_SURFACE, window, SelectColorSpace(fuzzData));
    ExerciseSurface(surface, fuzzData);
    if (fuzzData.Select(2) != 0) {
        EglWrapperSurface::Disconnect(window);
    }
    surface->Destroy();
    OH_NativeWindow_DestroyNativeWindow(window);
}
} // namespace

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }
    FuzzData fuzzData(data, size);
    if (fuzzData.Select(2) == 0) {
        FuzzSurfaceWithoutWindow(fuzzData);
    } else {
        FuzzSurfaceWithWindow(fuzzData);
    }
    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    (void)OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
