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

#include <sys/mman.h>

#include "gtest/gtest.h"
#include "draw/canvas.h"
#include "draw/surface.h"
#include "image/image.h"
#include "image/image_info.h"
#include "pixel_map.h"

#include "transaction/rs_ashmem_helper.h"
#include "transaction/rs_marshalling_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSAshmemUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAshmemUnitTest::SetUpTestCase() {}
void RSAshmemUnitTest::TearDownTestCase() {}
void RSAshmemUnitTest::SetUp() {}
void RSAshmemUnitTest::TearDown() {}

/**
 * @tc.name: CreateAshmemAllocator001
 * @tc.desc: test results of create ashmemAllocator_
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSAshmemUnitTest, CreateAshmemAllocator001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create ashmemAllocator_
     */
    size_t size = 1024;
    auto ashmemAllocator_ = ashmemAllocator_::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ashmemAllocator_ != nullptr);
    ASSERT_FALSE(ashmemAllocator_->GetFd() < 0);
    ASSERT_EQ(ashmemAllocator_->GetSize(), size);
    ASSERT_TRUE(ashmemAllocator_->GetData() != nullptr);
}

/**
 * @tc.name: CreateAshmemAllocator002
 * @tc.desc: test results of create ashmemAllocator_
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSAshmemUnitTest, CreateAshmemAllocator002, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create ashmemAllocator_
     */
    size_t size = 1024;
    int fd = AshmemCreate("RSAshmemUnitTest002", size);
    auto ashmemAllocator_ = ashmemAllocator_::CreateAshmemAllocatorWithFd(fd, size, PROT_READ);
    ASSERT_TRUE(ashmemAllocator_ != nullptr);
    ASSERT_FALSE(ashmemAllocator_->GetFd() < 0);
    ASSERT_EQ(ashmemAllocator_->GetFd(), fd);
    ASSERT_EQ(ashmemAllocator_->GetSize(), size);
    ASSERT_TRUE(ashmemAllocator_->GetData() != nullptr);
}

/**
 * @tc.name: AshmemAllocatorDealloc001
 * @tc.desc: test results of Dealloc ashmemAllocator_
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSAshmemUnitTest, AshmemAllocatorDealloc001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create ashmemAllocator_
     */
    size_t size = 1024;
    auto ashmemAllocator_ = ashmemAllocator_::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ashmemAllocator_ != nullptr);

    /**
     * @tc.steps: step2. Dealloc ashmemAllocator_
     */
    ashmemAllocator_->Dealloc(ashmemAllocator_->GetData());
    ASSERT_EQ(ashmemAllocator_->GetFd(), -1);
    ASSERT_TRUE(ashmemAllocator_->GetData() == nullptr);
}

/**
 * @tc.name: AshmemAllocatorWriteAndCopy001
 * @tc.desc: test results of ashmemAllocator_ write and copy
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSAshmemUnitTest, AshmemAllocatorWriteAndCopy001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. init array
     */
    int size = 512;
    int arr[size];
    for (int i = 0; i < size; ++i) {
        arr[i] = i;
    }

    /**
     * @tc.steps: step2. create ashmemAllocator_
     */
    size_t arrBytes = sizeof(arr);
    auto ashmemAllocator_ = ashmemAllocator_::CreateAshmemAllocator(arrBytes, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ashmemAllocator_ != nullptr);

    /**
     * @tc.steps: step3. write to ashmemAllocator_
     */
    ASSERT_FALSE(ashmemAllocator_->WriteToAshmem(nullptr, arrBytes));
    ASSERT_FALSE(ashmemAllocator_->WriteToAshmem(arr, arrBytes + 5));
    ashmemAllocator_->WriteToAshmem(arr, arrBytes);
    ASSERT_EQ(memcmp(ashmemAllocator_->GetData(), arr, arrBytes), 0);

    /**
     * @tc.steps: step4. read from ashmemAllocator_
     */
    void* copyData = ashmemAllocator_->CopyFromAshmem(arrBytes);
    ASSERT_EQ(memcmp(copyData, arr, arrBytes), 0);
    free(copyData);
}

static std::shared_ptr<Drawing::Image> CreateDrawingImage(int width, int height)
{
    int start = 4;
    int end = 2;
    const Drawing::ImageInfo info =
        Drawing::ImageInfo(width, height, Drawing::COLORTYPE_N32, Drawing::ALPHATYPE_OPAQUE);
    auto surface(Drawing::Surface::MakeRaster(info));
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(width / start, height / start, width / end, height / end));
    canvas->DetachBrush();
    return surface->GetImageSnapshot();
}

