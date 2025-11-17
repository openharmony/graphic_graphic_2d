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

#include "rs_profiler_test_tree.h"

#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd.h"
#include "recording/recording_handle.h"
#include "rs_profiler.h"
#include "rs_profiler_capture_recorder.h"
#include "rs_profiler_capturedata.h"
#include "rs_profiler_command.h"
#include "rs_profiler_file.h"
#include "rs_profiler_log.h"
#include "rs_profiler_settings.h"
#include "rs_profiler_utils.h"

#include "command/rs_base_node_command.h"
#include "command/rs_canvas_drawing_node_command.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_display_node_command.h"
#include "command/rs_effect_node_command.h"
#include "command/rs_node_command.h"
#include "command/rs_proxy_node_command.h"
#include "command/rs_root_node_command.h"
#include "command/rs_surface_node_command.h"
#include "common/rs_common_def.h"
#include "common/rs_rect.h"
#include "common/rs_vector4.h"
#include "draw/color.h"
#include "draw/core_canvas.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "modifier_ng/background/rs_background_color_render_modifier.h"
#include "modifier_ng/geometry/rs_bounds_render_modifier.h"
#include "modifier_ng/geometry/rs_frame_render_modifier.h"
#include "modifier_ng/geometry/rs_transform_render_modifier.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "transaction/rs_client_to_render_connection.h"
#include "pipeline/render_thread/rs_uni_render_util.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_screen_render_node.h"
#include "platform/common/rs_log.h"
#include "render/rs_typeface_cache.h"
#include "transaction/rs_marshalling_helper.h"
#include "utils/rect.h"
#include "utils/sampling_options.h"

