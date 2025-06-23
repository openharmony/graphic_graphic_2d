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

/**
* @file rs_interfaces.h
* @brief API provided by the RS for internal components.
*/

#ifndef RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H
#define RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H

#include <memory>
#include <mutex>

#include "memory/rs_memory_graphic.h"
#include "transaction/rs_render_service_client.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ipc_callbacks/rs_iocclusion_change_callback.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSInterfaces {
public:
    static RSInterfaces &GetInstance();
    RSInterfaces(const RSInterfaces &) = delete;
    void operator=(const RSInterfaces &) = delete;

    /**
     * @brief Set the focus window information to renderService.
     * @param info Focus window information, Please refer to the definition for the specific content included.
     * @return 0 means success, others failed.
     */
    int32_t SetFocusAppInfo(const FocusAppInfo& info);

    /**
     * @brief Get the id of default screen.
     * @return Default screen id.
     */
    ScreenId GetDefaultScreenId();

    /**
     * @brief Get the id of cureent screen, for bootAnimation only.
     * @return Cureent screen id.
     */
    ScreenId GetActiveScreenId();

    /**
     * @brief Get the id of all screen.
     * @return A vector of screen id.
     */
    std::vector<ScreenId> GetAllScreenIds();

#ifndef ROSEN_CROSS_PLATFORM
    /**
     * @brief Create virtual screen with params.
     * @param name Virtual screen name.
     * @param width Virtual screen width, max: MAX_VIRTUAL_SCREEN_WIDTH.
     * @param height Virtual screen height, max: MAX_VIRTUAL_SCREEN_HEIGHT.
     * @param surface Virtual screen surface, if not nullptr, vote for 60Hz.
     * @param mirrorId Decide which screen id to mirror, INVALID_SCREEN_ID means do not mirror any screen.
     * @param flags Virtual screen security layer option, 0: screen level, 1: window level.
     * @param whiteList List of surface node id, only these nodes can be drawn on this screen.
     * @return Virtual screen id, INVALID_SCREEN_ID means failed.
     */
    ScreenId CreateVirtualScreen(
        const std::string &name,
        uint32_t width,
        uint32_t height,
        sptr<Surface> surface,
        ScreenId mirrorId = 0,
        int flags = 0,
        std::vector<NodeId> whiteList = {});

    /**
     * @brief Set list of surface node id, these nodes will be excluded from this screen.
     * @param id Valid screen id: set screen record black list; INVALID_SCREEN_ID: set screen cast black list.
     * @param blackListVector List of surface node id. If the screen id is INVALID_SCREEN_ID, the blackListVector will
     * apply to all virtual screens.
     * @return 0 means success.
     */
    int32_t SetVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);

    /**
     * @brief Use nodeType to Set blackList For mirror screen.
     * @param id Screen id.
     * @param typeBlackListVector Vector of NodeType.
     * @return 0 means success, others failed.
     */
    int32_t SetVirtualScreenTypeBlackList(ScreenId id, std::vector<NodeType>& typeBlackListVector);

    /**
     * @brief Add list of surfaceNodeId excluded on virtual screen.
     * @param id Screen id.
     * @param blackListVector Vector of surfaceNodeId excluded on virtual screen.
     * @return 0 means success, others failed.
     */
    int32_t AddVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);
    
    /**
     * @brief Remove list of surfaceNodeId excluded on virtual screen.
     * @param id screen id.
     * @param blackListVector Vector of surfaceNodeId excluded on virtual screen.
     * @return 0 means success, others failed.
     */
    int32_t RemoveVirtualScreenBlackList(ScreenId id, std::vector<NodeId>& blackListVector);

    /**
     * @brief Set securtity layer exemption list for mirror screen.
     * @param id screen id.
     * @param securityExemptionList exemption list for securtity layer.
     * @return 0 means success, others failed.
     */
    int32_t SetVirtualScreenSecurityExemptionList(ScreenId id, const std::vector<NodeId>& securityExemptionList);

    /**
     * @brief Set mask image displayed on virtual mirror screen when security layer is present.
     * @param id Virtual screen id.
     * @param securityMask Mask image to be set, nullptr means no mask.
     * @return 0 means success.
     */
    int32_t SetScreenSecurityMask(ScreenId id, std::shared_ptr<Media::PixelMap> securityMask);
    
    /**
     * @brief Set Visible Rect for mirror screen.
     * @param id Virtual screen id.
     * @param mainScreenRect visible rect.
     * @param supportRotation if the value is true, rotation is supported. Otherwise, rotation is not supported.
     * @return 0 means success, others failed.
     */
    int32_t SetMirrorScreenVisibleRect(ScreenId id, const Rect& mainScreenRect, bool supportRotation = false);

    /**
     * @brief Set if cast screen black list is enabled, surface node in black list will not be drawn.
     * @param id Virtual screen id.
     * @param enable True means enable, false means disable.
     * @return 0 means success, others failed.
     */
    int32_t SetCastScreenEnableSkipWindow(ScreenId id, bool enable);

    /**
     * @brief Set producer surface for virtual screen.
     * @param id Virtual screen id.
     * @param surface Producer surface.
     * @return 0 means success, others failed.
     */
    int32_t SetVirtualScreenSurface(ScreenId id, sptr<Surface> surface);
