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

#include "mock_rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
namespace Mock {
std::once_flag RSScreenManagerMock::createFlag_;
sptr<OHOS::Rosen::Mock::RSScreenManagerMock> RSScreenManagerMock::instance_ = nullptr;

RSScreenManagerMock::RSScreenManagerMock() = default;
RSScreenManagerMock::~RSScreenManagerMock() = default;

sptr<RSScreenManagerMock> RSScreenManagerMock::GetInstance()
{
    std::call_once(createFlag_, []() {
        instance_ = new RSScreenManagerMock();
    });

    return instance_;
}

} // namespace Mock
} // namespace Rosen
} // namespace OHOS