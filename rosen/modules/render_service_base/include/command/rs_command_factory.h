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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_FACTORY_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_FACTORY_H

#ifdef ROSEN_OHOS
#include <parcel.h>
#include <unordered_map>

#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
class RSCommand;
using UnmarshallingFunc = RSCommand* (*)(Parcel& parcel);

class RSCommandFactory {
public:
    static RSCommandFactory& Instance();

    void Register(uint16_t type, uint16_t subtype, UnmarshallingFunc func);
    UnmarshallingFunc GetUnmarshallingFunc(uint16_t type, uint16_t subtype);

private:
    RSCommandFactory() = default;
    ~RSCommandFactory() = default;

    std::unordered_map<uint32_t, UnmarshallingFunc> unmarshallingFuncLUT_;
};

// Helper class for automatically registry
template<uint16_t commandType, uint16_t commandSubType, UnmarshallingFunc func>
class RSCommandRegister {
public:
    RSCommandRegister()
    {
        RSCommandFactory::Instance().Register(commandType, commandSubType, func);
    }
};

} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_COMMAND_FACTORY_H
