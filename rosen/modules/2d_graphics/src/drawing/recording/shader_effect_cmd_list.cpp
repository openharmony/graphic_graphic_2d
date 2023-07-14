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

#include "recording/shader_effect_cmd_list.h"

#include "recording/cmd_list_helper.h"
#include "image/image.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ShaderEffectCmdList> ShaderEffectCmdList::CreateFromData(const CmdListData& data, bool isCopy)
{
    auto cmdList = std::make_shared<ShaderEffectCmdList>();
    if (isCopy) {
        cmdList->opAllocator_.BuildFromDataWithCopy(data.first, data.second);
    }
    else {
        cmdList->opAllocator_.BuildFromData(data.first, data.second);
    }
    return cmdList;
}

std::shared_ptr<ShaderEffect> ShaderEffectCmdList::Playback() const
{
    if (opAllocator_.GetSize() == 0) {
        return nullptr;
    }

    int32_t offset = 0;
    std::shared_ptr<ShaderEffect> se = nullptr;
    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr != nullptr) {
            switch (itemPtr->GetType()) {
                case ShaderEffectOpItem::OPITEM_HEAD:
                    break;
                case ShaderEffectOpItem::CREATE_COLOR_SHADER:
                    se = static_cast<CreateColorShaderOpItem*>(itemPtr)->Playback();
                    break;
                case ShaderEffectOpItem::CREATE_BLEND_SHADER:
                    se = static_cast<CreateBlendShaderOpItem*>(itemPtr)->Playback(*this);
                    break;
                case ShaderEffectOpItem::CREATE_IMAGE_SHADER:
                    se = static_cast<CreateImageShaderOpItem*>(itemPtr)->Playback(*this);
                    break;
                case ShaderEffectOpItem::CREATE_PICTURE_SHADER:
                    se = static_cast<CreatePictureShaderOpItem*>(itemPtr)->Playback(*this);
                    break;
                case ShaderEffectOpItem::CREATE_LINEAR_GRADIENT:
                    se = static_cast<CreateLinearGradientOpItem*>(itemPtr)->Playback(*this);
                    break;
                case ShaderEffectOpItem::CREATE_RADIAL_GRADIENT:
                    se = static_cast<CreateRadialGradientOpItem*>(itemPtr)->Playback(*this);
                    break;
                case ShaderEffectOpItem::CREATE_TWO_POINT_CONICAL:
                    se = static_cast<CreateTwoPointConicalOpItem*>(itemPtr)->Playback(*this);
                    break;
                case ShaderEffectOpItem::CREATE_SWEEP_GRADIENT:
                    se = static_cast<CreateSweepGradientOpItem*>(itemPtr)->Playback(*this);
                    break;
                default:
                    LOGE("ShaderEffectCmdList unknown OpItem type!");
                    break;
            }
            offset = itemPtr->GetNextOpItemOffset();
        } else {
            LOGE("ShaderEffectCmdList Playback failed!");
            break;
        }
    } while (offset != 0);

    return se;
}

/* OpItem */
CreateColorShaderOpItem::CreateColorShaderOpItem(ColorQuad color)
    : ShaderEffectOpItem(CREATE_COLOR_SHADER), color_(color) {}

std::shared_ptr<ShaderEffect> CreateColorShaderOpItem::Playback() const
{
    return ShaderEffect::CreateColorShader(color_);
}

CreateBlendShaderOpItem::CreateBlendShaderOpItem(const CmdListHandle& dst, const CmdListHandle& src, BlendMode mode)
    : ShaderEffectOpItem(CREATE_BLEND_SHADER), dst_(dst), src_(src), mode_(mode) {}

std::shared_ptr<ShaderEffect> CreateBlendShaderOpItem::Playback(const CmdList& cmdList) const
{
    auto dst = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(cmdList, dst_);
    auto src = CmdListHelper::GetFromCmdList<ShaderEffectCmdList, ShaderEffect>(cmdList, src_);
    if (dst == nullptr || src == nullptr) {
        LOGE("ShaderEffectCmdList Playback failed!");
        return nullptr;
    }

    return ShaderEffect::CreateBlendShader(*dst, *src, mode_);
}

CreateImageShaderOpItem::CreateImageShaderOpItem(const ImageHandle& image, TileMode tileX, TileMode tileY,
    const SamplingOptions& sampling, const Matrix& matrix) : ShaderEffectOpItem(CREATE_IMAGE_SHADER),
    image_(image), tileX_(tileX), tileY_(tileY), samplingOptions_(sampling), matrix_(matrix) {}

