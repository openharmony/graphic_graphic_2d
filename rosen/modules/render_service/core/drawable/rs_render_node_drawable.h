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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_H

#include <memory>

#include "drawable/rs_render_node_drawable_adapter.h"

namespace OHOS::Rosen {
class RSRenderNode;
class RSPaintFilterCanvas;

// Used by RSUniRenderThread and RSChildrenDrawable
class RSRenderNodeDrawable : public RSRenderNodeDrawableAdapter {
public:
    explicit RSRenderNodeDrawable(std::shared_ptr<const RSRenderNode>&& node);
    ~RSRenderNodeDrawable() override = default;

    static RSRenderNodeDrawable::Ptr OnGenerate(std::shared_ptr<const RSRenderNode> node);
    void OnDraw(RSPaintFilterCanvas* canvas) const override;

protected:
    using Registrar = RenderNodeDrawableRegistrar<RSRenderNodeType::RS_NODE, OnGenerate>;
    static Registrar instance_;

    std::shared_ptr<const RSRenderNode> renderNode_;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_H
