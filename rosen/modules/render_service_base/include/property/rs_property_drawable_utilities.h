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

#include <list>
#include <utility>

#include "property/rs_property_drawable.h"

namespace OHOS::Rosen {
// ============================================================================
// Save and Restore
class RSSaveDrawable : public RSPropertyDrawable {
public:
    explicit RSSaveDrawable(std::shared_ptr<int> content);
    ~RSSaveDrawable() override = default;
    void Draw(RSModifierContext& context) override;

private:
    std::shared_ptr<int> content_;
};

class RSRestoreDrawable : public RSPropertyDrawable {
public:
    explicit RSRestoreDrawable(std::shared_ptr<int> content);
    ~RSRestoreDrawable() override = default;
    void Draw(RSModifierContext& context) override;

private:
    std::shared_ptr<int> content_;
};

class RSCustomSaveDrawable : public RSPropertyDrawable {
public:
    explicit RSCustomSaveDrawable(
        std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content, RSPaintFilterCanvas::SaveType type);
    ~RSCustomSaveDrawable() override = default;
    void Draw(RSModifierContext& context) override;

private:
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content_;
    RSPaintFilterCanvas::SaveType type_;
};

class RSCustomRestoreDrawable : public RSPropertyDrawable {
public:
    explicit RSCustomRestoreDrawable(std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content);
    ~RSCustomRestoreDrawable() override = default;
    void Draw(RSModifierContext& context) override;

private:
    std::shared_ptr<RSPaintFilterCanvas::SaveStatus> content_;
};

// ============================================================================
// Alpha
class RSAlphaDrawable : public RSPropertyDrawable {
public:
    explicit RSAlphaDrawable(float alpha);
    ~RSAlphaDrawable() override = default;
    void Draw(RSModifierContext& context) override;

    static std::unique_ptr<RSPropertyDrawable> Generate(const RSProperties& properties);

protected:
    float alpha_;
};

class RSAlphaOffscreenDrawable : public RSAlphaDrawable {
public:
    explicit RSAlphaOffscreenDrawable(float alpha);

    void Draw(RSModifierContext& context) override;
    void OnGeometryChange(const RSProperties& properties) override;

private:
#ifndef USE_ROSEN_DRAWING
    SkRect rect_;
#else
    Drawing::Brush brush_;
#endif
};

// ============================================================================
// Adapter for DrawCmdList
class RSDrawCmdListDrawable : public RSPropertyDrawable {
public:
    explicit RSDrawCmdListDrawable(std::shared_ptr<DrawCmdList> content);
    ~RSDrawCmdListDrawable() override = default;
    void Draw(RSModifierContext& context) override;

private:
    std::shared_ptr<DrawCmdList> content_;
};

// Adapter for RSRenderModifier
class RSModifierDrawable : public RSPropertyDrawable {
public:
    explicit RSModifierDrawable(std::list<std::shared_ptr<RSRenderModifier>> content);
    ~RSModifierDrawable() override = default;

    void Draw(RSModifierContext& context) override;

private:
    std::list<std::shared_ptr<RSRenderModifier>> content_;
};
}; // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PROPERTY_RS_PROPERTY_DRAWABLE_UTILITIES_H
