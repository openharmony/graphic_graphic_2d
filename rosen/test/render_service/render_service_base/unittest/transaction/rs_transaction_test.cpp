/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>
#include "transaction/transaction.h"
#include "mock_transaction.h"
#include "buffer_utils.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSTransactionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<Mock::MockCallback> callback_;
    sptr<Mock::MockListener> listener_;
    sptr<RSTransactionManager> manager_;
    static void OnDeleteBufferFunc(uint32_t value);
    void WriteTransactionInfoFromMessageParcel(MessageParcel& parcel, TransactionInfo info);
};
class OnBufferAvailableTest : public IBufferConsumerListener {
public:
    void OnBufferAvailable() override {};
};
void RSTransactionManagerTest::SetUpTestCase() {}
void RSTransactionManagerTest::TearDownTestCase() {}
void RSTransactionManagerTest::SetUp()
{
    callback_ = new Mock::MockCallback();
    listener_ = new Mock::MockListener();
    manager_ = RSTransactionManager::Create(42, "test", 4, callback_, listener_); // 42 : unique id
    ASSERT_NE(manager_, nullptr);
}
void RSTransactionManagerTest::TearDown()
{
    manager_ = nullptr;
    listener_ = nullptr;
    callback_ = nullptr;
}

void RSTransactionManagerTest::OnDeleteBufferFunc(uint32_t value)
{
    (void)value;
}
void RSTransactionManagerTest::WriteTransactionInfoFromMessageParcel(MessageParcel& parcel, TransactionInfo info)
{
    parcel.WriteUint32(0);
    parcel.WriteBool(false);
    uint32_t colorGamut = 0;
    uint32_t transform = 0;
    int32_t scalingMode = 0;
    uint32_t configTransform = 0;
    parcel.WriteInt32(info.bufferInfo.bufferRequestConfig.width);
    parcel.WriteInt32(info.bufferInfo.bufferRequestConfig.height);
    parcel.WriteInt32(info.bufferInfo.bufferRequestConfig.strideAlignment);
    parcel.WriteInt32(info.bufferInfo.bufferRequestConfig.format);
    parcel.WriteUint64(info.bufferInfo.bufferRequestConfig.usage);
    parcel.WriteInt32(info.bufferInfo.bufferRequestConfig.timeout);
    parcel.WriteUint32(colorGamut);
    parcel.WriteUint32(configTransform);
    parcel.WriteInt32(scalingMode);
    parcel.WriteUint32(transform);
}

