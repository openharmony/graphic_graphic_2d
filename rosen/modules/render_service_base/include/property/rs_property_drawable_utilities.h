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

#ifndef RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_UTILITIES_H
#define RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_UTILITIES_H

#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_property_drawable.h"

namespace OHOS::Rosen {
// ============================================================================
// alias (reference or soft link) of another drawable
class RSAliasDrawable : public RSPropertyDrawable {
public:
    explicit RSAliasDrawable(RSPropertyDrawableSlot slot);
    ~RSAliasDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

private:
    RSPropertyDrawableSlot slot_;
};

// ============================================================================
// Save and Restore
class RSSaveDrawable : public RSPropertyDrawable {
public:
    explicit RSSaveDrawable(std::shared_ptr<int> content);
    ~RSSaveDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

private:
    std::shared_ptr<int> content_;
};

class RSRestoreDrawable : public RSPropertyDrawable {
public:
    explicit RSRestoreDrawable(std::shared_ptr<int> content);
    ~RSRestoreDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

private:
    std::shared_ptr<int> content_;
};

class RSCustomSaveDrawable : public RSPropertyDrawable {
public:
    explicit RSCustomSaveDrawable(
        std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content, RSPaintFilterCanvas::SaveType type);
    ~RSCustomSaveDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

private:
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content_;
    RSPaintFilterCanvas::SaveType type_;
};

class RSCustomRestoreDrawable : public RSPropertyDrawable {
public:
    explicit RSCustomRestoreDrawable(std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content);
    ~RSCustomRestoreDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

private:
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content_;
};

// ============================================================================
// Alpha
class RSAlphaDrawable : public RSPropertyDrawable {
public:
    explicit RSAlphaDrawable(float alpha);
    ~RSAlphaDrawable() override = default;
    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

    static RSPropertyDrawable::DrawablePtr Generate(const RSRenderContent& content);

protected:
    float alpha_;
};

class RSAlphaOffscreenDrawable : public RSAlphaDrawable {
public:
    explicit RSAlphaOffscreenDrawable(float alpha);

    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;
};

// ============================================================================
// Adapter for RSRenderModifier
class RSModifierDrawable : public RSPropertyDrawable {
public:
    explicit RSModifierDrawable(RSModifierType type);
    ~RSModifierDrawable() override = default;

    void Draw(const RSRenderContent& content, RSPaintFilterCanvas& canvas) const override;

private:
    RSModifierType type_;
};
};     // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_UTILITIES_H
