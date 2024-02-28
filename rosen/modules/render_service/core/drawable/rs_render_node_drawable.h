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

#include "drawable/rs_drawable.h"

#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RSRenderNode;
class RSDisplayRenderNode;
class RSCanvasRenderNode;

class RSB_EXPORT RSRenderNodeDrawable : public RSDrawable {
public:
    explicit RSRenderNodeDrawable(const std::shared_ptr<RSRenderNode>& renderNode);
    ~RSRenderNodeDrawable() override = default;

    static std::shared_ptr<RSRenderNodeDrawable> OnGenerate(std::shared_ptr<RSRenderNode> node);
    void OnDraw(RSPaintFilterCanvas& canvas) const override;

protected:
    std::shared_ptr<RSRenderNode> renderNode_;
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_H