#endif

    /**
     * @brief Remove virtual screen.
     * @param id Virtual screen id.
     */
    void RemoveVirtualScreen(ScreenId id);

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    /**
     * @brief Sets the parameters for inverting the color of the variable cursor.
     * @param darkBuffer Dark buffer area.
     * @param brightBuffer Bright color buffer area Enable cast black list for virtual screen or not.
     * @param interval Cursor color obtaining interval.
     * @return 0 means success, others failed.
     */
    int32_t SetPointerColorInversionConfig(float darkBuffer, float brightBuffer, int64_t interval, int32_t rangeSize);
    
    /**
     * @brief Indicates whether to enable color inversion of the variable cursor or not.
     * @param enable enable color inversion of the variable cursor or not.
     * @return 0 means success, others failed.
     */
    int32_t SetPointerColorInversionEnabled(bool enable);
    
    /**
     * @brief Register the callback for changing the color of the cursor.
     * @param callback callback for changing the color of the cursor.
     * @return 0 means success, others failed.
     */
    int32_t RegisterPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback);
    
    /**
     * @brief UnRegister the callback for changing the color of the cursor.
     * @return 0 means success, others failed.
     */
    int32_t UnRegisterPointerLuminanceChangeCallback();
#endif

    /**
     * @brief Set screen connection status change callback.
     * on the screen connection status is changed.
     * @param callback Callback of the screen connection status changed.
     * @return Returns int32_t, return value == 0 success, otherwise, failed.
     */
    int32_t SetScreenChangeCallback(const ScreenChangeCallback &callback);

    /**
     * @brief Set watermark for surfaceNode.
     * @param name Watermark name.
     * @param watermark Watermark pixelmap.
     * @return set watermark success return true, else return false.
     */
    bool SetWatermark(const std::string& name, std::shared_ptr<Media::PixelMap> watermark);
    
    /**
     * @brief Get pixelmaps generated by this selfDrawingNodes in the process.
     * @param pixelMapInfoVector is a vector of PixelMapInfo.
     * @param pid Indiactes id of process.
     * @return return value == 0 success, otherwise, failed.
     */
    int32_t GetPixelMapByProcessId(std::vector<PixelMapInfo>& pixelMapInfoVector, pid_t pid);
    
    /**
     * @brief Get snapshot of surfaceNode.
     * @param node Indicates which node, usually poiter to a window.
     * @param callback When the snapshot is compelete, the callback will be triggered.
     * @param captureConfig Indicates the configrutation items required for snapshot.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSurfaceCapture(std::shared_ptr<RSSurfaceNode> node, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});
    
    /**
     * @brief Get snapshot of surfaceNode, and security layer area is a drawn as a blur instead of white.
     * @param node Indicates which node, usually poiter to a window.
     * @param callback When the snapshot is compelete, the callback will be triggered.
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
     * @brief Get snapshot of surfaceNode or displayNode by NodeId.
     * @param id The id of surfaceNode or displayNode.
     * @param callback When the snapshot is compelete, the callback will be triggered.
     * @param captureConfig Indicates the configrutation items required for snapshot.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSurfaceCapture(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});
    
    /**
     * @brief Get component snapshot.
     * @param node can be rootNode、surfaceNode、canvasNode、CanvasDrawingNode.
     * @param callback When the snapshot is compelete, the callback will be triggered.
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
     * @brief Get component snapshot.
     * @param node can be rootNode、surfaceNode、canvasNode、CanvasDrawingNode.
     * @param callback When the snapshot is compelete, the callback will be triggered.
     * @param captureConfig Indicates the configrutation items required for snapshot.
     * @param specifiedAreaRect Indicates the range that user wants to clip the snapshot.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSurfaceCaptureForUIWithConfig(std::shared_ptr<RSNode> node,
        std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig = {},
        const Drawing::Rect& specifiedAreaRect = Drawing::Rect(0.f, 0.f, 0.f, 0.f));
    
    /**
     * @brief Get a list of pixelmap information, each node of the component node tree will have a pixelmap.
     * @param node can be rootNode、surfaceNode、canvasNode、CanvasDrawingNode.
     * @return return a vector of pair, the first element is the NodeId, the second element is the pixelmap.
     */
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>>
        TakeSurfaceCaptureSoloNodeList(std::shared_ptr<RSNode> node);
    
    /**
     * @brief Get snapshot of surfaceNode, and security layer area is a drawn normally, not white.
     * @param node Indicates which node, usually poiter to a window.
     * @param callback When the snapshot is compelete, the callback will be triggered.
     * @param captureConfig Indicates the configrutation items required for snapshot.
     * @return return true if snaphot success, else return false.
     */
    bool TakeSelfSurfaceCapture(std::shared_ptr<RSSurfaceNode> node, std::shared_ptr<SurfaceCaptureCallback> callback,
        RSSurfaceCaptureConfig captureConfig = {});
    
    /**
     * @brief Get snapshot of surfaceNode, and show the snapshot instead of surfaceNode.
     * @param node Indicates which node, usually poiter to a window.
     * @param isFreeze Indicates freeze or unfreeze this surfaceNode.
     * @param callback When the snapshot is compelete, the callback will be triggered.
     * @param captureConfig Indicates the configrutation items required for snapshot.
     * @param blurRadius Indicates blur radius of blur area.
     * @return return true if snaphot success, else return false.
     */
    bool SetWindowFreezeImmediately(std::shared_ptr<RSSurfaceNode> node, bool isFreeze,
        std::shared_ptr<SurfaceCaptureCallback> callback, RSSurfaceCaptureConfig captureConfig = {},
        float blurRadius = 1E-6);
    
    /**
     * @brief Get component snapshot Within the given node range.
     * @param beginNode Indicates first child of snapshot.
     * @param endNode Indicates end child of snapshot.
     * @param useBeginNodeSize Indicates Whether use the size of begin node.
     * @param callback When the snapshot is compelete, the callback will be triggered.
     * @param scaleX Indicates the scale of X-axis.
     * @param scaleY Indicates the scale of Y-axis.
     * @param isSync Indicates Whether wait until render is finished.
     * @return return true if snaphot success, else return false.
     */
    bool TakeUICaptureInRange(std::shared_ptr<RSNode> beginNode, std::shared_ptr<RSNode> endNode, bool useBeginNodeSize,
        std::shared_ptr<SurfaceCaptureCallback> callback, float scaleX, float scaleY, bool isSync);
    
    /**
     * @brief Simplify the original interfaces setboundaries for cursor movemonet and reduce the workload.
     * @param rsNodeId Indicates id of node.
     * @param positionX Indicates x coordinate position.
     * @param positionY Indicates Y coordinate position.
     * @param positionZ Indicates z coordinate position.
     * @param positionW Indicates w coordinate position.
     * @return return true if set success, else return false.
     */
    bool SetHwcNodeBounds(int64_t rsNodeId, float positionX, float positionY, float positionZ, float positionW);
    
    /**
     * @brief Register typeface.
     * @param typeface font's typeface.
     * @return return true if register success, else return false.
     */
    bool RegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface);
    /**
     * @brief UnRegister typeface.
     * @param typeface font's typeface.
     * @return return true if unRegister success, else return false.
     */
    bool UnRegisterTypeface(std::shared_ptr<Drawing::Typeface>& typeface);
