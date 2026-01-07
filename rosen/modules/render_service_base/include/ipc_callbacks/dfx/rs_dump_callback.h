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
#ifndef RS_IDUMP_CALLBACK_H
#define RS_IDUMP_CALLBACK_H

#include <iremote_broker.h>
#include <string>

namespace OHOS {
namespace Rosen {
class RSIDumpCallback : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(U"ohos.rosen.DumpCallback");
    RSIDumpCallback() = default;
    virtual ~RSIDumpCallback() noexcept = default;
    virtual void OnDumpResult(std::string& dumpResult) = 0;
};


} // namespace Rosen
} // namespace OHOS

#endif // RS_IDUMP_CALLBACK_H