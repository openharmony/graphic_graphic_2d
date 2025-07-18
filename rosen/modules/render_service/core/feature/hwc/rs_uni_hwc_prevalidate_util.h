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

#ifndef UNI_HWC_PREVALIDATE_UTIL_H
#define UNI_HWC_PREVALIDATE_UTIL_H

#include <array>
#include <map>
#include <vector>

#include "screen_manager/screen_types.h"
#include "screen_manager/rs_screen_manager.h"
#include "feature/hwc/rs_uni_hwc_prevalidate_common.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node.h"
#include "feature/round_corner_display/rs_rcd_surface_render_node_drawable.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {

typedef struct RequestLayerInfo {
    uint64_t id;                          /**< Layer ID */
    RequestRect srcRect;                         /**< Source Rect of Surface */
    RequestRect dstRect;                         /**< Destination Rect of Surface */
    uint32_t zOrder;                      /**< Zorder of Surface */
    int format;                           /**< Format of Surface Buffer */
    int transform;                        /**< Transform of Surface Buffer */
    int compressType;                     /**< CompressType of Surface Buffer */
    uint64_t bufferUsage;                 /**< Usage of Surface Buffer */
    uint64_t layerUsage;                  /**< Usage of RequestLayerInfo */
    /**< Extra parameters of frame, format: [key, parameter] */
    std::unordered_map<std::string, std::vector<int8_t>> perFrameParameters;
    CldInfo cldInfo;
    uint32_t fps = 120;
    BufferHandle* bufferHandle = nullptr;
} RequestLayerInfo;

using RequestCompositionType = enum class RequestComposition : int32_t {
    INVALID = 0,
    CLIENT = 1,
    DEVICE = 2,
    OFFLINE_DEVICE = 3,
    DEVICE_VSCF = 4,
};

using PreValidateFunc = int32_t (*)(uint32_t,
    const std::vector<RequestLayerInfo> &, std::map<uint64_t, RequestCompositionType> &);

class RSUniHwcPrevalidateUtil {
public:
    static RSUniHwcPrevalidateUtil& GetInstance();
    bool PreValidate(
        ScreenId id, std::vector<RequestLayerInfo> infos, std::map<uint64_t, RequestCompositionType> &strategy);
    bool CreateSurfaceNodeLayerInfo(uint32_t zorder,
        RSSurfaceRenderNode::SharedPtr node, GraphicTransformType transform, uint32_t fps, RequestLayerInfo &info);
    bool CreateScreenNodeLayerInfo(uint32_t zorder,
        RSScreenRenderNode::SharedPtr node, const ScreenInfo &screenInfo, uint32_t fps, RequestLayerInfo &info);
    bool CreateRCDLayerInfo(
        RSRcdSurfaceRenderNode::SharedPtr node, const ScreenInfo &screenInfo, uint32_t fps, RequestLayerInfo &info);
    bool IsPrevalidateEnable();
    bool GetPrevalidateEnabled();
    void CollectSurfaceNodeLayerInfo(
        std::vector<RequestLayerInfo>& prevalidLayers, std::vector<RSBaseRenderNode::SharedPtr>& surfaceNodes,
        uint32_t curFps, uint32_t& zOrder, const ScreenInfo& screenInfo);
private:
    RSUniHwcPrevalidateUtil();
    ~RSUniHwcPrevalidateUtil();

    bool IsYUVBufferFormat(RSSurfaceRenderNode::SharedPtr node) const;
    bool IsNeedDssRotate(GraphicTransformType transform) const;
    void CopyCldInfo(const CldInfo& src, RequestLayerInfo& info);
    void LayerRotate(
        RequestLayerInfo& info, const sptr<IConsumerSurface>& surface, const ScreenInfo &screenInfo);
    bool CheckIfDoArsrPre(const RSSurfaceRenderNode::SharedPtr node);
    void CheckIfDoCopybit(const RSSurfaceRenderNode::SharedPtr node, GraphicTransformType transform,
        RequestLayerInfo& info);
    static bool CheckHwcNodeAndGetPointerWindow(
        const RSSurfaceRenderNode::SharedPtr& node, RSSurfaceRenderNode::SharedPtr& pointerWindow);
    static void EmplaceSurfaceNodeLayer(
        std::vector<RequestLayerInfo>& prevalidLayers, RSSurfaceRenderNode::SharedPtr node,
        uint32_t curFps, uint32_t& zOrder, const ScreenInfo& screenInfo);

    void *preValidateHandle_ = nullptr;
    PreValidateFunc preValidateFunc_ = nullptr;
    bool loadSuccess_ = false;
    bool isPrevalidateHwcNodeEnable_ = false;
    bool arsrPreEnabled_ = false;
    bool isCopybitSupported_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // UNI_HWC_PREVALIDATE_UTIL_H