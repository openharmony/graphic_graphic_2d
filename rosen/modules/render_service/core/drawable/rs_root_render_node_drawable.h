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

#ifndef RENDER_SERVICE_DRAWABLE_RS_ROOT_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_DRAWABLE_RS_ROOT_RENDER_NODE_DRAWABLE_H

#include "drawable/rs_canvas_render_node_drawable.h"

namespace OHOS::Rosen {
class RSRootRenderNode;

namespace DrawableV2 {
class RSRootRenderNodeDrawable : public RSCanvasRenderNodeDrawable {
public:
    explicit RSRootRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    ~RSRootRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(Drawing::Canvas& canvas) override;
    void OnCapture(Drawing::Canvas& canvas) override;

private:
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::ROOT_NODE, OnGenerate>;
    static Registrar instance_;
};
} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_DRAWABLE_RS_ROOT_RENDER_NODE_DRAWABLE_H
