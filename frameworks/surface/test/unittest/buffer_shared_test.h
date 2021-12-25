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

#ifndef FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_SHARED_TEST_H
#define FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_SHARED_TEST_H

#include <map>

#include <display_type.h>
#include <gtest/gtest.h>
#include <surface.h>
#include "test_header.h"

#include "buffer_extra_data_impl.h"
#include "buffer_queue.h"

namespace OHOS {
class BufferSharedTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    static inline sptr<Surface> surf = nullptr;
    static inline sptr<Surface> producerSurface1 = nullptr;
    static inline sptr<Surface> producerSurface2 = nullptr;
    static inline sptr<SurfaceBuffer> buffer1 = nullptr;
    static inline sptr<SurfaceBuffer> buffer2 = nullptr;
    static inline sptr<SurfaceBuffer> sbuffer1 = nullptr;
    static inline sptr<SurfaceBuffer> sbuffer2 = nullptr;
};
} // namespace OHOS


#endif // FRAMEWORKS_SURFACE_TEST_UNITTEST_BUFFER_QUEUE_TEST_H
