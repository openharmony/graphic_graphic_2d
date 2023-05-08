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

#include "image/image.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
std::shared_ptr<ShaderEffectCmdList> ShaderEffectCmdList::CreateFromData(const CmdListData& data)
{
    auto cmdList = std::make_shared<ShaderEffectCmdList>();
    if (cmdList == nullptr) {
        LOGE("ShaderEffectCmdList create from data failed!");
        return nullptr;
    }
    cmdList->opAllocator_.BuildFromData(data.first, data.second);
    return cmdList;
}

std::shared_ptr<ShaderEffectCmdList> ShaderEffectCmdList::CreateFromData(
    const CmdListData& data, const LargeObjectData& largeObjectData)
{
    auto cmdList = std::make_shared<ShaderEffectCmdList>();
    if (cmdList == nullptr) {
        LOGE("ShaderEffectCmdList create from data failed!");
        return nullptr;
    }
    cmdList->opAllocator_.BuildFromData(data.first, data.second);

    if (largeObjectData.second) {
        cmdList->largeObjectAllocator_.BuildFromData(largeObjectData.first, largeObjectData.second);
    }

    return cmdList;
}

int ShaderEffectCmdList::AddLargeObject(const LargeObjectData& data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    void* dst = largeObjectAllocator_.Add(data.first, data.second);
    if (dst == nullptr) {
        LOGE("ShaderEffectCmdList AddLargeObject failed!");
        return 0;
    }
    return largeObjectAllocator_.AddrToOffset(dst);
}

LargeObjectData ShaderEffectCmdList::GetLargeObjectData() const
{
    const void* data = largeObjectAllocator_.GetData();
    size_t size = largeObjectAllocator_.GetSize();
    return std::pair<const void*, size_t>(data, size);
}

