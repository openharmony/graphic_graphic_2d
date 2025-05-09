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

#include <climits>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sys/mman.h>
#include <unistd.h>

#include "ashmem.h"
#include "gtest/hwext/gtest-tag.h"
#include "ipc_file_descriptor.h"
#include "parameters.h"
#include "rs_trace.h"
#include "sandbox_utils.h"
#include "securec.h"
#include "sys_binder.h"

#include "transaction/rs_ashmem_helper.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSAshmemHelperTest : public testing::Test {
    static bool profilerEnabled_;

public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void CheckReadSafeFd(int fd);
    std::shared_ptr<AshmemAllocator> rsAshmemAllocator;
    std::function<int(Parcel&)> readFdDefaultFunc = [](Parcel& parcel) -> int {
        return static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    };
};

bool RSAshmemHelperTest::profilerEnabled_ = false;

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

void RSAshmemHelperTest::CheckReadSafeFd(const int fd)
{
    const auto parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(parcel);
    parcel->WriteFileDescriptor(fd);

    const auto beforeRead = parcel->GetReadPosition();

    const std::unordered_map<binder_size_t, int> fds = { { 0, fd } };
    AshmemFdContainer::Instance().Merge(fds);

    const auto safeFd = AshmemFdContainer::Instance().ReadSafeFd(*parcel, readFdDefaultFunc);

    const auto afterRead = parcel->GetReadPosition();

    const auto bytesRead = 24;
    EXPECT_TRUE(afterRead == bytesRead + beforeRead);

    ::close(safeFd);
}

/**
 * @tc.name: ReadSafeFdExactly24bytes
 * @tc.desc: ReadSafeFd read exactly 24 bytes
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, ReadSafeFdExactly24bytes, TestSize.Level1 | Reliability)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    const size_t dataSize = 100;
    const auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ashmemAllocator);
    const auto fd = ashmemAllocator->GetFd();
    EXPECT_TRUE(fd > 0);

    CheckReadSafeFd(fd);
}
} // namespace Rosen
} // namespace OHOS