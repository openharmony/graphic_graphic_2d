/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "color_manager_common.h"
#include "native_color_space_manager.h"
#include "ndk_color_space.h"
#include "platform/common/rs_log.h"

namespace {
    const std::map<ColorSpaceName, OHOS::ColorManager::ColorSpaceName> OH_TO_NATIVE_COLOR_SPACE_NAME_MAP {
        { ColorSpaceName::NONE, OHOS::ColorManager::ColorSpaceName::NONE },
        { ColorSpaceName::ADOBE_RGB, OHOS::ColorManager::ColorSpaceName::ADOBE_RGB },
        { ColorSpaceName::DCI_P3, OHOS::ColorManager::ColorSpaceName::DCI_P3 },
        { ColorSpaceName::DISPLAY_P3, OHOS::ColorManager::ColorSpaceName::DISPLAY_P3 },
        { ColorSpaceName::SRGB, OHOS::ColorManager::ColorSpaceName::SRGB },
        { ColorSpaceName::BT709, OHOS::ColorManager::ColorSpaceName::BT709 },
        { ColorSpaceName::BT601_EBU, OHOS::ColorManager::ColorSpaceName::BT601_EBU },
        { ColorSpaceName::BT601_SMPTE_C, OHOS::ColorManager::ColorSpaceName::BT601_SMPTE_C },
        { ColorSpaceName::BT2020_HLG, OHOS::ColorManager::ColorSpaceName::BT2020_HLG },
        { ColorSpaceName::BT2020_PQ, OHOS::ColorManager::ColorSpaceName::BT2020_PQ },
        { ColorSpaceName::P3_HLG, OHOS::ColorManager::ColorSpaceName::P3_HLG },
        { ColorSpaceName::P3_PQ, OHOS::ColorManager::ColorSpaceName::P3_PQ },
        { ColorSpaceName::ADOBE_RGB_LIMIT, OHOS::ColorManager::ColorSpaceName::ADOBE_RGB_LIMIT },
        { ColorSpaceName::DISPLAY_P3_LIMIT, OHOS::ColorManager::ColorSpaceName::DISPLAY_P3_LIMIT },
        { ColorSpaceName::SRGB_LIMIT, OHOS::ColorManager::ColorSpaceName::SRGB_LIMIT },
        { ColorSpaceName::BT709_LIMIT, OHOS::ColorManager::ColorSpaceName::BT709_LIMIT },
        { ColorSpaceName::BT601_EBU_LIMIT, OHOS::ColorManager::ColorSpaceName::BT601_EBU_LIMIT },
        { ColorSpaceName::BT601_SMPTE_C_LIMIT, OHOS::ColorManager::ColorSpaceName::BT601_SMPTE_C_LIMIT },
        { ColorSpaceName::BT2020_HLG_LIMIT, OHOS::ColorManager::ColorSpaceName::BT2020_HLG_LIMIT },
        { ColorSpaceName::BT2020_PQ_LIMIT, OHOS::ColorManager::ColorSpaceName::BT2020_PQ_LIMIT },
        { ColorSpaceName::P3_HLG_LIMIT, OHOS::ColorManager::ColorSpaceName::P3_HLG_LIMIT },
        { ColorSpaceName::P3_PQ_LIMIT, OHOS::ColorManager::ColorSpaceName::P3_PQ_LIMIT },
        { ColorSpaceName::LINEAR_P3, OHOS::ColorManager::ColorSpaceName::LINEAR_P3 },
        { ColorSpaceName::LINEAR_SRGB, OHOS::ColorManager::ColorSpaceName::LINEAR_SRGB },
        { ColorSpaceName::LINEAR_BT709, OHOS::ColorManager::ColorSpaceName::LINEAR_BT709 },
        { ColorSpaceName::LINEAR_BT2020, OHOS::ColorManager::ColorSpaceName::LINEAR_BT2020 },
        { ColorSpaceName::DISPLAY_SRGB, OHOS::ColorManager::ColorSpaceName::DISPLAY_SRGB },
        { ColorSpaceName::DISPLAY_P3_SRGB, OHOS::ColorManager::ColorSpaceName::DISPLAY_P3_SRGB },
        { ColorSpaceName::DISPLAY_P3_HLG, OHOS::ColorManager::ColorSpaceName::DISPLAY_P3_HLG },
        { ColorSpaceName::DISPLAY_P3_PQ, OHOS::ColorManager::ColorSpaceName::DISPLAY_P3_PQ },
        { ColorSpaceName::CUSTOM, OHOS::ColorManager::ColorSpaceName::CUSTOM },
    };

    void OHPrimariesToColorSpacePrimaries(
        const ColorSpacePrimaries& primaries, OHOS::ColorManager::ColorSpacePrimaries& csPrimaries)
    {
        csPrimaries.rX = primaries.rX;
        csPrimaries.rY = primaries.rY;
        csPrimaries.gX = primaries.gX;
        csPrimaries.gY = primaries.gY;
        csPrimaries.bX = primaries.bX;
        csPrimaries.bY = primaries.bY;
        csPrimaries.wX = primaries.wX;
        csPrimaries.wY = primaries.wY;
    }
}

