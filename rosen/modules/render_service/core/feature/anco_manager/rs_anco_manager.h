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
#ifndef RS_ANCO_MANAGER_H
#define RS_ANCO_MANAGER_H

#include "common/rs_common_def.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "screen_manager/screen_types.h"

namespace OHOS::Rosen {
class RSAncoManager {
public:
    static RSAncoManager* Instance();
    AncoHebcStatus GetAncoHebcStatus() const;
    void SetAncoHebcStatus(AncoHebcStatus hebcStatus);
    bool AncoOptimizeDisplayNode(std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
        std::vector<std::shared_ptr<RSSurfaceRenderNode>>& hardwareEnabledNodes,
        ScreenRotation rotation, uint32_t width, uint32_t height);
    virtual bool IsAncoOptimize(ScreenRotation rotation);
    // When the anco node is rendered in a unified way, ancoSrcCrop is effective
    static void UpdateCropRectForAnco(const uint32_t ancoFlags, const Rect& cropRect, Drawing::Rect& srcRect);
    // When the anco layer is redrawed in dss, ancoSrcCrop is effective
    static void UpdateCropRectForAnco(const uint32_t ancoFlags, const GraphicIRect& cropRect, Drawing::Rect& srcRect);
    // When the anco node generates a layer, ancoSrcCrop takes effect
    static void UpdateLayerSrcRectForAnco(const uint32_t ancoFlags,
                                          const GraphicIRect& cropRect, GraphicIRect& srcRect);
    static bool IsAncoSfv(const uint32_t ancoFlags);

private:
    bool AncoOptimizeCheck(bool isHebc, int nodesCnt, int sfvNodesCnt);
    // anco displayNode use hebc
    std::atomic<int32_t> ancoHebcStatus_ = static_cast<int32_t>(AncoHebcStatus::INITIAL);
protected:
    RSAncoManager() = default;
    virtual ~RSAncoManager() = default;
};
} // namespace OHOS::Rosen
#endif // RS_ANCO_MANAGER_H
