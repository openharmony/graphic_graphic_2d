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

#include "color_picker_common.h"

#include "effect_errors.h"
#include "effect_utils.h"
namespace OHOS {
namespace Rosen {
using namespace OHOS::ColorManager;
using namespace OHOS::Media;

namespace {
constexpr uint32_t REGION_SIZE = 4;
constexpr double PROPORTION_COLORS_NUM_LIMIT = 10.0; // proportion colors limit num 10
constexpr uint32_t MALLOC_FAILED = -1;
} // namespace
thread_local std::shared_ptr<ColorPickerCommon> ColorPickerCommon::sConstructor_ = nullptr;

std::shared_ptr<ColorPickerCommon> ColorPickerCommon::CreateColorPicker(
    const std::shared_ptr<PixelMap>& pixmap, uint32_t& errorCode)
{
    auto context = std::make_shared<ColorPickerCommon>();
    if (context == nullptr) {
        EFFECT_LOG_E("[ColorPickerCommon]failed to create ColorPickerCommon with null context.");
        errorCode = MALLOC_FAILED;
        return nullptr;
    }

    context->sColorPicker_ = ColorPicker::CreateColorPicker(pixmap, errorCode);
    if (errorCode != SUCCESS) {
        EFFECT_LOG_E("[ColorPickerCommon]failed to create ColorPickerCommon with invalid value");
        return nullptr;
    }
    sConstructor_ = context;

    return context;
}

std::shared_ptr<Rosen::ColorPickerCommon> ColorPickerCommon::CreateColorPicker(
    const std::shared_ptr<Media::PixelMap>& pixmap, std::vector<double> region, uint32_t& errorCode)
{
    auto context = std::make_shared<ColorPickerCommon>();
    if (context == nullptr) {
        EFFECT_LOG_E("[ColorPickerCommon]failed to create ColorPickerCommon with null context.");
        errorCode = MALLOC_FAILED;
        return nullptr;
    }

    double coordinatesBuffer[REGION_SIZE];
    for (uint32_t i = 0; i < REGION_SIZE; i++) {
        coordinatesBuffer[i] = std::clamp<double>(region[i], 0.0, 1.0);
    }
    context->sColorPicker_ = ColorPicker::CreateColorPicker(pixmap, coordinatesBuffer, errorCode);
    if (errorCode != SUCCESS) {
        EFFECT_LOG_E("[ColorPickerCommon]failed to create ColorPickerCommon with invalid value");
        return nullptr;
    }
    sConstructor_ = context;

    return context;
}

uint32_t ColorPickerCommon::GetMainColor(ColorManager::Color& color)
{
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<Rosen::ColorPickerCommon> thisColorPicker = sConstructor_;
    if (sConstructor_ != nullptr) {
        thisColorPicker->nativeColorPicker_ = sConstructor_->sColorPicker_;
        if (thisColorPicker->nativeColorPicker_ == nullptr) {
            EFFECT_LOG_E("[ColorPickerCommon]GetMainColor, empty native ColorPicker");
            errorCode = ERROR;
            return errorCode;
        }
        errorCode = thisColorPicker->nativeColorPicker_->GetMainColor(color);
    }
    return errorCode;
}

uint32_t ColorPickerCommon::GetLargestProportionColor(ColorManager::Color& color)
{
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<Rosen::ColorPickerCommon> thisColorPicker = sConstructor_;
    if (thisColorPicker != nullptr) {
        thisColorPicker->nativeColorPicker_ = sConstructor_->sColorPicker_;
        if (thisColorPicker->nativeColorPicker_ == nullptr) {
            EFFECT_LOG_E("[ColorPickerCommon]GetLargestProportionColor, empty native ColorPicker");
            errorCode = ERROR;
            return errorCode;
        }
        errorCode = thisColorPicker->nativeColorPicker_->GetLargestProportionColor(color);
    }
    return errorCode;
}

uint32_t ColorPickerCommon::GetHighestSaturationColor(ColorManager::Color& color)
{
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<Rosen::ColorPickerCommon> thisColorPicker = sConstructor_;
    if (thisColorPicker != nullptr) {
        thisColorPicker->nativeColorPicker_ = sConstructor_->sColorPicker_;
        if (thisColorPicker->nativeColorPicker_ == nullptr) {
            EFFECT_LOG_E("[ColorPickerCommon]GetHighestSaturationColor, empty native ColorPicker");
            errorCode = ERROR;
            return errorCode;
        }
        errorCode = thisColorPicker->nativeColorPicker_->GetHighestSaturationColor(color);
    }
    return errorCode;
}

uint32_t ColorPickerCommon::GetAverageColor(ColorManager::Color& color)
{
    uint32_t errorCode = SUCCESS;
    std::shared_ptr<Rosen::ColorPickerCommon> thisColorPicker = sConstructor_;
    if (thisColorPicker != nullptr) {
        thisColorPicker->nativeColorPicker_ = sConstructor_->sColorPicker_;
        if (thisColorPicker->nativeColorPicker_ == nullptr) {
            EFFECT_LOG_E("[ColorPickerCommon]GetAverageColor, empty native ColorPicker");
            errorCode = ERROR;
            return errorCode;
        }
        errorCode = thisColorPicker->nativeColorPicker_->GetAverageColor(color);
    }

    return errorCode;
}

bool ColorPickerCommon::IsBlackOrWhiteOrGrayColor(uint32_t color, uint32_t& errorCode)
{
    bool rst = false;
    std::shared_ptr<Rosen::ColorPickerCommon> thisColorPicker = sConstructor_;
    if (thisColorPicker != nullptr) {
        thisColorPicker->nativeColorPicker_ = sConstructor_->sColorPicker_;
        if (thisColorPicker->nativeColorPicker_ == nullptr) {
            EFFECT_LOG_E("[ColorPickerCommon]IsBlackOrWhiteOrGrayColor, empty native ColorPicker");
            errorCode = ERROR;
            return rst;
        }
        rst = thisColorPicker->nativeColorPicker_->IsBlackOrWhiteOrGrayColor(color);
    }
    return rst;
}

std::vector<ColorManager::Color> ColorPickerCommon::GetTopProportionColors(double colorCount, uint32_t& errorCode)
{
    unsigned int colorsNum = 0;
    std::vector<ColorManager::Color> colors = {};
    std::shared_ptr<Rosen::ColorPickerCommon> thisColorPicker = sConstructor_;
    if (thisColorPicker != nullptr) {
        thisColorPicker->nativeColorPicker_ = sConstructor_->sColorPicker_;
        if (thisColorPicker->nativeColorPicker_ == nullptr) {
            EFFECT_LOG_E("[ColorPickerCommon]GetTopProportionColors, empty native ColorPicker");
            errorCode = ERROR;
            return colors;
        }
    }
    colorsNum = static_cast<unsigned int>(std::clamp(colorCount, 0.0, PROPORTION_COLORS_NUM_LIMIT));
    colors = thisColorPicker->nativeColorPicker_->GetTopProportionColors(colorsNum);

    return colors;
}

} // namespace Rosen
} // namespace OHOS