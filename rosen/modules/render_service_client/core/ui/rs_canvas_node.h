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
 * @file rs_canvas_node.h
 *
 * @brief Defines the properties and methods for RSCanvasNode class.
 */

#ifndef RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H
#define RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H

#include "modifier/rs_cmd_list_image_collector.h"
#include "pipeline/rs_recording_canvas.h"
#include "ui/rs_node.h"
#include "command/rs_canvas_node_command.h"
#include "command_modifier/rs_canvas_node_command_modifier.h"

namespace OHOS {
namespace Rosen {
class RSNodeMap;

/**
 * @class RSCanvasNode
 *
 * @brief Represents a canvas node in the rendering service.
 */
class RSC_EXPORT RSCanvasNode : public RSNode {
public:
    using WeakPtr = std::weak_ptr<RSCanvasNode>;
    using SharedPtr = std::shared_ptr<RSCanvasNode>;
    static inline constexpr RSUINodeType Type = RSUINodeType::CANVAS_NODE;

    /**
     * @brief Get the type of the RSNode.
     * 
     * @return The type of the RSNode.
     */
    RSUINodeType GetType() const override
    {
        return Type;
    }

    /**
     * @brief Destructor for RSCanvasNode.
     */
    ~RSCanvasNode() override;

    /**
     * @brief Creates a shared pointer to an RSCanvasNode.
     * 
     * @param isRenderServiceNode Indicates whether the node is a render service node. Defaults to false.
     * @param isTextureExportNode Indicates whether the node is a texture export node. Defaults to false.
     * @param rsUIContext A shared pointer to an RSUIContext object. Defaults to nullptr.
     * @return A shared pointer to the created RSCanvasNode.
     */
    static SharedPtr Create(bool isRenderServiceNode = false, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);

    /**
     * @brief Begins recording drawing commands on the canvas.
     * 
     * @param width The width of the recording canvas.
     * @param height The height of the recording canvas.
     * @return A pointer to an ExtendRecordingCanvas object for recording drawing commands.
     */
    ExtendRecordingCanvas* BeginRecording(int width, int height);

    /**
     * @brief Checks if the canvas is currently recording.
     * 
     * @return true if the canvas is recording; false otherwise.
     */
    bool IsRecording() const;

    /**
     * @brief Completes the drawing record and add the drawing command.
     */
    void FinishRecording();

    /**
     * @brief Gets the width of the paint used in the canvas node.
     * 
     * @return The width of the paint.
     */
    float GetPaintWidth() const;

    /**
     * @brief Gets the height of the paint used in the canvas node.
     * 
     * @return The height of the paint.
     */
    float GetPaintHeight() const;

    /**
     * @brief Draws on the canvas node using the specified modifier type and drawing function.
     *
     * @param type The type of modifier to be applied during the drawing operation.
     * @param func The drawing function that defines the drawing logic to be executed on the node.
     */
    void DrawOnNode(ModifierNG::RSModifierType type, DrawFunc func) override;

    /**
     * @brief Controls freeze state of canvas node rendering content
     *
     * Freezes current frame data when enabled, preventing content refresh until unfrozen.
     * Used for scenarios requiring static snapshots or temporary rendering suspension.
     *
     * @param isFreeze Freeze control flag:
     *                - true: Freeze current content
     *                - false: Resume normal rendering pipeline
     */
    void SetFreeze(bool isFreeze, bool isMarkedByUI = false) override;

    /**
     * @brief Sets the HDR display properties.
     *
     * @param hdrPresent True means HDR display; false otherwise.
     */
    void SetHDRPresent(bool hdrPresent);

    /**
     * @brief Sets the canvas node's color gamut.
     *
     * @param colorGamut The enum of ColorSpaceName.
     */
    void SetColorGamut(uint32_t colorGamut);

    /**
     * @brief Sets the callback function to be called when the bounds of the canvas node change.
     *
     * @param callback The function to be called when the bounds change.
     */
    void SetBoundsChangedCallback(BoundsChangedCallback callback) override;

    // HybridDraw Start
    bool IsHybridRenderCanvas() const
    {
        return false;
    }

    bool GetBitmap(Drawing::Bitmap& bitmap, std::shared_ptr<Drawing::DrawCmdList> drawCmdList = nullptr)
    {
        return false;
    }

    bool GetPixelmap(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Drawing::DrawCmdListPtr> drawCmdList = nullptr, const Drawing::Rect* rect = nullptr)
    {
        return false;
    }

    bool ResetSurface()
    {
        return false;
    }
    // HybridDraw End
    
    /**
     * @brief Set a pixel map.
     *
     * @param pixelMap A shared pointer to a Media::PixelMap object.
     */
    void SetPixelmap(const std::shared_ptr<Media::PixelMap>& pixelMap);

protected:
    RSCanvasNode(bool isRenderServiceNode, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSCanvasNode(bool isRenderServiceNode, NodeId id, bool isTextureExportNode = false,
        std::shared_ptr<RSUIContext> rsUIContext = nullptr);
    RSCanvasNode(const RSCanvasNode&) = delete;
    RSCanvasNode(const RSCanvasNode&&) = delete;
    RSCanvasNode& operator=(const RSCanvasNode&) = delete;
    RSCanvasNode& operator=(const RSCanvasNode&&) = delete;

    void CreateRenderNode() override;

    BoundsChangedCallback boundsChangedCallback_;
 
    virtual void OnFinishRecording(Drawing::DrawCmdListPtr& drawCmdList, ModifierNG::RSModifierType modifierType);
private:
    ExtendRecordingCanvas* recordingCanvas_ = nullptr;
    bool recordingUpdated_ = false;
    mutable std::mutex mutex_;

    friend class RSUIDirector;
    friend class RSAnimation;
    friend class RSPathAnimation;
    friend class RSPropertyAnimation;
    friend class RSNodeMap;
    friend class RSNodeMapV2;
    void OnBoundsSizeChanged() override;
    void CreateRenderNodeForTextureExportSwitch() override;
    void RegisterNodeMap() override;

    std::unique_ptr<CmdListImage> cmdListImages_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

/** @} */
#endif // RENDER_SERVICE_CLIENT_CORE_UI_RS_CANVAS_NODE_H
