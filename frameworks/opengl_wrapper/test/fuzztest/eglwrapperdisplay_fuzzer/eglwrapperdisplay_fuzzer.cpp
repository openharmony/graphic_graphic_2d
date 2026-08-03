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

#include "eglwrapperdisplay_fuzzer.h"

#include <EGL/egl.h>
#include <array>
#include <cstdlib>
#include <mutex>
#include <securec.h>
#include <unistd.h>

#include "egl_blob_cache.h"
#include "egl_defs.h"
#include "egl_wrapper_display.h"
#include "egl_wrapper_surface.h"

namespace OHOS {
namespace {
constexpr size_t BYTE_ARRAY_SIZE = 16;
constexpr size_t DISPLAY_SCENARIO_COUNT = 4;
constexpr size_t TARGET_SCENARIO_COUNT = 2;

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

    void Fill(std::array<uint8_t, BYTE_ARRAY_SIZE>& output)
    {
        for (auto& value : output) {
            value = Read<uint8_t>();
        }
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

void FuzzBlobValues(FuzzData& fuzzData)
{
    static constexpr std::array<uint8_t, 4> KEY = { 1, 2, 3, 4 };
    static constexpr std::array<uint8_t, 4> VALUE = { 5, 6, 7, 8 };
    static constexpr std::array<uint8_t, 4> UPDATED_VALUE = { 8, 7, 6, 5 };
    std::array<uint8_t, BYTE_ARRAY_SIZE> output {};
    auto* cache = BlobCache::Get();
    cache->SetBlobLock(nullptr, KEY.size(), VALUE.data(), VALUE.size());
    cache->SetBlobLock(KEY.data(), 0, VALUE.data(), VALUE.size());
    static std::once_flag insertFlag;
    std::call_once(insertFlag, [cache]() {
        cache->SetBlobLock(KEY.data(), KEY.size(), VALUE.data(), VALUE.size());
        cache->SetBlobLock(KEY.data(), KEY.size(), UPDATED_VALUE.data(), UPDATED_VALUE.size());
    });
    EGLsizeiANDROID outputSize = static_cast<EGLsizeiANDROID>(fuzzData.Select(output.size()));
    (void)cache->GetBlobLock(nullptr, KEY.size(), output.data(), outputSize);
    (void)cache->GetBlobLock(KEY.data(), KEY.size(), output.data(), outputSize);
    (void)cache->GetBlobLock(KEY.data(), KEY.size(), output.data(), output.size());
}

void FuzzBlobHeader(FuzzData& fuzzData)
{
    alignas(uint32_t) std::array<uint8_t, CACHE_HEAD + BYTE_ARRAY_SIZE> cacheData {};
    std::array<uint8_t, BYTE_ARRAY_SIZE> payload {};
    fuzzData.Fill(payload);
    (void)memcpy_s(cacheData.data() + CACHE_HEAD, payload.size(), payload.data(), payload.size());
    auto* cache = BlobCache::Get();
    (void)cache->ValidFile(cacheData.data(), CACHE_HEAD - 1);
    (void)memcpy_s(cacheData.data(), cacheData.size(), "BAD!", CACHE_MAGIC_HEAD);
    (void)cache->ValidFile(cacheData.data(), cacheData.size());
    (void)memcpy_s(cacheData.data(), cacheData.size(), "OSOH", CACHE_MAGIC_HEAD);
    uint32_t crc = cache->CrcGen(cacheData.data() + CACHE_HEAD, cacheData.size() - CACHE_HEAD);
    (void)memcpy_s(cacheData.data() + CACHE_MAGIC_HEAD, sizeof(crc), &crc, sizeof(crc));
    (void)cache->ValidFile(cacheData.data(), cacheData.size());
    cacheData[CACHE_HEAD] ^= 1;
    (void)cache->ValidFile(cacheData.data(), cacheData.size());
}

void ReadTemporaryBlobFile(const uint8_t* cacheData, size_t writeSize)
{
    auto* cache = BlobCache::Get();
    char filePath[] = "/data/local/tmp/egl_blob_cache_XXXXXX";
    int fd = mkstemp(filePath);
    if (fd < 0) {
        cache->BlobCacheReadFromDisk(filePath);
        return;
    }
    (void)write(fd, cacheData, writeSize);
    (void)close(fd);
    cache->BlobCacheReadFromDisk(filePath);
    (void)unlink(filePath);
}

void FuzzBlobFile(FuzzData& fuzzData)
{
    constexpr size_t RECORD_DATA_SIZE = 8;
    constexpr size_t CACHE_FILE_SIZE = CACHE_HEAD + sizeof(BlobCache::CacheHeader) + RECORD_DATA_SIZE;
    alignas(BlobCache::CacheHeader) std::array<uint8_t, CACHE_FILE_SIZE> cacheData {};
    std::array<uint8_t, BYTE_ARRAY_SIZE> payload {};
    fuzzData.Fill(payload);
    auto* header = reinterpret_cast<BlobCache::CacheHeader*>(cacheData.data() + CACHE_HEAD);
    header->keySize = RECORD_DATA_SIZE / 2;
    header->valueSize = RECORD_DATA_SIZE / 2;
    (void)memcpy_s(header->mData, RECORD_DATA_SIZE, payload.data(), RECORD_DATA_SIZE);
    (void)memcpy_s(cacheData.data(), cacheData.size(), "OSOH", CACHE_MAGIC_HEAD);
    auto* cache = BlobCache::Get();
    uint32_t crc = cache->CrcGen(cacheData.data() + CACHE_HEAD, cacheData.size() - CACHE_HEAD);
    (void)memcpy_s(cacheData.data() + CACHE_MAGIC_HEAD, sizeof(crc), &crc, sizeof(crc));
    ReadTemporaryBlobFile(cacheData.data(), CACHE_HEAD - 1);
    ReadTemporaryBlobFile(cacheData.data(), cacheData.size());
}

void FuzzBlobCache(FuzzData& fuzzData)
{
    static std::once_flag diskFlag;
    std::call_once(diskFlag, [&fuzzData]() {
        BlobCache::Get()->SetCacheDir("/path/that/does/not/exist/egl_blob_cache");
        FuzzBlobFile(fuzzData);
    });
    FuzzBlobValues(fuzzData);
    BlobCache::Get()->SetCacheShaderSize(0);
    BlobCache::Get()->SetCacheShaderSize(MAX_SHADER + 1);
    BlobCache::Get()->SetCacheShaderSize(static_cast<int32_t>(fuzzData.Select(MAX_SHADER)) + 1);
    (void)BlobCache::Get()->GetCacheSize();
    (void)BlobCache::Get()->GetMapSize();
    FuzzBlobHeader(fuzzData);
}

void FuzzDisplayObjects(EglWrapperDisplay* display, FuzzData& fuzzData)
{
    ScopedHookLoad hookState;
    EGLint value = 0;
    EGLint intAttributes[] = { EGL_WIDTH, fuzzData.Read<EGLint>(), EGL_NONE };
    EGLAttrib attributes[] = { EGL_WIDTH, fuzzData.Read<EGLAttrib>(), EGL_NONE };
    auto* surface = new EglWrapperSurface(display, EGL_NO_SURFACE, nullptr, EGL_GL_COLORSPACE_SRGB_KHR);
    EGLSurface surfaceHandle = reinterpret_cast<EGLSurface>(surface);
    (void)display->QuerySurface(surfaceHandle, EGL_GL_COLORSPACE_KHR, &value);
    (void)display->CopyBuffers(surfaceHandle, nullptr);
    (void)display->SwapBuffers(surfaceHandle);
    (void)display->BindTexImage(surfaceHandle, fuzzData.Read<EGLint>());
    (void)display->ReleaseTexImage(surfaceHandle, fuzzData.Read<EGLint>());
    (void)display->SurfaceAttrib(surfaceHandle, fuzzData.Read<EGLint>(), fuzzData.Read<EGLint>());
    (void)display->LockSurfaceKHR(surfaceHandle, intAttributes);
    (void)display->UnlockSurfaceKHR(surfaceHandle);
    (void)display->CreateImage(EGL_NO_CONTEXT, fuzzData.Read<EGLenum>(), nullptr, attributes);
    (void)display->CreateImageKHR(EGL_NO_CONTEXT, fuzzData.Read<EGLenum>(), nullptr, intAttributes);
    surface->Destroy();
}

void FuzzDisplayDamage(EglWrapperDisplay* display, FuzzData& fuzzData)
{
    ScopedHookLoad hookState;
    EGLint rects[] = { fuzzData.Read<EGLint>(), fuzzData.Read<EGLint>(), 1, 1 };
    auto* surface = new EglWrapperSurface(display, EGL_NO_SURFACE);
    EGLSurface surfaceHandle = reinterpret_cast<EGLSurface>(surface);
    (void)display->SwapBuffersWithDamageKHR(surfaceHandle, nullptr, 1);
    (void)display->SwapBuffersWithDamageKHR(surfaceHandle, rects, -1);
    (void)display->SwapBuffersWithDamageKHR(surfaceHandle, rects, 1);
    (void)display->SetDamageRegionKHR(surfaceHandle, nullptr, 1);
    (void)display->SetDamageRegionKHR(surfaceHandle, rects, 1);
    (void)display->SwapBuffersWithDamageEXT(surfaceHandle, rects, 1);
    (void)display->GetCompositorTimingSupportedANDROID(surfaceHandle, fuzzData.Read<EGLint>());
    (void)display->GetFrameTimestampSupportedANDROID(surfaceHandle, fuzzData.Read<EGLint>());
    (void)display->PresentationTimeANDROID(surfaceHandle, fuzzData.Read<EGLnsecsANDROID>());
    surface->Destroy();
}

void FuzzDisplayCreation(EglWrapperDisplay* display, FuzzData& fuzzData)
{
    ScopedHookLoad hookState;
    EGLint intAttributes[] = { EGL_NONE };
    EGLAttrib attributes[] = { EGL_NONE };
    (void)display->CreateEglContext(nullptr, EGL_NO_CONTEXT, intAttributes);
    (void)display->CreateEglSurface(nullptr, nullptr, intAttributes);
    (void)display->CreatePbufferSurface(nullptr, intAttributes);
    (void)display->CreatePixmapSurface(nullptr, nullptr, intAttributes);
    (void)display->CreatePbufferFromClientBuffer(fuzzData.Read<EGLenum>(), nullptr, nullptr, intAttributes);
    (void)display->CreatePlatformWindowSurface(nullptr, nullptr, attributes);
    (void)display->CreatePlatformPixmapSurface(nullptr, nullptr, attributes);
    (void)display->CreatePlatformWindowSurfaceEXT(nullptr, nullptr, intAttributes);
    (void)display->CreatePlatformPixmapSurfaceEXT(nullptr, nullptr, intAttributes);
    (void)display->CreateStreamProducerSurfaceKHR(nullptr, EGL_NO_STREAM_KHR, intAttributes);
    (void)display->DestroyImage(EGL_NO_IMAGE);
    (void)display->DestroyImageKHR(EGL_NO_IMAGE_KHR);
}

void FuzzDisplayValidation(EglWrapperDisplay* display, FuzzData& fuzzData)
{
    EGLint value = 0;
    (void)EglWrapperDisplay::GetWrapperDisplay(EGL_NO_DISPLAY);
    (void)EglWrapperDisplay::ValidateEglContext(EGL_NO_CONTEXT);
    (void)EglWrapperDisplay::ValidateEglSurface(EGL_NO_SURFACE);
    (void)display->MakeCurrent(EGL_NO_SURFACE, reinterpret_cast<EGLSurface>(display), EGL_NO_CONTEXT);
    (void)display->DestroyEglContext(EGL_NO_CONTEXT);
    (void)display->DestroyEglSurface(EGL_NO_SURFACE);
    (void)display->QueryContext(EGL_NO_CONTEXT, fuzzData.Read<EGLint>(), &value);
    (void)display->QuerySurface(EGL_NO_SURFACE, fuzzData.Read<EGLint>(), &value);
}

void FuzzDisplay(FuzzData& fuzzData)
{
    auto* display = GetInitializedDisplay();
    if (display == nullptr) {
        (void)EglWrapperDisplay::GetWrapperDisplay(EGL_NO_DISPLAY);
        return;
    }
    switch (fuzzData.Select(DISPLAY_SCENARIO_COUNT)) {
        case 0:
            FuzzDisplayValidation(display, fuzzData);
            break;
        case 1:
            FuzzDisplayObjects(display, fuzzData);
            break;
        case 2:
            FuzzDisplayDamage(display, fuzzData);
            break;
        default:
            FuzzDisplayCreation(display, fuzzData);
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
    if (fuzzData.Select(TARGET_SCENARIO_COUNT) == 0) {
        FuzzBlobCache(fuzzData);
    } else {
        FuzzDisplay(fuzzData);
    }
    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    (void)OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
