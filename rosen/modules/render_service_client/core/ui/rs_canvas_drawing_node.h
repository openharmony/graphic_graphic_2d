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
 * @file rs_canvas_drawing_node.h
 *
 * @brief Defines the properties and methods for RSCanvasDrawingNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_DRAWING_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_DRAWING_NODE_H

#include "pixel_map.h"

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include <mutex>
#include "surface_buffer.h"
#endif

#include "ui/rs_canvas_node.h"

class SkCanvas;

namespace OHOS {
namespace Rosen {
class RSNodeMap;

/**
 * @class RSCanvasDrawingNode
 *
 * @brief Represents a drawing node in the rendering service.
 *
 * This class is responsible for handling drawing operations on a canvas.
 */
class RSC_EXPORT RSCanvasDrawingNode : public RSCanvasNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasDrawingNode>;
    using SharedPtr = std::shared_ptr<RSCanvasDrawingNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::CANVAS_DRAWING_NODE;

    /**
     * @brief Gets the type of the RSNode.
     * 
     * @return The type of the RSNode.
     */
    RSUINodeType GetType() const override
    {
        return Type;
    }

    /**
     * @brief Destructor for RSCanvasDrawingNode.
     */
    ~RSCanvasDrawingNode() override;

    /**
     * @brief Create a new RSCanvasDrawingNode instance.
     * 
     * @param isRenderServiceNode Indicates if the node is a render service node.
     * @param isTextureExportNode Indicates if the node is a texture export node.
     * @param rsUIContext The RSUIContext to be used.
     * @return A shared pointer to the created RSCanvasDrawingNode instance.
     */
    static SharedPtr Create(bool isRenderServiceNode = false, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    
    /**
     * @brief Gets a bitmap representation of the drawing content.
     *
     * @param bitmap The bitmap object to store the retrieved drawing content.
     * @param drawCmdList Optional parameter specifying a list of drawing commands to apply. Defaults to nullptr.
     * @param rect Optional parameter specifying the rectangular region to retrieve. 
     * @return true if the bitmap is successfully retrieved; false otherwise.
     */
    bool GetBitmap(Drawing::Bitmap& bitmap,
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr, const Drawing::Rect* rect = nullptr);
    
    /**
     * @brief Gets a pixel map representation of the drawing node.
     *
     * @param pixelmap A shared pointer to a Media::PixelMap object where the pixel map data will be stored.
     * @param drawCmdList An optional shared pointer containing drawing commands to be applied. Defaults to nullptr.
     * @param rect An optional pointer specifying the region of interest.
     * @return true if the pixel map was successfully retrieved; false otherwise.
     */
    bool GetPixelmap(std::shared_ptr<Media::PixelMap> pixelmap,
        std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr, const Drawing::Rect* rect = nullptr);
    
    /**
     * @brief Resets the surface with the specified width and height.
     *
     * @param width The new width of the surface.
     * @param height The new height of the surface.
     * @return true if the surface was successfully reset; false otherwise.
     */
    bool ResetSurface(int width, int height);

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    /**
     * @brief Callback invoked when the SurfaceBuffer for this Canvas node has been created or changed.
     *
     * This is called by the global Canvas callback router when RS notifies the app
     * that a new SurfaceBuffer is available for this node. The buffer is stored with a resetSurfaceIndex
     * for version control - only buffers with newer resetSurfaceIndexes are accepted.
     *
     * This unified buffer storage handles both:
     * 1. Pre-allocated buffers from async ResetSurface tasks
     * 2. Buffers sent by RS via this callback
     *
     * @param buffer The SurfaceBuffer containing DMA memory to be held by the app.
     *               Ignored if resetSurfaceIndex is stale (older than previously set buffer).
     */
    void OnSurfaceBufferChanged(sptr<SurfaceBuffer> buffer, uint32_t resetSurfaceIndex);
#endif

protected:
    RSCanvasDrawingNode(
        bool isRenderServiceNode, bool isTextureExportNode = false, std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSCanvasDrawingNode(const RSCanvasDrawingNode&) = delete;
    RSCanvasDrawingNode(const RSCanvasDrawingNode&&) = delete;
    RSCanvasDrawingNode& operator=(const RSCanvasDrawingNode&) = delete;
    RSCanvasDrawingNode& operator=(const RSCanvasDrawingNode&&) = delete;

    /**
     * @brief Creates a render node for exporting texture with a switch mechanism.
     */
    void CreateRenderNodeForTextureExportSwitch() override;

    void SetIsOnTheTree(bool onTheTree) override;

private:
    /**
     * @brief Registers the node in the node map.
     */
    void RegisterNodeMap() override;

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    static uint32_t GenerateResetSurfaceIndex();

    static void PreAllocateDMABuffer(
        std::weak_ptr<RSCanvasDrawingNode> weakNode, NodeId nodeId, int width, int height, uint32_t resetSurfaceIndex);

    bool isNeverOnTree_ = true;

    // Canvas SurfaceBuffer management for memory attribution
    // This buffer contains DMA memory that RS and app both hold, causing memory to be counted to app process.
    //
    // Unified buffer storage supporting both:
    // 1. Pre-allocated DMA buffer from async ResetSurface task
    // 2. DMA buffer received from RS via OnSurfaceBufferChanged callback
    //
    // resetSurfaceIndex-based versioning ensures newer buffers override older ones:
    // - Pre-allocation task: allocates and submits with resetSurfaceIndex
    // - RS callback: may send buffer with different resetSurfaceIndex
    // - Only the buffer with the latest resetSurfaceIndex is kept
    sptr<SurfaceBuffer> canvasSurfaceBuffer_ = nullptr;

    mutable std::mutex surfaceBufferMutex_;

    uint32_t resetSurfaceIndex_ = 0;
#endif
};
} // namespace Rosen
} // namespace OHOS
/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_DRAWING_NODE_H