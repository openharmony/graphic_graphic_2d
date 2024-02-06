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

#include "ui/rs_canvas_drawing_node.h"

#include "command/rs_canvas_drawing_node_command.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
RSCanvasDrawingNode::RSCanvasDrawingNode(bool isRenderServiceNode, bool isTextureExportNode)
    : RSCanvasNode(isRenderServiceNode, isTextureExportNode) {}

RSCanvasDrawingNode::~RSCanvasDrawingNode() {}

RSCanvasDrawingNode::SharedPtr RSCanvasDrawingNode::Create(bool isRenderServiceNode, bool isTextureExportNode)
{
    SharedPtr node(new RSCanvasDrawingNode(isRenderServiceNode, isTextureExportNode));
    RSNodeMap::MutableInstance().RegisterNode(node);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (!transactionProxy) {
        return node;
    }

    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasDrawingNodeCreate>(node->GetId(), isTextureExportNode);
    transactionProxy->AddCommand(command, node->IsRenderServiceNode());
    return node;
}

bool RSCanvasDrawingNode::ResetSurface()
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (!transactionProxy) {
        return false;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasDrawingNodeResetSurface>(GetId());
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    return true;
}

#ifndef USE_ROSEN_DRAWING
bool RSCanvasDrawingNode::GetBitmap(SkBitmap& bitmap, std::shared_ptr<DrawCmdList> drawCmdList, const SkRect* rect)
{
    if (IsUniRenderEnabled()) {
        auto renderServiceClient =
            std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
        if (renderServiceClient == nullptr) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetBitmap renderServiceClient is nullptr!");
            return false;
        }
        bool ret = renderServiceClient->GetBitmap(GetId(), bitmap);
        if (!ret) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetBitmap GetBitmap failed");
            return ret;
        }
    } else {
        auto node =
            RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(GetId());
        if (node == nullptr) {
            RS_LOGE("RSCanvasDrawingNode::GetBitmap cannot find NodeId: [%{public}" PRIu64 "]", GetId());
            return false;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("RSCanvasDrawingNode::GetBitmap RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
            return false;
        }
        auto getBitmapTask = [&node, &bitmap]() { bitmap = node->GetBitmap(); };
        RSRenderThread::Instance().PostSyncTask(getBitmapTask);
        if (bitmap.empty()) {
            return false;
        }
    }
    if (drawCmdList == nullptr) {
        RS_LOGD("RSCanvasDrawingNode::GetBitmap drawCmdList == nullptr");
    } else {
        SkCanvas canvas(bitmap);
        drawCmdList->Playback(canvas, rect);
    }
    return true;
}

bool RSCanvasDrawingNode::GetPixelmap(
    std::shared_ptr<Media::PixelMap> pixelmap, std::shared_ptr<DrawCmdList> drawCmdList, const SkRect* rect)
{
    if (!pixelmap) {
        RS_LOGE("RSCanvasDrawingNode::GetPixelmap: pixelmap is nullptr");
        return false;
    }
    if (IsUniRenderEnabled()) {
        auto renderServiceClient =
            std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
        if (renderServiceClient == nullptr) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetPixelmap: renderServiceClient is nullptr!");
            return false;
        }
        bool ret = renderServiceClient->GetPixelmap(GetId(), pixelmap, rect, drawCmdList);
        if (!ret || !pixelmap) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetPixelmap: GetPixelmap failed");
            return false;
        }
    } else {
        auto node =
            RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(GetId());
        if (node == nullptr) {
            RS_LOGE("RSCanvasDrawingNode::GetPixelmap: cannot find NodeId: [%{public}" PRIu64 "]", GetId());
            return false;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("RSCanvasDrawingNode::GetPixelmap: RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
            return false;
        }
        bool ret = false;
        auto getPixelmapTask = [&node, &pixelmap, rect, &ret, &drawCmdList]() {
            ret = node->GetPixelmap(pixelmap, rect, UINT32_MAX, drawCmdList);
        };
        RSRenderThread::Instance().PostSyncTask(getPixelmapTask);
        if (!ret || !pixelmap) {
            return false;
        }
    }
    return true;
}

#else
bool RSCanvasDrawingNode::GetBitmap(Drawing::Bitmap& bitmap,
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList, const Drawing::Rect* rect)
{
    if (IsUniRenderEnabled()) {
        auto renderServiceClient =
            std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
        if (renderServiceClient == nullptr) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetBitmap renderServiceClient is nullptr!");
            return false;
        }
        bool ret = renderServiceClient->GetBitmap(GetId(), bitmap);
        if (!ret) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetBitmap GetBitmap failed");
            return ret;
        }
    } else {
        auto node =
            RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(GetId());
        if (node == nullptr) {
            RS_LOGE("RSCanvasDrawingNode::GetBitmap cannot find NodeId: [%{public}" PRIu64 "]", GetId());
            return false;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("RSCanvasDrawingNode::GetBitmap RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
            return false;
        }
        auto getBitmapTask = [&node, &bitmap]() { bitmap = node->GetBitmap(); };
        RSRenderThread::Instance().PostSyncTask(getBitmapTask);
        if (bitmap.IsValid()) {
            return false;
        }
    }
    if (drawCmdList == nullptr) {
        RS_LOGD("RSCanvasDrawingNode::GetBitmap drawCmdList == nullptr");
    } else {
        Drawing::Canvas canvas;
        canvas.Bind(bitmap);
        drawCmdList->Playback(canvas, rect);
    }
    return true;
}

bool RSCanvasDrawingNode::GetPixelmap(std::shared_ptr<Media::PixelMap> pixelmap,
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList, const Drawing::Rect* rect)
{
    if (!pixelmap) {
        RS_LOGE("RSCanvasDrawingNode::GetPixelmap: pixelmap is nullptr");
        return false;
    }
    if (IsUniRenderEnabled()) {
        auto renderServiceClient =
            std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
        if (renderServiceClient == nullptr) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetPixelmap: renderServiceClient is nullptr!");
            return false;
        }
        bool ret = renderServiceClient->GetPixelmap(GetId(), pixelmap, rect, drawCmdList);
        if (!ret || !pixelmap) {
            ROSEN_LOGE("RSCanvasDrawingNode::GetPixelmap: GetPixelmap failed");
            return false;
        }
    } else {
        auto node =
            RSRenderThread::Instance().GetContext().GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(GetId());
        if (node == nullptr) {
            RS_LOGE("RSCanvasDrawingNode::GetPixelmap: cannot find NodeId: [%{public}" PRIu64 "]", GetId());
            return false;
        }
        if (node->GetType() != RSRenderNodeType::CANVAS_DRAWING_NODE) {
            RS_LOGE("RSCanvasDrawingNode::GetPixelmap: RenderNodeType != RSRenderNodeType::CANVAS_DRAWING_NODE");
            return false;
        }
        bool ret = false;
        auto getPixelmapTask = [&node, &pixelmap, rect, &ret, &drawCmdList]() {
            ret = node->GetPixelmap(pixelmap, rect);
        };
        RSRenderThread::Instance().PostSyncTask(getPixelmapTask);
        if (!ret || !pixelmap) {
            return false;
        }
    }
    return true;
}
#endif
} // namespace Rosen
} // namespace OHOS