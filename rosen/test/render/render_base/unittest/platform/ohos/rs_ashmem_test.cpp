/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <sys/mman.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include <memory>

#include "ashmem.h"
#include "securec.h"
#include "rs_trace.h"
#include "sys_binder.h"
#include "sandbox_utils.h"

#include "transaction/rs_ashmem_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAshmemHelperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<AshmemAllocator> rsAshmemAllocator;
};

void RSAshmemHelperTest::SetUpTestCase() {}
void RSAshmemHelperTest::TearDownTestCase() {}
void RSAshmemHelperTest::SetUp()
{
    size_t size = 10;
    int fd = 10;
    rsAshmemAllocator = std::make_shared<AshmemAllocator>(fd, size);
}
void RSAshmemHelperTest::TearDown()
{
    rsAshmemAllocator.reset();
}

std::shared_ptr<MessageParcel> CreateMessageParcel()
{
    size_t offsetsSize = 2;
    Allocator* allocator = nullptr;
    auto dataParcel = std::make_shared<MessageParcel>(allocator);
    dataParcel->objectOffsets_ = new binder_size_t[offsetsSize];
    dataParcel->objectOffsets_[0] = 0;
    dataParcel->objectOffsets_[1] = sizeof(flat_binder_object);
    dataParcel->objectCursor_ = offsetsSize;
    dataParcel->objectsCapacity_ = offsetsSize;

    size_t dataSize = offsetsSize * sizeof(flat_binder_object);
    if (dataParcel->data_ != nullptr) {
        delete[] dataParcel->data_;
    }
    
    dataParcel->dataSize_ = dataSize;
    dataParcel->data_ = new uint8_t[dataSize];
    binder_size_t* object = reinterpret_cast<binder_size_t*>(dataParcel->GetObjectOffsets());
    uintptr_t parcelData = dataParcel->GetData();
    for (size_t pos = 0; pos < dataParcel->objectCursor_; pos++) {
        flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(parcelData + object[pos]);
        if (dataParcel->objectOffsets_[pos] != dataParcel->objectOffsets_[0]) {
            flat->hdr.type = BINDER_TYPE_FDR;
        } else {
            flat->hdr.type = BINDER_TYPE_FD;
        }
        flat->handle = static_cast<int>(pos);
    }
    return dataParcel;
}

