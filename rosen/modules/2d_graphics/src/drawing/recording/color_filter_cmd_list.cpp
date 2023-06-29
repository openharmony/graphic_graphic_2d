/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "recording/color_filter_cmd_list.h"

#include "recording/cmd_list_helper.h"
#include "recording/recording_color_filter.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ColorFilterCmdList> ColorFilterCmdList::CreateFromData(const CmdListData& data)
{
    auto cmdList = std::make_shared<ColorFilterCmdList>();
    cmdList->opAllocator_.BuildFromData(data.first, data.second);
    return cmdList;
}

std::shared_ptr<ColorFilter> ColorFilterCmdList::Playback() const
{
    if (opAllocator_.GetSize() == 0) {
        return nullptr;
    }

    int32_t offset = 0;
    std::shared_ptr<ColorFilter> cf = nullptr;
    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr == nullptr) {
            LOGE("ColorFilterCmdList Playback failed!");
            break;
        }

        switch (itemPtr->GetType()) {
            case ColorFilterOpItem::OPITEM_HEAD:
                break;
            case ColorFilterOpItem::CREATE_BLEND_MODE:
                cf = static_cast<CreateBlendModeOpItem*>(itemPtr)->Playback();
                break;
            case ColorFilterOpItem::CREATE_COMPOSE:
                cf = static_cast<CreateComposeOpItem*>(itemPtr)->Playback(*this);
                break;
            case ColorFilterOpItem::CREATE_MATRIX:
                cf = static_cast<CreateMatrixOpItem*>(itemPtr)->Playback();
                break;
            case ColorFilterOpItem::CREATE_LINEAR_TO_SRGB_GAMMA:
                cf = static_cast<CreateLinearToSrgbGammaOpItem*>(itemPtr)->Playback();
                break;
            case ColorFilterOpItem::CREATE_SRGB_GAMMA_TO_LINEAR:
                cf = static_cast<CreateSrgbGammaToLinearOpItem*>(itemPtr)->Playback();
                break;
            case ColorFilterOpItem::CREATE_LUMA:
                cf = static_cast<CreateLumaOpItem*>(itemPtr)->Playback();
                break;
            case ColorFilterOpItem::COMPOSE:
                if (cf != nullptr) {
                    static_cast<ColorFilterComposeOpItem*>(itemPtr)->Playback(*cf, *this);
                }
                break;
            default:
                LOGE("ColorFilterCmdList unknown OpItem type!");
                break;
        }
        offset = itemPtr->GetNextOpItemOffset();
    } while (offset != 0);

    return cf;
}

/* OpItem */
CreateBlendModeOpItem::CreateBlendModeOpItem(ColorQuad c, BlendMode mode)
    : ColorFilterOpItem(CREATE_BLEND_MODE), color_(c), mode_(mode) {}

std::shared_ptr<ColorFilter> CreateBlendModeOpItem::Playback() const
{
    return ColorFilter::CreateBlendModeColorFilter(color_, mode_);
}

CreateComposeOpItem::CreateComposeOpItem(const CmdListHandle& f1, const CmdListHandle& f2)
    : ColorFilterOpItem(CREATE_COMPOSE), colorFilter1_(f1), colorFilter2_(f2) {}

std::shared_ptr<ColorFilter> CreateComposeOpItem::Playback(const CmdList& cmdList) const
{
    auto colorFilter1 = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(cmdList, colorFilter1_);
    auto colorFilter2 = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(cmdList, colorFilter2_);
    if (!colorFilter1 || !colorFilter2) {
        LOGE("ColorFilterCmdList Playback failed!");
        return nullptr;
    }

    return ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
}

CreateMatrixOpItem::CreateMatrixOpItem(const ColorMatrix& m) : ColorFilterOpItem(CREATE_MATRIX), colorMatrix_(m) {}

std::shared_ptr<ColorFilter> CreateMatrixOpItem::Playback() const
{
    return ColorFilter::CreateMatrixColorFilter(colorMatrix_);
}

CreateLinearToSrgbGammaOpItem::CreateLinearToSrgbGammaOpItem() : ColorFilterOpItem(CREATE_LINEAR_TO_SRGB_GAMMA) {}

std::shared_ptr<ColorFilter> CreateLinearToSrgbGammaOpItem::Playback() const
{
    return ColorFilter::CreateLinearToSrgbGamma();
}

CreateSrgbGammaToLinearOpItem::CreateSrgbGammaToLinearOpItem() : ColorFilterOpItem(CREATE_SRGB_GAMMA_TO_LINEAR) {}

std::shared_ptr<ColorFilter> CreateSrgbGammaToLinearOpItem::Playback() const
{
    return ColorFilter::CreateSrgbGammaToLinear();
}

CreateLumaOpItem::CreateLumaOpItem() : ColorFilterOpItem(CREATE_LUMA) {}

std::shared_ptr<ColorFilter> CreateLumaOpItem::Playback() const
{
    return ColorFilter::CreateLumaColorFilter();
}

ColorFilterComposeOpItem::ColorFilterComposeOpItem(const CmdListHandle& filter)
    : ColorFilterOpItem(COMPOSE), filter_(filter) {}

void ColorFilterComposeOpItem::Playback(ColorFilter& filter, const CmdList& cmdList) const
{
    auto colorFilter = CmdListHelper::GetFromCmdList<ColorFilterCmdList, ColorFilter>(cmdList, filter_);
    if (!colorFilter) {
        LOGE("ColorFilterCmdList Playback failed!");
        return;
    }

    filter.Compose(*colorFilter);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
