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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_FRAME_GEOMETRY_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_FRAME_GEOMETRY_H

#include "include/core/SkPaint.h"

#include "property/rs_property_drawable.h"

namespace OHOS::Rosen {
class RSFrameGeometryDrawable : public RSPropertyDrawable {
public:
    explicit RSFrameGeometryDrawable() = default;
    ~RSFrameGeometryDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
};

// ============================================================================
// ClipFrame
class RSClipFrameDrawable : public RSPropertyDrawable {
public:
    explicit RSClipFrameDrawable() = default;
    ~RSClipFrameDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
};

// ============================================================================
//
class RSColorFilterDrawable : public RSPropertyDrawable {
public:
    explicit RSColorFilterDrawable(Drawing::Brush&& brush) : brush_(std::move(brush)) {}
    ~RSColorFilterDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);
    bool Update(const RSRenderContent& content) override;

private:
    Drawing::Brush brush_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_FRAME_GEOMETRY_H