#ifndef ROSEN_ARKUI_X
    /**
     * @brief Set active mode of the screen(The combination of resolution of resolution and refresh rate is a mode.
     * the display calls back the mode set by the user and then set is to RS.)
     * @param id id of screen.
     * @param modeId the id of the active mode to set to the screen.
     */
    void SetScreenActiveMode(ScreenId id, uint32_t modeId);
    
    /**
     * @brief Statistics of application GPU memory.
     * @param pid Indiactes id of process.
     * @return MemoryGraphic, include the application memory info.
     */
    MemoryGraphic GetMemoryGraphic(int pid);
    
    /**
     * @brief Statistics of all application GPU memory.
     * @return a vector of MemoryGraphic, include all application GPU memory.
     */
    std::vector<MemoryGraphic> GetMemoryGraphics();
#endif // !ROSEN_ARKUI_X
    /**
     * @brief Get total App memory size.
     * @param cpuMemSize cpu size of Total App.
     * @param gpuMemSize gpu size of Total App.
     * @return return true if get success.
     */
    bool GetTotalAppMemSize(float& cpuMemSize, float& gpuMemSize);

#ifndef ROSEN_ARKUI_X
    /**
     * @brief Set render resolution of physical screen.
     * @param id Id of the physical screen.
     * @param width Width to set, and should be greater than physical width.
     * @param height Height to set, and should be greater than physical height.
     * @return StatusCode. Returns value, return value == 0 success, otherwise, failed.
     */
    int32_t SetPhysicalScreenResolution(ScreenId id, uint32_t width, uint32_t height);

    /**
     * @brief Set virtual screen resolution.
     * @param id Virtual screen id.
     * @param width Virtual screen width, max: MAX_VIRTUAL_SCREEN_WIDTH.
     * @param height Virtual screen height, max: MAX_VIRTUAL_SCREEN_HEIGHT.
     * @return 0 means success, others failed.
     */
    int32_t SetVirtualScreenResolution(ScreenId id, uint32_t width, uint32_t height);
#endif // !ROSEN_ARKUI_X

    /**
     * @brief Set if auto rotation is enabled for virtual mirror screen, keep content always horizontal.
     * @param id Virtual screen id.
     * @param canvasRotation True means enable, false means disable.
     * @return True if success, false if failed.
     */
    bool SetVirtualMirrorScreenCanvasRotation(ScreenId id, bool canvasRotation);

    /**
     * @brief Set if resize buffer and keep content horizontal while source is rotated.
     * @param id Virtual screen id.
     * @param isAutoRotation True means enable, false means disable.
     * @return 0 means success, others failed.
     */
    int32_t SetVirtualScreenAutoRotation(ScreenId id, bool isAutoRotation);

    /**
     * @brief Set scale mode for virtual mirror screen.
     * @param id Virtual screen id.
     * @param scaleMode Scale mode, see ScreenScaleMode.
     * @return True if success, false if failed.
     */
    bool SetVirtualMirrorScreenScaleMode(ScreenId id, ScreenScaleMode scaleMode);

    // WMS set dark color display mode to RS
    /**
     * @brief Notify if system themes switch to dark mode.
     * @param isDark whether is dark mode.
     * @return True if success, false if failed.
     */
    bool SetGlobalDarkColorMode(bool isDark);
