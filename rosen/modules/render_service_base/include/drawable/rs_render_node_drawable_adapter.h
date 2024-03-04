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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_ADAPTER_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_ADAPTER_H

#include <memory>
#include <unordered_map>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS::Rosen {
class RSRenderNode;
namespace Drawing {
    class Canvas;
}

class RSB_EXPORT RSRenderNodeDrawableAdapter {
public:
    RSRenderNodeDrawableAdapter() = default;
    virtual ~RSRenderNodeDrawableAdapter() = default;

    // delete
    RSRenderNodeDrawableAdapter(const RSRenderNodeDrawableAdapter&) = delete;
    RSRenderNodeDrawableAdapter(const RSRenderNodeDrawableAdapter&&) = delete;
    RSRenderNodeDrawableAdapter& operator=(const RSRenderNodeDrawableAdapter&) = delete;
    RSRenderNodeDrawableAdapter& operator=(const RSRenderNodeDrawableAdapter&&) = delete;

    // This method can only be called in RenderThread
    virtual void OnDraw(Drawing::Canvas* canvas) const = 0;

    using Ptr = std::unique_ptr<RSRenderNodeDrawableAdapter>;
    static Ptr OnGenerate(const std::shared_ptr<const RSRenderNode>& node);

protected:
    using Generator = Ptr (*)(std::shared_ptr<const RSRenderNode>);
    static std::unordered_map<RSRenderNodeType, Generator> GeneratorMap;

    template<RSRenderNodeType type, Generator generator>
    class RenderNodeDrawableRegistrar {
    public:
        RenderNodeDrawableRegistrar()
        {
            RSRenderNodeDrawableAdapter::GeneratorMap.emplace(type, generator);
        }
    };
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_ADAPTER_H