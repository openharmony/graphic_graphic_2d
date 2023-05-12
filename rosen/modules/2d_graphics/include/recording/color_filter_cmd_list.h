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

#ifndef COLOR_FILTER_CMD_LILST_H
#define COLOR_FILTER_CMD_LILST_H

#include "effect/color_filter.h"
#include "recording/cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class ColorFilterCmdList : public CmdList {
public:
    ColorFilterCmdList() = default;
    ~ColorFilterCmdList() override = default;

    /*
     * @brief       Creates a ColorFilterCmdList with contiguous buffers.
     * @param data  A contiguous buffers.
     */
    static std::shared_ptr<ColorFilterCmdList> CreateFromData(const CmdListData& data);

    /*
     * @brief  Creates a ColorFilter by the ColorFilterCmdList playback operation.
     */
    std::shared_ptr<ColorFilter> Playback() const;
};

/* OpItem */
class ColorFilterOpItem : public OpItem {
public:
    explicit ColorFilterOpItem(uint32_t type) : OpItem(type) {}
    ~ColorFilterOpItem() = default;

    enum Type : uint32_t {
        OPITEM_HEAD = 0,    // OPITEM_HEAD must be 0
        CREATE_BLEND_MODE,
        CREATE_COMPOSE,
        CREATE_MATRIX,
        CREATE_LINEAR_TO_SRGB_GAMMA,
        CREATE_SRGB_GAMMA_TO_LINEAR,
        CREATE_LUMA,
        COMPOSE
    };
};

class CreateBlendModeOpItem : public ColorFilterOpItem {
public:
    CreateBlendModeOpItem(ColorQuad c, BlendMode mode);
    ~CreateBlendModeOpItem() = default;

    std::shared_ptr<ColorFilter> Playback() const;
private:
    ColorQuad color_;
    BlendMode mode_;
};

class CreateComposeOpItem : public ColorFilterOpItem {
public:
    CreateComposeOpItem(const CmdListHandle& f1, const CmdListHandle& f2);
    ~CreateComposeOpItem() = default;

    std::shared_ptr<ColorFilter> Playback(const CmdList& cmdList) const;
private:
    CmdListHandle colorFilter1_;
    CmdListHandle colorFilter2_;
};

class CreateMatrixOpItem : public ColorFilterOpItem {
public:
    explicit CreateMatrixOpItem(const ColorMatrix& m);
    ~CreateMatrixOpItem() = default;

    std::shared_ptr<ColorFilter> Playback() const;
private:
    ColorMatrix colorMatrix_;
};

class CreateLinearToSrgbGammaOpItem : public ColorFilterOpItem {
public:
    CreateLinearToSrgbGammaOpItem();
    ~CreateLinearToSrgbGammaOpItem() = default;

    std::shared_ptr<ColorFilter> Playback() const;
};

class CreateSrgbGammaToLinearOpItem : public ColorFilterOpItem {
public:
    CreateSrgbGammaToLinearOpItem();
    ~CreateSrgbGammaToLinearOpItem() = default;

    std::shared_ptr<ColorFilter> Playback() const;
};

class CreateLumaOpItem : public ColorFilterOpItem {
public:
    CreateLumaOpItem();
    ~CreateLumaOpItem() = default;

    std::shared_ptr<ColorFilter> Playback() const;
};

class ColorFilterComposeOpItem : public ColorFilterOpItem {
public:
    explicit ColorFilterComposeOpItem(const CmdListHandle& filter);
    ~ColorFilterComposeOpItem() = default;

    void Playback(ColorFilter& filter, const CmdList& cmdList) const;
private:
    CmdListHandle filter_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
