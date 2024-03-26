/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "render/rs_mask.h"

#include "include/core/SkPictureRecorder.h"
#include "recording/recording_handle.h"
#include "draw/pen.h"

#include "recording/mask_cmd_list.h"
#include "recording/cmd_list_helper.h"
#include "effect/shader_effect.h"
#include "platform/common/rs_log.h"
#include "render/rs_pixel_map_util.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSMask> RSMask::CreateGradientMask(const Drawing::Brush& maskBrush)
{
    auto mask = std::make_shared<RSMask>();
    if (mask) {
        mask->SetMaskBrush(maskBrush);
        mask->SetMaskType(MaskType::GRADIENT);
    }
    return mask;
}

std::shared_ptr<RSMask> RSMask::CreatePathMask(const Drawing::Path& maskPath, const Drawing::Brush& maskBrush)
{
    auto mask = std::make_shared<RSMask>();
    if (mask) {
        mask->SetMaskPath(maskPath);
        mask->SetMaskBrush(maskBrush);
        mask->SetMaskType(MaskType::PATH);
    }
    return mask;
}

std::shared_ptr<RSMask> RSMask::CreatePathMask(
    const Drawing::Path& maskPath, const Drawing::Pen& maskPen, const Drawing::Brush& maskBrush)
{
    auto mask = std::make_shared<RSMask>();
    if (mask) {
        mask->SetMaskPath(maskPath);
        mask->SetMaskPen(maskPen);
        mask->SetMaskBrush(maskBrush);
        mask->SetMaskType(MaskType::PATH);
    }
    return mask;
}

std::shared_ptr<RSMask> RSMask::CreateSVGMask(double x, double y, double scaleX, double scaleY,
    const sk_sp<SkSVGDOM>& svgDom)
{
    auto mask = std::make_shared<RSMask>();
    if (mask) {
        mask->SetSvgX(x);
        mask->SetSvgY(y);
        mask->SetScaleX(scaleX);
        mask->SetScaleY(scaleY);
        mask->SetSvgDom(svgDom);
        mask->SetMaskType(MaskType::SVG);
    }
    return mask;
}

std::shared_ptr<RSMask> RSMask::CreatePixelMapMask(const std::shared_ptr<Media::PixelMap> pixelMap)
{
    auto mask = std::make_shared<RSMask>();
    if (mask) {
        mask->SetPixelMap(pixelMap);
        mask->SetMaskType(MaskType::PIXEL_MAP);
    }
    return mask;
}

RSMask::RSMask()
{
    maskPath_ = std::make_shared<Drawing::Path>();
}

RSMask::~RSMask()
{
}

void RSMask::SetSvgX(double x)
{
    svgX_ = x;
}

double RSMask::GetSvgX() const
{
    return svgX_;
}

void RSMask::SetSvgY(double y)
{
    svgY_ = y;
}

double RSMask::GetSvgY() const
{
    return svgY_;
}

void RSMask::SetScaleX(double scaleX)
{
    scaleX_ = scaleX;
}

double RSMask::GetScaleX() const
{
    return scaleX_;
}

void RSMask::SetScaleY(double scaleY)
{
    scaleY_ = scaleY;
}

double RSMask::GetScaleY() const
{
    return scaleY_;
}

void RSMask::SetMaskPath(const Drawing::Path& path)
{
    maskPath_ = std::make_shared<Drawing::Path>(path);
}

std::shared_ptr<Drawing::Path> RSMask::GetMaskPath() const
{
    return maskPath_;
}

void RSMask::SetMaskPen(const Drawing::Pen& pen)
{
    maskPen_ = pen;
}

Drawing::Pen RSMask::GetMaskPen() const
{
    return maskPen_;
}

void RSMask::SetMaskBrush(const Drawing::Brush& brush)
{
    maskBrush_ = brush;
}

Drawing::Brush RSMask::GetMaskBrush() const
{
    return maskBrush_;
}

void RSMask::SetSvgDom(const sk_sp<SkSVGDOM>& svgDom)
{
    svgDom_ = svgDom;
}

sk_sp<SkSVGDOM> RSMask::GetSvgDom() const
{
    return svgDom_;
}

std::shared_ptr<Drawing::Picture> RSMask::GetSvgPicture() const
{
    return svgPicture_;
}

void RSMask::SetMaskType(MaskType type)
{
    type_ = type;
}

void RSMask::SetPixelMap(const std::shared_ptr<Media::PixelMap> pixelMap)
{
    pixelMap_ = pixelMap;
    image_ = RSPixelMapUtil::ExtractDrawingImage(pixelMap_);
}

std::shared_ptr<Media::PixelMap> RSMask::GetPixelMap() const
{
    return pixelMap_;
}

std::shared_ptr<Drawing::Image> RSMask::GetImage() const
{
    return image_;
}

bool RSMask::IsSvgMask() const
{
    return (type_ == MaskType::SVG);
}

bool RSMask::IsGradientMask() const
{
    return (type_ == MaskType::GRADIENT);
}

bool RSMask::IsPixelMapMask() const
{
    return (type_ == MaskType::PIXEL_MAP);
}

bool RSMask::IsPathMask() const
{
    return (type_ == MaskType::PATH);
}

