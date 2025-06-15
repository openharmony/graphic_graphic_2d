/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "rs_graphic_test_img.h"

#include "drawing/draw/core_canvas.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_property_modifier.h"
#include "utils/sampling_options.h"

#define WIDTH_INDEX 2
#define HEIGHT_INDEX 3

namespace OHOS {
namespace Rosen {

std::shared_ptr<Media::PixelMap> DecodePixelMap(const std::string& pathName, const Media::AllocatorType& allocatorType)
{
    uint32_t errCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(pathName, Media::SourceOptions(), errCode);
    if (imageSource == nullptr || errCode != 0) {
        std::cout << "imageSource : " << (imageSource != nullptr) << ", err:" << errCode << std::endl;
        return nullptr;
    }
    Media::DecodeOptions decodeOpt;
    decodeOpt.allocatorType = allocatorType;
    std::shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
    if (pixelmap == nullptr || errCode != 0) {
        std::cout << "pixelmap == nullptr, err:" << errCode << std::endl;
        return nullptr;
    }
    return pixelmap;
}

std::shared_ptr<Media::PixelMap> DecodePixelMap(
    const uint8_t* data, uint32_t size, const Media::AllocatorType& allocatorType)
{
    uint32_t errCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource =
        Media::ImageSource::CreateImageSource(data, size, Media::SourceOptions(), errCode);
    if (imageSource == nullptr || errCode != 0) {
        std::cout << "imageSource : " << (imageSource != nullptr) << ", err:" << errCode << std::endl;
        return nullptr;
    }
    Media::DecodeOptions decodeOpt;
    decodeOpt.allocatorType = allocatorType;
    std::shared_ptr<Media::PixelMap> pixelmap = imageSource->CreatePixelMap(decodeOpt, errCode);
    if (pixelmap == nullptr || errCode != 0) {
        std::cout << "pixelmap == nullptr, err:" << errCode << std::endl;
        return nullptr;
    }
    return pixelmap;
}

std::shared_ptr<Rosen::RSCanvasNode> SetUpNodeBgImage(const std::string& pathName, const Rosen::Vector4f bounds)
{
    std::shared_ptr<Media::PixelMap> pixelmap = DecodePixelMap(pathName, Media::AllocatorType::SHARE_MEM_ALLOC);
    auto image = std::make_shared<Rosen::RSImage>();
    image->SetPixelMap(pixelmap);
    image->SetImageFit((int)ImageFit::FILL);
    auto node = Rosen::RSCanvasNode::Create();
    node->SetBounds(bounds);
    node->SetBgImageSize(bounds[WIDTH_INDEX], bounds[HEIGHT_INDEX]);
    node->SetBgImage(image);
    return node;
}

std::shared_ptr<Rosen::RSCanvasNode> SetUpNodeBgImage(const uint8_t* data, uint32_t size, const Rosen::Vector4f bounds)
{
    std::shared_ptr<Media::PixelMap> pixelmap = DecodePixelMap(data, size, Media::AllocatorType::SHARE_MEM_ALLOC);
    auto image = std::make_shared<Rosen::RSImage>();
    image->SetPixelMap(pixelmap);
    image->SetImageFit((int)ImageFit::FILL);
    auto node = Rosen::RSCanvasNode::Create();
    node->SetBounds(bounds);
    node->SetBgImageSize(bounds[WIDTH_INDEX], bounds[HEIGHT_INDEX]);
    node->SetBgImage(image);
    return node;
}

void ImageCustomModifier::SetWidth(int32_t width)
{
    if (width_ == nullptr) {
        width_ = std::make_shared<RSProperty<int32_t>>(width);
        AttachProperty(width_);
    } else {
        width_->Set(width);
    }
}

void ImageCustomModifier::SetHeight(int32_t height)
{
    if (height_ == nullptr) {
        height_ = std::make_shared<RSProperty<int32_t>>(height);
        AttachProperty(height_);
    } else {
        height_->Set(height);
    }
}

void ImageCustomModifier::SetPixelMapPath(std::string pathName)
{
    if (pathName_ == nullptr) {
        pathName_ = std::make_shared<RSProperty<std::string>>(pathName);
        AttachProperty(pathName_);
    } else {
        pathName_->Set(pathName);
    }
}

void ImageCustomModifier::Draw(DrawingContext& context) const
{
    if (!width_ || !height_ || !pathName_) {
        std::cout << "Has nullptr, Draw none\n";
        Drawing::Rect rect;
        Drawing::Brush brush;
        context.canvas->AttachBrush(brush);
        context.canvas->DrawRect(rect);
        context.canvas->DetachBrush();
        return;
    }
    int32_t width = width_->Get();
    int32_t height = height_->Get();
    std::string pathName = pathName_->Get();
    auto pixelmap_ = DecodePixelMap(pathName, Media::AllocatorType::SHARE_MEM_ALLOC);
    if (pixelmap_ == nullptr) {
        std::cout << "***Test*** : pixelmap_ == nullptr\n";
        return;
    }
    auto image = std::make_shared<Rosen::RSImage>();
    image->SetPixelMap(pixelmap_);
    image->SetImageFit((int)ImageFit::FILL);
    image->CanvasDrawImage(*context.canvas, Drawing::Rect(0, 0, width, height), Drawing::SamplingOptions());
    return;
}
} // namespace Rosen
} // namespace OHOS