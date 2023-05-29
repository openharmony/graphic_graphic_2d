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

#ifndef SHADER_EFFECT_CMD_LILST_H
#define SHADER_EFFECT_CMD_LILST_H

#include "effect/shader_effect.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ShaderEffectCmdList : public CmdList {
public:
    ShaderEffectCmdList() = default;
    ~ShaderEffectCmdList() override = default;

    uint32_t GetType() const override
    {
        return Type::SHADER_EFFECT_CMD_LIST;
    }

    /*
     * @brief       Creates a ShaderEffectCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<ShaderEffectCmdList> CreateFromData(const CmdListData& data);

    /*
     * @brief   Create a ShaderEffect by the ShaderEffectCmdList playback operation.
     */
    std::shared_ptr<ShaderEffect> Playback() const;

private:
    MemAllocator largeObjectAllocator_;
};

/* OpItem */
class ShaderEffectOpItem : public OpItem {
public:
    explicit ShaderEffectOpItem(uint32_t type) : OpItem(type) {}
    ~ShaderEffectOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD = 0,    // OPITEM_HEAD must be 0
        CREATE_COLOR_SHADER,
        CREATE_BLEND_SHADER,
        CREATE_IMAGE_SHADER,
        CREATE_PICTURE_SHADER,
        CREATE_LINEAR_GRADIENT,
        CREATE_RADIAL_GRADIENT,
        CREATE_TWO_POINT_CONICAL,
        CREATE_SWEEP_GRADIENT
    };
};

class CreateColorShaderOpItem : public ShaderEffectOpItem {
public:
    explicit CreateColorShaderOpItem(ColorQuad color);
    ~CreateColorShaderOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback() const;
private:
    ColorQuad color_;
};

class CreateBlendShaderOpItem : public ShaderEffectOpItem {
public:
    CreateBlendShaderOpItem(const CmdListHandle& dst, const CmdListHandle& src, BlendMode mode);
    ~CreateBlendShaderOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback(const CmdList& cmdList) const;
private:
    CmdListHandle dst_;
    CmdListHandle src_;
    BlendMode mode_;
};

class CreateImageShaderOpItem : public ShaderEffectOpItem {
public:
    CreateImageShaderOpItem(const ImageHandle& image, TileMode tileX, TileMode tileY,
        const SamplingOptions& sampling, const Matrix& matrix);
    ~CreateImageShaderOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback(const CmdList& cmdList) const;
private:
    ImageHandle image_;
    TileMode tileX_;
    TileMode tileY_;
    SamplingOptions samplingOptions_;
    Matrix matrix_;
};

class CreatePictureShaderOpItem : public ShaderEffectOpItem {
public:
    CreatePictureShaderOpItem(const ImageHandle& picture, TileMode tileX, TileMode tileY,
        FilterMode mode, const Matrix& matrix, const Rect& rect);
    ~CreatePictureShaderOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback(const CmdList& cmdList) const;
private:
    ImageHandle picture_;
    TileMode tileX_;
    TileMode tileY_;
    FilterMode filterMode_;
    Matrix matrix_;
    Rect rect_;
};

class CreateLinearGradientOpItem : public ShaderEffectOpItem {
public:
    CreateLinearGradientOpItem(const Point& startPt, const Point& endPt,
        const std::pair<int32_t, size_t>& colors, const std::pair<int32_t, size_t>& pos, TileMode mode);
    ~CreateLinearGradientOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback(const CmdList& cmdList) const;
private:
    Point startPt_;
    Point endPt_;
    std::pair<int32_t, size_t> colors_;
    std::pair<int32_t, size_t> pos_;
    TileMode mode_;
};
class CreateRadialGradientOpItem : public ShaderEffectOpItem {
public:
    CreateRadialGradientOpItem(const Point& centerPt, scalar radius,
        const std::pair<int32_t, size_t>& colors, const std::pair<int32_t, size_t>& pos, TileMode mode);
    ~CreateRadialGradientOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback(const CmdList& cmdList) const;
private:
    Point centerPt_;
    scalar radius_;
    std::pair<int32_t, size_t> colors_;
    std::pair<int32_t, size_t> pos_;
    TileMode mode_;
};

class CreateTwoPointConicalOpItem : public ShaderEffectOpItem {
public:
    CreateTwoPointConicalOpItem(const Point& startPt, scalar startRadius, const Point& endPt, scalar endRadius,
        const std::pair<int32_t, size_t>& colors, const std::pair<int32_t, size_t>& pos, TileMode mode);
    ~CreateTwoPointConicalOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback(const CmdList& cmdList) const;
private:
    Point startPt_;
    scalar startRadius_;
    Point endPt_;
    scalar endRadius_;
    std::pair<int32_t, size_t> colors_;
    std::pair<int32_t, size_t> pos_;
    TileMode mode_;
};

class CreateSweepGradientOpItem : public ShaderEffectOpItem {
public:
    CreateSweepGradientOpItem(const Point& centerPt, const std::pair<int32_t, size_t>& colors,
        const std::pair<int32_t, size_t>& pos, TileMode mode, scalar startAngle, scalar endAngle);
    ~CreateSweepGradientOpItem() = default;

    std::shared_ptr<ShaderEffect> Playback(const CmdList& cmdList) const;
private:
    Point centerPt_;
    std::pair<int32_t, size_t> colors_;
    std::pair<int32_t, size_t> pos_;
    TileMode mode_;
    scalar startAngle_;
    scalar endAngle_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
