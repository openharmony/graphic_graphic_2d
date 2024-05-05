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

namespace OHOS {
namespace Rosen {
namespace Drawing {
struct ImageHandle {
    uint32_t offset;
    size_t size;
    int32_t width;
    int32_t height;
    ColorType colorType;
    AlphaType alphaType;
};

struct OpDataHandle {
    uint32_t offset;
    size_t size;
};

struct FlattenableHandle {
    uint32_t offset;
    size_t size;
    uint32_t type;
};

struct CmdListHandle {
    uint32_t type;
    uint32_t offset;
    size_t size;
    uint32_t imageOffset;
    size_t imageSize;
    uint32_t bitmapOffset;
    size_t bitmapSize;
};

struct GroupInfoHandle {
    std::pair<uint32_t, size_t> vec1;
    std::pair<uint32_t, size_t> vec2;
};

struct RenderGroupHandle {
    std::pair<uint32_t, size_t> infos;
    DrawingSColor color;
};

struct SymbolLayersHandle {
    uint32_t id;
    std::pair<uint32_t, size_t> layers;
    std::pair<uint32_t, size_t> groups;
};

struct SymbolOpHandle {
    SymbolLayersHandle symbolLayerHandle;
    OpDataHandle pathHandle;
    uint64_t symbolId = 0;
};

struct PaintHandle {
    bool isAntiAlias;
    Paint::PaintStyle style = Paint::PaintStyle::PAINT_NONE;
    Color color = Color::COLOR_BLACK;
    BlendMode mode = BlendMode::SRC_OVER;
    scalar width;
    scalar miterLimit;
    Pen::CapStyle capStyle;
    Pen::JoinStyle joinStyle;
    Filter::FilterQuality filterQuality;
    OpDataHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;
    FlattenableHandle pathEffectHandle;
    OpDataHandle blurDrawLooperHandle;
};

struct BrushHandle {
    Color color;
    BlendMode mode;
    bool isAntiAlias;
    Filter::FilterQuality filterQuality;
    OpDataHandle colorSpaceHandle;
    FlattenableHandle shaderEffectHandle;
    FlattenableHandle colorFilterHandle;
    FlattenableHandle imageFilterHandle;
    FlattenableHandle maskFilterHandle;
};

struct PenHandle {
    scalar width;
    scalar miterLimit;
    Pen::CapStyle capStyle;
    Pen::JoinStyle joinStyle;
    FlattenableHandle pathEffectHandle;
    Color color;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