namespace OHOS::Rosen {

Drawing::Image TestTreeBuilder::GenerateRandomImage(int width, int height)
{
    const auto& bitmapFormat = Drawing::BitmapFormat {
        .colorType = Drawing::COLORTYPE_RGBA_8888,
        .alphaType = Drawing::ALPHATYPE_OPAQUE,
    };
    auto bitmap = Drawing::Bitmap();
    bitmap.Build(width, height, bitmapFormat);
    auto pixelsPtr = static_cast<uint8_t*>(bitmap.GetPixels());
    if (pixelsPtr) {
        const auto& imageInfo = bitmap.GetImageInfo();
        size_t bytesPerPixel = static_cast<size_t>(imageInfo.GetBytesPerPixel());
        int heightImage = bitmap.GetHeight();
        int rowBytes = bitmap.GetRowBytes();

        for (int i = 0; i < heightImage; ++i) {
            for (int j = 0; j < rowBytes; ++j) {
                *pixelsPtr = distribution_(mt_);
                pixelsPtr += 1;
            }
        }
    }
    auto image = Drawing::Image();
    image.BuildFromBitmap(bitmap);
    return image;
}

TestTreeBuilder::TestTreeBuilder()
    : mt_(std::random_device()()), insideId_(0), withDisplay_(false), withScreenNode_(false),
      withPatchedGlobalRoot_(false)
{}

void TestTreeBuilder::CreateNode00(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // DISPLAY or ROOT node +0
    NodeId screenNodeId = insideId_++;
    NodeId currentId = insideId_++;
    if (withDisplay_) {
        if (withPatchedGlobalRoot_) {
            RootNodeCommandHelper::Create(context, Utils::PatchNodeId(0));
        }
        if (withScreenNode_) {
            auto node = std::make_shared<RSScreenRenderNode>(screenNodeId, screenNodeId, context.weak_from_this());
            context.GetMutableNodeMap().RegisterRenderNode(node);

            if (withPatchedGlobalRoot_) {
                BaseNodeCommandHelper::AddChild(context, Utils::PatchNodeId(0), screenNodeId, 0);
            } else {
                context.GetGlobalRootRenderNode()->AddChild(node);
            }

            HRPIDN("BuildTestTree: Builded Render Screen node wit id: %" PRIu64, screenNodeId);
        }
        RSDisplayNodeConfig displayNodeConfig {};
        displayNodeConfig.screenId = screenNodeId;
        DisplayNodeCommandHelper::Create(context, currentId, displayNodeConfig);

        HRPIDN("BuildTestTree: Builded Display node wit id: %" PRIu64, currentId);
    } else {
        RootNodeCommandHelper::Create(context, currentId);
        HRPIDN("BuildTestTree: Builded Root node wit id: %" PRIu64, currentId);
    }

    auto positionZProperty = std::make_shared<RSRenderAnimatableProperty<float>>(visibleZPosition);
    auto positionZModifier = std::make_shared<ModifierNG::RSTransformRenderModifier>();
    positionZModifier->AttachProperty(ModifierNG::RSPropertyType::POSITION_Z, positionZProperty);

    RSNodeCommandHelper::AddModifierNG(context, currentId, positionZModifier);
    const auto& node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
}

void TestTreeBuilder::CreateNode01(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // CANVAS node +1
    NodeId currentId = insideId_++;
    RSCanvasNodeCommandHelper::Create(context, currentId);

    auto boundsProperty =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(startX, startY, width, height), zero);
    auto boundsModifier = std::make_shared<ModifierNG::RSBoundsRenderModifier>();
    boundsModifier->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, boundsProperty);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifier);

    auto frameProperty =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(startX, startY, width, height), one);
    auto frameModifier = std::make_shared<ModifierNG::RSFrameRenderModifier>();
    frameModifier->AttachProperty(ModifierNG::RSPropertyType::FRAME, frameProperty);
    RSNodeCommandHelper::AddModifierNG(context, currentId, frameModifier);

    auto positionZProperty = std::make_shared<RSRenderAnimatableProperty<float>>(visibleZPosition, two);
    auto positionZModifier = std::make_shared<ModifierNG::RSTransformRenderModifier>();
    positionZModifier->AttachProperty(ModifierNG::RSPropertyType::POSITION_Z, positionZProperty);
    RSNodeCommandHelper::AddModifierNG(context, currentId, positionZModifier);

    auto backgroundColorProperty = std::make_shared<RSRenderAnimatableProperty<Color>>(almostWhite_, three);
    auto backgroundColorModifier = std::make_shared<ModifierNG::RSBackgroundColorRenderModifier>();
    backgroundColorModifier->AttachProperty(ModifierNG::RSPropertyType::BACKGROUND_COLOR, backgroundColorProperty);
    RSNodeCommandHelper::AddModifierNG(context, currentId, backgroundColorModifier);

    BaseNodeCommandHelper::AddChild(context, currentId - one, currentId, zero);

    auto node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
    HRPIDN("BuildTestTree: Builded Canvas node wit id: %" PRIu64, currentId);
}

void TestTreeBuilder::CreateNode02(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // DRAWING_CANVAS node +2
    NodeId currentId = insideId_++;
    RSCanvasDrawingNodeCommandHelper::Create(context, currentId);

    auto boundsPropertyV11 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0, 0, width13, height));
    auto boundsModifierV11 = std::make_shared<ModifierNG::RSBoundsRenderModifier>();
    boundsModifierV11->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, boundsPropertyV11);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifierV11);

    BaseNodeCommandHelper::AddChild(context, currentId - one, currentId, zero);

    auto node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
    HRPIDN("BuildTestTree: Builded Drawing Canvas node wit id: %" PRIu64, currentId);
}

void TestTreeBuilder::CreateNode03(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // SURFACE node +3
    NodeId currentId = insideId_++;
    SurfaceNodeCommandHelper::Create(context, currentId);

    auto boundsPropertyV20 =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(width13, 0, width23, height));
    auto boundsModifierV20 = std::make_shared<ModifierNG::RSBoundsRenderModifier>();
    boundsModifierV20->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, boundsPropertyV20);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifierV20);

    BaseNodeCommandHelper::AddChild(context, currentId - two, currentId, zero);

    auto node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
    HRPIDN("BuildTestTree: Builded Surface node wit id: %" PRIu64, currentId);
}

