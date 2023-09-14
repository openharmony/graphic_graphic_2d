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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H

#include "property/rs_property_drawable.h"

namespace OHOS::Rosen {
class RSBoundsGeometryDrawable : public RSPropertyDrawable {
public:
    explicit RSBoundsGeometryDrawable(const SkMatrix& boundsMatrix);
    ~RSBoundsGeometryDrawable() override = default;
    void Draw(RSModifierContext& context) override;

    static std::unique_ptr<RSPropertyDrawable> Generate(const RSProperties& properties);

private:
    SkMatrix boundsMatrix_;
};

class RSClipBoundsDrawable : public RSPropertyDrawable {
public:
    explicit RSClipBoundsDrawable(const SkRect& content) : content_(content) {}
    ~RSClipBoundsDrawable() override = default;
    void Draw(RSModifierContext& context) override;

    static std::unique_ptr<RSPropertyDrawable> Generate(const RSProperties& properties);

private:
    SkRect content_;
};

// ============================================================================
// Border
class RSBorderDrawable : public RSPropertyDrawable {
public:
    explicit RSBorderDrawable(SkPaint&& paint) : RSPropertyDrawable(), paint_(std::move(paint)) {}
    ~RSBorderDrawable() override = default;
    static std::unique_ptr<RSPropertyDrawable> Generate(const RSProperties& properties);

protected:
    SkPaint paint_;
};

class RSBorderDRRectDrawable : public RSBorderDrawable {
public:
    explicit RSBorderDRRectDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderDRRectDrawable() override = default;
    void OnGeometryChange(const RSProperties& properties) override;
    void Draw(RSModifierContext& context) override;

private:
    SkRRect inner_;
    SkRRect outer_;
};

class RSBorderFourLineDrawable : public RSBorderDrawable {
public:
    explicit RSBorderFourLineDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderFourLineDrawable() override = default;
    void OnGeometryChange(const RSProperties& properties) override;
    void Draw(RSModifierContext& context) override;

private:
    RectF rect_;
};

class RSBorderPathDrawable : public RSBorderDrawable {
public:
    explicit RSBorderPathDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderPathDrawable() override = default;
    void OnGeometryChange(const RSProperties& properties) override;
    void Draw(RSModifierContext& context) override;

private:
    SkPath borderPath_;
};

class RSBorderFourLineRoundCornerDrawable : public RSBorderDrawable {
public:
    explicit RSBorderFourLineRoundCornerDrawable(SkPaint&& paint, const RSProperties& properties);
    ~RSBorderFourLineRoundCornerDrawable() override = default;
    void OnGeometryChange(const RSProperties& properties) override;
    void Draw(RSModifierContext& context) override;

private:
    SkRRect innerRrect_;
    SkRRect rrect_;
};
// ============================================================================
}; // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_BOUNDS_GEOMETRY_H