/**
 * @tc.name: CreateAshmemAllocatorWithFdTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, CreateAshmemAllocatorWithFdTest001, TestSize.Level1)
{
    int fd1 = 0;
    size_t size1 = -10;
    auto mapType = PROT_READ | PROT_WRITE;
    auto res1 = AshmemAllocator::CreateAshmemAllocatorWithFd(fd1, size1, mapType);
    ASSERT_EQ(res1, nullptr);

    int fd2 = 10;
    size_t size2 = 10;
    auto res2 = AshmemAllocator::CreateAshmemAllocatorWithFd(fd2, size2, mapType);
    ASSERT_EQ(res2, nullptr);
}

/**
 * @tc.name: CreateAshmemAllocatorTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, CreateAshmemAllocatorTest001, TestSize.Level1)
{
    auto mapType = PROT_READ | PROT_WRITE;

    size_t size1 = -10;
    auto res = AshmemAllocator::CreateAshmemAllocator(size1, mapType);
    ASSERT_EQ(res, nullptr);

    size_t size2 = 10;
    res = AshmemAllocator::CreateAshmemAllocator(size2, mapType);
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name: WriteToAshmemTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, WriteToAshmemTest001, TestSize.Level1)
{
    int fd = 10;
    size_t size0 = 10;
    AshmemAllocator ashmemAllocator(fd, size0);
    ashmemAllocator.WriteToAshmem(nullptr, size0);
    ASSERT_FALSE(ashmemAllocator.WriteToAshmem(nullptr, size0));
}

/**
 * @tc.name: MapAshmemTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, MapAshmemTest001, TestSize.Level1)
{
    int fd = 10;
    size_t size = -10;
    auto mapType = PROT_READ;

    AshmemAllocator ashmemAllocator(fd, size);
    ashmemAllocator.MapAshmem(mapType);
    ASSERT_FALSE(ashmemAllocator.MapAshmem(mapType));
}

/**
 * @tc.name: CopyFromAshmemTest001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, CopyFromAshmemTest001, TestSize.Level1)
{
    int fd = 10;
    size_t size0 = 10;
    AshmemAllocator ashmemAllocator(fd, size0);

    // case 1
    size_t size1 = 0;
    ashmemAllocator.CopyFromAshmem(size1);
    ASSERT_EQ(ashmemAllocator.CopyFromAshmem(size1), nullptr);

    // case 2
    size_t size2 = 100;
    ashmemAllocator.CopyFromAshmem(size2);
    ASSERT_EQ(ashmemAllocator.CopyFromAshmem(size2), nullptr);
    
    // case 3
    size_t size3 = 200000001;
    ashmemAllocator.size_ = 200000002;
    ASSERT_EQ(ashmemAllocator.CopyFromAshmem(size3), nullptr);
}

/**
 * @tc.name: DeallocTest001
 * @tc.desc: Verify function Dealloc
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, DeallocTest001, TestSize.Level1)
{
    rsAshmemAllocator->data_ =
        ::mmap(nullptr, rsAshmemAllocator->size_, PROT_READ, MAP_SHARED, rsAshmemAllocator->fd_, 0);
    void* badData = ::mmap(nullptr, rsAshmemAllocator->size_, PROT_READ, MAP_SHARED, rsAshmemAllocator->fd_, 0);
    rsAshmemAllocator->Dealloc(nullptr);
    rsAshmemAllocator->Dealloc(badData);
    rsAshmemAllocator->Dealloc(rsAshmemAllocator->data_);
    EXPECT_EQ(rsAshmemAllocator->GetData(), nullptr);
    rsAshmemAllocator->data_ = nullptr;
    rsAshmemAllocator->fd_ = -2;
    rsAshmemAllocator->Dealloc(rsAshmemAllocator->data_);
    EXPECT_NE(rsAshmemAllocator->GetFd(), -1);
}

/**
 * @tc.name: ReallocTest001
 * @tc.desc: Verify function Realloc
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, ReallocTest001, TestSize.Level1)
{
    void* wrongData = ::mmap(nullptr, rsAshmemAllocator->size_, PROT_READ, MAP_SHARED, rsAshmemAllocator->fd_, 0);
    size_t newSize1 = 1;
    EXPECT_EQ(rsAshmemAllocator->Realloc(wrongData, newSize1), nullptr);
}

/**
 * @tc.name: InjectFileDescriptorTest001
 * @tc.desc: Verify function InjectFileDescriptor
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, InjectFileDescriptorTest001, TestSize.Level1)
{
    MessageParcel ashmemParcel;
    RSAshmemHelper rsAshmemHelper;
    auto dataParcel0 = CreateMessageParcel();
    rsAshmemHelper.InjectFileDescriptor(dataParcel0, &ashmemParcel);
    EXPECT_NE(dataParcel0->GetOffsetsSize(), 0);
}

/**
 * @tc.name: CopyFileDescriptorTest001
 * @tc.desc: Verify function CopyFileDescriptor
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, CopyFileDescriptorTest001, TestSize.Level1)
{
    MessageParcel ashmemParcel;
    RSAshmemHelper rsAshmemHelper;
    auto dataParcel0 = CreateMessageParcel();
    rsAshmemHelper.CopyFileDescriptor(&ashmemParcel, dataParcel0);
    EXPECT_NE(dataParcel0->GetOffsetsSize(), 0);
}

/**
 * @tc.name: ParseFromAshmemParcelTest001
 * @tc.desc: Verify function ParseFromAshmemParcel
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, ParseFromAshmemParcelTest001, TestSize.Level1)
{
    MessageParcel ashmemParcel;
    RSAshmemHelper rsAshmemHelper;
    EXPECT_EQ(rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel), nullptr);
}

/**
 * @tc.name: CreateAshmemParcelTest001
 * @tc.desc: Verify function CreateAshmemParcel
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, CreateAshmemParcelTest001, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    auto dataParcel = CreateMessageParcel();
    EXPECT_NE(rsAshmemHelper.CreateAshmemParcel(dataParcel), nullptr);
}
} // namespace Rosen
} // namespace OHOS