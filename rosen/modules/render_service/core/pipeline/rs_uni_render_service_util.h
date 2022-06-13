/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_SERVICE_UTIL_H
#define RENDER_SERVICE_CORE_PIPELINE_RS_UNI_RENDER_SERVICE_UTIL_H

#include "pipeline/rs_processor.h"
#include "pipeline/rs_dirty_region_manager.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_render_service_util.h"
#include "screen_manager/rs_screen_manager.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
class RsUniRenderServiceUtil {
public:
    static bool isUniRender();
    static void DrawBufferOnCanvas(RSSurfaceRenderNode& node, std::unique_ptr<RSPaintFilterCanvas>& canvas);
#ifdef RS_ENABLE_EGLIMAGE
    static void DrawImageOnCanvas(RSSurfaceRenderNode& node, std::unique_ptr<RSPaintFilterCanvas>& canvas);
#endif // RS_ENABLE_EGLIMAGE
};
} // Rosen
} // OHOS
#endif
