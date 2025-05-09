/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H

#include "ui/rs_node.h"


namespace OHOS {
namespace Rosen {
class RSNodeMap;

class RSC_EXPORT RSCanvasNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasNode>;
    using SharedPtr = std::shared_ptr<RSCanvasNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::CANVAS_NODE;
    RSUINodeType GetType() const override
    {
        return Type;
    }

    ~RSCanvasNode() override;

    static SharedPtr Create(bool isRenderServiceNode = false, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    ExtendRecordingCanvas* BeginRecording(int width, int height);
    bool IsRecording() const;
    void FinishRecording();
    float GetPaintWidth() const;
    float GetPaintHeight() const;
    void DrawOnNode(RSModifierType type, DrawFunc func) override;

    void SetFreeze(bool isFreeze) override;

    void SetHDRPresent(bool hdrPresent);

    void SetIsWideColorGamut(bool isWideColorGamut);

    void SetBoundsChangedCallback(BoundsChangedCallback callback) override;

    void CheckThread();

    /**
     * @brief Set linked node id in PC window resize scenario.
     * @param rootNodeId source RSRootNode id.
     */
    void SetLinkedRootNodeId(NodeId rootNodeId);
    NodeId GetLinkedRootNodeId();

    bool Marshalling(Parcel& parcel) const;
    static SharedPtr Unmarshalling(Parcel& parcel);

#ifdef RS_ENABLE_VK
    void SetHybridRenderCanvas(bool hybridRenderCanvas) override
    {
        hybridRenderCanvas_ = hybridRenderCanvas;
    }
    bool GetBitmap(Drawing::Bitmap& bitmap, std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr);
    bool GetPixelmap(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr, const Drawing::Rect* rect = nullptr);
    bool ResetSurface(int width, int height);
#endif

protected:
    RSCanvasNode(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSCanvasNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSCanvasNode(const RSCanvasNode&) = delete;
    RSCanvasNode(const RSCanvasNode&&) = delete;
    RSCanvasNode& operator=(const RSCanvasNode&) = delete;
    RSCanvasNode& operator=(const RSCanvasNode&&) = delete;
    BoundsChangedCallback boundsChangedCallback_;

private:
    ExtendRecordingCanvas* recordingCanvas_ = nullptr;
    bool recordingUpdated_ = false;
    mutable std::mutex mutex_;

    friend class RSUIDirector;
    friend class RSAnimation;
    friend class RSPathAnimation;
    friend class RSPropertyAnimation;
    friend class RSNodeMap;
    friend class RSNodeMapV2;
    void OnBoundsSizeChanged() const override;
    void CreateRenderNodeForTextureExportSwitch() override;
    void RegisterNodeMap() override;
    pid_t tid_;

    // [Attention] Only used in PC window resize scene now
    NodeId linkedRootNodeId_ = INVALID_NODEID;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H
