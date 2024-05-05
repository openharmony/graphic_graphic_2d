/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <memory>
#include <sys/mman.h>
#include <unistd.h>

#include "ashmem.h"
#include "sandbox_utils.h"
#include "securec.h"

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

/**
 * @tc.name: CreateAshmemAllocatorTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, CreateAshmemAllocatorTest, TestSize.Level1)
{
    size_t size = -10;
    auto mapType = PROT_READ | PROT_WRITE;
    auto res = AshmemAllocator::CreateAshmemAllocator(size, mapType);
    ASSERT_EQ(res, nullptr);
    size = 10;
    res = AshmemAllocator::CreateAshmemAllocator(size, mapType);
    ASSERT_NE(res, nullptr);
}

/**
 * @tc.name: CreateAshmemAllocatorWithFdTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, CreateAshmemAllocatorWithFdTest, TestSize.Level1)
{
    size_t size1 = -10;
    auto mapType = PROT_READ | PROT_WRITE;
    int fd1 = 0;
    auto res = AshmemAllocator::CreateAshmemAllocatorWithFd(fd1, size1, mapType);
    ASSERT_EQ(res, nullptr);

    int fd2 = 10;
    size_t size2 = 10;
    auto res02 = AshmemAllocator::CreateAshmemAllocatorWithFd(fd2, size2, mapType);
    ASSERT_EQ(res02, nullptr);
}

/**
 * @tc.name: MapAshmemTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, MapAshmemTest, TestSize.Level1)
{
    size_t size = -10;
    auto mapType = PROT_READ;
    int fd = 10;
    AshmemAllocator ashmemAllocator(fd, size);
    ashmemAllocator.MapAshmem(mapType);
    ASSERT_FALSE(ashmemAllocator.MapAshmem(mapType));
}

/**
 * @tc.name: WriteToAshmemTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, WriteToAshmemTest, TestSize.Level1)
{
    size_t size = 10;
    int fd = 10;
    AshmemAllocator ashmemAllocator(fd, size);
    ashmemAllocator.WriteToAshmem(nullptr, size);
    ASSERT_FALSE(ashmemAllocator.WriteToAshmem(nullptr, size));
}

/**
 * @tc.name: CopyFromAshmemTest
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, CopyFromAshmemTest, TestSize.Level1)
{
    size_t size_ = 10;
    int fd = 10;
    AshmemAllocator ashmemAllocator(fd, size_);
    size_t size1 = 100;
    ashmemAllocator.CopyFromAshmem(size1);
    ASSERT_EQ(ashmemAllocator.CopyFromAshmem(size1), nullptr);

    size_t size2 = 0;
    ashmemAllocator.CopyFromAshmem(size2);
    ASSERT_EQ(ashmemAllocator.CopyFromAshmem(size2), nullptr);
}

/**
 * @tc.name: GetFdTest
 * @tc.desc: Verify function GetFd
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, GetFdTest, TestSize.Level1)
{
    EXPECT_EQ(rsAshmemAllocator->GetFd(), 10);
}

/**
 * @tc.name: GetSizeTest
 * @tc.desc: Verify function GetSize
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, GetSizeTest, TestSize.Level1)
{
    EXPECT_EQ(rsAshmemAllocator->GetSize(), 10);
}

/**
 * @tc.name: GetDataTest
 * @tc.desc: Verify function GetData
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, GetDataTest, TestSize.Level1)
{
    EXPECT_EQ(rsAshmemAllocator->GetData(), nullptr);
}

/**
 * @tc.name: DeallocTest
 * @tc.desc: Verify function Dealloc
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, DeallocTest, TestSize.Level1)
{
    rsAshmemAllocator->data_ =
        ::mmap(nullptr, rsAshmemAllocator->size_, PROT_READ, MAP_SHARED, rsAshmemAllocator->fd_, 0);
    void* wrongData = ::mmap(nullptr, rsAshmemAllocator->size_, PROT_READ, MAP_SHARED, rsAshmemAllocator->fd_, 0);
    rsAshmemAllocator->Dealloc(wrongData);
    rsAshmemAllocator->Dealloc(rsAshmemAllocator->data_);
    EXPECT_EQ(rsAshmemAllocator->GetData(), nullptr);
}

/**
 * @tc.name: AllocTest
 * @tc.desc: Verify function Alloc
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, AllocTest, TestSize.Level1)
{
    size_t size = 1;
    EXPECT_EQ(rsAshmemAllocator->Alloc(size), nullptr);
}

/**
 * @tc.name: ReallocTest
 * @tc.desc: Verify function Realloc
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, ReallocTest, TestSize.Level1)
{
    size_t newSize = 1;
    void* wrongData = ::mmap(nullptr, rsAshmemAllocator->size_, PROT_READ, MAP_SHARED, rsAshmemAllocator->fd_, 0);
    EXPECT_EQ(rsAshmemAllocator->Realloc(wrongData, newSize), nullptr);
}

/**
 * @tc.name: CopyFileDescriptorTest
 * @tc.desc: Verify function CopyFileDescriptor
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, CopyFileDescriptorTest, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    MessageParcel ashmemParcel;
    auto dataParcel = std::make_shared<MessageParcel>();
    rsAshmemHelper.CopyFileDescriptor(&ashmemParcel, dataParcel);
    EXPECT_EQ(dataParcel->GetOffsetsSize(), 0);
}

/**
 * @tc.name: InjectFileDescriptorTest
 * @tc.desc: Verify function InjectFileDescriptor
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, InjectFileDescriptorTest, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    MessageParcel ashmemParcel;
    auto dataParcel = std::make_shared<MessageParcel>();
    rsAshmemHelper.InjectFileDescriptor(dataParcel, &ashmemParcel);
    EXPECT_EQ(dataParcel->GetData(), 0);
}

/**
 * @tc.name: CreateAshmemParcelTest
 * @tc.desc: Verify function CreateAshmemParcel
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, CreateAshmemParcelTest, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    auto dataParcel = std::make_shared<MessageParcel>();
    EXPECT_EQ(rsAshmemHelper.CreateAshmemParcel(dataParcel), nullptr);
}

/**
 * @tc.name: ParseFromAshmemParcelTest
 * @tc.desc: Verify function ParseFromAshmemParcel
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, ParseFromAshmemParcelTest, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    MessageParcel ashmemParcel;
    EXPECT_EQ(rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel), nullptr);
}
} // namespace Rosen
} // namespace OHOS