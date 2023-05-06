/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#include "recording/recording_canvas.h"

#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"
#include "recording/recording_path.h"
#include "recording/recording_color_filter.h"
#include "recording/recording_color_space.h"
#include "recording/recording_image_filter.h"
#include "recording/recording_mask_filter.h"
#include "recording/recording_path_effect.h"
#include "recording/recording_shader_effect.h"
#include "draw/color.h"
#include "effect/color_filter.h"
#include "effect/color_space.h"
#include "effect/filter.h"
#include "effect/image_filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

RecordingCanvas::RecordingCanvas(int width, int height) : Canvas(width, height)
{
    cmdList_ = std::make_shared<DrawCmdList>(width, height);
}

std::shared_ptr<DrawCmdList> RecordingCanvas::GetDrawCmdList() const
{
    return cmdList_;
}

void RecordingCanvas::DrawPoint(const Point& point)
{
    cmdList_->AddOp<DrawPointOpItem>(point);
}

void RecordingCanvas::DrawLine(const Point& startPt, const Point& endPt)
{
    cmdList_->AddOp<DrawLineOpItem>(startPt, endPt);
}

void RecordingCanvas::DrawRect(const Rect& rect)
{
    cmdList_->AddOp<DrawRectOpItem>(rect);
}

void RecordingCanvas::DrawRoundRect(const RoundRect& roundRect)
{
    cmdList_->AddOp<DrawRoundRectOpItem>(roundRect);
}

void RecordingCanvas::DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner)
{
    cmdList_->AddOp<DrawNestedRoundRectOpItem>(outer, inner);
}

void RecordingCanvas::DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    cmdList_->AddOp<DrawArcOpItem>(oval, startAngle, sweepAngle);
}

void RecordingCanvas::DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle)
{
    cmdList_->AddOp<DrawPieOpItem>(oval, startAngle, sweepAngle);
}

void RecordingCanvas::DrawOval(const Rect& oval)
{
    cmdList_->AddOp<DrawOvalOpItem>(oval);
}

void RecordingCanvas::DrawCircle(const Point& centerPt, scalar radius)
{
    cmdList_->AddOp<DrawCircleOpItem>(centerPt, radius);
}

void RecordingCanvas::DrawPath(const Path& path)
{
    auto pathCmdList = static_cast<const RecordingPath&>(path).GetCmdList();
    if (pathCmdList == nullptr) {
        LOGE("RecordingCanvas::DrawPath, path is valid!");
        return;
    }
    int offset = cmdList_->AddCmdListData(pathCmdList->GetData());
    CmdListSiteInfo pathCmdListInfo(offset, pathCmdList->GetData().second);
    cmdList_->AddOp<DrawPathOpItem>(pathCmdListInfo);
}

void RecordingCanvas::DrawBackground(const Brush& brush)
{
    Filter filter = brush.GetFilter();

    BrushSiteInfo brushSiteInfo = {
        StoreColorSpace(brush.GetColorSpace().get(), cmdList_),
        StoreColorSpaceLargeObject(brush.GetColorSpace().get(), cmdList_),
        StoreShaderEffect(brush.GetShaderEffect().get(), cmdList_),
        StoreShaderEffectLargeObject(brush.GetShaderEffect().get(), cmdList_),
        StoreColorFilter(filter.GetColorFilter().get(), cmdList_),
        StoreImageFilter(filter.GetImageFilter().get(), cmdList_),
        StoreMaskFilter(filter.GetMaskFilter().get(), cmdList_)
    };

    cmdList_->AddOp<DrawBackgroundOpItem>(brush.GetColor(), brush.GetBlendMode(), brush.IsAntiAlias(),
        filter.GetFilterQuality(), brushSiteInfo);
}

void RecordingCanvas::DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos,
    scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
{
    auto pathCmdList = static_cast<const RecordingPath&>(path).GetCmdList();
    if (pathCmdList == nullptr) {
        LOGE("RecordingCanvas::DrawShadow, path is valid!");
        return;
    }
    int offset = cmdList_->AddCmdListData(pathCmdList->GetData());
    CmdListSiteInfo pathCmdListInfo(offset, pathCmdList->GetData().second);
    cmdList_->AddOp<DrawShadowOpItem>(
        planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag, pathCmdListInfo);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