static NativeColorSpaceManager* OHNativeColorSpaceManagerToNativeColorSpaceManager(
    OH_NativeColorSpaceManager* nativeColorSpaceManager)
{
    return reinterpret_cast<NativeColorSpaceManager*>(nativeColorSpaceManager);
}

static OH_NativeColorSpaceManager* NativeColorSpaceManagerToOHNativeColorSpaceManager(
    NativeColorSpaceManager* nativeColorSpaceManager)
{
    return reinterpret_cast<OH_NativeColorSpaceManager*>(nativeColorSpaceManager);
}

OH_NativeColorSpaceManager* OH_NativeColorSpaceManager_CreateFromName(
    ColorSpaceName colorSpaceName)
{
    if (colorSpaceName > ColorSpaceName::LINEAR_BT2020 || colorSpaceName < ColorSpaceName::NONE) {
        RS_LOGE("ColorSpaceName is invalid: %{public}d", OHOS::ColorManager::CMErrorCode::CM_ERROR_INVALID_PARAM);
        return nullptr;
    }
    if (colorSpaceName == ColorSpaceName::NONE || colorSpaceName == ColorSpaceName::CUSTOM) {
        RS_LOGD("ColorSpaceName is invalid: %{public}d",
            OHOS::ColorManager::CMErrorCode::CM_ERROR_ABNORMAL_PARAM_VALUE);
        return nullptr;
    }
    auto csName = OH_TO_NATIVE_COLOR_SPACE_NAME_MAP.at(colorSpaceName);
    NativeColorSpaceManager* nativeColorSpaceManager = new(std::nothrow) NativeColorSpaceManager(csName);
    if (!nativeColorSpaceManager) {
        RS_LOGE("create colorspace error");
        return nullptr;
    }
    return NativeColorSpaceManagerToOHNativeColorSpaceManager(nativeColorSpaceManager);
}

OH_NativeColorSpaceManager* OH_NativeColorSpaceManager_CreateFromPrimariesAndGamma(
    ColorSpacePrimaries primaries, float gamma)
{
    OHOS::ColorManager::ColorSpacePrimaries csPrimaries;
    OHPrimariesToColorSpacePrimaries(primaries, csPrimaries);
    NativeColorSpaceManager* nativeColorSpaceManager = new(std::nothrow) NativeColorSpaceManager(csPrimaries, gamma);
    if (!nativeColorSpaceManager) {
        RS_LOGE("create colorspace error");
        return nullptr;
    }
    return NativeColorSpaceManagerToOHNativeColorSpaceManager(nativeColorSpaceManager);
}

void OH_NativeColorSpaceManager_Destroy(OH_NativeColorSpaceManager* nativeColorSpaceManager)
{
    if (!nativeColorSpaceManager) {
        RS_LOGE("Parameter is nullptr, please check");
        return;
    }
    delete OHNativeColorSpaceManagerToNativeColorSpaceManager(nativeColorSpaceManager);
}

int OH_NativeColorSpaceManager_GetColorSpaceName(
    OH_NativeColorSpaceManager* nativeColorSpaceManager)
{
    NativeColorSpaceManager* nativeCPM = OHNativeColorSpaceManagerToNativeColorSpaceManager(
        nativeColorSpaceManager);
    if (!nativeCPM) {
        RS_LOGE("Parameter is nullptr, please check: %{public}d",
            OHOS::ColorManager::CMErrorCode::CM_ERROR_INVALID_PARAM);
        return 0;
    }
    return nativeCPM->GetInnerColorSpace().GetColorSpaceName();
}

WhitePointArray OH_NativeColorSpaceManager_GetWhitePoint(
    OH_NativeColorSpaceManager* nativeColorSpaceManager)
{
    NativeColorSpaceManager* nativeCPM = OHNativeColorSpaceManagerToNativeColorSpaceManager(
        nativeColorSpaceManager);
    if (!nativeCPM) {
        RS_LOGE("Parameter is nullptr, please check: %{public}d",
            OHOS::ColorManager::CMErrorCode::CM_ERROR_INVALID_PARAM);
        return WhitePointArray();
    }
    WhitePointArray whitePointArray;
    whitePointArray.arr[0] = nativeCPM->GetInnerColorSpace().GetWhitePoint()[0];
    whitePointArray.arr[1] = nativeCPM->GetInnerColorSpace().GetWhitePoint()[1];
    return whitePointArray;
}

float OH_NativeColorSpaceManager_GetGamma(
    OH_NativeColorSpaceManager* nativeColorSpaceManager)
{
    NativeColorSpaceManager* nativeCPM = OHNativeColorSpaceManagerToNativeColorSpaceManager(
        nativeColorSpaceManager);
    if (!nativeCPM) {
        RS_LOGE("Parameter is nullptr, please check: %{public}d",
            OHOS::ColorManager::CMErrorCode::CM_ERROR_INVALID_PARAM);
        return 0.f;
    }
    return nativeCPM->GetInnerColorSpace().GetGamma();
}