/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_MODIFIER_NG_RS_CONTENT_RENDER_MODIFIER_H
#define RENDER_SERVICE_BASE_MODIFIER_NG_RS_CONTENT_RENDER_MODIFIER_H

#include "drawable/rs_render_node_drawable_adapter.h"
#include "modifier_ng/rs_render_modifier_ng.h"


namespace OHOS::Rosen::ModifierNG {
class RSB_EXPORT RSChildrenDrawable : public RSDrawable {
public:
    RSChildrenDrawable() = default;
    ~RSChildrenDrawable() override = default;

    // TBD
    static inline constexpr auto Type = ModifierNG::RSModifierType::CHILDREN;
    ModifierNG::RSModifierType GetType() const
    {
        return Type;
    };

    void Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect) override;
    void Purge() override;

protected:
    void OnSync() override;

    // std::weak_ptr<RSRenderNode> target_;

private:
    std::vector<std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter>> childrenDrawableVec_;
    std::vector<std::shared_ptr<DrawableV2::RSRenderNodeDrawableAdapter>> stagingChildrenDrawableVec_;

    // Shared Transition related
    // bool childrenHasSharedTransition_ = false;
    // bool OnSharedTransition(const std::shared_ptr<RSRenderNode>& node);
};
} // namespace OHOS::Rosen::ModifierNG
#endif // RENDER_SERVICE_BASE_MODIFIER_NG_RS_CONTENT_RENDER_MODIFIER_H