#ifndef ROSEN_ARKUI_X
    /**
     * @brief Get virtual screen resolution.
     * @param id Virtual screen id.
     * @return Virtual screen resolution, see RSVirtualScreenResolution.
     */
    RSVirtualScreenResolution GetVirtualScreenResolution(ScreenId id);
    
    /**
     * @brief RenderService will cease rendering when screen power is off. If necessary
     * call this to refresh one moreframe.
     */
    void MarkPowerOffNeedProcessOneFrame();
    
    /**
     * @brief Call this to force refresh one more frame.
     */
    void RepaintEverything();
    
    /**
     * @brief RenderService will cease rendering when screen power is off. If necessary, call this disable such control
     * in virtual screen cases.
     */
    void DisablePowerOffRenderControl(ScreenId id);

    /**
     * @brief Set power of the screen.
     * @param id Id of the screen to set power status.
     * @param status The status to set to the screen.
     */
    void SetScreenPowerStatus(ScreenId id, ScreenPowerStatus status);
    
    /**
     * @brief Get active mode of the screen.
     * @param id Id of the screen to get active mode.
     * @return RSScreenModeInfo including the screen width, height, and refresh rates.
     */
    RSScreenModeInfo GetScreenActiveMode(ScreenId id);
#endif // !ROSEN_ARKUI_X

    /**
     * @brief Set screen's RefreshRate.
     * @param id Id of the screen.
     * @param sceneId Id of the scene.
     * @param rate refresh rate.
     */
    void SetScreenRefreshRate(ScreenId id, int32_t sceneId, int32_t rate);

    /**
     * @brief Set refresh mode.
     * @param refreshRateMode refresh rate mode.
     */
    void SetRefreshRateMode(int32_t refreshRateMode);

    /**
     * @brief Synchronize LTPO Expected RateRange.
     * @param id Frame Rate Linker Id.
     * @param range frame rate range.
     * @param animatorExpectedFrameRate animator expected frame rate.
     */
    void SyncFrameRateRange(FrameRateLinkerId id, const FrameRateRange& range, int32_t animatorExpectedFrameRate);
    
    /**
     * @brief Get screen current refresh rate.
     * @param id id of screen.
     * @return if success, return rate != 0, else return 0.
     */
    uint32_t GetScreenCurrentRefreshRate(ScreenId id);
    
    /**
     * @brief Get screen current refresh mode.
     * @return customFrameRate mode.
     */
    int32_t GetCurrentRefreshRateMode();

    /**
     * @brief Get screen supported refresh rate.
     * @param id id of screen.
     * @return Return supported refresh rate.
     */
    std::vector<int32_t> GetScreenSupportedRefreshRates(ScreenId id);

    /**
     * @brief Get shwo refresh rate enabled.
     * @return Return true, if get success.
     */
    bool GetShowRefreshRateEnabled();

    /**
     * @brief Set show refresh rate enabled.
     * @param enabled
     * @param type
     */
    void SetShowRefreshRateEnabled(bool enabled, int32_t type = 1);
    
    /**
     * @brief Get realtime refresh rate by screen id.
     * @param id id of screenled.
     * @return Return realtime refresh rate.
     */
    uint32_t GetRealtimeRefreshRate(ScreenId id);
    
    /**
     * @brief Get fps string by pid.
     * @param pid indicates id of process.
     * @return Return fps string if success.
     */
    std::string GetRefreshInfo(pid_t pid);
    std::string GetRefreshInfoToSP(NodeId id);

#ifndef ROSEN_ARKUI_X
    /**
     * @brief Get active mode of the screen.
     * @param id Id of the screen to get supported modes.
     * @return A list of RSScreenModeInfo.
     */
    std::vector<RSScreenModeInfo> GetScreenSupportedModes(ScreenId id);

    /**
     * @brief Get capability of screen.
     * @param id id of the screen.
     * @return RSScreenCapability.
     */
    RSScreenCapability GetScreenCapability(ScreenId id);

    /**
     * @brief Get power status of the screen.
     * @param id id of the screen.
     * @return ScreenPowerStatus.
     */
    ScreenPowerStatus GetScreenPowerStatus(ScreenId id);

    /**
     * @brief Get date of screen.
     * @param id id of the screen.
     * @return RSScreenData.
     */
    RSScreenData GetScreenData(ScreenId id);
