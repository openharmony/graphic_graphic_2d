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

/**
 * @addtogroup RenderNodeDisplay
 * @{
 *
 * @brief Display render nodes.
 */

/**
 * @file rs_texture_export.h
 *
 * @brief Defines the properties and methods for RSTextureExport class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_TEXTURE_EXPORT_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_TEXTURE_EXPORT_H

#include "ui/rs_ui_director.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

class RSC_EXPORT RSTextureExport {
public:
    /**
     * @brief Constructor for RSTextureExport.
     *
     * @param rootNode The root node of the texture export.
     * @param surfaceId The ID of the surface to be exported.
     */
    RSTextureExport(std::shared_ptr<RSNode> rootNode, SurfaceId surfaceId);

    /**
     * @brief Destructor for RSTextureExport.
     */
    ~RSTextureExport();

    /**
     * @brief Performs the texture export operation.
     *
     * @return true if the texture export is successful; otherwise, false.
     */
    bool DoTextureExport();

    /**
     * @brief Stops the texture export operation.
     */
    void StopTextureExport();

    /**
     * @brief Updates the buffer information for texture export.
     *
     * @param x The x-coordinate of the buffer.
     * @param y The y-coordinate of the buffer.
     * @param width The width of the buffer.
     * @param height The height of the buffer.
     */
    void UpdateBufferInfo(float x, float y, float width, float height);
private:
    std::shared_ptr<RSUIDirector> rsUiDirector_;
    std::shared_ptr<RSNode> rootNode_;
    SurfaceId surfaceId_;
    std::shared_ptr<RSSurfaceNode> virtualSurfaceNode_;
    std::shared_ptr<RSNode> virtualRootNode_;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_TEXTURE_EXPORT_H