std::shared_ptr<ShaderEffect> ShaderEffectCmdList::Playback() const
{
    int offset = 0;
    std::shared_ptr<ShaderEffect> se = nullptr;

    do {
        OpItem* itemPtr = static_cast<OpItem*>(opAllocator_.OffsetToAddr(offset));
        if (itemPtr != nullptr) {
            switch (itemPtr->GetType()) {
                case ShaderEffectOpItem::CREATE_COLOR_SHADER:
                    se = static_cast<CreateColorShaderOpItem*>(itemPtr)->Playback();
                    break;
                case ShaderEffectOpItem::CREATE_BLEND_SHADER:
                    se = static_cast<CreateBlendShaderOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_IMAGE_SHADER:
                    se = static_cast<CreateImageShaderOpItem*>(itemPtr)->Playback(largeObjectAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_PICTURE_SHADER:
                    se = static_cast<CreatePictureShaderOpItem*>(itemPtr)->Playback(largeObjectAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_LINEAR_GRADIENT:
                    se = static_cast<CreateLinearGradientOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_RADIAL_GRADIENT:
                    se = static_cast<CreateRadialGradientOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_TWO_POINT_CONICAL:
                    se = static_cast<CreateTwoPointConicalOpItem*>(itemPtr)->Playback(opAllocator_);
                    break;
                case ShaderEffectOpItem::CREATE_SWEEP_GRADIENT:
                    se = static_cast<CreateSweepGradientOpItem*>(itemPtr)->Playback(opAllocator_);
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
static void LoadColors(const MemAllocator& allocator, const std::pair<int, size_t>& in, std::vector<ColorQuad>& out)
{
    if (in.second > 0 && allocator.OffsetToAddr(in.first) != nullptr) {
        auto* colorsData = static_cast<ColorQuad*>(allocator.OffsetToAddr(in.first));
        int colorsCount = in.second / sizeof(ColorQuad);
        for (int i = 0; i < colorsCount; i++) {
            out.push_back(*colorsData);
            colorsData++;
        }
    }
}

static void LoadPos(const MemAllocator& allocator, const std::pair<int, size_t>& in, std::vector<scalar>& out)
{
    if (in.second > 0 && allocator.OffsetToAddr(in.first) != nullptr) {
        auto* posData = static_cast<scalar*>(allocator.OffsetToAddr(in.first));
        int posCount = in.second / sizeof(scalar);
        for (int i = 0; i < posCount; i++) {
            out.push_back(*posData);
            posData++;
        }
    }
}

CreateColorShaderOpItem::CreateColorShaderOpItem(ColorQuad color)
    : ShaderEffectOpItem(CREATE_COLOR_SHADER), color_(color) {}

std::shared_ptr<ShaderEffect> CreateColorShaderOpItem::Playback() const
{
    return ShaderEffect::CreateColorShader(color_);
}

CreateBlendShaderOpItem::CreateBlendShaderOpItem(const std::pair<int, size_t>& dst, const std::pair<int, size_t>& src,
    BlendMode mode) : ShaderEffectOpItem(CREATE_BLEND_SHADER), dst_(dst), src_(src), mode_(mode) {}

std::shared_ptr<ShaderEffect> CreateBlendShaderOpItem::Playback(const MemAllocator& allocator) const
{
    void* dstPtr = allocator.OffsetToAddr(dst_.first);
    void* srcPtr = allocator.OffsetToAddr(src_.first);
    if (!dstPtr || !srcPtr) {
        LOGE("ShaderEffectCmdList offset invalid!");
        return nullptr;
    }

    auto cmdListDst = ShaderEffectCmdList::CreateFromData({ dstPtr, dst_.second });
    auto cmdListSrc = ShaderEffectCmdList::CreateFromData({ srcPtr, src_.second });
    if (!cmdListDst || !cmdListSrc) {
        LOGE("ShaderEffectCmdList create cmd list failed!");
        return nullptr;
    }

    auto dst = cmdListDst->Playback();
    auto src = cmdListSrc->Playback();
    if (!dst || !src) {
        LOGE("ShaderEffectCmdList Playback failed!");
        return nullptr;
    }

    return ShaderEffect::CreateBlendShader(*dst, *src, mode_);
}

CreateImageShaderOpItem::CreateImageShaderOpItem(const LargeObjectInfo& image, TileMode tileX, TileMode tileY,
    const SamplingOptions& sampling, const Matrix& matrix) : ShaderEffectOpItem(CREATE_IMAGE_SHADER),
    image_(image), tileX_(tileX), tileY_(tileY), samplingOptions_(sampling), matrix_(matrix) {}

std::shared_ptr<ShaderEffect> CreateImageShaderOpItem::Playback(const MemAllocator& largeObjectAllocator) const
{
    void* ptr = largeObjectAllocator.OffsetToAddr(image_.first);
    if (!ptr) {
        LOGE("image offset invalid!");
        return nullptr;
    }
    auto imageData = std::make_shared<Data>();
    imageData->BuildWithoutCopy(ptr, image_.second);
    Image image;
    if (image.Deserialize(imageData) == false) {
        LOGE("image deserialize failed!");
        return nullptr;
    }

    return ShaderEffect::CreateImageShader(image, tileX_, tileY_, samplingOptions_, matrix_);
}

CreatePictureShaderOpItem::CreatePictureShaderOpItem(const LargeObjectInfo& picture, TileMode tileX, TileMode tileY,
    FilterMode mode, const Matrix& matrix, const Rect& rect) : ShaderEffectOpItem(CREATE_PICTURE_SHADER),
    picture_(picture), tileX_(tileX), tileY_(tileY), filterMode_(mode), matrix_(matrix), rect_(rect) {}

std::shared_ptr<ShaderEffect> CreatePictureShaderOpItem::Playback(const MemAllocator& largeObjectAllocator) const
{
    void* ptr = largeObjectAllocator.OffsetToAddr(picture_.first);
    if (!ptr) {
        LOGE("picture offset invalid!");
        return nullptr;
    }
    auto pictureData = std::make_shared<Data>();
    pictureData->BuildWithoutCopy(ptr, picture_.second);
    Picture picture;
    if (picture.Deserialize(pictureData) == false) {
        LOGE("picture deserialize failed!");
        return nullptr;
    }

    return ShaderEffect::CreatePictureShader(picture, tileX_, tileY_, filterMode_, matrix_, rect_);
}

CreateLinearGradientOpItem::CreateLinearGradientOpItem(const Point& startPt, const Point& endPt,
    const std::pair<int, size_t>& colors, const std::pair<int, size_t>& pos, TileMode mode)
    : ShaderEffectOpItem(CREATE_LINEAR_GRADIENT), startPt_(startPt), endPt_(endPt),
    colors_(colors), pos_(pos), mode_(mode) {}

std::shared_ptr<ShaderEffect> CreateLinearGradientOpItem::Playback(const MemAllocator& allocator) const
{
    std::vector<ColorQuad> colors;
    LoadColors(allocator, colors_, colors);

    std::vector<scalar> pos;
    LoadPos(allocator, pos_, pos);
    return ShaderEffect::CreateLinearGradient(startPt_, endPt_, colors, pos, mode_);
}

CreateRadialGradientOpItem::CreateRadialGradientOpItem(const Point& centerPt, scalar radius,
    const std::pair<int, size_t>& colors, const std::pair<int, size_t>& pos, TileMode mode)
    : ShaderEffectOpItem(CREATE_RADIAL_GRADIENT), centerPt_(centerPt), radius_(radius),
    colors_(colors), pos_(pos), mode_(mode) {}

std::shared_ptr<ShaderEffect> CreateRadialGradientOpItem::Playback(const MemAllocator& allocator) const
{
    std::vector<ColorQuad> colors;
    LoadColors(allocator, colors_, colors);

    std::vector<scalar> pos;
    LoadPos(allocator, pos_, pos);
    return ShaderEffect::CreateRadialGradient(centerPt_, radius_, colors, pos, mode_);
}

CreateTwoPointConicalOpItem::CreateTwoPointConicalOpItem(const Point& startPt, scalar startRadius, const Point& endPt,
    scalar endRadius, const std::pair<int, size_t>& colors, const std::pair<int, size_t>& pos, TileMode mode)
    : ShaderEffectOpItem(CREATE_TWO_POINT_CONICAL), startPt_(startPt), startRadius_(startRadius), endPt_(endPt),
    endRadius_(endRadius), colors_(colors), pos_(pos), mode_(mode) {}

std::shared_ptr<ShaderEffect> CreateTwoPointConicalOpItem::Playback(const MemAllocator& allocator) const
{
    std::vector<ColorQuad> colors;
    LoadColors(allocator, colors_, colors);

    std::vector<scalar> pos;
    LoadPos(allocator, pos_, pos);
    return ShaderEffect::CreateTwoPointConical(startPt_, startRadius_, endPt_, endRadius_, colors, pos, mode_);
}

CreateSweepGradientOpItem::CreateSweepGradientOpItem(const Point& centerPt, const std::pair<int, size_t>& colors,
    const std::pair<int, size_t>& pos, TileMode mode, scalar startAngle, scalar endAngle)
    : ShaderEffectOpItem(CREATE_SWEEP_GRADIENT), centerPt_(centerPt), colors_(colors),
    pos_(pos), mode_(mode), startAngle_(startAngle), endAngle_(endAngle) {}

std::shared_ptr<ShaderEffect> CreateSweepGradientOpItem::Playback(const MemAllocator& allocator) const
{
    std::vector<ColorQuad> colors;
    LoadColors(allocator, colors_, colors);

    std::vector<scalar> pos;
    LoadPos(allocator, pos_, pos);
    return ShaderEffect::CreateSweepGradient(centerPt_, colors, pos, mode_, startAngle_, endAngle_);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
