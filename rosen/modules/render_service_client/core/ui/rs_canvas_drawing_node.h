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

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_DRAWING_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_DRAWING_NODE_H

#include "pixel_map.h"

#include "ui/rs_canvas_node.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
class RSNodeMap;

class RSC_EXPORT RSCanvasDrawingNode : public RSCanvasNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasDrawingNode>;
    using SharedPtr = std::shared_ptr<RSCanvasDrawingNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::CANVAS_DRAWING_NODE;

    RSUINodeType GetType() const override
    {
        return Type;
    }

    ~RSCanvasDrawingNode() override;
    static SharedPtr Create(bool isRenderServiceNode = false, bool isTextureExportNode = false);
    bool GetBitmap(Drawing::Bitmap& bitmap,
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr, const Drawing::Rect* rect = nullptr);
    bool GetPixelmap(std::shared_ptr<Media::PixelMap> pixelmap,
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr, const Drawing::Rect* rect = nullptr);
    bool ResetSurface();

protected:
    RSCanvasDrawingNode(bool isRenderServiceNode, bool isTextureExportNode = false);
    RSCanvasDrawingNode(const RSCanvasDrawingNode&) = delete;
    RSCanvasDrawingNode(const RSCanvasDrawingNode&&) = delete;
    RSCanvasDrawingNode& operator=(const RSCanvasDrawingNode&) = delete;
    RSCanvasDrawingNode& operator=(const RSCanvasDrawingNode&&) = delete;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_DRAWING_NODE_H