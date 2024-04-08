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
#include "test_base.h"

#include <sstream>
#include <string>

#include "test_common.h"
#include "image/image.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
void TestBase::SetCanvas(TestDisplayCanvas* canvas)
{
    playbackCanvas_ = canvas;
}
int TestBase::CreateBitmapCanvas()
{
    OHOS::Media::InitializationOptions opts;
    opts.size.width = width_;
    opts.size.height = height_;
    opts.editable = true;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = OHOS::Media::PixelMap::Create(opts);
    pixelMap_.reset(pixelMap.get());
    pixelMap.release();
    if (pixelMap_ == nullptr) {
        TestCommon::Log("failed to create pixelmap");
        return RET_FAILED;
    }
    Drawing::BitmapFormat format {Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE};
    bitmap_.Build(width_, height_, format);
    OHOS::Media::ImageInfo imageInfo;
    pixelMap_->GetImageInfo(imageInfo);
    Drawing::ImageInfo drawingImageInfo { imageInfo.size.width, imageInfo.size.height,
        TestCommon::PixelFormatToDrawingColorType(imageInfo.pixelFormat),
        TestCommon::AlphaTypeToDrawingAlphaType(imageInfo.alphaType),
        TestCommon::ColorSpaceToDrawingColorSpace(imageInfo.colorSpace) };
    bitmap_.SetInfo(drawingImageInfo);
    void* pixel = const_cast<void*>(reinterpret_cast<const void*>(pixelMap_->GetPixels()));
    if (pixel == nullptr) {
        TestCommon::Log("failed to GetPixels");
        return RET_FAILED;
    }
    bitmap_.SetPixels(pixel);
    bitmapCanvas_ = std::make_shared<Drawing::Canvas>();
    if (bitmapCanvas_ == nullptr) {
        TestCommon::Log("failed to create bitmapCanvas");
        return RET_FAILED;
    }
    bitmapCanvas_->Bind(bitmap_);
    bitmapCanvas_->DrawColor(background_);
    return RET_OK;
}
int TestBase::DiasplayToScreen()
{
    if (playbackCanvas_ == nullptr) {
        TestCommon::Log("playbackCanvas_ is null ");
        return RET_FAILED;
    }
    playbackCanvas_->DrawColor(0xff000000); //0xff000000 black
    playbackCanvas_->DrawBitmap(bitmap_, 0, 0);
    return RET_OK;
}
void TestBase::SetFileName(std::string fileName)
{
    fileName_ = fileName;
}
void TestBase::SetTestCount(uint32_t count)
{
    testCount_ = count;
}
void TestBase::TestFunctionCpu()
{
    if (CreateBitmapCanvas() != RET_OK) {
        return;
    }
    OnTestFunctionCpu(bitmapCanvas_.get());
    (void)DiasplayToScreen();
    (void)TestCommon::PackingPixmap(pixelMap_, fileName_);
}
void TestBase::TestPerformanceCpu()
{
    if (CreateBitmapCanvas() != RET_OK) {
        return;
    }
    LogStart();
    OnTestPerformanceCpu(bitmapCanvas_.get());
    LogEnd();
    (void)DiasplayToScreen();
}
void TestBase::TestFunctionGpuUpScreen()
{
    if (playbackCanvas_ == nullptr) {
        TestCommon::Log("playbackCanvas_ is null ");
        return;
    }
    OnTestFunctionGpuUpScreen(playbackCanvas_);
}
void TestBase::TestPerformanceGpuUpScreen()
{
    if (playbackCanvas_ == nullptr) {
        TestCommon::Log("playbackCanvas_ is null ");
        return;
    }
    LogStart();
    OnTestPerformanceGpuUpScreen(playbackCanvas_);
    LogEnd();
}
void TestBase::LogStart()
{
    timeStart_ = std::chrono::high_resolution_clock::now();
}
void TestBase::LogEnd()
{
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    int time = std::chrono::duration_cast<std::chrono::milliseconds>(end - timeStart_).count();
    std::ostringstream stream;
    stream << "DrawingApiTest TotalApiCallTime: [" << time << "]\n";
    stream << "DrawingApiTest TotalApiCallCount: [" << testCount_ << "]";
    TestCommon::Log(stream.str());
}
} // namespace Rosen
} // namespace OHOS