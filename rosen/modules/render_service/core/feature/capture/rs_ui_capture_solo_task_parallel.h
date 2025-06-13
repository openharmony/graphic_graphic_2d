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

#ifndef RS_UI_CAPTURE_SOLO_TASK_PARALLEL
#define RS_UI_CAPTURE_SOLO_TASK_PARALLEL
#define EGL_EGLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES

#include "drawable/rs_render_node_drawable.h"

#include "pixel_map.h"
#include "system/rs_system_parameters.h"
#include "draw/surface.h"
#include "draw/color.h"
#include "pipeline/rs_effect_render_node.h"
#include "common/rs_common_def.h"
#include "system/rs_system_parameters.h"

namespace OHOS {
namespace Rosen {
class RSUiCaptureSoloTaskParallel {
public:
    explicit RSUiCaptureSoloTaskParallel(NodeId nodeId, const RSSurfaceCaptureConfig& captureConfig)
        : nodeId_(nodeId), captureConfig_(captureConfig) {}
    ~RSUiCaptureSoloTaskParallel() = default;

    /**
     * @brief Get a list of pixelmap information, each node of the component node tree will have a pixelmap.
     * @param id Indicates the NodeId of the RSNode.
     * @param captureConfig Indicates the configuration of snapshot like scale, translation, rotation.
     * @return Returns a vector of pair, the first element is the NodeId, the second element is the pixelmap.
     */
    static std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> CaptureSoloNode(
        NodeId id, const RSSurfaceCaptureConfig& captureConfig);

    /**
     * @brief Get a pixelmap, which is the snapshot of the node, only contains the node itself.\n
     *
     * Child nodes of the param node will not be included in the snapshot.
     * Used for scenarios requiring a snapshot of a specific node, such as capturing a specific component.
     *
     * @param id Indicates the NodeId of the RSNode.
     * @param captureConfig Indicates the configuration of snapshot like scale, translation, rotation.
     * @return Returns a pixelmap.
     */
    static std::unique_ptr<Media::PixelMap> CaptureSoloNodePixelMap(
        NodeId id, const RSSurfaceCaptureConfig& captureConfig);
    bool CreateResources();
    bool Run();
    static bool CopyDataToPixelMap(std::shared_ptr<Drawing::Image> img,
        const std::unique_ptr<Media::PixelMap>& pixelMap,
        std::shared_ptr<Drawing::ColorSpace> colorSpace = nullptr);
#ifdef RS_ENABLE_UNI_RENDER
    static std::function<void()> CreateSurfaceSyncCopyTask(std::shared_ptr<Drawing::Surface> surface,
        std::unique_ptr<Media::PixelMap>& pixelMap, NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        int32_t rotation = 0);
#endif

private:
    std::shared_ptr<Drawing::Surface> CreateSurface(const std::unique_ptr<Media::PixelMap>& pixelmap) const;
    std::unique_ptr<Media::PixelMap> CreatePixelMapByNode(std::shared_ptr<RSRenderNode> node) const;
    std::unique_ptr<Media::PixelMap> pixelMap_ = nullptr;
    std::shared_ptr<DrawableV2::RSRenderNodeDrawable> nodeDrawable_ = nullptr;
    NodeId nodeId_ = INVALID_NODEID;
    RSSurfaceCaptureConfig captureConfig_ = {};
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_UI_CAPTURE_SOLO_TASK_PARALLEL
