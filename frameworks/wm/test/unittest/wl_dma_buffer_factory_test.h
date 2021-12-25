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

#ifndef FRAMEWORKS_WM_TEST_UNITTEST_WL_DMA_BUFFER_FACTORY_TEST_H
#define FRAMEWORKS_WM_TEST_UNITTEST_WL_DMA_BUFFER_FACTORY_TEST_H

#include <gtest/gtest.h>
#include <window_manager.h>

#include "wl_dma_buffer_factory.h"

namespace OHOS {
class WlDMABufferFactoryTest : public testing::Test, public IBufferConsumerListenerClazz {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    virtual void OnBufferAvailable() override;

private:
    static inline GSError initRet = GSERROR_NOT_INIT;
    static inline sptr<Surface> csurf = nullptr;
    static inline sptr<Surface> psurf = nullptr;
    static inline sptr<SurfaceBuffer> sbuffer = nullptr;
};
}

#endif // FRAMEWORKS_WM_TEST_UNITTEST_WL_DMA_BUFFER_FACTORY_TEST_H
