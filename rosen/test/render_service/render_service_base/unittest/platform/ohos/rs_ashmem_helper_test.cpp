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
#include <fcntl.h>
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
#include "platform/ohos/transaction/zidl/rs_iclient_to_render_connection.h"

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
 * @tc.name: ValidateSealedMemfdTest
 * @tc.desc: Verify function ValidateSealedMemfd
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, ValidateSealedMemfdTest, TestSize.Level1)
{
    // invalid fd is rejected
    EXPECT_FALSE(AshmemAllocator::ValidateSealedMemfd(-1, 10));
    // fd without seals and with insufficient size is rejected
    EXPECT_FALSE(AshmemAllocator::ValidateSealedMemfd(0, 10));

    size_t size = 64;
    auto writer = AshmemAllocator::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    ASSERT_NE(writer, nullptr);
    int fd = writer->GetFd();
    ASSERT_TRUE(fd > 0);
    // not sealed yet
    EXPECT_FALSE(AshmemAllocator::ValidateSealedMemfd(fd, size));
    ASSERT_TRUE(writer->Seal());
    EXPECT_TRUE(AshmemAllocator::ValidateSealedMemfd(fd, size));
    // declared size larger than the memfd is rejected
    EXPECT_FALSE(AshmemAllocator::ValidateSealedMemfd(fd, size + 1));
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
    parcel->WriteFileDescriptor(fd);

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

    // use owned fds: ~AshmemFdWorker always closes fds inserted with shouldCloseFd = true
    int firstFd = open("/dev/null", O_RDONLY);
    int secondFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(firstFd, 0);
    ASSERT_GE(secondFd, 0);

    pid_t callingPid = 0;
    auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(callingPid);
    ASSERT_TRUE(ashmemFdWorker);
    ashmemFdWorker->isFdContainerUpdated_ = true;
    ashmemFdWorker->InsertFdWithOffset(firstFd, 2, false);
    ashmemFdWorker->InsertFdWithOffset(secondFd, 4, true);
    EXPECT_EQ(static_cast<int>(ashmemFdWorker->fds_.size()), 0);
    ashmemFdWorker->isFdContainerUpdated_ = false;
    ashmemFdWorker->InsertFdWithOffset(firstFd, 2, true);
    ashmemFdWorker->InsertFdWithOffset(firstFd, 2, false);
    ashmemFdWorker->InsertFdWithOffset(secondFd, 4, true);
    ashmemFdWorker->InsertFdWithOffset(secondFd, 4, false);
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

    // use an owned fd: ~AshmemFdWorker always closes fds inserted with shouldCloseFd = true
    int ownedFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(ownedFd, 0);

    pid_t callingPid = 0;
    auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(callingPid);
    ASSERT_TRUE(ashmemFdWorker);
    ashmemFdWorker->isFdContainerUpdated_ = false;
    ashmemFdWorker->InsertFdWithOffset(ownedFd, 2, true);
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
 * @tc.name: DestructorClosesFdsTest
 * @tc.desc: Verify ~AshmemFdWorker always closes the fds collected with shouldCloseFd = true
 * @tc.type: FUNC
 * @tc.require: issue#IBESIQ
 */
HWTEST_F(RSAshmemHelperTest, DestructorClosesFdsTest, TestSize.Level1)
{
    AshmemFdContainer::SetIsUnmarshalThread(true);
    AshmemFdContainer::Instance().Clear();

    int tmpFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(tmpFd, 0);
    int ownedFd = dup(tmpFd);
    int keptFd = dup(tmpFd);
    ::close(tmpFd);
    ASSERT_GE(ownedFd, 0);
    ASSERT_GE(keptFd, 0);
    {
        auto ashmemFdWorker = std::make_unique<AshmemFdWorker>(0);
        ashmemFdWorker->InsertFdWithOffset(ownedFd, 1, true);
        ashmemFdWorker->InsertFdWithOffset(keptFd, 2, false);
    }
    // the fd marked shouldCloseFd is closed by the destructor; the other one is left untouched
    EXPECT_EQ(fcntl(ownedFd, F_GETFD), -1);
    EXPECT_GE(fcntl(keptFd, F_GETFD), 0);
    ::close(keptFd);
}

/**
 * @tc.name: SealTest
 * @tc.desc: Verify function Seal
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, SealTest, TestSize.Level1)
{
    size_t size = 16;
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    ASSERT_NE(ashmemAllocator, nullptr);
    ASSERT_TRUE(ashmemAllocator->Seal());
    // the writable mapping is dropped by Seal
    EXPECT_EQ(ashmemAllocator->GetData(), nullptr);
    // sealing again fails because F_SEAL_SEAL is already set
    EXPECT_FALSE(ashmemAllocator->Seal());

    AshmemAllocator invalidAllocator(-1, size);
    EXPECT_FALSE(invalidAllocator.Seal());
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
    // empty parcel can not create ashmem parcel
    auto dataParcel = std::make_shared<MessageParcel>();
    EXPECT_EQ(rsAshmemHelper.CreateAshmemParcel(dataParcel), nullptr);

    // pure data parcel creates ashmem parcel
    auto pureDataParcel = std::make_shared<MessageParcel>();
    pureDataParcel->WriteInt32(0);
    pureDataParcel->WriteBool(true);
    EXPECT_NE(rsAshmemHelper.CreateAshmemParcel(pureDataParcel), nullptr);

    // parcels carrying fds are supported
    auto fdParcel = std::make_shared<MessageParcel>();
    fdParcel->WriteInt32(0);
    int tmpFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(tmpFd, 0);
    ASSERT_TRUE(fdParcel->WriteFileDescriptor(tmpFd));
    ::close(tmpFd);
    EXPECT_NE(rsAshmemHelper.CreateAshmemParcel(fdParcel), nullptr);

    // parcels carrying non-fd binder objects are refused (CreateMessageParcel contains FDR)
    auto parcelWithUnsupportedObjects = CreateMessageParcel();
    EXPECT_EQ(rsAshmemHelper.CreateAshmemParcel(parcelWithUnsupportedObjects), nullptr);
}

/**
 * @tc.name: CreateAndParseAshmemParcelTest
 * @tc.desc: Verify the full ashmem parcel roundtrip: fd payloads are erased in the sealed
 *           shared memory and safely read through the fd container
 * @tc.type:FUNC
 * @tc.require:issuesI9JRWH
 */
HWTEST_F(RSAshmemHelperTest, CreateAndParseAshmemParcelTest, TestSize.Level1)
{
    // sender side: build a transaction-data-like parcel with an fd object
    auto dataParcel = std::make_shared<MessageParcel>();
    dataParcel->WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel->WriteInt32(0); // 0: indicate normal parcel
    constexpr int32_t payload = 12345;
    dataParcel->WriteInt32(payload);
    size_t fdOffset = dataParcel->GetWritePosition();
    int tmpFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(tmpFd, 0);
    ASSERT_TRUE(dataParcel->WriteFileDescriptor(tmpFd));
    ::close(tmpFd);
    ASSERT_EQ(dataParcel->GetOffsetsSize(), 1);

    auto ashmemParcel = RSAshmemHelper::CreateAshmemParcel(dataParcel);
    ASSERT_NE(ashmemParcel, nullptr);

    // receiver side: consume the token and the ashmem flag as the stub does
    auto token = ashmemParcel->ReadInterfaceToken();
    ASSERT_EQ(token, RSIClientToRenderConnection::GetDescriptor());
    ASSERT_EQ(ashmemParcel->ReadInt32(), 1);

    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr;
    std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;
    auto parsedParcel =
        RSAshmemHelper::ParseFromAshmemParcel(ashmemParcel.get(), ashmemFdWorker, ashmemFlowControlUnit, 0);
    ASSERT_NE(parsedParcel, nullptr);
    ASSERT_NE(ashmemFdWorker, nullptr);
    ASSERT_EQ(ashmemFdWorker->fds_.count(static_cast<binder_size_t>(fdOffset)), 1);

    // the fd payload in shared memory was erased by the sender
    const flat_binder_object* flat =
        reinterpret_cast<const flat_binder_object*>(parsedParcel->GetData() + fdOffset);
    EXPECT_EQ(flat->handle, static_cast<uint32_t>(-1));

    // the unmarshal thread reads the fd through the container instead of shared memory
    AshmemFdContainer::SetIsUnmarshalThread(true);
    ashmemFdWorker->PushFdsToContainer();
    ASSERT_EQ(parsedParcel->ReadInt32(), payload);
    ASSERT_EQ(parsedParcel->GetReadPosition(), fdOffset);
    int safeFd = AshmemFdContainer::Instance().ReadSafeFd(*parsedParcel, nullptr);
    EXPECT_GE(safeFd, 0);
    ::close(safeFd);
    AshmemFdContainer::SetIsUnmarshalThread(false);
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

#ifdef RS_ENABLE_UNI_RENDER
/**
 * @tc.name: ParseFromAshmemParcelFdCloseOnAllocatorFail
 * @tc.desc: Verify ParseFromAshmemParcel closes fd when ashmemAllocator fails with valid fd
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, ParseFromAshmemParcelFdCloseOnAllocatorFail, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    MessageParcel ashmemParcel;
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr;
    std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;

    // Write dataSize
    constexpr uint32_t dataSize = 16;
    ashmemParcel.WriteUint32(dataSize);

    // Write a valid fd (not a sealed memfd) - lseek size of /dev/null is 0 (< dataSize) and it has
    // no seals, so ValidateSealedMemfd fails, but fd >= 0 so it should be closed
    int devNullFd = open("/dev/null", O_RDONLY);
    ASSERT_GE(devNullFd, 0);
    ashmemParcel.WriteFileDescriptor(devNullFd);

    auto result = rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel, ashmemFdWorker, ashmemFlowControlUnit);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ParseFromAshmemParcelNegativeFdOnAllocatorFail
 * @tc.desc: Verify ParseFromAshmemParcel handles negative fd when ashmemAllocator fails
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, ParseFromAshmemParcelNegativeFdOnAllocatorFail, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    MessageParcel ashmemParcel;
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr;
    std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;

    // Write dataSize
    constexpr uint32_t dataSize = 16;
    ashmemParcel.WriteUint32(dataSize);

    // Write a invalid fd (-1) via WriteFileDescriptor with a dummy descriptor
    // ReadFileDescriptor will return -1 when no valid fd is in the parcel
    // This creates the fd < 0 branch where ::close(fd) is NOT called
    sptr<IPCFileDescriptor> badDescriptor = new IPCFileDescriptor(-1);
    ashmemParcel.WriteObject(badDescriptor);

    auto result = rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel, ashmemFdWorker, ashmemFlowControlUnit);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ParseFromAshmemParcelOffsetsOverflowRejectedWithFdWorker
 * @tc.desc: Verify ParseFromAshmemParcel rejects parcels whose declared offsets exceed the
 *           data capacity and leaves the caller-provided fdWorker untouched
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, ParseFromAshmemParcelOffsetsOverflowRejectedWithFdWorker, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = std::make_unique<AshmemFdWorker>(0);
    std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;

    // Create a real sealed memfd with data
    constexpr uint32_t dataSize = 64;
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    ASSERT_NE(ashmemAllocator, nullptr);

    int32_t dummyData = 0;
    ashmemAllocator->WriteToAshmem(&dummyData, sizeof(dummyData));

    MessageParcel ashmemParcel;
    ashmemParcel.WriteUint32(dataSize);
    ashmemParcel.WriteFileDescriptor(ashmemAllocator->GetFd());

    // 100 offsets can never fit into 64 bytes of data
    constexpr int32_t offsetSize = 100;
    ashmemParcel.WriteInt32(offsetSize);

    ASSERT_TRUE(ashmemAllocator->Seal());

    auto result = rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel, ashmemFdWorker, ashmemFlowControlUnit);
    EXPECT_EQ(result, nullptr);
    // the caller-provided fdWorker is left untouched
    EXPECT_TRUE(ashmemFdWorker->fds_.empty());
}

/**
 * @tc.name: ParseFromAshmemParcelOffsetsReadFailNoFdWorker
 * @tc.desc: Verify ParseFromAshmemParcel rejects parcels whose offsets buffer is missing
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, ParseFromAshmemParcelOffsetsReadFailNoFdWorker, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr;
    std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;

    // Create a real sealed memfd with data
    constexpr uint32_t dataSize = 64;
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    ASSERT_NE(ashmemAllocator, nullptr);

    int32_t dummyData = 0;
    ashmemAllocator->WriteToAshmem(&dummyData, sizeof(dummyData));

    MessageParcel ashmemParcel;
    ashmemParcel.WriteUint32(dataSize);
    ashmemParcel.WriteFileDescriptor(ashmemAllocator->GetFd());

    constexpr int32_t offsetSize = 1;
    ashmemParcel.WriteInt32(offsetSize);

    ASSERT_TRUE(ashmemAllocator->Seal());

    auto result = rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel, ashmemFdWorker, ashmemFlowControlUnit);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ParseFromAshmemParcelReadIntFailWithFdWorker
 * @tc.desc: Verify ParseFromAshmemParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, ParseFromAshmemParcelReadIntFailWithFdWorker, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = std::make_unique<AshmemFdWorker>(0);
    std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;

    // Create a real sealed memfd with data
    constexpr uint32_t dataSize = 64;
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    ASSERT_NE(ashmemAllocator, nullptr);

    int32_t nonZeroData = 1;
    ashmemAllocator->WriteToAshmem(&nonZeroData, sizeof(nonZeroData));

    MessageParcel ashmemParcel;
    ashmemParcel.WriteUint32(dataSize);
    ashmemParcel.WriteFileDescriptor(ashmemAllocator->GetFd());

    ashmemParcel.WriteInt32(0);

    ASSERT_TRUE(ashmemAllocator->Seal());

    auto result = rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel, ashmemFdWorker, ashmemFlowControlUnit);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: ParseFromAshmemParcelReadIntFailNoFdWorker
 * @tc.desc: Verify ParseFromAshmemParcel handles null offsets without fdWorker
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSAshmemHelperTest, ParseFromAshmemParcelReadIntFailNoFdWorker, TestSize.Level1)
{
    RSAshmemHelper rsAshmemHelper;
    std::unique_ptr<AshmemFdWorker> ashmemFdWorker = nullptr;
    std::shared_ptr<AshmemFlowControlUnit> ashmemFlowControlUnit = nullptr;

    // Create a real sealed memfd with data
    constexpr uint32_t dataSize = 64;
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(dataSize, PROT_READ | PROT_WRITE);
    ASSERT_NE(ashmemAllocator, nullptr);

    int32_t nonZeroData = 1;
    ashmemAllocator->WriteToAshmem(&nonZeroData, sizeof(nonZeroData));

    MessageParcel ashmemParcel;
    ashmemParcel.WriteUint32(dataSize);
    ashmemParcel.WriteFileDescriptor(ashmemAllocator->GetFd());

    ashmemParcel.WriteInt32(0);

    ASSERT_TRUE(ashmemAllocator->Seal());

    auto result = rsAshmemHelper.ParseFromAshmemParcel(&ashmemParcel, ashmemFdWorker, ashmemFlowControlUnit);
    EXPECT_EQ(result, nullptr);
}
#endif
} // namespace Rosen
} // namespace OHOS