/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cj_color_manager.h"

namespace OHOS {
namespace ColorManager {
CjColorManager::CjColorManager(std::shared_ptr<ColorSpace> ptr)
{
    colorSpaceToken_ = ptr;
}

std::tuple<int32_t, std::string, std::shared_ptr<ColorSpace>> CjColorManager::create(ApiColorSpaceType csType)
{
    if (csType == ApiColorSpaceType::UNKNOWN || csType == ApiColorSpaceType::CUSTOM) {
        CMLOGE("[CJ]ColorSpaceType is invalid: %{public}u", csType);
        return std::make_tuple(static_cast<int32_t>(CJ_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_ENUM_USAGE)),
            "Parameter value is abnormal. Cannot create color manager object using ApiColorSpaceType 5",
            nullptr);
    }
    std::shared_ptr<ColorSpace> colorSpace =
            std::make_shared<ColorSpace>(CJ_TO_NATIVE_COLOR_SPACE_NAME_MAP.at(csType));
    if (colorSpace == nullptr) {
        return std::make_tuple(static_cast<int32_t>(CJ_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR)),
            "Parameter value is abnormal. Fail to create JsColorSpaceObject with input parameter(s).",
            nullptr);
    }
    CMLOGI("[CJ]OnCreateColorSpace CreateJsColorSpaceObject is called");
    return std::make_tuple(0, "", colorSpace);
}

std::shared_ptr<ColorSpace> CjColorManager::create(ColorSpacePrimaries primaries, float gamma, int32_t* errCode)
{
    std::shared_ptr<ColorSpace> colorSpace = std::make_shared<ColorSpace>(primaries, gamma);
    if (colorSpace == nullptr) {
        *errCode = static_cast<int32_t>(CJ_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR));
        return nullptr;
    }
    CMLOGI("[CJ]OnCreateColorSpace CreateJsColorSpaceObject is called");
    *errCode = 0;
    return colorSpace;
}

uint32_t CjColorManager::GetColorSpaceName(int32_t* errCode)
{
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[CJ]colorSpaceToken_ is nullptr");
        *errCode = static_cast<int32_t>(CJ_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR));
        return 0;
    }
    ColorSpaceName csName = colorSpaceToken_->GetColorSpaceName();
    if (NATIVE_TO_CJ_COLOR_SPACE_TYPE_MAP.count(csName) != 0) {
        CMLOGI("[CJ]get color space name %{public}u, api type %{public}u",
            csName, NATIVE_TO_CJ_COLOR_SPACE_TYPE_MAP.at(csName));
        *errCode = 0;
        return static_cast<uint32_t>(NATIVE_TO_CJ_COLOR_SPACE_TYPE_MAP.at(csName));
    }
    CMLOGE("[CJ]get color space name %{public}u, but not in api type", csName);
    *errCode = static_cast<int32_t>(CJ_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_INVALID_PARAM));
    return 0;
}

std::array<float, DIMES_2> CjColorManager::GetWhitePoint(int32_t* errCode)
{
    std::array<float, DIMES_2> wp;
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[CJ]colorSpaceToken_ is nullptr");
        *errCode = static_cast<int32_t>(CJ_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR));
        return wp;
    }
    wp = colorSpaceToken_->GetWhitePoint();
    *errCode = 0;
    return wp;
}

float CjColorManager::GetGamma(int32_t* errCode)
{
    if (colorSpaceToken_ == nullptr) {
        CMLOGE("[CJ]colorSpaceToken_ is nullptr");
        *errCode = static_cast<int32_t>(CJ_TO_ERROR_CODE_MAP.at(CMError::CM_ERROR_NULLPTR));
        return 0;
    }
    float gamma = colorSpaceToken_->GetGamma();
    *errCode = 0;
    return gamma;
}
}
}