#ifdef ROSEN_OHOS
bool RSMask::Marshalling(Parcel& parcel) const
{
    if (!(RSMarshallingHelper::Marshalling(parcel, type_) &&
            RSMarshallingHelper::Marshalling(parcel, svgX_) &&
            RSMarshallingHelper::Marshalling(parcel, svgY_) &&
            RSMarshallingHelper::Marshalling(parcel, scaleX_) &&
            RSMarshallingHelper::Marshalling(parcel, scaleY_))) {
        ROSEN_LOGE("RSMask::Marshalling failed!");
        return false;
    }

    if (!MarshallingPathAndBrush(parcel)) {
        ROSEN_LOGE("RSMask::Marshalling failed!");
        return false;
    }

    if (IsSvgMask()) {
        ROSEN_LOGD("SVG RSMask::Marshalling");
        SkPictureRecorder recorder;
        SkCanvas* recordingCanvas = recorder.beginRecording(SkRect::MakeSize(svgDom_->containerSize()));
        svgDom_->render(recordingCanvas);
        sk_sp<SkPicture> picture = recorder.finishRecordingAsPicture();
        if (!RSMarshallingHelper::Marshalling(parcel, picture)) {
            ROSEN_LOGE("RSMask::Marshalling SkPicture failed!");
            return false;
        }
    }

    if (IsPixelMapMask() && !RSMarshallingHelper::Marshalling(parcel, pixelMap_)) {
        ROSEN_LOGE("RSMask::Marshalling Pixelmap failed!");
        return false;
    }
    return true;
}

bool RSMask::MarshallingPathAndBrush(Parcel& parcel) const
{
    Drawing::CmdListData listData;
    auto maskCmdList = Drawing::MaskCmdList::CreateFromData(listData, true);
    Drawing::Filter filter = maskBrush_.GetFilter();
    Drawing::BrushHandle brushHandle = {
        maskBrush_.GetColor(),
        maskBrush_.GetBlendMode(),
        maskBrush_.IsAntiAlias(),
        filter.GetFilterQuality(),
        Drawing::CmdListHelper::AddColorSpaceToCmdList(*maskCmdList,
            maskBrush_.GetColorSpace()),
        Drawing::CmdListHelper::AddShaderEffectToCmdList(*maskCmdList,
            maskBrush_.GetShaderEffect()),
        Drawing::CmdListHelper::AddColorFilterToCmdList(*maskCmdList,
            filter.GetColorFilter()),
        Drawing::CmdListHelper::AddImageFilterToCmdList(*maskCmdList,
            filter.GetImageFilter()),
        Drawing::CmdListHelper::AddMaskFilterToCmdList(*maskCmdList,
            filter.GetMaskFilter()),
    };
    maskCmdList->AddOp<Drawing::MaskBrushOpItem>(brushHandle);

    Drawing::PenHandle penHandle = {
        maskPen_.GetWidth(),
        maskPen_.GetMiterLimit(),
        maskPen_.GetCapStyle(),
        maskPen_.GetJoinStyle(),
        Drawing::CmdListHelper::AddPathEffectToCmdList(*maskCmdList,
            maskPen_.GetPathEffect()),
        maskPen_.GetColor(),
    };
    maskCmdList->AddOp<Drawing::MaskPenOpItem>(penHandle);

    auto pathHandle = Drawing::CmdListHelper::AddPathToCmdList(*maskCmdList, *maskPath_);
    maskCmdList->AddOp<Drawing::MaskPathOpItem>(pathHandle);

    if (!RSMarshallingHelper::Marshalling(parcel, maskCmdList)) {
        ROSEN_LOGE("RSMask::MarshallingPathAndBrush failed!");
        return false;
    }
    return true;
}

RSMask* RSMask::Unmarshalling(Parcel& parcel)
{
    auto rsMask = std::make_unique<RSMask>();
    if (!(RSMarshallingHelper::Unmarshalling(parcel, rsMask->type_) &&
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->svgX_) &&
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->svgY_) &&
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->scaleX_) &&
            RSMarshallingHelper::Unmarshalling(parcel, rsMask->scaleY_))) {
        ROSEN_LOGE("RSMask::Unmarshalling failed!");
        return nullptr;
    }

    std::shared_ptr<Drawing::MaskCmdList> maskCmdList = nullptr;
    if (!RSMarshallingHelper::Unmarshalling(parcel, maskCmdList)) {
        ROSEN_LOGE("RSMask::Unmarshalling failed!");
        return nullptr;
    }
    if (maskCmdList) {
        maskCmdList->Playback(rsMask->maskPath_, rsMask->maskPen_, rsMask->maskBrush_);
    }

    if (rsMask->IsSvgMask()) {
        ROSEN_LOGD("SVG RSMask::Unmarshalling");
        if (!RSMarshallingHelper::Unmarshalling(parcel, rsMask->svgPicture_)) {
            ROSEN_LOGE("RSMask::Unmarshalling SkPicture failed!");
            return nullptr;
        }
    }

    if (rsMask->IsPixelMapMask() && !RSMarshallingHelper::Unmarshalling(parcel, rsMask->pixelMap_)) {
        ROSEN_LOGE("RSMask::Unmarshalling pixelmap failed!");
        return nullptr;
    }

    if (!rsMask->image_ && rsMask->pixelMap_) {
        rsMask->image_ = RSPixelMapUtil::ExtractDrawingImage(rsMask->pixelMap_);
    }
    return rsMask.release();
}
#endif
} // namespace Rosen
} // namespace OHOS