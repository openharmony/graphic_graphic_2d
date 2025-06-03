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
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_surface_extractor.h
 *
 * @brief Defines the properties and methods for RSDisplayNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_EXECTOR_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_EXECTOR_H

#include "platform/drawing/rs_surface.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {

/**
 * @class RSSurfaceExtractor
 *
 * @brief The class for extracting RSSurface from a RSSurfaceNode.
 */
class RSC_EXPORT RSSurfaceExtractor {
public:
    /**
     * @brief Extracts RSSurface.
     *
     * @param node A shared pointer to the RSSurfaceNode from which to extract the RSSurface.
     * @return The extracted RSSurface, or nullptr if node is nullptr.
     */
    static std::shared_ptr<RSSurface> ExtractRSSurface(std::shared_ptr<RSSurfaceNode> node);
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_SURFACE_EXECTOR_H