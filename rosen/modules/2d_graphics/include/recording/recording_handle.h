/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
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

#ifndef RECORDING_HANDLE_H
#define RECORDING_HANDLE_H

#include "draw/paint.h"
#include "draw/pen.h"
#include "effect/filter.h"
#include "utils/scalar.h"
#include "text/hm_symbol.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct ImageHandle {
    size_t offset = 0;
    size_t size = 0;
    int32_t width = 0;
    int32_t height = 0;
    ColorType colorType = COLORTYPE_UNKNOWN;
    AlphaType alphaType = ALPHATYPE_UNKNOWN;
};

struct OpDataHandle {
    size_t offset = 0;
    size_t size = 0;
};

struct FlattenableHandle {
    size_t offset = 0;
    size_t size = 0;
    uint32_t type = 0;
};

struct CmdListHandle {
    uint32_t type = 0;
    size_t offset = 0;
    size_t size = 0;
    size_t imageOffset = 0;
    size_t imageSize = 0;
    size_t bitmapOffset = 0;
    size_t bitmapSize = 0;
};

struct GroupInfoHandle {
    std::pair<size_t, size_t> vec1 = {0, 0};
    std::pair<size_t, size_t> vec2 = {0, 0};
};

struct RenderGroupHandle {
    std::pair<size_t, size_t> infos = {0, 0};
    DrawingSColor color;
};

struct SymbolLayersHandle {
    uint32_t id = 0;
    std::pair<size_t, size_t> layers = {0, 0};
    std::pair<size_t, size_t> groups = {0, 0};
};

struct LatticeHandle {
    std::pair<size_t, size_t> fXDivs = {0, 0};
    std::pair<size_t, size_t> fYDivs = {0, 0};
    std::pair<size_t, size_t> fRectTypes = {0, 0};
    int fXCount = 0;
    int fYCount = 0;
    std::pair<size_t, size_t> fBounds = {0, 0};
    std::pair<size_t, size_t> fColors = {0, 0};
};

struct SymbolOpHandle {
    SymbolLayersHandle symbolLayerHandle;
    OpDataHandle pathHandle;
    uint64_t symbolId = 0;
};

struct PaintHandle {
    bool isAntiAlias = false;
    uint8_t blenderEnabled = 1;
    Paint::PaintStyle style = Paint::PaintStyle::PAINT_NONE;
    union {
        Color color = Color::COLOR_BLACK;
        UIColor uiColor;
    };
    BlendMode mode = BlendMode::SRC_OVER;
    scalar width = 0.0f;
    scalar miterLimit = 0.0f;
    Pen::CapStyle capStyle = Pen::CapStyle::FLAT_CAP;
    Pen::JoinStyle joinStyle = Pen::JoinStyle::MITER_JOIN;
    Filter::FilterQuality filterQuality = Filter::FilterQuality::NONE;
    OpDataHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle blenderHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;
    FlattenableHandle pathEffectHandle;
    OpDataHandle blurDrawLooperHandle;

    PaintHandle()
    {
        isAntiAlias = false;
        blenderEnabled = 1;
        style = Paint::PaintStyle::PAINT_NONE;
        color = Color::COLOR_BLACK;
        mode = BlendMode::SRC_OVER;
        width = 0.0f;
        miterLimit = 0.0f;
        capStyle = Pen::CapStyle::FLAT_CAP;
        joinStyle = Pen::JoinStyle::MITER_JOIN;
        filterQuality = Filter::FilterQuality::NONE;
        colorSpaceHandle = { 0, 0 };
        shaderEffectHandle = { 0, 0, 0 };
        blenderHandle = { 0, 0, 0 };
        colorFilterHandle = { 0, 0, 0 };
        imageFilterHandle = { 0, 0, 0 };
        maskFilterHandle = { 0, 0, 0 };
        pathEffectHandle = { 0, 0, 0 };
        blurDrawLooperHandle = { 0, 0 };
    }

    PaintHandle(const PaintHandle& other)
    {
        isAntiAlias = other.isAntiAlias;
        blenderEnabled = other.blenderEnabled;
        style = other.style;
        if (other.IsUIColor()) {
            uiColor = other.uiColor;
        } else {
            color = other.color;
        }
        mode = other.mode;
        width = other.width;
        miterLimit = other.miterLimit;
        capStyle = other.capStyle;
        joinStyle = other.joinStyle;
        filterQuality = other.filterQuality;
        colorSpaceHandle = other.colorSpaceHandle;
        shaderEffectHandle = other.shaderEffectHandle;
        blenderHandle = other.blenderHandle;
        colorFilterHandle = other.colorFilterHandle;
        imageFilterHandle = other.imageFilterHandle;
        maskFilterHandle = other.maskFilterHandle;
        pathEffectHandle = other.pathEffectHandle;
        blurDrawLooperHandle = other.blurDrawLooperHandle;
    }

    ~PaintHandle() {}

