/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "setscreeninfo_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_SET_SCREEN_POWER_STATUS = 0;
const uint8_t DO_SET_SCREEN_BACK_LIGHT = 1;
const uint8_t DO_SET_SCREEN_ACTIVE_RECT = 2;
const uint8_t DO_SET_SCREEN_CORRECTION = 3;
const uint8_t DO_SET_SCREEN_SKIP_FRAME_INTERVAL = 4;
const uint8_t DO_SET_SCREEN_OFFSET = 5;
const uint8_t DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW = 6;
const uint8_t DO_SET_SCREEN_SECURITY_MASK = 7;
const uint8_t DO_SET_DUAL_SCREEN_STATE = 8;
const uint8_t DO_SET_MIRROR_SCREEN_VISIBLE_RECT = 9;
const uint8_t DO_SET_PHYSICAL_SCREEN_RESOLUTION = 10;
const uint8_t DO_SET_ROG_SCREEN_RESOLUTION = 11;
const uint8_t DO_SET_SCREEN_ACTIVE_MODE = 12;
const uint8_t DO_SET_SCREEN_CHANGE_CALLBACK = 13;
const uint8_t DO_SET_SCREEN_FRAME_GRAVITY = 14;
const uint8_t DO_SET_TP_FEATURE_CONFIG = 15;
const uint8_t TARGET_SIZE = 16;

void DoSetScreenPowerStatus(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(fdp.ConsumeIntegral<uint32_t>());
    g_rsInterfaces->SetScreenPowerStatus(id, status);
}

void DoSetScreenBackLight(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t level = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->SetScreenBacklight(id, level);
}

void DoSetScreenActiveRect(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    Rect activeRect;
    activeRect.left_ = fdp.ConsumeIntegral<int32_t>();
    activeRect.top_ = fdp.ConsumeIntegral<int32_t>();
    activeRect.width_ = fdp.ConsumeIntegral<uint32_t>();
    activeRect.height_ = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->SetScreenActiveRect(id, activeRect);
}

void DoSetScreenCorrection(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    ScreenRotation screenRotation = static_cast<ScreenRotation>(fdp.ConsumeIntegral<uint32_t>());
    g_rsInterfaces->SetScreenCorrection(id, screenRotation);
}

void DoSetScreenSkipFrameInterval(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t skipFrameInterval = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->SetScreenSkipFrameInterval(id, skipFrameInterval);
}

void DoSetScreenOffset(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t offsetX = fdp.ConsumeIntegral<int32_t>();
    int32_t offsetY = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->SetScreenOffset(id, offsetX, offsetY);
}

void DoSetCastScreenEnableSkipWindow(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    bool enable = fdp.ConsumeBool();
    g_rsInterfaces->SetCastScreenEnableSkipWindow(id, enable);
}

void DoSetScreenSecurityMask(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    Media::InitializationOptions opts;
    opts.size.width = fdp.ConsumeIntegral<int32_t>();
    opts.size.height = fdp.ConsumeIntegral<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(fdp.ConsumeIntegral<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(fdp.ConsumeIntegral<int32_t>());
    opts.editable = fdp.ConsumeBool();
    opts.useSourceIfMatch = fdp.ConsumeBool();
    std::shared_ptr<Media::PixelMap> securityMask = Media::PixelMap::Create(opts);
    if (securityMask != nullptr) {
        g_rsInterfaces->SetScreenSecurityMask(id, securityMask);
    }
}

void DoSetDualScreenState(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    DualScreenStatus status = static_cast<DualScreenStatus>(fdp.ConsumeIntegral<uint64_t>());
    g_rsInterfaces->SetDualScreenState(id, status);
}

void DoSetMirrorScreenVisibleRect(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    Rect mainScreenRect;
    mainScreenRect.left_ = fdp.ConsumeIntegral<int32_t>();
    mainScreenRect.top_ = fdp.ConsumeIntegral<int32_t>();
    mainScreenRect.width_ = fdp.ConsumeIntegral<uint32_t>();
    mainScreenRect.height_ = fdp.ConsumeIntegral<int32_t>();
    bool supportRotation = fdp.ConsumeBool();
    g_rsInterfaces->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
}

void DoSetPhysicalScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->SetPhysicalScreenResolution(id, width, height);
}

void DoSetRogScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->SetRogScreenResolution(id, width, height);
}

void DoSetScreenActiveMode(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t modeId = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->SetScreenActiveMode(id, modeId);
}

void DoSetScreenChangeCallback(FuzzedDataProvider& fdp)
{
    ScreenId screenId = fdp.ConsumeIntegral<uint64_t>();
    auto callback = [screenId](ScreenId id, ScreenEvent e, ScreenChangeReason r) {
        (void)id;
        (void)e;
        (void)r;
    };
    g_rsInterfaces->SetScreenChangeCallback(callback);
}

void DoSetScreenFrameGravity(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t gravity = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->SetScreenFrameGravity(id, gravity);
}

#ifdef TP_FEATURE_ENABLE
void DoSetTpFeatureConfig(FuzzedDataProvider& fdp)
{
    int32_t feature = fdp.ConsumeIntegral<int32_t>();
    std::string config = fdp.ConsumeRandomLengthString(10);
    auto tpFeatureConfigType = static_cast<TpFeatureConfigType>(fdp.ConsumeIntegral<uint8_t>());
    g_rsInterfaces->SetTpFeatureConfig(feature, config.c_str(), tpFeatureConfigType);
}
#endif

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_SCREEN_POWER_STATUS:
            OHOS::Rosen::DoSetScreenPowerStatus(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_BACK_LIGHT:
            OHOS::Rosen::DoSetScreenBackLight(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_RECT:
            OHOS::Rosen::DoSetScreenActiveRect(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_CORRECTION:
            OHOS::Rosen::DoSetScreenCorrection(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SKIP_FRAME_INTERVAL:
            OHOS::Rosen::DoSetScreenSkipFrameInterval(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_OFFSET:
            OHOS::Rosen::DoSetScreenOffset(fdp);
            break;
        case OHOS::Rosen::DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW:
            OHOS::Rosen::DoSetCastScreenEnableSkipWindow(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SECURITY_MASK:
            OHOS::Rosen::DoSetScreenSecurityMask(fdp);
            break;
        case OHOS::Rosen::DO_SET_DUAL_SCREEN_STATE:
            OHOS::Rosen::DoSetDualScreenState(fdp);
            break;
        case OHOS::Rosen::DO_SET_MIRROR_SCREEN_VISIBLE_RECT:
            OHOS::Rosen::DoSetMirrorScreenVisibleRect(fdp);
            break;
        case OHOS::Rosen::DO_SET_PHYSICAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetPhysicalScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_SET_ROG_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetRogScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoSetScreenActiveMode(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_CHANGE_CALLBACK:
            OHOS::Rosen::DoSetScreenChangeCallback(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_FRAME_GRAVITY:
            OHOS::Rosen::DoSetScreenFrameGravity(fdp);
            break;
#ifdef TP_FEATURE_ENABLE
        case OHOS::Rosen::DO_SET_TP_FEATURE_CONFIG:
            OHOS::Rosen::DoSetTpFeatureConfig(fdp);
            break;
#endif
        default:
            return -1;
    }
    return 0;
}
