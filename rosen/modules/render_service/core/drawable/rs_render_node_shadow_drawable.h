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

#ifndef RENDER_SERVICE_DRAWABLE_RS_RENDER_NODE_SHADOW_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_RENDER_NODE_SHADOW_DRAWABLE_H

#include "drawable/rs_render_node_drawable_adapter.h"

namespace OHOS::Rosen::DrawableV2 {
class RSRenderNodeShadowDrawable : public RSRenderNodeDrawableAdapter {
public:
    explicit RSRenderNodeShadowDrawable(std::shared_ptr<const RSRenderNode> node)
        : RSRenderNodeDrawableAdapter(std::move(node))
    {}
    ~RSRenderNodeShadowDrawable() override = default;

    static Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);

    void Draw(Drawing::Canvas& canvas) override;
    void DumpDrawableTree(int32_t depth, std::string& out) const override;

private:
    using Registrar = RenderNodeShadowDrawableRegistrar<OnGenerate>;
    static Registrar instance_;
};

} // namespace OHOS::Rosen::DrawableV2
#endif // RENDER_SERVICE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_H
