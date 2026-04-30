/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "nativeimage_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <native_image.h>

namespace OHOS {
namespace {
constexpr size_t MATRIX_SIZE = 16;
constexpr uint8_t CREATE_TYPE_TEX = 0;
constexpr uint8_t CREATE_TYPE_CONSUMER = 1;
constexpr uint8_t CREATE_TYPE_TEX_SINGLE = 2;
constexpr uint8_t CREATE_TYPE_CONSUMER_SINGLE = 3;
constexpr uint8_t CREATE_TYPE_MAX = CREATE_TYPE_CONSUMER_SINGLE;
constexpr uint32_t MAX_TEXTURE_TARGET = 0xFF;
constexpr int32_t MAX_DEFAULT_SIZE = 4096;

void OnFrameAvailable(void* context)
{
    if (context == nullptr) {
        return;
    }
    auto* callbackState = static_cast<uint8_t*>(context);
    *callbackState ^= 1;
}

OH_NativeImage* CreateNativeImage(FuzzedDataProvider& fdp)
{
    const uint32_t textureId = fdp.ConsumeIntegral<uint32_t>();
    const uint32_t textureTarget = fdp.ConsumeIntegralInRange<uint32_t>(0, MAX_TEXTURE_TARGET);
    const bool singleBufferMode = fdp.ConsumeBool();
    const uint8_t createType = fdp.ConsumeIntegralInRange<uint8_t>(0, CREATE_TYPE_MAX);
    switch (createType) {
        case CREATE_TYPE_TEX:
            return OH_NativeImage_Create(textureId, textureTarget);
        case CREATE_TYPE_CONSUMER:
            return OH_ConsumerSurface_Create();
        case CREATE_TYPE_TEX_SINGLE:
            return OH_NativeImage_CreateWithSingleBufferMode(textureId, textureTarget, singleBufferMode);
        case CREATE_TYPE_CONSUMER_SINGLE:
            return OH_ConsumerSurface_CreateWithSingleBufferMode(singleBufferMode);
        default:
            return nullptr;
    }
}

void ExerciseListenerApi(OH_NativeImage* image, FuzzedDataProvider& fdp)
{
    uint8_t callbackState = 0;
    OH_OnFrameAvailableListener listener;
    listener.context = &callbackState;
    listener.onFrameAvailable = fdp.ConsumeBool() ? OnFrameAvailable : nullptr;
    (void)OH_NativeImage_SetOnFrameAvailableListener(image, listener);
    (void)OH_NativeImage_UnsetOnFrameAvailableListener(image);
}

void ReleaseBufferIfNeeded(OH_NativeImage* image, OHNativeWindowBuffer* buffer, int fenceFd)
{
    if (buffer != nullptr) {
        (void)OH_NativeImage_ReleaseNativeWindowBuffer(image, buffer, fenceFd);
        return;
    }
    if (fenceFd >= 0) {
        close(fenceFd);
    }
}

void ExerciseBufferApis(OH_NativeImage* image)
{
    OHNativeWindowBuffer* nativeWindowBuffer = nullptr;
    int fenceFd = -1;
    (void)OH_NativeImage_AcquireNativeWindowBuffer(image, &nativeWindowBuffer, &fenceFd);
    ReleaseBufferIfNeeded(image, nativeWindowBuffer, fenceFd);

    OHNativeWindowBuffer* latestNativeWindowBuffer = nullptr;
    int latestFenceFd = -1;
    (void)OH_NativeImage_AcquireLatestNativeWindowBuffer(image, &latestNativeWindowBuffer, &latestFenceFd);
    ReleaseBufferIfNeeded(image, latestNativeWindowBuffer, latestFenceFd);
}

void ExerciseImageApis(OH_NativeImage* image, FuzzedDataProvider& fdp)
{
    (void)OH_NativeImage_AcquireNativeWindow(image);
    (void)OH_NativeImage_AttachContext(image, fdp.ConsumeIntegral<uint32_t>());
    (void)OH_NativeImage_DetachContext(image);
    (void)OH_NativeImage_UpdateSurfaceImage(image);
    (void)OH_NativeImage_GetTimestamp(image);

    float matrix[MATRIX_SIZE] = { 0 };
    (void)OH_NativeImage_GetTransformMatrix(image, matrix);
    (void)OH_NativeImage_GetTransformMatrixV2(image, matrix);
    (void)OH_NativeImage_GetBufferMatrix(image, matrix);

    uint64_t surfaceId = 0;
    (void)OH_NativeImage_GetSurfaceId(image, &surfaceId);

    ExerciseListenerApi(image, fdp);
    ExerciseBufferApis(image);

    const uint64_t usage = fdp.ConsumeIntegral<uint64_t>();
    const int32_t width = fdp.ConsumeIntegralInRange<int32_t>(1, MAX_DEFAULT_SIZE);
    const int32_t height = fdp.ConsumeIntegralInRange<int32_t>(1, MAX_DEFAULT_SIZE);
    (void)OH_ConsumerSurface_SetDefaultUsage(image, usage);
    (void)OH_ConsumerSurface_SetDefaultSize(image, width, height);
    (void)OH_NativeImage_SetDropBufferMode(image, fdp.ConsumeBool());

    (void)OH_NativeImage_ReleaseTextImage(image);
    OH_NativeBuffer_ColorSpace colorSpace {};
    (void)OH_NativeImage_GetColorSpace(image, &colorSpace);

    bool isReleased = false;
    (void)OH_NativeImage_IsReleased(image, &isReleased);
    (void)OH_NativeImage_Release(image);
}

void ExerciseInvalidInputs()
{
    float matrix[MATRIX_SIZE] = { 0 };
    uint64_t surfaceId = 0;
    int fenceFd = -1;
    OHNativeWindowBuffer* nativeWindowBuffer = nullptr;
    bool isReleased = false;
    OH_NativeBuffer_ColorSpace colorSpace {};

    (void)OH_NativeImage_AcquireNativeWindow(nullptr);
    (void)OH_NativeImage_AttachContext(nullptr, 0);
    (void)OH_NativeImage_DetachContext(nullptr);
    (void)OH_NativeImage_UpdateSurfaceImage(nullptr);
    (void)OH_NativeImage_GetTimestamp(nullptr);
    (void)OH_NativeImage_GetTransformMatrix(nullptr, matrix);
    (void)OH_NativeImage_GetTransformMatrixV2(nullptr, matrix);
    (void)OH_NativeImage_GetBufferMatrix(nullptr, matrix);
    (void)OH_NativeImage_GetSurfaceId(nullptr, &surfaceId);
    (void)OH_NativeImage_GetColorSpace(nullptr, &colorSpace);
    (void)OH_NativeImage_IsReleased(nullptr, &isReleased);
    (void)OH_NativeImage_AcquireNativeWindowBuffer(nullptr, &nativeWindowBuffer, &fenceFd);
    (void)OH_NativeImage_AcquireLatestNativeWindowBuffer(nullptr, &nativeWindowBuffer, &fenceFd);
    (void)OH_NativeImage_ReleaseNativeWindowBuffer(nullptr, nativeWindowBuffer, fenceFd);
    (void)OH_ConsumerSurface_SetDefaultUsage(nullptr, 0);
    (void)OH_ConsumerSurface_SetDefaultSize(nullptr, 1, 1);
    (void)OH_NativeImage_SetDropBufferMode(nullptr, false);
    (void)OH_NativeImage_ReleaseTextImage(nullptr);
    (void)OH_NativeImage_Release(nullptr);
    OH_NativeImage_Destroy(nullptr);
}

void RunNativeImageApiFuzz(FuzzedDataProvider& fdp)
{
    ExerciseInvalidInputs();

    OH_NativeImage* image = CreateNativeImage(fdp);
    if (image == nullptr) {
        return;
    }

    ExerciseImageApis(image, fdp);
    OH_NativeImage_Destroy(&image);
}
} // namespace
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }
    if (size < 1) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);
    OHOS::RunNativeImageApiFuzz(fdp);
    return 0;
}