#endif // !ROSEN_ARKUI_X
    /**
     * @brief Get blacklight values of the screen.
     * @param id id of the screen.
     * @return blacklight values.
     */
    int32_t GetScreenBacklight(ScreenId id);

    /**
     * @brief Set backlight value of the screen.
     * @param id Id of the screen to set its backlight value.
     * @param level The value of backlight.
     */
    void SetScreenBacklight(ScreenId id, uint32_t level);

    int32_t GetScreenSupportedColorGamuts(ScreenId id, std::vector<ScreenColorGamut>& mode);

    /**
     * @brief Get the screen supported mock metaDataKeys.
     * @param id Id of the screen.
     * @param keys Output parameters, The screen supported mock metaDataKeys will insert to keys.
     * @return 0 success, others failed.
     */
    int32_t GetScreenSupportedMetaDataKeys(ScreenId id, std::vector<ScreenHDRMetadataKey>& keys);

    /**
     * @brief Get the screen supported mock metaDataKeys.
     * @param id Id of the screen.
     * @param keys Output parameters, The screen supported mock metaDataKeys will insert to keys.
     * @return 0 success, others failed.
     */
    int32_t GetScreenColorGamut(ScreenId id, ScreenColorGamut& mode);

    int32_t SetScreenColorGamut(ScreenId id, int32_t modeIdx);

    /**
     * @brief Set screen's gamut map mode.
     * @param id Id of the screen.
     * @param mode Gamut map mode to set for the screen.
     * @return 0 success, others failed.
     */
    int32_t SetScreenGamutMap(ScreenId id, ScreenGamutMap mode);

    /**
     * @brief Set screen correction, used to correct screen rotation.
     * @param id Screen id.
     * @param screenRotation Screen correction, see ScreenRotation.
     * @return 0 means success, others failed.
     */
    int32_t SetScreenCorrection(ScreenId id, ScreenRotation screenRotation);

    /**
     * @brief Get screen's gamut map mode.
     * @param id Id of the screen.
     * @param mode Gamut map mode to set for the screen.
     * @return 0 success, others failed.
     */
    int32_t GetScreenGamutMap(ScreenId id, ScreenGamutMap& mode);

    /**
     * @brief Get the HDR Capability of the screen.
     * @param id Id of the screen.
     * @param screenHdrCapability The HDR Capability of the screen.
     * @return 0 success, others failed.
     */
    int32_t GetScreenHDRCapability(ScreenId id, RSScreenHDRCapability& screenHdrCapability);

    /**
     * @brief Get the pixel format of the screen.
     * @param id Id of the screen.
     * @param pixelFormat The pixel format of the screen.
     * @return 0 success, others failed.
     */
    int32_t GetPixelFormat(ScreenId id, GraphicPixelFormat& pixelFormat);

    /**
     * @brief Set the pixel format of the screen.
     * @param id Id of the screen.
     * @param pixelFormat This parameters will set to the pixel format of the screen.
     * @return 0 success, others failed.
     */
    int32_t SetPixelFormat(ScreenId id, GraphicPixelFormat pixelFormat);

    /**
     * @brief Get the hardware supported HDR format.
     * @param id Id of the screen.
     * @param hdrFormats The hardware supported HDR format will set to this parameter.
     * @return 0 success, others failed.
     */
    int32_t GetScreenSupportedHDRFormats(ScreenId id, std::vector<ScreenHDRFormat>& hdrFormats);

    /**
     * @brief Get the HDR format of the current screen.
     * @param id Id of the screen.
     * @param hdrFormats The HDR format of the current screen will set to this parameter.
     * @return 0 success, others failed.
     */
    int32_t GetScreenHDRFormat(ScreenId id, ScreenHDRFormat& hdrFormat);

    /**
     * @brief Set the HDR format of the current screen.
     * @param id Id of the screen.
     * @param modeIdx This parameter will set to the currentHDRFormatIdx.
     * @return 0 success, others failed.
     */
    int32_t SetScreenHDRFormat(ScreenId id, int32_t modeIdx);

    /**
     * @brief Get the color space supported by the screen.
     * @param id Id of the screen.
     * @param colorSpaces Output parameters, records the color space supported by the screen.
     * @return 0 success, others failed.
     */
    int32_t GetScreenSupportedColorSpaces(ScreenId id, std::vector<GraphicCM_ColorSpaceType>& colorSpaces);

    /**
     * @brief Get the color space of the screen.
     * @param id Id of the screen.
     * @param colorSpace Output parameters, records the color space of the screen.
     * @return 0 success, others failed.
     */
    int32_t GetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType& colorSpace);

    /**
     * @brief Set screen's color space.
     * @param id Id of the screen.
     * @param colorSpace color space to set for the screen.
     * @return 0 success, others failed.
     */
    int32_t SetScreenColorSpace(ScreenId id, GraphicCM_ColorSpaceType colorSpace);

    /**
     * @brief Get the type of the screen.
     * @param id Id of the screen.
     * @param screenType screen type of the screen.
     * @return 0 success, others failed.
     */
    int32_t GetScreenType(ScreenId id, RSScreenType& screenType);

    int32_t GetDisplayIdentificationData(ScreenId id, uint8_t& outPort, std::vector<uint8_t>& edidData);

    /* skipFrameInterval : decide how many frames apart to refresh a frame,
       DEFAULT_SKIP_FRAME_INTERVAL means refresh each frame,
       change screen refresh rate finally */
    int32_t SetScreenSkipFrameInterval(ScreenId id, uint32_t skipFrameInterval);
    
    /**
     * @brief Set virtual screen refresh rate.
     * @param id Id of the screen.
     * @param maxRefreshRate Indicates max refresh rate.
     * @param actualRefreshRate Indicates actual refresh rate.
     * @return 0 success, others failed.
     */
    int32_t SetVirtualScreenRefreshRate(ScreenId id, uint32_t maxRefreshRate, uint32_t& actualRefreshRate);

    /**
     * @brief Set screen active rect, part of screen that can be drawn.
     * @param id Screen id.
     * @param activeRect Screen active rect.
     * @return 0 means success, others failed.
     */
    uint32_t SetScreenActiveRect(ScreenId id, const Rect& activeRect);

    /**
     * @brief Create a VsyncReceiver instance.
     * @param name Indicates the name of the VsyncReceiver instance.
     * @param looper Indicates the pointer of a eventHandle instance.
     * @param actualRefreshRate Indicates actual refresh rate.
     * @return return VsyncReceiver instance pointer.
     */
    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr);

    /**
     * @brief Create a VsyncReceiver instance.
     * @param name Indicates the name of the VsyncReceiver instance.
     * @param id Indicates the FrameRateLinker id.
     * @param looper Indicates the pointer of a eventHandle instance.
     * @param windowNodeId Indicates the windowNode id.
     * @param fromXcomponent Indicates whether the source is from xcomponent.
     * @return return VsyncReceiver instance pointer.
     */
    std::shared_ptr<VSyncReceiver> CreateVSyncReceiver(
        const std::string& name,
        uint64_t id,
        const std::shared_ptr<OHOS::AppExecFwk::EventHandler> &looper = nullptr,
        NodeId windowNodeId = 0,
        bool fromXcomponent = false);

    /**
     * @brief Create a pixelmap obeject from surface id.
     * @param surfaceId Indicates the id of surface.
     * @param srcRect Indicates the area that requires a rectangle.
     * @return return a pixelmap obeject.
     */
    std::shared_ptr<Media::PixelMap> CreatePixelMapFromSurfaceId(uint64_t surfaceId, const Rect &srcRect);

    /**
     * @brief Register window occlusion change callback.
     * @param callback callback fuction.
     * @return 0 success, else failed.
     */
    int32_t RegisterOcclusionChangeCallback(const OcclusionChangeCallback& callback);

    /**
     * @brief Register web surface occlusion change callback.
     * @param callback callback fuction.
     * @param partitionPoints is a vector of area ratio.
     * @return 0 success, else failed.
     */
    int32_t RegisterSurfaceOcclusionChangeCallback(
        NodeId id, const SurfaceOcclusionChangeCallback& callback, std::vector<float>& partitionPoints);

    /**
     * @brief UnRegister web surface occlusion change callback.
     * @param id is the node id indicates which surface occlusion change callback needs to be unRegister.
     * @return 0 success, else failed.
     */
    int32_t UnRegisterSurfaceOcclusionChangeCallback(NodeId id);

    /**
     * @brief Register HgmConfigChangeCallback.
     * @param callback callback fuction.
     * @return Register result, 0 success, else failed.
     */
    int32_t RegisterHgmConfigChangeCallback(const HgmConfigChangeCallback& callback);

    /**
     * @brief Register HgmRefreshRateModeChangeCallback.
     * @param callback callback fuction.
     * @return Register result, 0 success, else failed.
     */
    int32_t RegisterHgmRefreshRateModeChangeCallback(const HgmRefreshRateModeChangeCallback& callback);

    /**
     * @brief Register HgmRefreshRateUpdateCallback.
     * @param callback callback fuction.
     * @return Register result, 0 success, else failed.
     */
    int32_t RegisterHgmRefreshRateUpdateCallback(const HgmRefreshRateUpdateCallback& callback);

    /**
     * @brief UnRegister HgmRefreshRateUpdateCallback.
     * @return UnRegister result, 0 success, else failed.
     */
    int32_t UnRegisterHgmRefreshRateUpdateCallback();

    /**
     * @brief Register the first frame commit callback function.
     * @param callback Indicates functions that need to be registered.
     * @return Register result, 0 success, else failed.
     */
    int32_t RegisterFirstFrameCommitCallback(const FirstFrameCommitCallback& callback);

    /**
     * @brief UnRegister the first frame commit callback function.
     * @param callback Indicates functions that need to be Unregistered.
     * @return UnRegister result, 0 success, else failed.
     */
    int32_t UnRegisterFirstFrameCommitCallback();

    /**
     * @brief Register FrameRateLinkerExpectedFpsUpdateCallback.
     * @param callback Indicates functions that need to be registered.
     * @return Register result, 0 success, else failed.
     */
    int32_t RegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid,
        const FrameRateLinkerExpectedFpsUpdateCallback& callback);

    /**
     * @brief UnRegister FrameRateLinkerExpectedFpsUpdateCallback.
     * @param callback Indicates functions that need to be Unregistered.
     * @return UnRegister result, 0 success, else failed.
     */
    int32_t UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(int32_t dstPid);

    /**
     * @brief Set appWindow number.
     * @param num winodw number.
     */
    void SetAppWindowNum(uint32_t num);

    /*
    * @brief Set the system overload Animated Scenes to RS for special load shedding.
    * @param systemAnimatedScenes indicates the system animation scene.
    * @param isRegularAnimation indicates irregular windows in the animation scene.
    * @return true if succeed, otherwise false.
    */
    bool SetSystemAnimatedScenes(SystemAnimatedScenes systemAnimatedScenes, bool isRegularAnimation = false);

    /**
     * @brief Set display safe watermark.
     * @param watermarkImg is an image displaying a water image.
     * @param isShow is a flag that enable/disables watermark display.
     */
    void ShowWatermark(const std::shared_ptr<Media::PixelMap> &watermarkImg, bool isShow);

    /**
     * @brief Resize virtual screen.
     * @param id Virtual screen id.
     * @param width Virtual screen width, max: MAX_VIRTUAL_SCREEN_WIDTH.
     * @param height Virtual screen height, max: MAX_VIRTUAL_SCREEN_HEIGHT.
     * @return 0 means success, others failed.
     */
    int32_t ResizeVirtualScreen(ScreenId id, uint32_t width, uint32_t height);

    /**
     * @brief Report jank statistics of render service.
     */
    void ReportJankStats();

    void NotifyLightFactorStatus(int32_t lightFactorStatus);

    /**
     * @brief Notify package event list.
     * @param listSize the size of list.
     * @param packageList the list of package.
     */
    void NotifyPackageEvent(uint32_t listSize, const std::vector<std::string>& packageList);

    /**
     * @brief Notify app strategy config change event.
     * @param pkgName the name of package.
     * @param listSize the size of list.
     * @param newConfig new config.
     */
    void NotifyAppStrategyConfigChangeEvent(const std::string& pkgName, uint32_t listSize,
        const std::vector<std::pair<std::string, std::string>>& newConfig);

    /**
     * @brief Notify Refresh rate event.
     * @param eventInfo the information of event.
     */
    void NotifyRefreshRateEvent(const EventInfo& eventInfo);

    /*
    * @brief Support setting softVsync frame rate through windowId.
    * @param eventInfos a map,key is windowId, value is eventInfo
    */
    void SetWindowExpectedRefreshRate(const std::unordered_map<uint64_t, EventInfo>& eventInfos);

    /*
    * @brief Support setting softVsync frame rate through vsyncName.
    * @param eventInfos a map,key is vsyncName, value is eventInfo.
    */
    void SetWindowExpectedRefreshRate(const std::unordered_map<std::string, EventInfo>& eventInfos);

    /**
     * @brief The game vsync is associated with linkerId. The vsync frequency is reduced to control frames.
     * @param pid is the game process ID.
     * @param name is the name of the game thread.
     * @param rateDiscount Indicates the vsync frequency reduction rate.
     * @return return bool, return true success, return false, failed.
     */
    bool NotifySoftVsyncRateDiscountEvent(uint32_t pid, const std::string &name, uint32_t rateDiscount);

    /**
     * @brief Notify touch event.
     * @param touchStatus status of touch.
     * @param touchCnt the count of touch.
     */
    void NotifyTouchEvent(int32_t touchStatus, int32_t touchCnt);

    /**
     * @brief Notify dynamic mode event.
     * @param enableDynamicMode indicates whether enable dynamic mode.
     */
    void NotifyDynamicModeEvent(bool enableDynamicMode);

    /**
     * @brief Notify dynamic switching HGM config.
     * @param eventName event name.
     * @param state event state.
     */
    void NotifyHgmConfigEvent(const std::string &eventName, bool state);

    /**
     * @brief Notify xcomponet exported frame rate.
     * @param id string id.
     * @param expectedFrameRate expected frame rate.
     */
    void NotifyXComponentExpectedFrameRate(const std::string& id, int32_t expectedFrameRate);

    /**
     * @brief Pre-processing phase of interaction jank statistics.
     * @param info database of render service.
     */
    void ReportEventResponse(DataBaseRs info);

    /**
     * @brief Post-processing phase of interaction jank statistics.
     * @param info database of render service.
     */
    void ReportEventComplete(DataBaseRs info);

    /**
     * @brief Report interaction jank statistics.
     * @param info database of render service.
     */
    void ReportEventJankFrame(DataBaseRs info);

    /**
     * @brief Report game state information.
     * @param info Include pid, uid, render thread id, and foreground or background state of game.
     */
    void ReportGameStateData(GameStateData info);

    /**
     * @brief Report jank state scene start.
     * @param info Include app info.
     */
    void ReportRsSceneJankStart(AppInfo info);

    /**
     * @brief Report jank state scene end.
     * @param info Include app info.
     */
    void ReportRsSceneJankEnd(AppInfo info);

    /**
     * @brief Enable cache for rotation.
     */
    void EnableCacheForRotation();

    /**
     * @brief Disable cache for rotation.
     */
    void DisableCacheForRotation();

    /**
     * @brief Register RenderService death callback.
     * @param callback callback function.
     */
    void SetOnRemoteDiedCallback(const OnRemoteDiedCallback& callback);

    /**
     * @brief Set the system curtain screen status.
     * @param isCurtainScreenOn Specify the curtain screen status.
     */
    void SetCurtainScreenUsingStatus(bool isCurtainScreenOn);

    /**
     * @brief Set the process ID list requiring frame dropping. Next time RS triggers rending,
     * it will purge queued frames of corresponding self-rendering nodes in bufferQueue, and use the latest frame
     * buffer for screen display.
     * @param pidList Process ID list requiring frame dropping.
     */
    void DropFrameByPid(const std::vector<int32_t> pidList);

    /**
     * @brief Get active dirty region info.
     * @return ActiveDirtyRegionInfo, contains the dirty area, number of frames to be drawn, and window name.
     */
    std::vector<ActiveDirtyRegionInfo> GetActiveDirtyRegionInfo() const;

    /**
     * @brief Get global dirty region info.
     * @return GlobalDirtyRegionInfo, include the area of the global dirty, number of frames to be drawn,
     * and number of frames to be skipped to be drawn.
     */
    GlobalDirtyRegionInfo GetGlobalDirtyRegionInfo() const;

    /**
     * @brief Get Layer compose info.
     * @return LayerComposeInfo, include the total number of composite frames,
     * the number of uniformRenderFrame, and the number of redrawFrame.
     */
    LayerComposeInfo GetLayerComposeInfo() const;

    /**
     * @brief Get hwc disabled reason info.
     * @return HwcDisabledReasonInfos, include the number of times that the HWC is disabled.
     */
    HwcDisabledReasonInfos GetHwcDisabledReasonInfo() const;

    int64_t GetHdrOnDuration() const;

    /**
     * @brief Set vma cache status.
     * @param flag wma cache status.
     */
    void SetVmaCacheStatus(bool flag);

