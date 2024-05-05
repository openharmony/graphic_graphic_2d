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
#include <stdint.h>
#include <vector>

#include "screen_manager/screen_types.h"
#include "screen_manager/rs_screen_manager.h"
#include "pipeline/round_corner_display/rs_rcd_surface_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS {
namespace Rosen {
constexpr uint64_t USAGE_UNI_LAYER = 1ULL << 60;
 
struct RequestRect {
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t w = 0;
    uint32_t h = 0;
};
 
typedef struct RequestLayerInfo {
    uint64_t id;                          /**< Layer ID */
    RequestRect srcRect;                         /**< Source Rect of Surface */
    RequestRect dstRect;                         /**< Destination Rect of Surface */
    uint32_t zOrder;                      /**< Zorder of Surface */
    int format;                           /**< Format of Surface Buffer */
    int transform;                        /**< Transform of Surface Buffer */
    int compressType;                     /**< CompressType of Surface Buffer */
    uint64_t usage;                       /**< Usage of Surface Buffer */
    /**< Extra parameters of frame, format: [key, parameter] */
    std::unordered_map<std::string, std::vector<int8_t>> perFrameParameters;
    CldInfo *cldInfo = nullptr;
    uint32_t fps = 120;
} RequestLayerInfo;

using RequestCompositionType = enum class RequestComposition : int32_t {
    INVALID = 0,
    CLIENT = 1,
    DEVICE = 2,
    OFFLINE_DEVICE = 3,
};

using PreValidateFunc = int32_t (*)(uint32_t,
    const std::vector<RequestLayerInfo> &, std::map<uint64_t, RequestCompositionType> &);

class RSUniHwcPrevalidateUtil {
public:
    static RSUniHwcPrevalidateUtil& GetInstance();
    bool PreValidate(
        ScreenId id, std::vector<RequestLayerInfo> infos, std::map<uint64_t, RequestCompositionType> &strategy);
    RequestLayerInfo CreateSurfaceNodeLayerInfo(
        RSSurfaceRenderNode::SharedPtr node, GraphicTransformType transform, uint32_t fps);
    RequestLayerInfo CreateDisplayNodeLayerInfo(
        uint32_t zorder, RSDisplayRenderNode::SharedPtr node, const ScreenInfo &screenInfo, uint32_t fps);
    RequestLayerInfo CreateRCDLayerInfo(
        RSRcdSurfaceRenderNode::SharedPtr node, const ScreenInfo &screenInfo, uint32_t fps);
    bool IsLoadSuccess() const;
private:
    RSUniHwcPrevalidateUtil();
    ~RSUniHwcPrevalidateUtil();

    void CopyCldInfo(CldInfo src, RequestLayerInfo& info);
    void LayerRotate(
        RequestLayerInfo& info, const sptr<IConsumerSurface>& surface, const ScreenInfo &screenInfo);

    void *preValidateHandle_ = nullptr;
    PreValidateFunc preValidateFunc_ = nullptr;
    bool loadSuccess = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // UNI_HWC_PREVALIDATE_UTIL_H