/**
 * @tc.name: SkImageAshmem001
 * @tc.desc: test results of ashmem serialization of SkImage
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSAshmemUnitTest, SkImageAshmem001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create SkImage
     */
    int width = 200;
    int height = 300;
    int pixelBytes = 4;
    auto drawingImage = CreateDrawingImage(width, height);
    ASSERT_TRUE(drawingImage != nullptr);

    /**
     * @tc.steps: step2. serialize ashmem
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, drawingImage));
    ASSERT_TRUE(parcel.GetOffsetsSize() > 0);
    ASSERT_TRUE(parcel.GetDataSize() > 0);
    ASSERT_TRUE((int)parcel.GetDataSize() < width * height * pixelBytes);
}

static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height)
{
    int start = 4;
    int end = 2;
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    auto pixelmap = Media::PixelMap::Create(opts);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    if (address == nullptr) {
        return nullptr;
    }
    const Drawing::ImageInfo info =
        Drawing::ImageInfo(width, height, Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_OPAQUE);
    auto surface(Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes()));
    auto canvas = surface->GetCanvas();
    canvas->Clear(Drawing::Color::COLOR_YELLOW);
    Drawing::Brush brush;
    brush.SetColor(Drawing::Color::COLOR_RED);
    canvas->AttachBrush(brush);
    canvas->DrawRect(Drawing::Rect(width / start, height / start, width / end, height / end));
    canvas->DetachBrush();
    return pixelmap;
}

/**
 * @tc.name: PixelMapAshmem001
 * @tc.desc: test results of ashmem serialization of PixelMap
 * @tc.type:FUNC
 * @tc.require: issueI5HRIF
 */
HWTEST_F(RSAshmemUnitTest, PixelMapAshmem001, Function | MediumTest | Level2)
{
    /**
     * @tc.steps: step1. create PixelMap
     */
    int width = 200;
    int height = 300;
    int pixelBytes = 4;
    auto pixelMap = CreatePixelMap(width, height);
    ASSERT_TRUE(pixelMap != nullptr);

    /**
     * @tc.steps: step2. serialize ashmem
     */
    MessageParcel parcel;
    ASSERT_TRUE(RSMarshallingHelper::Marshalling(parcel, pixelMap));
    ASSERT_TRUE(parcel.GetOffsetsSize() > 0);
    ASSERT_TRUE(parcel.GetDataSize() > 0);
    ASSERT_TRUE((int)parcel.GetDataSize() < width * height * pixelBytes);
}

/**
 * @tc.name: CreateAshmemParcel001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemUnitTest, CreateAshmemParcel001, Function | MediumTest | Level2)
{
    auto dataParcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(RSAshmemHelper::CreateAshmemParcel(dataParcel) == nullptr);
    dataParcel->WriteInt32(0);
    dataParcel->WriteBool(true);
    ASSERT_TRUE(RSAshmemHelper::CreateAshmemParcel(dataParcel) != nullptr);
    size_t size = 1024;
    auto ashmemAllocator_ = ashmemAllocator_::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    int fd = ashmemAllocator_->GetFd();
    ASSERT_TRUE(fd > 0);
    dataParcel->WriteFileDescriptor(fd);
    ASSERT_TRUE(RSAshmemHelper::CreateAshmemParcel(dataParcel) != nullptr);
}

/**
 * @tc.name: ParseFromAshmemParcel001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemUnitTest, ParseFromAshmemParcel001, Function | MediumTest | Level2)
{
    MessageParcel ashmemParcel;
    ASSERT_TRUE(RSAshmemHelper::ParseFromAshmemParcel(&ashmemParcel) == nullptr);
    ashmemParcel.WriteInt32(0);
    ashmemParcel.WriteBool(true);
    size_t size = 1024;
    auto ashmemAllocator_ = ashmemAllocator_::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    int fd = ashmemAllocator_->GetFd();
    std::cout << fd << std::endl;
    ASSERT_TRUE(fd > 0);
    ASSERT_TRUE(ashmemParcel.WriteFileDescriptor(fd));
    ASSERT_TRUE(RSAshmemHelper::ParseFromAshmemParcel(&ashmemParcel) == nullptr);
}
} // namespace OHOS::Rosen
