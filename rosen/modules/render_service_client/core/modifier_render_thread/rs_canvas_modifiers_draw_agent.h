/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_AGENT_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_AGENT_H

#include "modifier_render_thread/rs_canvas_modifiers_draw.h"

namespace OHOS::Rosen {
class RSModifiersDrawThread;
class RSUIContext;
class RSUIDirector;
class RSCanvasDrawingNode;
class RSCanvasModifiersDrawAgent final {
public:
    RSCanvasModifiersDrawAgent();
    ~RSCanvasModifiersDrawAgent();

    RSCanvasModifiersDrawAgent(const RSCanvasModifiersDrawAgent&) = delete;
    RSCanvasModifiersDrawAgent(const RSCanvasModifiersDrawAgent&&) = delete;
    RSCanvasModifiersDrawAgent& operator=(const RSCanvasModifiersDrawAgent&) = delete;
    RSCanvasModifiersDrawAgent& operator=(const RSCanvasModifiersDrawAgent&&) = delete;

private:
    void SetCacheDir(const std::string& cacheDir);

    void OnNodeCreate(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface);

    void OnNodeRelease(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface);

    void OnNodeStateChanged(NodeId nodeId, RSNodeState state);

    void ResetSurface(NodeId nodeId, int width, int height, bool sizeOutOfGpuLimit, GraphicColorGamut colorSpace);

    bool GetBitmap(NodeId nodeId, Drawing::Bitmap& bitmap);

    bool GetPixelMap(NodeId nodeId, std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
        Drawing::DrawCmdListPtr drawCmdList);

    void UpdateCanvasContent(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList);

    void SubmitAndCollectCanvasBuffers();

    void SwapTransactionConfigList(std::vector<RSTransactionConfig>& transactionConfigList);

    std::shared_ptr<RSCanvasModifiersDraw> canvasModifiersDraw_ = nullptr;

    friend class RSModifiersDrawThread;
    friend class RSUIContext;
    friend class RSUIDirector;
    friend class RSCanvasDrawingNode;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_AGENT_H
