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

#ifndef RECORDING_CANVAS_H
#define RECORDING_CANVAS_H

#include "draw/canvas.h"
#include "recording/draw_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
/*
 * @brief  RecordingCanvas is an empty canvas, which does not act on any surface,
 * and is used to record the sequence of draw calls for the canvas.
 * Draw calls are kept in linear memory in DrawCmdList, Subsequent playback can be performed through DrawCmdList.
 */
class RecordingCanvas : public Canvas {
public:
    RecordingCanvas(int width, int height);
    ~RecordingCanvas() override = default;

    std::shared_ptr<DrawCmdList> GetDrawCmdList() const;

    Type GetType() const override
    {
        return Type::RECORDING;
    }

    void DrawPoint(const Point& point) override;
    void DrawLine(const Point& startPt, const Point& endPt) override;
    void DrawRect(const Rect& rect) override;
    void DrawRoundRect(const RoundRect& roundRect) override;
    void DrawNestedRoundRect(const RoundRect& outer, const RoundRect& inner) override;
    void DrawArc(const Rect& oval, scalar startAngle, scalar sweepAngle) override;
    void DrawPie(const Rect& oval, scalar startAngle, scalar sweepAngle) override;
    void DrawOval(const Rect& oval) override;
    void DrawCircle(const Point& centerPt, scalar radius) override;
    void DrawPath(const Path& path) override;
    void DrawBackground(const Brush& brush) override;
    void DrawShadow(const Path& path, const Point3& planeParams, const Point3& devLightPos, scalar lightRadius,
        Color ambientColor, Color spotColor, ShadowFlags flag) override;

    void DrawBitmap(const Bitmap& bitmap, const scalar px, const scalar py) override;
    void DrawBitmap(Media::PixelMap& pixelMap, const scalar px, const scalar py) override;
    void DrawImage(const Image& image, const scalar px, const scalar py, const SamplingOptions& sampling) override;
    void DrawImageRect(const Image& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
        SrcRectConstraint constraint = SrcRectConstraint::STRICT_SRC_RECT_CONSTRAINT) override;
    void DrawImageRect(const Image& image, const Rect& dst, const SamplingOptions& sampling) override;
    void DrawPicture(const Picture& picture) override;

    void ClipRect(const Rect& rect, ClipOp op, bool doAntiAlias) override;
    void ClipRoundRect(const RoundRect& roundRect, ClipOp op, bool doAntiAlias) override;
    void ClipPath(const Path& path, ClipOp op, bool doAntiAlias) override;

    void SetMatrix(const Matrix& matrix) override;
    void ResetMatrix() override;
    void ConcatMatrix(const Matrix& matrix) override;
    void Translate(scalar dx, scalar dy) override;
    void Scale(scalar sx, scalar sy) override;
    void Rotate(scalar deg, scalar sx, scalar sy) override;
    void Shear(scalar sx, scalar sy) override;

    void Flush() override;
    void Clear(ColorQuad color) override;
    void Save() override;
    void SaveLayer(const SaveLayerRec& saveLayerRec) override;
    void Restore() override;

    CoreCanvas& AttachPen(const Pen& pen) override;
    CoreCanvas& AttachBrush(const Brush& brush) override;
    CoreCanvas& DetachPen() override;
    CoreCanvas& DetachBrush() override;

private:
    std::shared_ptr<DrawCmdList> cmdList_ = nullptr;
    int saveCount_ = 0;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
