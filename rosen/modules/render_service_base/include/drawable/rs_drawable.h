/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_H

#include <memory>

namespace OHOS::Rosen {
class RSRenderNode;
class RSPaintFilterCanvas;

// Pure virtual base class
class RSDrawable {
public:
    RSDrawable() = default;
    virtual ~RSDrawable() = default;

    // type definition
    using Ptr = std::shared_ptr<RSDrawable>;
    // using Vec = std::array<Ptr, static_cast<size_t>(RSDrawableSlot::MAX)>;
    // using Generator = std::function<Ptr(const RSRenderNode&)>;

    // Call in RenderThread during drawing
    virtual void OnDraw(RSPaintFilterCanvas* canvas) const = 0;

    // not copyable and moveable
    RSDrawable(const RSDrawable&) = delete;
    RSDrawable(const RSDrawable&&) = delete;
    RSDrawable& operator=(const RSDrawable&) = delete;
    RSDrawable& operator=(const RSDrawable&&) = delete;
};

class RSChildrenDrawableContent;
class RSChildrenDrawable : public RSDrawable {
public:
    RSChildrenDrawable(std::shared_ptr<const RSChildrenDrawableContent> content);
    ~RSChildrenDrawable() override = default;

    void OnDraw(RSPaintFilterCanvas* canvas) const override;

private:
    std::shared_ptr<const RSChildrenDrawableContent> content_;
};

class RSCustomModifierDrawableContent;
class RSCustomModifierDrawable : public RSDrawable {
public:
    RSCustomModifierDrawable(std::shared_ptr<const RSCustomModifierDrawableContent> content);
    ~RSCustomModifierDrawable() override = default;

    void OnDraw(RSPaintFilterCanvas* canvas) const override;

private:
    std::shared_ptr<const RSCustomModifierDrawableContent> content_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_DRAWABLE_H