#ifdef TP_FEATURE_ENABLE
    /**
     * @brief Set touch config of screen.
     * @param feature Feature ID.
     * @param config Configuration string.
     * @param tpFeatureConfigType The type of TpFeatureConfigType.
     */
    void SetTpFeatureConfig(int32_t feature, const char* config,
        TpFeatureConfigType tpFeatureConfigType = TpFeatureConfigType::DEFAULT_TP_FEATURE);
#endif

    /**
     * @brief Set virtual screen using status, vote for 60Hz if being used.
     * @param isVirtualScreenUsingStatus True means using virtual screen, false means not using.
     */
    void SetVirtualScreenUsingStatus(bool isVirtualScreenUsingStatus);

    /**
     * @brief Register UIExtension callback function.
     * @param userId the id of user.
     * @param callback Callback function.
     * @param unobscured Indicates whether is unobscured.
     */
    int32_t RegisterUIExtensionCallback(uint64_t userId, const UIExtensionCallback& callback, bool unobscured = false);

    /**
     * @brief Set virtual screen status.
     * @param id Virtual screen id.
     * @param screenStatus Virtual screen status, see VirtualScreenStatus.
     * @return True if success, false if failed.
     */
    bool SetVirtualScreenStatus(ScreenId id, VirtualScreenStatus screenStatus);

    /**
     * @brief Set all anco node ForceDoDirect.
     * @param direct is need to set ForceDoDirect.
     * @return the execution result successful.
     */
    bool SetAncoForceDoDirect(bool direct);

    /**
     * @brief Set free multi window status.
     * @param enable Indicates whether enable.
     */
    void SetFreeMultiWindowStatus(bool enable);

    bool RegisterTransactionDataCallback(int32_t pid, uint64_t timeStamp, std::function<void()> callback);

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

    void SetLayerTopForHWC(const std::string &nodeIdStr, bool isTop, uint32_t zOrder);

    // Make this node(nodeIdStr) should do DSS composition and set the layer to top. otherwise do GPU composition.
    /**
     * @brief Set selfdrawing component of stylus engine force use DSS.
     * @param nodeIdStr surfaceNode name.
     * @param isTop is function switch.
     */
    void SetLayerTop(const std::string &nodeIdStr, bool isTop);

    // Make this node(nodeIdStr) should do DSS composition and set the surface force refresh.
    /**
     * @brief Set selfdrawing component of stylus engine force refresh.
     * @param nodeIdStr surfaceNode name.
     * @param isForceRefresh is function switch.
     */
    void SetForceRefresh(const std::string &nodeIdStr, bool isForceRefresh);

    /**
     * @brief Configures the stylus engine's self-drawing component to skip gamut conversion during redering.
     * @param nodeIdStr The node name identifying which stylus engine's
     * self-drawing component should bypass gamut convesion.
     * @param isColorFollow is function switch.
     */
    void SetColorFollow(const std::string &nodeIdStr, bool isColorFollow);

    /**
     * @brief Notify Screen Switched.
     */
    void NotifyScreenSwitched();

    /**
     * @brief Call RequestNextVsync to force refresh one frame with the interface.
     */
    void ForceRefreshOneFrameWithNextVSync();

    /**
     * @brief Register a canvasNode as a container for nodes on the desktop.
     * It is a child node of displayNode to achieve unified scalling and other effects.
     * @param nodeId The id of the canvasNode, which will be moved below the displayNode of its subtree
     * and marked as windowcontainer.
     * @param value Mark a node as windowContainer or not.
     */
    void SetWindowContainer(NodeId nodeId, bool value);

    /**
     * @brief Register selfdrawingNode rect change callback.
     * @param callback callback function.
     * @return return value == 0 success, return value == others, failed.
     */
    int32_t RegisterSelfDrawingNodeRectChangeCallback(const SelfDrawingNodeRectChangeCallback& callback);

