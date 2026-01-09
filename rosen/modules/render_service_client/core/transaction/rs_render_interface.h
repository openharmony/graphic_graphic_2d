/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_RENDRE_INTERFACE_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_RENDRE_INTERFACE_H

#include <mutex>
#include <map>

#include "common/rs_common_def.h"
#include "transaction/rs_render_pipeline_client.h"
#include "platform/drawing/rs_surface.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSRenderInterface {
public:
    RSRenderInterface();
    ~RSRenderInterface() noexcept;
    /**
     * @brief Get snapshot of surfaceNode.
     * @param node Indicates which node, usually point to a window.
     * @param callback When the snapshot is complete, the callback will be triggered.
     * @param captureConfig Indicates the configuration items required for snapshot.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});

    /**
     * @brief Get snapshot of surfaceNode, and security layer area is a drawn as a blur instead of white.
     * @param node Indicates which node, usually point o a window.
     * @param callback When the snapshot is complete, the callback will be triggered.
     * @param blurRadius Indicates blur radius of blur area.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSurfaceCaptureWithBlur(std::shared_ptr<RSSurfaceNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig = {},
        float blurRadius = 1E-6);

    /**
     * @brief Take display node capture.
     * @param node Display node.
     * @param callback Callback to be called when capture is done.
     * @param captureConfig Capture config, see RSSurfaceCaptureConfig.
     * @return True if capture task is successfully created, false if failed.
     */
    bool TakeSurfaceCapture(std::shared_ptr<RSDisplayNode> node, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});

    /**
     * @brief Set appWindow number.
     * @param num winodw number.
     */
    void SetAppWindowNum(uint32_t num);

    /**
     * @brief Call RequestNextVsync to force refresh one frame with the interface.
     */
    void ForceRefreshOneFrameWithNextVSync();

    /**
     * @brief Get snapshot of surfaceNode or displayNode by NodeId.
     * @param id The id of surfaceNode or displayNode.
     * @param callback When the snapshot is complete, the callback will be triggered.
     * @param captureConfig Indicates the configuration items required for snapshot.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});

    /**
     * @brief Get component snapshot.
     * @param node can be rootNode、surfaceNode、canvasNode、CanvasDrawingNode.
     * @param callback When the snapshot is complete, the callback will be triggered.
     * @param scaleX Indicates the scale of X-axis.
     * @param scaleY Indicates the scale of Y-axis.
     * @param isSync Indicates Whether wait until render is finished.
     * @param specifiedAreaRect Indicates the range that user wants to clip the snapshot.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSurfaceCaptureForUI(std::shared_ptr<RSNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX = 1.f, float scaleY = 1.f,
        bool isSync = false, const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f));

    /**
     * @brief Get a list of pixelmap information, each node of the component node tree will have a pixelmap.
     * @param node can be rootNode、surfaceNode、canvasNode、CanvasDrawingNode.
     * @return return a vector of pair, the first element is the NodeId, the second element is the pixelmap.
     */
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>>
        TakeSurfaceCaptureSoloNodeList(std::shared_ptr<RSNode> node);
    
    /**
     * @brief Get snapshot of surfaceNode, and security layer area is a drawn normally, not white.
     * @param node Indicates which node, usually point to a window.
     * @param callback When the snapshot is complete, the callback will be triggered.
     * @param captureConfig Indicates the configuration items required for snapshot.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSelfSurfaceCapture(std::shared_ptr<RSSurfaceNode> node, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});

    /**
     * @brief Component snapshot for divided render.
     * @param id The id of the component.
     * @param callback Callback function.
     * @param scaleX Indicates the scale of X-axis.
     * @param scaleY Indicates the scale of Y-axis.
     */
    bool TakeSurfaceCaptureForUIWithoutUni(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        float scaleX, float scaleY);

    /**
     * @brief Get component snapshot Within the given node range.
     * @param beginNode Indicates first child of snapshot.
     * @param endNode Indicates end child of snapshot.
     * @param useBeginNodeSize Indicates Whether use the size of begin node.
     * @param callback When the snapshot is complete, the callback will be triggered.
     * @param scaleX Indicates the scale of X-axis.
     * @param scaleY Indicates the scale of Y-axis.
     * @param isSync Indicates Whether wait until render is finished.
     * @return return true if snaphot success, else return false.
     */
    bool TakeUICaptureInRange(std::shared_ptr<RSNode> beginNode, std::shared_ptr<RSNode> endNode, bool useBeginNodeSize,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY, bool isSync);

    /**
     * @brief Take snapshot of displayNode.
     * @param node Indicates a display node to be captured.
     * @param callback Indicates callback to be triggered when snapshot is compeleted.
     * @param captureConfig Indicates the configrutation items required for snapshot.
     * @param checkDrmAndSurfaceLock Indicates the flag to check existing drm or surface lock window.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSurfaceCaptureWithAllWindows(std::shared_ptr<RSDisplayNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig,
        bool checkDrmAndSurfaceLock);

    /**
     * @brief Freeze or unfreeze screen.
     * @param node Indicates a display node to freeze or unfreeze.
     * @param isFreeze Indicates freeze or unfreeze the specified display node.
     * @return return true if freeze or unfreeze success, else return false.
     */
    bool FreezeScreen(std::shared_ptr<RSDisplayNode> node, bool isFreeze);

    /**
     * @brief Get snapshot of surfaceNode, and show the snapshot instead of surfaceNode.
     * @param node Indicates which node, usually point to a window.
     * @param isFreeze Indicates freeze or unfreeze this surfaceNode.
     * @param callback When the snapshot is complete, the callback will be triggered.
     * @param captureConfig Indicates the configuration items required for snapshot.
     * @param blurRadius Indicates blur radius of blur area.
     * @return return true if snaphot success, else return false.
     */
    bool SetWindowFreezeImmediately(std::shared_ptr<RSSurfaceNode> node, bool isFreeze,
        std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig = {},
        float blurRadius = 1E-6);

    /**
     * @brief Register a canvasNode as a container for nodes on the desktop.
     * It is a child node of displayNode to achieve unified scalling and other effects.
     * @param nodeId The id of the canvasNode, which will be moved below the displayNode of its subtree
     * and marked as windowcontainer.
     * @param value Mark a node as windowContainer or not.
     */
    void SetWindowContainer(NodeId nodeId, bool value);

    /**
     * @brief Register a callback to listen for the state of the buffer held by the DrawSurfaceBuffer OpItem,
     * such as when acquire is complete ir drawing is finished.
     * @param pid Application process ID, which is usually the sequence id of the buffer.
     * @param uid Unique ID, corresponding one-to-one with the buffer.
     * @param callback It is triggered when buffer state changes.
     * @return bool Returns true if successfully send to RS, otherwise false.
     */
    bool RegisterSurfaceBufferCallback(pid_t pid, uint64_t uid,
        std::shared_ptr<SurfaceBufferCallback> callback);

    /**
     * @brief UnRegister the DrawSurfaceBuffer op callback that holds the buffer state.
     * @param pid Application process ID, which is usually the sequence id of the buffer.
     * @param uid Unique ID, corresponding one-to-one with the buffer.
     * @return bool Returns true if successfully send to RS, otherwise false.
     */
    bool UnregisterSurfaceBufferCallback(pid_t pid, uint64_t uid);

    bool RegisterTransactionDataCallback(uint64_t token, uint64_t timeStamp, std::function<void()> callback);

    bool SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY,
        float positionZ, float positionW);

    void SetLayerTopForHWC(NodeId nodeId, bool isTop, uint32_t zOrder);

    /**
     * @brief Set the focus window information to renderService.
     * @param info Focus window information, Please refer to the definition for the specific content included.
     * @return 0 means success, others failed.
     */
    int32_t SetFocusAppInfo(const FocusAppInfo& info);

    /**
     * @brief Set all anco node ForceDoDirect.
     * @param direct is need to set ForceDoDirect.
     * @return the execution result successful.
     */
    bool SetAncoForceDoDirect(bool direct);

    /**
     * @brief clear uifirst node cache
     * @param id surface node id
     */
    void ClearUifirstCache(NodeId id);

    /**
     * @brief Set the process ID list requiring frame dropping. Next time RS triggers rending,
     * it will purge queued frames of corresponding self-rendering nodes in bufferQueue, and use the latest frame
     * buffer for screen display.
     * @param pidList Process ID list requiring frame dropping.
     */
    void DropFrameByPid(const std::vector<int32_t> pidList);

    /**
     * @brief Get brightness info by screenId.
     * @param screenId is screen id.
     * @return Returns 0 success, otherwise, failed.
     */
    int32_t GetBrightnessInfo(ScreenId screenId, BrightnessInfo& brightnessInfo);

    /**
     * @brief Get the HDR status of the current screen.
     * @param id Id of the screen.
     * @param hdrStatus The HDR status of the current screen.
     * @return 0 success, others failed.
     */
    int32_t GetScreenHDRStatus(ScreenId id, HdrStatus& hdrStatus);

    /**
     * @brief Set frame gravity of screen node
     * @param id Screen id.
     * @param gravity The gravity value of the screen node.
     */
    void SetScreenFrameGravity(ScreenId id, int32_t gravity);
    
    // WMS set dark color display mode to RS
    /**
     * @brief Notify if system themes switch to dark mode.
     * @param isDark whether is dark mode.
     * @return True if success, false if failed.
     */
    bool SetGlobalDarkColorMode(bool isDark);

    /*
     * @brief Set the system overload Animated Scenes to RS for special load shedding.
     * @param systemAnimatedScenes indicates the system animation scene.
     * @param isRegularAnimation indicates irregular windows in the animation scene.
     * @return true if succeed, otherwise false.
     */
    bool SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation = false);
    
    /**
     * @brief Get high contrast text state.
     * @return Return true if high contrast text enabled, otherwise false.
     */
    bool GetHighContrastTextState();
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    /**
     * @brief Register Canvas SurfaceBuffer callback for memory attribution.
     * @param callback is the Canvas SurfaceBuffer callback.
     */
    void RegisterCanvasCallback(sptr<RSICanvasSurfaceBufferCallback> callback);

    /**
     * @brief Submit Canvas pre-allocated buffer to RS.
     * @param nodeId is the canvas node id.
     * @param buffer is the pre-allocated DMA buffer.
     * @param resetSurfaceIndex is the index of ResetSurface.
     * @return Returns 0 success, otherwise, failed.
     */
    int32_t SubmitCanvasPreAllocatedBuffer(NodeId nodeId, sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex);
