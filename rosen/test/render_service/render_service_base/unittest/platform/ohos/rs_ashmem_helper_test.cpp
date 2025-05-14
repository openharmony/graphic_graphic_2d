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
#include <climits>
#include <iostream>
#include <memory>
#include <sys/mman.h>
#include <unistd.h>

#include "ashmem.h"
#include "gtest/hwext/gtest-tag.h"
#include "ipc_file_descriptor.h"
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
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    std::shared_ptr<AshmemAllocator> rsAshmemAllocator;
    std::function<int(Parcel&)> readFdDefaultFunc = [](Parcel& parcel) -> int {
        return static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    };
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
    Allocator* allocator = nullptr;
    size_t offsetsSize = 2;
    auto dataParcel = std::make_shared<MessageParcel>(allocator);
    dataParcel->objectOffsets_ = new binder_size_t[offsetsSize];
    dataParcel->objectOffsets_[0] = 0;
    dataParcel->objectOffsets_[1] = sizeof(flat_binder_object);
    dataParcel->objectsCapacity_ = offsetsSize;
    dataParcel->objectCursor_ = offsetsSize;

    size_t dataSize = offsetsSize * sizeof(flat_binder_object);
    if (dataParcel->data_ != nullptr) {
        delete[] dataParcel->data_;
    }
    dataParcel->data_ = new uint8_t[dataSize];
    dataParcel->dataSize_ = dataSize;
    binder_size_t* object = reinterpret_cast<binder_size_t*>(dataParcel->GetObjectOffsets());
    uintptr_t data = dataParcel->GetData();
    for (size_t i = 0; i < dataParcel->objectCursor_; i++) {
        flat_binder_object* flat = reinterpret_cast<flat_binder_object*>(data + object[i]);
        if (dataParcel->objectOffsets_[0] == dataParcel->objectOffsets_[i]) {
            flat->hdr.type = BINDER_TYPE_FD;
        } else {
            flat->hdr.type = BINDER_TYPE_FDR;
        }
        flat->handle = static_cast<int>(i);
    }
    return dataParcel;
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
    // for test
    size_t size3 = 200000001;
    ashmemAllocator.size_ = 200000002;
    ASSERT_EQ(ashmemAllocator.CopyFromAshmem(size3), nullptr);
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
    rsAshmemAllocator->Dealloc(nullptr);
    rsAshmemAllocator->Dealloc(wrongData);
    rsAshmemAllocator->Dealloc(rsAshmemAllocator->data_);
    EXPECT_EQ(rsAshmemAllocator->GetData(), nullptr);
    rsAshmemAllocator->fd_ = -2;
    rsAshmemAllocator->data_ = nullptr;
    rsAshmemAllocator->Dealloc(rsAshmemAllocator->data_);
    EXPECT_NE(rsAshmemAllocator->GetFd(), -1);
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
 * @tc.name: ReadSafeFdTest001
 * @tc.desc: Verify function ReadSafeFd
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, ReadSafeFdTest001, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(parcel);

    AshmemFdContainer::Instance().isUseFdContainer_ = true;
    int safeFd1 = AshmemFdContainer::Instance().ReadSafeFd(*parcel, nullptr);
    EXPECT_TRUE(safeFd1 < 0);

    AshmemFdContainer::Instance().isUseFdContainer_ = false;
    int safeFd2 = AshmemFdContainer::Instance().ReadSafeFd(*parcel, nullptr);
    EXPECT_TRUE(safeFd2 < 0);

    AshmemFdContainer::SetIsUnmarshalThread(false);
    AshmemFdContainer::Instance().isUseFdContainer_ = true;
    int safeFd3 = AshmemFdContainer::Instance().ReadSafeFd(*parcel, nullptr);
    EXPECT_TRUE(safeFd3 < 0);

    AshmemFdContainer::Instance().isUseFdContainer_ = false;
    int safeFd4 = AshmemFdContainer::Instance().ReadSafeFd(*parcel, nullptr);
    EXPECT_TRUE(safeFd4 < 0);

    int safeFd5 = AshmemFdContainer::Instance().ReadSafeFd(*parcel, readFdDefaultFunc);
    EXPECT_TRUE(safeFd5 < 0);
}