    void SetIsUIColor(bool isUIColor)
    {
        if (isUIColor) {
            blenderEnabled |= 0x80;
        } else {
            blenderEnabled &= 0x7F;
        }
    }

    bool IsUIColor() const
    {
        return blenderEnabled & 0x80;
    }

    void SetBlenderEnabled(bool isBlender)
    {
        if (isBlender) {
            blenderEnabled |= 0x01;
        } else {
            blenderEnabled &= 0xFE;
        }
    }

    bool GetBlenderEnabled() const
    {
        return blenderEnabled & 0x01;
    }
};

struct BrushHandle {
    union {
        Color color = Color::COLOR_BLACK;
        UIColor uiColor;
    };
    BlendMode mode = BlendMode::SRC_OVER;
    bool isAntiAlias = false;
    uint8_t blenderEnabled = 1;
    Filter::FilterQuality filterQuality = Filter::FilterQuality::NONE;
    OpDataHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle blenderHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;

    BrushHandle()
    {
        color = Color::COLOR_BLACK;
        mode = BlendMode::SRC_OVER;
        isAntiAlias = false;
        blenderEnabled = 1;
        filterQuality = Filter::FilterQuality::NONE;
        colorSpaceHandle = { 0, 0 };
        shaderEffectHandle = { 0, 0, 0 };
        blenderHandle = { 0, 0, 0 };
        colorFilterHandle = { 0, 0, 0 };
        imageFilterHandle = { 0, 0, 0 };
        maskFilterHandle = { 0, 0, 0 };
    }

    BrushHandle(const BrushHandle& other)
    {
        if (other.IsUIColor()) {
            uiColor = other.uiColor;
        } else {
            color = other.color;
        }
        mode = other.mode;
        isAntiAlias = other.isAntiAlias;
        blenderEnabled = other.blenderEnabled;
        filterQuality = other.filterQuality;
        colorSpaceHandle = other.colorSpaceHandle;
        shaderEffectHandle = other.shaderEffectHandle;
        blenderHandle = other.blenderHandle;
        colorFilterHandle = other.colorFilterHandle;
        imageFilterHandle = other.imageFilterHandle;
        maskFilterHandle = other.maskFilterHandle;
    }

    ~BrushHandle() {}

    void SetIsUIColor(bool isUIColor)
    {
        if (isUIColor) {
            blenderEnabled |= 0x80;
        } else {
            blenderEnabled &= 0x7F;
        }
    }

    bool IsUIColor() const
    {
        return blenderEnabled & 0x80;
    }

    void SetBlenderEnabled(bool isBlender)
    {
        if (isBlender) {
            blenderEnabled |= 0x01;
        } else {
            blenderEnabled &= 0xFE;
        }
    }

    bool GetBlenderEnabled() const
    {
        return blenderEnabled & 0x01;
    }
};

struct PenHandle {
    scalar width = 0.0f;
    scalar miterLimit = 0.0f;
    Pen::CapStyle capStyle = Pen::CapStyle::FLAT_CAP;
    Pen::JoinStyle joinStyle = Pen::JoinStyle::MITER_JOIN;
    FlattenableHandle pathEffectHandle;
    union {
        Color color = Color::COLOR_BLACK;
        UIColor uiColor;
    };

    PenHandle()
    {
        width = 0.0f;
        miterLimit = 0.0f;
        capStyle = Pen::CapStyle::FLAT_CAP;
        joinStyle = Pen::JoinStyle::MITER_JOIN;
        pathEffectHandle = { 0, 0, 0 };
        color = Color::COLOR_BLACK;
    }

    PenHandle(const PenHandle& other)
    {
        width = other.width;
        miterLimit = other.miterLimit;
        capStyle = other.capStyle;
        joinStyle = other.joinStyle;
        pathEffectHandle = other.pathEffectHandle;
        if (other.IsUIColor()) {
            uiColor = other.uiColor;
        } else {
            color = other.color;
        }
    }

    ~PenHandle() {}

    void SetIsUIColor(bool isUIColor)
    {
        if (isUIColor) {
            capStyle = static_cast<Pen::CapStyle>(static_cast<uint32_t>(capStyle) | 0x80000000);
        } else {
            capStyle = static_cast<Pen::CapStyle>(static_cast<uint32_t>(capStyle) & 0x7FFFFFFF);
        }
    }

    bool IsUIColor() const
    {
        return static_cast<uint32_t>(capStyle) & 0x80000000;
    }

    void SetCapStyle(Pen::CapStyle penCapStyle)
    {
        capStyle = static_cast<Pen::CapStyle>(
            (static_cast<uint32_t>(capStyle) & 0x80000000) + static_cast<uint32_t>(penCapStyle));
    }

    Pen::CapStyle GetCapStyle() const
    {
        return static_cast<Pen::CapStyle>(static_cast<uint32_t>(capStyle) & 0x7FFFFFFF);
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