#endif
    /**
     * @brief Set watermark for surfaceNode.
     * @param pid pid of process.
     * @param name Watermark name. Note: ensure watermark name is unique.
     * @param watermark Watermark pixelmap.
     * @param nodeIdList Node id list
     * @param watermarkType custom or system watermark.
     * @return set watermark success return 0, else return errorCode.
     */
    uint32_t SetSurfaceWatermark(pid_t pid, const std::string &name,
        const std::shared_ptr<Media::PixelMap> &watermark,
        const std::vector<NodeId> &nodeIdList, SurfaceWatermarkType watermarkType);

    /**
     * @brief Set watermark for surfaceNode.
     * @param pid pid of process.
     * @param name Watermark name.
     * @param watermark Watermark pixelmap.
     */
    void ClearSurfaceWatermarkForNodes(pid_t pid, const std::string &name,
        const std::vector<NodeId> &nodeIdList);

    /**
     * @brief Set watermark for surfaceNode.
     * @param pid pid of process.
     * @param name Watermark name.
     */
    void ClearSurfaceWatermark(pid_t pid, const std::string &name);

private:
    std::unique_ptr<RSRenderPipelineClient> renderPipelineClient_;
    friend class RSUIContext;
    friend class RSApplicationAgentImpl;
    friend class RSDisplayNode;
    friend class RSSurfaceNode;
};
}
}
#endif // RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_RENDRE_INTERFACE_H