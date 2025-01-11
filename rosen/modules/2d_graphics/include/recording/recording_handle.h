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
    bool blenderEnabled = true;
    Paint::PaintStyle style = Paint::PaintStyle::PAINT_NONE;
    Color color = Color::COLOR_BLACK;
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
};

struct BrushHandle {
    Color color = Color::COLOR_BLACK;
    BlendMode mode = BlendMode::SRC_OVER;
    bool isAntiAlias = false;
    bool blenderEnabled = true;
    Filter::FilterQuality filterQuality = Filter::FilterQuality::NONE;
    OpDataHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle blenderHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;
};

struct PenHandle {
    scalar width = 0.0f;
    scalar miterLimit = 0.0f;
    Pen::CapStyle capStyle = Pen::CapStyle::FLAT_CAP;
    Pen::JoinStyle joinStyle = Pen::JoinStyle::MITER_JOIN;
    FlattenableHandle pathEffectHandle;
    Color color = Color::COLOR_BLACK;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
