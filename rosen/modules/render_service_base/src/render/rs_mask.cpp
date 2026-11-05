/*
 * Copyright (c) 2022-2026 Huawei Device Co., Ltd.
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

#include <securec.h>

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

void RSMask::Dump(std::string& out) const
{
    out += "[maskType:" + std::to_string(static_cast<int>(type_));
    out += " x:" + std::to_string(svgX_) + " y:" + std::to_string(svgY_);
    out += " scaleX:" + std::to_string(scaleX_) + " scaleY:" + std::to_string(scaleY_);
    out += " pen";
    maskPen_.Dump(out);
    out += " brush";
    maskBrush_.Dump(out);
    out += " path[";
    if (maskPath_ != nullptr) {
        out += "isValid:" + std::to_string(maskPath_->IsValid());
        auto bounds = maskPath_->GetBounds();
        out += " bounds[top:" + std::to_string(bounds.GetTop()) + " bottom:" + std::to_string(bounds.GetBottom());
        out += " left:" + std::to_string(bounds.GetLeft()) + " right:" + std::to_string(bounds.GetRight()) + "]";
        out += " drawingType:" + std::to_string(static_cast<int>(maskPath_->GetDrawingType()));
    }
    out += "] pixelMap[";
    if (pixelMap_ != nullptr) {
        out += " width:" + std::to_string(pixelMap_->GetWidth());
        out += " height:" + std::to_string(pixelMap_->GetHeight());
        out += " byteCount:" + std::to_string(pixelMap_->GetByteCount());
    }
    out += ']';
    if (image_ != nullptr) {
        out += " image";
        image_->Dump(out);
    }
    out += ']';
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

    if (IsPixelMapMask() && !RSMarshallingHelper::Marshalling(parcel, pixelMap_)) {
        ROSEN_LOGE("RSMask::Marshalling Pixelmap failed!");
        return false;
    }
    return true;
}

// Zero the vtable pointer at offset 0 of each OpItem in the arena before cross-process
// marshalling. OpItem is polymorphic, so placement new writes a vptr into every object;
// playback dispatches on the serialized type_ field and never dereferences the vptr, so it
// is dead weight in the wire format and leaking it discloses the lib2d_graphics module
// base address of the sending process.
bool RSMask::SanitizeArenaVptrs(const Drawing::CmdListData& cmdListData) const
{
    constexpr size_t VPTR_SIZE = sizeof(void*);
    // Sole ABI assumption: the Itanium C++ ABI puts the vptr of a single-inheritance polymorphic
    // class at offset 0 regardless of member order. Keep OpItem polymorphic and single-inheritance.

    const void* data = cmdListData.first;
    size_t size = cmdListData.second;
    if (size == 0) {
        return true;
    }
    if (data == nullptr) {
        return false;
    }
    if (size < sizeof(Drawing::OpItem)) {
        return true; // too small to hold any OpItem, so no vptr can be present
    }
    char* arena = const_cast<char*>(static_cast<const char*>(data));
    size_t offset = 0;
    // Each non-zero link advances offset by at least sizeof(OpItem) and keeps it bounded by
    // size, so the loop terminates without a counter.
    do {
        // Arena objects are genuinely constructed by MemAllocator::Allocate, so member access
        // via OpItem* is well-defined; the compiler computes field offsets the same way the
        // playback side does, instead of hard-coding them here.
        auto* op = reinterpret_cast<const Drawing::OpItem*>(arena + offset);
        size_t nextOffset = op->GetNextOpItemOffset();
        if (memset_s(arena + offset, size - offset, 0, VPTR_SIZE) != EOK) {
            return false;
        }
        // nextOffset == 0 is the chain-end signal and must bypass this validation. The upper
        // bound of a non-zero link must be validated with subtraction: a garbage-huge nextOffset
        // would wrap the addition in "nextOffset + sizeof(OpItem) <= size" and pass it, causing
        // an out-of-bounds access, while "size - sizeof(OpItem)" cannot underflow because the
        // size >= sizeof(OpItem) guard holds here. A corrupted chain (backward, overlapping or
        // out-of-range) fails closed so no unscrubbed byte is sent.
        if (nextOffset != 0 &&
            (nextOffset < offset + sizeof(Drawing::OpItem) || nextOffset > size - sizeof(Drawing::OpItem))) {
            return false;
        }
        offset = nextOffset;
    } while (offset != 0); // 0 means the end of the op chain
    return true;
}

bool RSMask::MarshallingPathAndBrush(Parcel& parcel) const
{
    Drawing::CmdListData listData;
    auto maskCmdList = Drawing::MaskCmdList::CreateFromData(listData, true);
    Drawing::Filter filter = maskBrush_.GetFilter();
    Drawing::BrushHandle brushHandle;
    if (maskBrush_.HasUIColor()) {
        brushHandle.uiColor = maskBrush_.GetUIColor();
    } else {
        brushHandle.color = maskBrush_.GetColor();
    }
    brushHandle.SetIsUIColor(maskBrush_.HasUIColor());
    brushHandle.mode = maskBrush_.GetBlendMode();
    brushHandle.isAntiAlias = maskBrush_.IsAntiAlias();
    brushHandle.SetBlenderEnabled(maskBrush_.GetBlenderEnabled());
    brushHandle.filterQuality = filter.GetFilterQuality();
    brushHandle.colorSpaceHandle = Drawing::CmdListHelper::AddColorSpaceToCmdList(*maskCmdList,
        maskBrush_.GetColorSpace());
    brushHandle.shaderEffectHandle = Drawing::CmdListHelper::AddShaderEffectToCmdList(*maskCmdList,
        maskBrush_.GetShaderEffect());
    brushHandle.blenderHandle = Drawing::CmdListHelper::AddBlenderToCmdList(*maskCmdList, maskBrush_.GetBlender());
    brushHandle.colorFilterHandle = Drawing::CmdListHelper::AddColorFilterToCmdList(*maskCmdList,
        filter.GetColorFilter());
    brushHandle.imageFilterHandle = Drawing::CmdListHelper::AddImageFilterToCmdList(*maskCmdList,
        filter.GetImageFilter());
    brushHandle.maskFilterHandle = Drawing::CmdListHelper::AddMaskFilterToCmdList(*maskCmdList, filter.GetMaskFilter());

    maskCmdList->AddOp<Drawing::MaskBrushOpItem>(brushHandle);

    Drawing::PenHandle penHandle;
    penHandle.width = maskPen_.GetWidth();
    penHandle.miterLimit = maskPen_.GetMiterLimit();
    penHandle.SetCapStyle(maskPen_.GetCapStyle());
    penHandle.joinStyle = maskPen_.GetJoinStyle();
    penHandle.pathEffectHandle = Drawing::CmdListHelper::AddPathEffectToCmdList(*maskCmdList, maskPen_.GetPathEffect());
    if (maskPen_.HasUIColor()) {
        penHandle.uiColor = maskPen_.GetUIColor();
    } else {
        penHandle.color = maskPen_.GetColor();
    }
    penHandle.SetIsUIColor(maskPen_.HasUIColor());
    maskCmdList->AddOp<Drawing::MaskPenOpItem>(penHandle);

    auto pathHandle = Drawing::CmdListHelper::AddPathToCmdList(*maskCmdList, *maskPath_);
    maskCmdList->AddOp<Drawing::MaskPathOpItem>(pathHandle);

    // Strip vptrs from the arena before it leaves the process (see SanitizeArenaVptrs);
    // a sanitize failure is pinpointed by SanitizeArenaVptrs's own logs.
    if (!SanitizeArenaVptrs(maskCmdList->GetData()) || !RSMarshallingHelper::Marshalling(parcel, maskCmdList)) {
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