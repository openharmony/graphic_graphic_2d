/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_I_CONSYMER_SURFACE_MOCK_H
#define GRAPHIC_I_CONSYMER_SURFACE_MOCK_H

#include "iconsumer_surface.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
namespace Mock {

class MockIConsumerSurface : public IConsumerSurface {
public:
    MockIConsumerSurface() = default;
    virtual ~ MockIConsumerSurface() = default;

    MOCK_METHOD1(Create, sptr<IConsumerSurface>(std::string));
    MOCK_METHOD1(RegisterConsumerListener, SurfaceError(sptr<IBufferConsumerListener>&));
    MOCK_METHOD3(AcquireBuffer, GSError(AcquireBufferReturnValue&, int64_t, bool));
    MOCK_METHOD2(ReleaseBuffer, GSError(sptr<SurfaceBuffer>&, const sptr<SyncFence>&));
    MOCK_METHOD(GetName, std::string&());
};

} // namespace Mock
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHIC_I_CONSYMER_SURFACE_MOCK_H