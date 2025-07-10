/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef GRAPHIC_RS_SCREEN_MANAGER_MOCK_H
#define GRAPHIC_RS_SCREEN_MANAGER_MOCK_H

#include "screen_manager/rs_screen_manager.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
namespace Mock {
class RSScreenManagerMock : public OHOS::Rosen::impl::RSScreenManager {
public:
    static sptr<RSScreenManagerMock> GetInstance();

    MOCK_METHOD2(SetScreenActiveMode, uint32_t(ScreenId, uint32_t));
private:
    RSScreenManagerMock();
    ~RSScreenManagerMock() override;
    RSScreenManagerMock(const RSScreenManagerMock&) = delete;
    RSScreenManagerMock& operator=(const RSScreenManagerMock&) = delete;

    static std::once_flag createFlag_;
    static sptr<OHOS::Rosen::Mock::RSScreenManagerMock> instance_;
};
} // namespace Mock
} // namespace Rosen
} // namespace OHOS
#endif // GRAPHIC_RS_SCREEN_MANAGER_MOCK_H