#ifdef RS_ENABLE_OVERLAY_DISPLAY
    int32_t SetOverlayDisplayMode(int32_t mode);
#endif

    /**
     * @brief Notify pageName change.
     * @param packageName the name of package.
     * @param pageName the name of pageUrl.
     * @param isEnter is whether to enter the pageUrl.
     */
    void NotifyPageName(const std::string &packageName, const std::string &pageName, bool isEnter);

    /**
     * @brief Get high contrast text state.
     * @return Return true if high contrast text enabled, otherwise false.
     */
    bool GetHighContrastTextState();

    bool SetBehindWindowFilterEnabled(bool enabled);

    bool GetBehindWindowFilterEnabled(bool& enabled);

    int32_t GetPidGpuMemoryInMB(pid_t pid, float &gpuMemInMB);
private:
    RSInterfaces();
    ~RSInterfaces() noexcept;
    /**
     * @brief Component snapshot for divided render.
     * @param id The id of the component.
     * @param callback Callback function.
     * @param scaleX Indicates the scale of X-axis.
     * @param scaleY Indicates the scale of Y-axis.
     */
    bool TakeSurfaceCaptureForUIWithoutUni(NodeId id, std::shared_ptr<SurfaceCaptureCallback> callback,
        float scaleX, float scaleY);

    std::unique_ptr<RSRenderServiceClient> renderServiceClient_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_TRANSACTION_RS_INTERFACES_H
