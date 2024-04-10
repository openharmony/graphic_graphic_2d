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
#include <map>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "utils/rect.h"

namespace OHOS::Rosen {
class RSRenderNode;
class RSRenderParams;
namespace Drawing {
class Canvas;
}

namespace DrawableV2 {
class RSB_EXPORT RSRenderNodeDrawableAdapter {
public:
    explicit RSRenderNodeDrawableAdapter(std::shared_ptr<const RSRenderNode>&& node);
    virtual ~RSRenderNodeDrawableAdapter() = default;

    // delete
    RSRenderNodeDrawableAdapter(const RSRenderNodeDrawableAdapter&) = delete;
    RSRenderNodeDrawableAdapter(const RSRenderNodeDrawableAdapter&&) = delete;
    RSRenderNodeDrawableAdapter& operator=(const RSRenderNodeDrawableAdapter&) = delete;
    RSRenderNodeDrawableAdapter& operator=(const RSRenderNodeDrawableAdapter&&) = delete;

    using Ptr = RSRenderNodeDrawableAdapter*;
    using SharedPtr = std::shared_ptr<RSRenderNodeDrawableAdapter>;
    using WeakPtr = std::weak_ptr<RSRenderNodeDrawableAdapter>;

    virtual void Draw(Drawing::Canvas& canvas) = 0;
    virtual void DumpDrawableTree(int32_t depth, std::string& out) const;

    static SharedPtr OnGenerate(const std::shared_ptr<const RSRenderNode>& node);
    static SharedPtr GetDrawableById(NodeId id);
    static SharedPtr OnGenerateShadowDrawable(const std::shared_ptr<const RSRenderNode>& node);

    void SetSkipShadow(bool skip)
    {
        skipShadow_ = skip;
    }

protected:
    // Util functions
    std::string DumpDrawableVec() const;
    bool QuickReject(Drawing::Canvas& canvas, RectF localDrawRect);
    bool HasFilterOrEffect() const;

    // Draw functions
    void DrawAll(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawUifirstContentChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawBackground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawContent(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawChildren(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawForeground(Drawing::Canvas& canvas, const Drawing::Rect& rect) const;
    void DrawBackgroundWithoutFilterAndEffect(Drawing::Canvas& canvas, const RSRenderParams& params) const;
    void DrawRangeImpl(Drawing::Canvas& canvas, const Drawing::Rect& rect, int8_t start, int8_t end) const;

    // Register utils
    using Generator = Ptr (*)(std::shared_ptr<const RSRenderNode>);
    template<RSRenderNodeType type, Generator generator>
    class RenderNodeDrawableRegistrar {
    public:
        RenderNodeDrawableRegistrar()
        {
            RSRenderNodeDrawableAdapter::GeneratorMap.emplace(type, generator);
        }
    };
    template<Generator generator>
    class RenderNodeShadowDrawableRegistrar {
    public:
        RenderNodeShadowDrawableRegistrar()
        {
            RSRenderNodeDrawableAdapter::shadowGenerator_ = generator;
        }
    };

    std::shared_ptr<const RSRenderNode> renderNode_;

private:
    static std::map<RSRenderNodeType, Generator> GeneratorMap;
    static Generator shadowGenerator_;
    static std::map<NodeId, WeakPtr> RenderNodeDrawableCache;
    static inline std::mutex cacheMapMutex_;
    bool skipShadow_ = false;
};

} // namespace DrawableV2
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_RENDER_NODE_DRAWABLE_ADAPTER_H