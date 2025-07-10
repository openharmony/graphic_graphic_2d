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

#ifndef HGM_USER_DEFINE_H
#define HGM_USER_DEFINE_H

#include <memory>

#include "hgm_command.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

namespace OHOS {
namespace Rosen {
class HgmUserDefineImpl;
class HgmUserDefine {
public:
    HgmUserDefine();
    ~HgmUserDefine() = default;
    void Init();
private:
    std::shared_ptr<HgmUserDefineImpl> impl_;
};
} // namespace Rosen
} // namespace OHOS
#endif // HGM_USER_DEFINE_H