/**
 * @tc.name: CreateTest001
 * @tc.desc: Create
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, CreateTest001, TestSize.Level1)
{
    EXPECT_EQ(nullptr, RSTransactionManager::Create(1, "n", 1, nullptr, listener_));
    EXPECT_EQ(nullptr, RSTransactionManager::Create(1, "n", 1, callback_, nullptr));
}

/**
 * @tc.name: GoBackgroundTest001
 * @tc.desc: GoBackground
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, GoBackgroundTest001, TestSize.Level1)
{
    EXPECT_CALL(*callback_, GoBackground(42)).Times(1);
    EXPECT_EQ(GSERROR_OK, manager_->GoBackground());
}
/**
 * @tc.name: QueueTransactionTest001
 * @tc.desc: QueueTransaction
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, QueueTransactionTest001, TestSize.Level1)
{
    RSTransactionConfig config;
    config.hasBuffer = true;
    config.transaction = nullptr;
    EXPECT_EQ(GSERROR_INVALID_ARGUMENTS, manager_->QueueTransaction(config));
}
/**
 * @tc.name: QueueTransactionTest001
 * @tc.desc: QueueTransaction
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, QueueTransaction001, TestSize.Level1)
{
    RSTransactionConfig config;
    config.hasBuffer = false;
    EXPECT_EQ(GSERROR_INVALID_ARGUMENTS, manager_->QueueTransaction(config));
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> tx = sptr<Transaction>(new Transaction(buffer));
    manager_->transactionQueueCache_[100] = tx; // 100 : seq
    config.transactionInfo.bufferInfo.sequence = 100; // 100 : seq
    EXPECT_EQ(GSERROR_INVALID_ARGUMENTS, manager_->QueueTransaction(config));
}
/**
 * @tc.name: QueueTransactionTest002
 * @tc.desc: QueueTransaction
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, QueueTransaction002, TestSize.Level1)
{
    RSTransactionConfig config;
    config.hasBuffer = true;
    sptr<Transaction> tx = sptr<Transaction>();
    config.transaction = tx;
    EXPECT_EQ(GSERROR_INVALID_ARGUMENTS, manager_->QueueTransaction(config));
}
/**
 * @tc.name: QueueThenAcquireAndReleaseTest001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, QueueThenAcquireAndReleaseTest001, TestSize.Level1)
{
    // prepare a transaction with buffer+fence
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<SyncFence> fence = new SyncFence(1000);  // assume valid
    std::vector<Rect> damages{{1, 2, 3, 4}};
    sptr<Transaction> tx = sptr<Transaction>(new Transaction(buffer));
    RSTransactionConfig config;
    config.hasBuffer = true;
    config.transaction = tx;
    EXPECT_CALL(*listener_, OnBufferAvailable()).Times(1);
    ASSERT_EQ(GSERROR_INVALID_ARGUMENTS, manager_->QueueTransaction(config));
    config.hasBuffer = false;
    config.transaction = tx;
    ASSERT_EQ(GSERROR_INVALID_ARGUMENTS, manager_->QueueTransaction(config));
    manager_->transactionQueueCache_[100] = tx; // 100 : seq
    ASSERT_EQ(GSERROR_INVALID_ARGUMENTS, manager_->QueueTransaction(config));
    manager_->transactionQueueCache_.clear();
    config.hasBuffer = true;
    config.transaction = tx;
    tx->SetFence(fence);
    config.delList.push_back(100); // 100 : seq
    ASSERT_EQ(GSERROR_OK, manager_->QueueTransaction(config));
    manager_->transactionListener_ = nullptr;
    ASSERT_EQ(GSERROR_OK, manager_->QueueTransaction(config));

    ASSERT_TRUE(manager_->QueryIfBufferAvailable());
    sptr<SurfaceBuffer> outBuf;
    sptr<SyncFence> outFence;
    int64_t ts;
    std::vector<Rect> outDamages;
    EXPECT_EQ(GSERROR_OK, manager_->AcquireBuffer(outBuf, outFence, ts, outDamages));
    EXPECT_EQ(buffer->GetSeqNum(), outBuf->GetSeqNum());
    EXPECT_EQ(tx->GetUiTimestamp(), ts);

    // cache should contain sequence=7
    EXPECT_CALL(*callback_, OnCompleted(testing::_)).WillOnce(testing::Return(GSERROR_OK));
    ASSERT_EQ(GSERROR_OK, manager_->ReleaseBuffer(buffer, fence));
}
/**
 * @tc.name: LogAndTraceAllBufferInPendingTransactionQueueLocked001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, LogAndTraceAllBufferInPendingTransactionQueueLocked001, TestSize.Level1)
{
    manager_->LogAndTraceAllBufferInPendingTransactionQueueLocked();
    ASSERT_EQ(manager_->pendingTransactionQueue_.size(), 0);
}
/**
 * @tc.name: LogAndTraceAllBufferInPendingTransactionQueueLocked002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, LogAndTraceAllBufferInPendingTransactionQueueLocked002, TestSize.Level1)
{
    sptr<Transaction> transaction = new Transaction();
    manager_->pendingTransactionQueue_.push_back(transaction);
    manager_->LogAndTraceAllBufferInPendingTransactionQueueLocked();
    ASSERT_EQ(manager_->pendingTransactionQueue_.size(), 1);
}
/**
 * @tc.name: AcquireBuffer001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer001, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    EXPECT_EQ(GSERROR_NO_BUFFER, manager_->AcquireBuffer(returnValue, 123, false));
}
/**
 * @tc.name: AcquireBuffer002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer002, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    EXPECT_EQ(GSERROR_NO_BUFFER, manager_->AcquireBuffer(returnValue, 0, false));
}
/**
 * @tc.name: AcquireBuffer003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer003, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    sptr<Transaction> transaction = new Transaction();
    transaction->SetAutoTimestamp(false);
    transaction->SetDesiredPresentTimestamp(10);
    manager_->pendingTransactionQueue_.push_back(transaction);
    EXPECT_EQ(GSERROR_NO_BUFFER_READY, manager_->AcquireBuffer(returnValue, 9, false));
    EXPECT_EQ(GSERROR_OK, manager_->AcquireBuffer(returnValue, 11, false));
}
/**
 * @tc.name: AcquireBuffer004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer004, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    sptr<Transaction> transaction = new Transaction();
    transaction->SetAutoTimestamp(false);
    transaction->SetDesiredPresentTimestamp(10);
    manager_->pendingTransactionQueue_.push_back(transaction);
    EXPECT_EQ(GSERROR_OK, manager_->AcquireBuffer(returnValue, 0, false));
}
/**
 * @tc.name: AcquireBuffer005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer005, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    sptr<Transaction> transaction = new Transaction();
    transaction->SetAutoTimestamp(true);
    transaction->SetDesiredPresentTimestamp(10);
    manager_->pendingTransactionQueue_.push_back(transaction);
    EXPECT_EQ(GSERROR_OK, manager_->AcquireBuffer(returnValue, 0, false));
}
/**
 * @tc.name: AcquireBuffer006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer006, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    sptr<Transaction> transaction = new Transaction();
    transaction->SetAutoTimestamp(false);
    transaction->SetDesiredPresentTimestamp(10 + RSTransactionManager::ONE_SECOND_TIMESTAMP);
    manager_->pendingTransactionQueue_.push_back(transaction);
    EXPECT_EQ(GSERROR_NO_BUFFER_READY, manager_->AcquireBuffer(returnValue, 11, false));
}
/**
 * @tc.name: AcquireBuffer007
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer007, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    sptr<Transaction> transaction = new Transaction();
    transaction->SetAutoTimestamp(true);
    transaction->SetDesiredPresentTimestamp(10);
    manager_->pendingTransactionQueue_.push_back(transaction);
    EXPECT_EQ(GSERROR_OK, manager_->AcquireBuffer(returnValue, 9, false));
    EXPECT_EQ(GSERROR_NO_BUFFER, manager_->AcquireBuffer(returnValue, 11, false));
}
/**
 * @tc.name: AcquireBuffer008
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer008, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    sptr<Transaction> transaction = new Transaction();
    transaction->SetAutoTimestamp(true);
    transaction->SetDesiredPresentTimestamp(10);
    manager_->pendingTransactionQueue_.push_back(transaction);
    EXPECT_EQ(GSERROR_OK, manager_->AcquireBuffer(returnValue, 9, false));
    EXPECT_EQ(GSERROR_NO_BUFFER, manager_->AcquireBuffer(returnValue, 11, true));
}
/**
 * @tc.name: AcquireBuffer009
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer009, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    sptr<Transaction> transaction = new Transaction();
    transaction->SetAutoTimestamp(true);
    transaction->SetDesiredPresentTimestamp(10);
    manager_->pendingTransactionQueue_.push_back(transaction);
    EXPECT_EQ(GSERROR_OK, manager_->AcquireBuffer(returnValue, 11, true));
}
/**
 * @tc.name: AcquireBuffer010
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBuffer010, TestSize.Level1)
{
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    sptr<Transaction> transaction = new Transaction();
    transaction->SetAutoTimestamp(false);
    transaction->SetDesiredPresentTimestamp(10);
    manager_->pendingTransactionQueue_.push_back(transaction);
    manager_->pendingTransactionQueue_.push_back(transaction);
    EXPECT_EQ(GSERROR_NO_BUFFER_READY, manager_->AcquireBuffer(returnValue, 9, false));
    EXPECT_EQ(GSERROR_OK, manager_->AcquireBuffer(returnValue, 11, false));
}
/**
 * @tc.name: ReleaseDropBuffers001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReleaseDropBuffers001, TestSize.Level1)
{
    std::vector<uint32_t> dropTest;
    manager_->ReleaseDropBuffers(dropTest);
    EXPECT_EQ(dropTest.size(), 0);
    dropTest.push_back(1);
    EXPECT_EQ(dropTest.size(), 1);
    manager_->ReleaseDropBuffers(dropTest);
    EXPECT_CALL(*callback_, OnDropBuffers(testing::_)).WillOnce(testing::Return(GSERROR_INTERNAL));
    manager_->ReleaseDropBuffers(dropTest);
    EXPECT_CALL(*callback_, OnDropBuffers(testing::_)).WillOnce(testing::Return(GSERROR_OK));
    manager_->ReleaseDropBuffers(dropTest);
}
/**
 * @tc.name: SetDefaultWidthAndHeight001
 * @tc.desc: SetDefaultWidthAndHeight
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, SetDefaultWidthAndHeight001, TestSize.Level1)
{
    EXPECT_CALL(*callback_, SetDefaultWidthAndHeight(testing::_)).WillOnce(testing::Return(GSERROR_OK));
    EXPECT_EQ(manager_->SetDefaultWidthAndHeight(8, 10), GSERROR_OK);
    EXPECT_CALL(*callback_, SetDefaultWidthAndHeight(testing::_)).WillOnce(testing::Return(GSERROR_INTERNAL));
    EXPECT_EQ(manager_->SetDefaultWidthAndHeight(8, 10), GSERROR_INTERNAL);
}
/**
 * @tc.name: Dump001
 * @tc.desc: Dump
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, Dump001, TestSize.Level1)
{
    std::string result;
    EXPECT_CALL(*callback_, OnSurfaceDump(testing::_)).WillOnce(testing::Return(GSERROR_OK));
    manager_->Dump(result);
    EXPECT_NE(result.size(), 0);
    EXPECT_CALL(*callback_, OnSurfaceDump(testing::_)).WillOnce(testing::Return(GSERROR_INTERNAL));
    manager_->Dump(result);
}
/**
 * @tc.name: AcquireBufferLocked001
 * @tc.desc: AcquireBufferLocked
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, AcquireBufferLocked001, TestSize.Level1)
{
    auto ret = manager_->AcquireBufferLocked();
    EXPECT_EQ(ret, nullptr);
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    manager_->pendingTransactionQueue_.push_back(trans);
    ret = manager_->AcquireBufferLocked();
    EXPECT_EQ(ret, trans);
}
/**
 * @tc.name: QueryIfBufferAvailable001
 * @tc.desc: QueryIfBufferAvailable
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, QueryIfBufferAvailable001, TestSize.Level1)
{
    auto ret = manager_->QueryIfBufferAvailable();
    EXPECT_EQ(ret, false);
}
/**
 * @tc.name: OnTunnelHandleChange001
 * @tc.desc: OnTunnelHandleChange
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, OnTunnelHandleChange001, TestSize.Level1)
{
    sptr<SurfaceTunnelHandle> handle = new SurfaceTunnelHandle();
    manager_->OnTunnelHandleChange(nullptr);
    EXPECT_EQ(manager_->tunnelHandle_, nullptr);
    manager_->OnTunnelHandleChange(handle);
    manager_->tunnelHandle_ = new SurfaceTunnelHandle();
    manager_->OnTunnelHandleChange(nullptr);
    manager_->OnTunnelHandleChange(handle);
    manager_->transactionListener_ = listener_;
    manager_->OnTunnelHandleChange(handle);
    manager_->tunnelHandle_ = handle;
    manager_->OnTunnelHandleChange(handle);
    manager_->tunnelHandle_ = nullptr;
    manager_->transactionListener_ = nullptr;
}
/**
 * @tc.name: OnGoBackground001
 * @tc.desc: OnGoBackground
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, OnGoBackground001, TestSize.Level1)
{
    manager_->transactionListener_ = listener_;
    manager_->OnGoBackground();
    EXPECT_NE(manager_->transactionListener_, nullptr);
    manager_->transactionListener_ = nullptr;
    manager_->OnGoBackground();
}
/**
 * @tc.name: OnCleanCache001
 * @tc.desc: OnCleanCache
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, OnCleanCache001, TestSize.Level1)
{
    manager_->transactionListener_ = listener_;
    manager_->OnCleanCache();
    EXPECT_NE(manager_->transactionListener_, nullptr);
    manager_->transactionListener_ = nullptr;
    manager_->OnCleanCache();
}
/**
 * @tc.name: OnTransformChange001
 * @tc.desc: OnTransformChange
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, OnTransformChange001, TestSize.Level1)
{
    manager_->transactionListener_ = listener_;
    manager_->OnTransformChange();
    EXPECT_NE(manager_->transactionListener_, nullptr);
    manager_->transactionListener_ = nullptr;
    manager_->OnTransformChange();
}
/**
 * @tc.name: OnDeleteBuffer001
 * @tc.desc: OnDeleteBuffer
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, OnDeleteBuffer001, TestSize.Level1)
{
    EXPECT_EQ(manager_->onBufferDeleteForRSHardwareThread_, nullptr);
    manager_->OnDeleteBuffer(0, true);
    manager_->OnDeleteBuffer(0, false);
    manager_->onBufferDeleteForRSHardwareThread_ = OnDeleteBufferFunc;
    manager_->OnDeleteBuffer(0, true);
    manager_->onBufferDeleteForRSHardwareThread_ = nullptr;
}
/**
 * @tc.name: RegisterConsumerListener001
 * @tc.desc: RegisterConsumerListener
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, RegisterConsumerListener001, TestSize.Level1)
{
    sptr<IBufferConsumerListener> listenerTmp = nullptr;
    EXPECT_EQ(manager_->RegisterConsumerListener(listenerTmp), GSERROR_INTERNAL);
    listenerTmp = new OnBufferAvailableTest();
    EXPECT_EQ(manager_->RegisterConsumerListener(listenerTmp), GSERROR_OK);
    manager_->transactionListener_ = nullptr;
}
/**
 * @tc.name: RegisterDeleteBufferListener001
 * @tc.desc: RegisterDeleteBufferListener
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, RegisterDeleteBufferListener001, TestSize.Level1)
{
    EXPECT_EQ(manager_->RegisterDeleteBufferListener(OnDeleteBufferFunc, true), GSERROR_OK);
    EXPECT_EQ(manager_->RegisterDeleteBufferListener(OnDeleteBufferFunc, false), GSERROR_OK);
}
/**
 * @tc.name: QueryMetaDataType001
 * @tc.desc: QueryMetaDataType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, QueryMetaDataType001, TestSize.Level1)
{
    HDRMetaDataType type;
    EXPECT_EQ(manager_->QueryMetaDataType(0, type), GSERROR_NO_ENTRY);
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    manager_->transactionQueueCache_[100] = trans; // 100 : seq
    EXPECT_EQ(manager_->QueryMetaDataType(100, type), GSERROR_OK); // 100 : seq
    manager_->transactionQueueCache_.clear();
}
/**
 * @tc.name: GetMetaData001
 * @tc.desc: GetMetaData
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, GetMetaData001, TestSize.Level1)
{
    std::vector<GraphicHDRMetaData> metaData;
    EXPECT_EQ(manager_->GetMetaData(0, metaData), GSERROR_NO_ENTRY);
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    manager_->transactionQueueCache_[100] = trans; // 100 : seq
    EXPECT_EQ(manager_->GetMetaData(100, metaData), GSERROR_OK); // 100 : seq
    manager_->transactionQueueCache_.clear();
}
/**
 * @tc.name: GetMetaDataSet001
 * @tc.desc: GetMetaDataSet
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, GetMetaDataSet001, TestSize.Level1)
{
    GraphicHDRMetadataKey key;
    std::vector<uint8_t> metaDataSet;
    EXPECT_EQ(manager_->GetMetaDataSet(0, key, metaDataSet), GSERROR_NO_ENTRY);
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    manager_->transactionQueueCache_[100] = trans; // 100 : seq
    EXPECT_EQ(manager_->GetMetaDataSet(100, key, metaDataSet), GSERROR_OK); // 100 : seq
    manager_->transactionQueueCache_.clear();
}
/**
 * @tc.name: UpdateTransactionConfig
 * @tc.desc: UpdateTransactionConfig
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, UpdateTransactionConfig001, TestSize.Level1)
{
    const RSTransactionManagerConfig config;
    EXPECT_EQ(manager_->UpdateTransactionConfig(config), GSERROR_OK);
}
/**
 * @tc.name: GetLastFlushedDesiredPresentTimeStamp001
 * @tc.desc: GetLastFlushedDesiredPresentTimeStamp
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, GetLastFlushedDesiredPresentTimeStamp001, TestSize.Level1)
{
    int64_t lastFlushedDesiredPresentTimeStamp;
    EXPECT_EQ(manager_->GetLastFlushedDesiredPresentTimeStamp(lastFlushedDesiredPresentTimeStamp), GSERROR_OK);
}
/**
 * @tc.name: GetBufferSupportFastCompose001
 * @tc.desc: GetBufferSupportFastCompose
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, GetBufferSupportFastCompose001, TestSize.Level1)
{
    bool bufferSupportFastCompose;
    EXPECT_EQ(manager_->GetBufferSupportFastCompose(bufferSupportFastCompose), GSERROR_OK);
}
/**
 * @tc.name: GetCycleBuffersNumber001
 * @tc.desc: GetCycleBuffersNumber
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, GetCycleBuffersNumber001, TestSize.Level1)
{
    uint32_t cycleBuffersNumber;
    manager_->rotatingBufferNumber_ = 0;
    EXPECT_EQ(manager_->GetCycleBuffersNumber(cycleBuffersNumber), GSERROR_OK);
    manager_->rotatingBufferNumber_ = 1;
    EXPECT_EQ(manager_->GetCycleBuffersNumber(cycleBuffersNumber), GSERROR_OK);
}
/**
 * @tc.name: ReleaseBufferLocked001
 * @tc.desc: ReleaseBufferLocked
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReleaseBufferLocked001, TestSize.Level1)
{
    sptr<SyncFence> fence = new SyncFence(1000);
    EXPECT_CALL(*callback_, OnCompleted(testing::_)).WillOnce(testing::Return(GSERROR_INTERNAL));
    auto ret = manager_->ReleaseBufferLocked(1, fence);
    EXPECT_EQ(ret, GSERROR_INTERNAL);
    EXPECT_CALL(*callback_, OnCompleted(testing::_)).WillOnce(testing::Return(GSERROR_OK));
    ret = manager_->ReleaseBufferLocked(1, fence);
    EXPECT_EQ(ret, GSERROR_OK);
}
/**
 * @tc.name: ReleaseBuffer001
 * @tc.desc: ReleaseBuffer
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReleaseBuffer001, TestSize.Level1)
{
    sptr<SyncFence> fence = new SyncFence(1000);
    sptr<SurfaceBuffer> buffer = nullptr;
    EXPECT_EQ(manager_->ReleaseBuffer(buffer, fence), GSERROR_INVALID_ARGUMENTS);
    buffer = OHOS::SurfaceBuffer::Create();
    EXPECT_EQ(manager_->ReleaseBuffer(buffer, fence), GSERROR_OK);
    EXPECT_CALL(*callback_, OnCompleted(testing::_)).WillOnce(testing::Return(GSERROR_INTERNAL));
    EXPECT_EQ(manager_->ReleaseBuffer(buffer, fence), GSERROR_INTERNAL);
}
/**
 * @tc.name: OnBufferDeleteForRS001
 * @tc.desc: OnBufferDeleteForRS
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, OnBufferDeleteForRS001, TestSize.Level1)
{
    EXPECT_EQ(manager_->onBufferDeleteForRSMainThread_, nullptr);
    EXPECT_EQ(manager_->onBufferDeleteForRSHardwareThread_, nullptr);
    manager_->onBufferDeleteForRSHardwareThread_ = OnDeleteBufferFunc;
    manager_->onBufferDeleteForRSMainThread_ = OnDeleteBufferFunc;
    manager_->OnBufferDeleteForRS(0);
}
/**
 * @tc.name: DumpCurrentFrameLayer001
 * @tc.desc: DumpCurrentFrameLayer
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, DumpCurrentFrameLayer001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> tx = sptr<Transaction>(new Transaction(buffer));
    manager_->pendingTransactionQueue_.push_back(tx);
    manager_->pendingTransactionQueue_.push_back(nullptr);
    ASSERT_EQ(manager_->pendingTransactionQueue_.size(), 2);
    manager_->DumpCurrentFrameLayer();
    system::SetParameter("debug.dumpstaticframe.enabled", "1");
    manager_->DumpCurrentFrameLayer();
}
/**
 * @tc.name: GetLastConsumerTime001
 * @tc.desc: GetLastConsumerTime
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, GetLastConsumerTime001, TestSize.Level1)
{
    int64_t lastConsumeTime;
    ASSERT_EQ(manager_->GetLastConsumeTime(lastConsumeTime), GSERROR_OK);
}
/**
 * @tc.name: GetSurfaceBufferTransformType001
 * @tc.desc: GetSurfaceBufferTransformType
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, GetSurfaceBufferTransformType001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    GraphicTransformType transformType;
    ASSERT_EQ(manager_->GetSurfaceBufferTransformType(nullptr, nullptr), SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(manager_->GetSurfaceBufferTransformType(nullptr, &transformType), SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(manager_->GetSurfaceBufferTransformType(buffer, nullptr), SURFACE_ERROR_INVALID_PARAM);
    ASSERT_EQ(manager_->GetSurfaceBufferTransformType(buffer, &transformType), GSERROR_OK);
}
/**
 * @tc.name: WriteToMessageParcel001
 * @tc.desc: WriteToMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, WriteToMessageParcel001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    trans->buffer_ = nullptr;
    trans->fence_ = nullptr;
    EXPECT_EQ(trans->WriteToMessageParcel(parcel, true), GSERROR_INTERNAL);
    trans->fence_ = new SyncFence(1000);
    EXPECT_EQ(trans->WriteToMessageParcel(parcel, true), GSERROR_INTERNAL);
    trans->buffer_ = OHOS::SurfaceBuffer::Create();
    trans->fence_ = nullptr;
    EXPECT_EQ(trans->WriteToMessageParcel(parcel, true), GSERROR_INTERNAL);
    trans->buffer_ = OHOS::SurfaceBuffer::Create();
    trans->fence_ = new SyncFence(1000);
    EXPECT_EQ(trans->WriteToMessageParcel(parcel, true), GSERROR_NOT_INIT);
    for (uint32_t i = 0; i < SURFACE_PARCEL_SIZE_LIMIT + 1; i++) {
        Rect rect = { 0, 0, 0, 0 };
        trans->damages_.emplace_back(rect);
    }
    EXPECT_EQ(trans->WriteToMessageParcel(parcel, true), GSERROR_INVALID_ARGUMENTS);
    trans->damages_.clear();
    Rect rect = { 0, 0, 0, 0 };
    trans->damages_.emplace_back(rect);
    EXPECT_EQ(trans->WriteToMessageParcel(parcel, true), GSERROR_NOT_INIT);
    TransactionInfo info;
    EXPECT_EQ(trans->ReadTransactionInfoFromMessageParcel(parcel, info), GSERROR_INVALID_ARGUMENTS);
}
/**
 * @tc.name: WriteToMessageParcel002
 * @tc.desc: WriteToMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, WriteToMessageParcel002, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    BufferHandle* handle = new BufferHandle();
    buffer->SetBufferHandle(handle);
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    trans->buffer_ = buffer;
    trans->fence_ = new SyncFence(1000);  // 1000 : fd
    trans->damages_.clear();
    Rect rect = { 0, 0, 0, 0 };
    trans->damages_.emplace_back(rect);
    EXPECT_EQ(trans->WriteToMessageParcel(parcel, true), GSERROR_OK);
    trans = nullptr;
    buffer = nullptr;
    handle = nullptr;
}
/**
 * @tc.name: WriteToMessageParcel003
 * @tc.desc: WriteToMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, WriteToMessageParcel003, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    BufferHandle* handle = new BufferHandle();
    buffer->SetBufferHandle(handle);
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    trans->buffer_ = buffer;
    trans->fence_ = new SyncFence(1000);  // 1000 : fd
    trans->damages_.clear();
    Rect rect = { 0, 0, 0, 0 };
    trans->damages_.emplace_back(rect);
    EXPECT_EQ(trans->WriteToMessageParcel(parcel, false), GSERROR_OK);
    trans = nullptr;
    buffer = nullptr;
    handle = nullptr;
}
/**
 * @tc.name: ReadFromTransactionInfo001
 * @tc.desc: ReadFromTransactionInfo
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReadFromTransactionInfo001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    const TransactionInfo info;
    EXPECT_EQ(trans->ReadFromTransactionInfo(info), GSERROR_OK);
}
/**
 * @tc.name: ReadTransactionInfoFromMessageParcel001
 * @tc.desc: ReadTransactionInfoFromMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReadTransactionInfoFromMessageParcel001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    parcel.WriteUint32(SURFACE_PARCEL_SIZE_LIMIT + 1);
    EXPECT_EQ(trans->ReadTransactionInfoFromMessageParcel(parcel, info), GSERROR_INVALID_ARGUMENTS);
    parcel.WriteUint32(1);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    WriteSurfaceBufferImpl(parcel, buffer->GetSeqNum(), buffer);
    auto ret = trans->ReadTransactionInfoFromMessageParcel(parcel, info);
    EXPECT_EQ(ret, GSERROR_INVALID_ARGUMENTS);
}
/**
 * @tc.name: ReadTransactionInfoFromMessageParcel002
 * @tc.desc: ReadTransactionInfoFromMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReadTransactionInfoFromMessageParcel002, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    parcel.WriteUint32(1);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    parcel.WriteInt32(0);
    WriteTransactionInfoFromMessageParcel(parcel, info);
    parcel.WriteUint32(HDRMetaDataType::HDR_NOT_USED);
    auto ret = trans->ReadTransactionInfoFromMessageParcel(parcel, info);
    EXPECT_EQ(ret, GSERROR_OK);
}
/**
 * @tc.name: ReadFromMessageParcel001
 * @tc.desc: ReadFromMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReadFromMessageParcel001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    parcel.WriteUint32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    WriteSurfaceBufferImpl(parcel, buffer->GetSeqNum(), buffer);
    EXPECT_EQ(trans->ReadFromMessageParcel(parcel), nullptr);
    buffer->WriteBufferProperty(parcel);
    trans->ReadFromMessageParcel(parcel);
}
/**
 * @tc.name: ReadFromMessageParcel002
 * @tc.desc: ReadFromMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReadFromMessageParcel002, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    parcel.WriteUint32(SURFACE_PARCEL_SIZE_LIMIT + 1);
    EXPECT_EQ(trans->ReadFromMessageParcel(parcel), nullptr);
}
/**
 * @tc.name: ReadFromMessageParcel003
 * @tc.desc: ReadFromMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReadFromMessageParcel003, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    BufferHandle* handle = new BufferHandle();
    handle->fd = 1000;
    handle->reserveFds = 0;
    handle->reserveInts = 0;
    buffer->SetBufferHandle(handle);
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    parcel.WriteUint32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    WriteSurfaceBufferImpl(parcel, buffer->GetSeqNum(), buffer);
    buffer->WriteBufferProperty(parcel);
    parcel.WriteInt64(1);
    parcel.WriteInt64(1);
    parcel.WriteBool(false);
    trans->WriteMetaDataToMessageParcel(parcel, HDRMetaDataType::HDR_META_DATA_SET, info.metaData, info.metaDataKey,
        info.metaDataSet);
    EXPECT_EQ(trans->ReadFromMessageParcel(parcel), nullptr);
    trans = nullptr;
    buffer = nullptr;
    handle = nullptr;
}
/**
 * @tc.name: ReadFromMessageParcel004
 * @tc.desc: ReadFromMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReadFromMessageParcel004, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    BufferHandle* handle = new BufferHandle();
    handle->fd = 1000;
    handle->reserveFds = 0;
    handle->reserveInts = 0;
    buffer->SetBufferHandle(handle);
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    parcel.WriteUint32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    parcel.WriteInt32(1);
    WriteSurfaceBufferImpl(parcel, buffer->GetSeqNum(), buffer);
    buffer->WriteBufferProperty(parcel);
    parcel.WriteInt64(1);
    parcel.WriteInt64(1);
    parcel.WriteBool(false);
    trans->WriteMetaDataToMessageParcel(parcel, HDRMetaDataType::HDR_NOT_USED, info.metaData, info.metaDataKey,
        info.metaDataSet);
    EXPECT_EQ(trans->ReadFromMessageParcel(parcel), nullptr);
    trans = nullptr;
    buffer = nullptr;
    handle = nullptr;
}
/**
 * @tc.name: ReadMetaDataFromMessageParcel001
 * @tc.desc: ReadMetaDataFromMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, ReadMetaDataFromMessageParcel001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    parcel.WriteUint32(0);
    auto ret = trans->ReadMetaDataFromMessageParcel(parcel, info.hdrMetaDataType, info.metaData, info.metaDataKey,
        info.metaDataSet);
    EXPECT_EQ(ret, GSERROR_OK);
    parcel.WriteUint32(1);
    std::vector<GraphicHDRMetaData> metaData = {{GRAPHIC_MATAKEY_RED_PRIMARY_X, 1}};
    WriteHDRMetaData(parcel, metaData);
    parcel.WriteUint32(0);
    auto metaDataSet = std::vector<uint8_t>(5);
    WriteHDRMetaDataSet(parcel, metaDataSet);
    ret = trans->ReadMetaDataFromMessageParcel(parcel, info.hdrMetaDataType, info.metaData, info.metaDataKey,
        info.metaDataSet);
    EXPECT_EQ(ret, GSERROR_OK);
}
/**
 * @tc.name: WriteMetaDataToMessageParcel001
 * @tc.desc: WriteMetaDataToMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, WriteMetaDataToMessageParcel001, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    auto ret = trans->WriteMetaDataToMessageParcel(parcel, info.hdrMetaDataType, info.metaData, info.metaDataKey,
        info.metaDataSet);
    EXPECT_EQ(ret, GSERROR_OK);
}
/**
 * @tc.name: WriteMetaDataToMessageParcel002
 * @tc.desc: WriteMetaDataToMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, WriteMetaDataToMessageParcel002, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    auto ret = trans->WriteMetaDataToMessageParcel(parcel, HDRMetaDataType::HDR_NOT_USED,
        info.metaData, info.metaDataKey, info.metaDataSet);
    EXPECT_EQ(ret, GSERROR_OK);
}
/**
 * @tc.name: WriteMetaDataToMessageParcel003
 * @tc.desc: WriteMetaDataToMessageParcel
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSTransactionManagerTest, WriteMetaDataToMessageParcel003, TestSize.Level1)
{
    sptr<SurfaceBuffer> buffer = OHOS::SurfaceBuffer::Create();
    sptr<Transaction> trans = sptr<Transaction>(new Transaction(buffer));
    MessageParcel parcel;
    TransactionInfo info;
    auto ret = trans->WriteMetaDataToMessageParcel(parcel, HDRMetaDataType::HDR_META_DATA_SET,
        info.metaData, info.metaDataKey, info.metaDataSet);
    EXPECT_EQ(ret, GSERROR_OK);
}
}