std::shared_ptr<ShaderEffect> CreateImageShaderOpItem::Playback(const CmdList& cmdList) const
{
    auto image = CmdListHelper::GetImageFromCmdList(cmdList, image_);
    if (image == nullptr) {
        return nullptr;
    }

    return ShaderEffect::CreateImageShader(*image, tileX_, tileY_, samplingOptions_, matrix_);
}

CreatePictureShaderOpItem::CreatePictureShaderOpItem(const ImageHandle& picture, TileMode tileX, TileMode tileY,
    FilterMode mode, const Matrix& matrix, const Rect& rect) : ShaderEffectOpItem(CREATE_PICTURE_SHADER),
    picture_(picture), tileX_(tileX), tileY_(tileY), filterMode_(mode), matrix_(matrix), rect_(rect) {}

std::shared_ptr<ShaderEffect> CreatePictureShaderOpItem::Playback(const CmdList& cmdList) const
{
    auto picture = CmdListHelper::GetPictureFromCmdList(cmdList, picture_);
    if (picture == nullptr) {
        return nullptr;
    }

    return ShaderEffect::CreatePictureShader(*picture, tileX_, tileY_, filterMode_, matrix_, rect_);
}

CreateLinearGradientOpItem::CreateLinearGradientOpItem(const Point& startPt, const Point& endPt,
    const std::pair<int32_t, size_t>& colors, const std::pair<int32_t, size_t>& pos, TileMode mode)
    : ShaderEffectOpItem(CREATE_LINEAR_GRADIENT), startPt_(startPt), endPt_(endPt),
    colors_(colors), pos_(pos), mode_(mode) {}

std::shared_ptr<ShaderEffect> CreateLinearGradientOpItem::Playback(const CmdList& cmdList) const
{
    auto colors = CmdListHelper::GetVectorFromCmdList<ColorQuad>(cmdList, colors_);
    auto pos = CmdListHelper::GetVectorFromCmdList<scalar>(cmdList, pos_);

    return ShaderEffect::CreateLinearGradient(startPt_, endPt_, colors, pos, mode_);
}

CreateRadialGradientOpItem::CreateRadialGradientOpItem(const Point& centerPt, scalar radius,
    const std::pair<int32_t, size_t>& colors, const std::pair<int32_t, size_t>& pos, TileMode mode)
    : ShaderEffectOpItem(CREATE_RADIAL_GRADIENT), centerPt_(centerPt), radius_(radius),
    colors_(colors), pos_(pos), mode_(mode) {}

std::shared_ptr<ShaderEffect> CreateRadialGradientOpItem::Playback(const CmdList& cmdList) const
{
    auto colors = CmdListHelper::GetVectorFromCmdList<ColorQuad>(cmdList, colors_);
    auto pos = CmdListHelper::GetVectorFromCmdList<scalar>(cmdList, pos_);

    return ShaderEffect::CreateRadialGradient(centerPt_, radius_, colors, pos, mode_);
}

CreateTwoPointConicalOpItem::CreateTwoPointConicalOpItem(const Point& startPt, scalar startRadius, const Point& endPt,
    scalar endRadius, const std::pair<int32_t, size_t>& colors, const std::pair<int32_t, size_t>& pos, TileMode mode)
    : ShaderEffectOpItem(CREATE_TWO_POINT_CONICAL), startPt_(startPt), startRadius_(startRadius), endPt_(endPt),
    endRadius_(endRadius), colors_(colors), pos_(pos), mode_(mode) {}

std::shared_ptr<ShaderEffect> CreateTwoPointConicalOpItem::Playback(const CmdList& cmdList) const
{
    auto colors = CmdListHelper::GetVectorFromCmdList<ColorQuad>(cmdList, colors_);
    auto pos = CmdListHelper::GetVectorFromCmdList<scalar>(cmdList, pos_);

    return ShaderEffect::CreateTwoPointConical(startPt_, startRadius_, endPt_, endRadius_, colors, pos, mode_);
}

CreateSweepGradientOpItem::CreateSweepGradientOpItem(const Point& centerPt, const std::pair<int32_t, size_t>& colors,
    const std::pair<int32_t, size_t>& pos, TileMode mode, scalar startAngle, scalar endAngle)
    : ShaderEffectOpItem(CREATE_SWEEP_GRADIENT), centerPt_(centerPt), colors_(colors),
    pos_(pos), mode_(mode), startAngle_(startAngle), endAngle_(endAngle) {}

std::shared_ptr<ShaderEffect> CreateSweepGradientOpItem::Playback(const CmdList& cmdList) const
{
    auto colors = CmdListHelper::GetVectorFromCmdList<ColorQuad>(cmdList, colors_);
    auto pos = CmdListHelper::GetVectorFromCmdList<scalar>(cmdList, pos_);

    return ShaderEffect::CreateSweepGradient(centerPt_, colors, pos, mode_, startAngle_, endAngle_);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
