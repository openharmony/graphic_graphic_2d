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

#include "transaction/rs_ashmem_helper.h"
#include "ashmem.h"
#include "securec.h"
#include "sandbox_utils.h"
#include <memory>
#include <sys/mman.h>
#include <unistd.h>

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
};

void RSAshmemHelperTest::SetUpTestCase() {}
void RSAshmemHelperTest::TearDownTestCase() {}
void RSAshmemHelperTest::SetUp() {}
void RSAshmemHelperTest::TearDown() {}

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
} // namespace Rosen
} // namespace OHOS