void TestTreeBuilder::CreateNode04(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // EFFECT node +4
    NodeId currentId = insideId_++;
    EffectNodeCommandHelper::Create(context, currentId);

    auto boundsPropertyV12 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0, 0, width13, height));
    auto boundsModifierV12 = std::make_shared<ModifierNG::RSBoundsRenderModifier>();
    boundsModifierV12->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, boundsPropertyV12);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifierV12);

    BaseNodeCommandHelper::AddChild(context, currentId - one, currentId, zero);

    auto node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
    HRPIDN("BuildTestTree: Builded Effect node wit id: %" PRIu64, currentId);
}

void TestTreeBuilder::CreateNode05(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // CANVAS node +5
    NodeId currentId = insideId_++;
    RSCanvasNodeCommandHelper::Create(context, currentId);

    auto rect = Drawing::Rect(0, 0, width13, height);
    auto roundRect = Drawing::RoundRect(rect, static_cast<float>(width13) / two, static_cast<float>(height) / five);

    auto color = Drawing::Color::COLOR_RED;
    auto paint = Drawing::Paint(color);
    paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
    auto drawRoundRect = std::make_shared<Drawing::DrawRoundRectOpItem>(roundRect, paint);
    auto drawCmds =
        std::make_shared<Drawing::DrawCmdList>(width13, height, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    drawCmds->AddDrawOp(drawRoundRect);
    RSCanvasNodeCommandHelper::UpdateRecording(
        context, currentId, drawCmds, static_cast<uint16_t>(ModifierNG::RSModifierType::CONTENT_STYLE));

    auto boundsPropertyV120 = std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(0, 0, width13, height));
    auto boundsModifierV120 = std::make_shared<ModifierNG::RSBoundsRenderModifier>();
    boundsModifierV120->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, boundsPropertyV120);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifierV120);

    auto frameModifierV120 = std::make_shared<ModifierNG::RSFrameRenderModifier>();
    frameModifierV120->AttachProperty(ModifierNG::RSPropertyType::FRAME, boundsPropertyV120);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifierV120);

    BaseNodeCommandHelper::AddChild(context, currentId - one, currentId, zero);

    auto node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
    HRPIDN("BuildTestTree: Builded Canvas node wit id: %" PRIu64, currentId);
}

void TestTreeBuilder::CreateNode06(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // CANVAS node +6
    NodeId currentId = insideId_++;
    RSCanvasNodeCommandHelper::Create(context, currentId);

    auto boundsPropertyV21 =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(width13, 0, width13, height13));
    auto boundsModifierV21 = std::make_shared<ModifierNG::RSBoundsRenderModifier>();
    boundsModifierV21->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, boundsPropertyV21);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifierV21);

    auto drawCmds =
        std::make_shared<Drawing::DrawCmdList>(width13, height13, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    RSCanvasNodeCommandHelper::UpdateRecording(
        context, currentId, drawCmds, static_cast<uint16_t>(ModifierNG::RSModifierType::CONTENT_STYLE));

    BaseNodeCommandHelper::AddChild(context, currentId - three, currentId, zero);

    auto node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
    HRPIDN("BuildTestTree: Builded Canvas node wit id: %" PRIu64, currentId);
}

void TestTreeBuilder::CreateNode07(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // CANVAS node +7
    NodeId currentId = insideId_++;
    RSCanvasNodeCommandHelper::Create(context, currentId);

    auto boundsPropertyV22 =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(width13, height13, width13, height13));
    auto boundsModifierV22 = std::make_shared<ModifierNG::RSBoundsRenderModifier>();
    boundsModifierV22->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, boundsPropertyV22);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifierV22);

    auto drawCmds =
        std::make_shared<Drawing::DrawCmdList>(width13, height13, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    RSCanvasNodeCommandHelper::UpdateRecording(
        context, currentId, drawCmds, static_cast<uint16_t>(ModifierNG::RSModifierType::BACKGROUND_STYLE));
    BaseNodeCommandHelper::AddChild(context, currentId - four, currentId, zero);

    auto node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
    HRPIDN("BuildTestTree: Builded Canvas node wit id: %" PRIu64, currentId);
}

