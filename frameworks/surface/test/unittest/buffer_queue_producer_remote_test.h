/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_QUEUE_PRODUCER_REMOTE_TEST_H
#define FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_QUEUE_PRODUCER_REMOTE_TEST_H

#include <display_type.h>
#include <gtest/gtest.h>
#include <iservice_registry.h>
#include <surface.h>

#include "buffer_extra_data_impl.h"
#include "buffer_queue_producer.h"

namespace OHOS {
class BufferQueueProducerRemoteTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline BufferRequestConfig requestConfig = {
        .width = 0x100,
        .height = 0x100,
        .strideAlignment = 0x8,
        .format = PIXEL_FMT_RGBA_8888,
        .usage = HBM_USE_CPU_READ | HBM_USE_CPU_WRITE | HBM_USE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig = {
        .damage = {
            .w = 0x100,
            .h = 0x100,
        },
    };
    static inline std::vector<int32_t> deletingBuffers;
    static inline int64_t timestamp = 0;
    static inline Rect damage = {};
    static inline sptr<IRemoteObject> robj = nullptr;
    static inline sptr<IBufferProducer> bp = nullptr;
    static inline sptr<BufferQueue> bq = nullptr;
    static inline sptr<BufferQueueProducer> bqp = nullptr;
    static inline BufferExtraDataImpl bedata;
    static inline int32_t systemAbilityID = 345154;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_QUEUE_PRODUCER_REMOTE_TEST_H