/**
 * @tc.name: ReadSafeFdTest002
 * @tc.desc: Verify function ReadSafeFd
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, ReadSafeFdTest002, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(parcel);

    std::unordered_map<binder_size_t, int> fds = { {0, INT_MAX} };
    AshmemFdContainer::Instance().Merge(fds);

    int safeFd = AshmemFdContainer::Instance().ReadSafeFd(*parcel, readFdDefaultFunc);
    EXPECT_TRUE(safeFd < 0);
}

/**
 * @tc.name: ReadSafeFdTest003
 * @tc.desc: Verify function ReadSafeFd
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, ReadSafeFdTest003, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    size_t dataSize = 1;
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ashmemAllocator);
    int fd = ashmemAllocator->GetFd();
    EXPECT_TRUE(fd > 0);

    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(parcel);

    std::unordered_map<binder_size_t, int> fds = { {0, fd} };
    AshmemFdContainer::Instance().Merge(fds);

    int safeFd = AshmemFdContainer::Instance().ReadSafeFd(*parcel, readFdDefaultFunc);
    EXPECT_TRUE(safeFd > 0);
    ::close(safeFd);
}

/**
 * @tc.name: ReadSafeFdTest004
 * @tc.desc: Verify function ReadSafeFd
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, ReadSafeFdTest004, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    size_t dataSize = 1;
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ashmemAllocator);
    int fd = ashmemAllocator->GetFd();
    EXPECT_TRUE(fd > 0);

    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(parcel);
    parcel->WriteFileDescriptor(fd);

    std::unordered_map<binder_size_t, int> fds = { {0, fd} };
    AshmemFdContainer::Instance().Merge(fds);

    int safeFd = AshmemFdContainer::Instance().ReadSafeFd(*parcel, readFdDefaultFunc);
    EXPECT_TRUE(safeFd > 0);
    ::close(safeFd);
}

/**
 * @tc.name: ReadSafeFdTest005
 * @tc.desc: Verify function ReadSafeFd
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, ReadSafeFdTest005, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    size_t dataSize = 1;
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    ASSERT_TRUE(ashmemAllocator);
    int fd = ashmemAllocator->GetFd();
    EXPECT_TRUE(fd > 0);

    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    ASSERT_TRUE(parcel);
    int parcelFd = dup(fd);
    EXPECT_TRUE(parcelFd > 0);
    sptr<IPCFileDescriptor> descriptor = new (std::nothrow) IPCFileDescriptor(parcelFd);
    ASSERT_TRUE(descriptor);
    ASSERT_TRUE(parcel->WriteObject<IPCFileDescriptor>(descriptor));

    std::unordered_map<binder_size_t, int> fds = { {0, parcelFd} };
    AshmemFdContainer::Instance().Merge(fds);

    int safeFd = AshmemFdContainer::Instance().ReadSafeFd(*parcel, readFdDefaultFunc);
    EXPECT_TRUE(safeFd > 0);
    ::close(safeFd);
}

/**
 * @tc.name: MergeTest
 * @tc.desc: Verify function Merge
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, MergeTest, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    std::unordered_map<binder_size_t, int> fds = { {1, 2}, {3, 4} };
    AshmemFdContainer::SetIsUnmarshalThread(false);
    AshmemFdContainer::Instance().Merge(fds);
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 0);
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Merge(fds);
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 2);
}

/**
 * @tc.name: ClearTest
 * @tc.desc: Verify function Clear
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, ClearTest, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    std::unordered_map<binder_size_t, int> fds = { {1, 2}, {3, 4} };
    AshmemFdContainer::Instance().Merge(fds);
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 2);
    AshmemFdContainer::SetIsUnmarshalThread(false);
    AshmemFdContainer::Instance().Clear();
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 2);
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 0);
    AshmemFdContainer::Instance().Clear();
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 0);
}

/**
 * @tc.name: PrintFdsTest
 * @tc.desc: Verify function PrintFds
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, PrintFdsTest, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    std::unordered_map<binder_size_t, int> fds = { {1, 2}, {3, 4} };
    AshmemFdContainer::Instance().Merge(fds);
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 2);
    std::string ret = AshmemFdContainer::Instance().PrintFds();
    EXPECT_NE(static_cast<int>(ret.size()), 0);
}

/**
 * @tc.name: InsertFdWithOffsetTest
 * @tc.desc: Verify function InsertFdWithOffset
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, InsertFdWithOffsetTest, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    pid_t callingPid = 0;
    auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(callingPid);
    ASSERT_TRUE(ashmemFdWorker);
    ashmemFdWorker->isFdContainerUpdated_ = true;
    ashmemFdWorker->InsertFdWithOffset(1, 2, false);
    ashmemFdWorker->InsertFdWithOffset(3, 4, true);
    EXPECT_EQ(static_cast<int>(ashmemFdWorker->fds_.size()), 0);
    ashmemFdWorker->isFdContainerUpdated_ = false;
    ashmemFdWorker->InsertFdWithOffset(1, 2, true);
    ashmemFdWorker->InsertFdWithOffset(1, 2, false);
    ashmemFdWorker->InsertFdWithOffset(3, 4, true);
    ashmemFdWorker->InsertFdWithOffset(3, 4, false);
    EXPECT_EQ(static_cast<int>(ashmemFdWorker->fds_.size()), 2);
}

/**
 * @tc.name: PushFdsToContainerTest
 * @tc.desc: Verify function PushFdsToContainer
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, PushFdsToContainerTest, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    pid_t callingPid = 0;
    auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(callingPid);
    ASSERT_TRUE(ashmemFdWorker);
    ashmemFdWorker->isFdContainerUpdated_ = false;
    ashmemFdWorker->InsertFdWithOffset(1, 2, true);
    ashmemFdWorker->InsertFdWithOffset(3, 4, false);
    EXPECT_EQ(static_cast<int>(ashmemFdWorker->fds_.size()), 2);
    ashmemFdWorker->isFdContainerUpdated_ = true;
    ashmemFdWorker->PushFdsToContainer();
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 0);
    ashmemFdWorker->isFdContainerUpdated_ = false;
    ashmemFdWorker->PushFdsToContainer();
    EXPECT_EQ(static_cast<int>(AshmemFdContainer::Instance().fds_.size()), 2);
    EXPECT_TRUE(ashmemFdWorker->isFdContainerUpdated_);
}

/**
 * @tc.name: EnableManualCloseFdsTest
 * @tc.desc: Verify function EnableManualCloseFds
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, EnableManualCloseFdsTest, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    pid_t callingPid = 0;
    auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(callingPid);
    ASSERT_TRUE(ashmemFdWorker);
    ashmemFdWorker->InsertFdWithOffset(-1, 1, true);
    ashmemFdWorker->InsertFdWithOffset(INT_MAX, 2, true);
    EXPECT_EQ(static_cast<int>(ashmemFdWorker->fds_.size()), 2);
    ashmemFdWorker->EnableManualCloseFds();
    EXPECT_TRUE(ashmemFdWorker->needManualCloseFds_);
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
    auto dataParcel = CreateMessageParcel();
    rsAshmemHelper.CopyFileDescriptor(&ashmemParcel, dataParcel);
    EXPECT_NE(dataParcel->GetOffsetsSize(), 0);
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
    auto dataParcel = CreateMessageParcel();
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr;
    rsAshmemHelper.InjectFileDescriptor(dataParcel, &ashmemParcel, ashmemFdWorker);
    EXPECT_NE(dataParcel->GetOffsetsSize(), 0);
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
    auto dataParcelf = CreateMessageParcel();
    EXPECT_NE(rsAshmemHelper.CreateAshmemParcel(dataParcelf), nullptr);
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
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr;
    std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;
    EXPECT_EQ(rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel, ashmemFdWorker, ashmemFlowControlUnit), nullptr);
}
} // namespace Rosen
} // namespace OHOS