void TestTreeBuilder::CreateNode08(RSContext& context, std::vector<std::shared_ptr<RSRenderNode>>& tree)
{
    // CANVAS node +8
    NodeId currentId = insideId_++;
    RSCanvasNodeCommandHelper::Create(context, currentId);

    auto boundsPropertyV23 =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(width13, height23, width13, height23));
    auto boundsModifierV23 = std::make_shared<ModifierNG::RSBoundsRenderModifier>();
    boundsModifierV23->AttachProperty(ModifierNG::RSPropertyType::BOUNDS, boundsPropertyV23);
    RSNodeCommandHelper::AddModifierNG(context, currentId, boundsModifierV23);

    auto framePropertyV23 =
        std::make_shared<RSRenderAnimatableProperty<Vector4f>>(Vector4f(width13, height23, width13, height23));
    auto frameModifierV23 = std::make_shared<ModifierNG::RSFrameRenderModifier>();
    frameModifierV23->AttachProperty(ModifierNG::RSPropertyType::FRAME, framePropertyV23);
    RSNodeCommandHelper::AddModifierNG(context, currentId, frameModifierV23);

    auto drawCmds =
        std::make_shared<Drawing::DrawCmdList>(width13, height13, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    const float singleRadius = 0.5f;
    std::vector<Drawing::Point> radius = {
        { singleRadius, singleRadius },
        { singleRadius, singleRadius },
        { singleRadius, singleRadius },
        { singleRadius, singleRadius },
    };
    auto clipAdaptiveRoundRectOpItem = Drawing::ClipAdaptiveRoundRectOpItem(radius);
    auto clipAdaptiveRoundRectOpItemPtr =
        std::make_shared<Drawing::ClipAdaptiveRoundRectOpItem>(clipAdaptiveRoundRectOpItem);
    drawCmds->AddDrawOp(clipAdaptiveRoundRectOpItemPtr);
    RSCanvasNodeCommandHelper::UpdateRecording(
        context, currentId, drawCmds, static_cast<uint16_t>(ModifierNG::RSModifierType::CONTENT_STYLE));

    auto newDrawCmds =
        std::make_shared<Drawing::DrawCmdList>(width13, height13, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    RSCanvasNodeCommandHelper::UpdateRecording(context, currentId,
        newDrawCmds, static_cast<uint16_t>(ModifierNG::RSModifierType::OVERLAY_STYLE));

    BaseNodeCommandHelper::AddChild(context, currentId - five, currentId, zero);

    auto node = RSProfiler::GetRenderNode(currentId);
    if (!node) {
        return;
    }
    node->ApplyModifiers();
    tree.push_back(node);
    HRPIDN("BuildTestTree: Builded Canvas node wit id: %" PRIu64, currentId);
}

std::vector<std::shared_ptr<RSRenderNode>> TestTreeBuilder::Build(
    RSContext& context, NodeId topId, bool withDisplay, bool withScreenNode, bool withPatchedGlobalRoot)
{
    using OHOS::Rosen::DisplayNodeCommandHelper;
    using OHOS::Rosen::EffectNodeCommandHelper;
    using OHOS::Rosen::RSCanvasNodeCommandHelper;
    using OHOS::Rosen::SurfaceNodeCommandHelper;

    std::vector<std::shared_ptr<RSRenderNode>> tree;
    insideId_ = topId;
    withDisplay_ = withDisplay;
    withScreenNode_ = withScreenNode;
    withPatchedGlobalRoot_ = withPatchedGlobalRoot;

    /* graph structure of tree:

        DISPLAY node OR ROOT node
            |- CANVAS node +1
                |- DRAWING_CANVAS node +2
                |- SURFACE node +3
                    |- EFFECT node +4
                    |   |- CANVAS node +5
                    |- CANVAS node +6
                    |- CANVAS node +7
                    |- CANVAS node +8
    */

    CreateNode00(context, tree);
    CreateNode01(context, tree);
    CreateNode02(context, tree);
    CreateNode03(context, tree);
    CreateNode04(context, tree);
    CreateNode05(context, tree);
    CreateNode06(context, tree);
    CreateNode07(context, tree);
    CreateNode08(context, tree);

    for (const auto& node : tree) {
        if (!node) {
            continue;
        }
        node->SetContentDirty();
        node->SetDirty();
    }

    return tree;
}
} // namespace OHOS::Rosen