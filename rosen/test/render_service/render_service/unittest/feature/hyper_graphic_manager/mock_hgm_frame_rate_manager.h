/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef MOCK_HGM_FRAME_RATE_MANAGER_H
#define MOCK_HGM_FRAME_RATE_MANAGER_H

#include <gmock/gmock.h>

#include "hgm_frame_rate_manager.h"

namespace OHOS {
namespace Rosen {
namespace Mock {
class MockHgmFrameRateManager : public HgmFrameRateManager {
public:
    MockHgmFrameRateManager() = default;
    virtual ~MockHgmFrameRateManager() = default;

    MOCK_CONST_METHOD0(AdaptiveStatus, int32_t());
    MOCK_CONST_METHOD0(IsGameNodeOnTree, bool());
    MOCK_METHOD0(IsNeedAdaptiveAfterUpdateMode, bool());
};
} // namespace Mock
} // namespace Rosen
} // namespace OHOS
#endif // MOCK_HGM_FRAME_RATE_MANAGER_H