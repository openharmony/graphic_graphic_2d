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

#ifndef SKIA_SERIAL_PROCS_H
#define SKIA_SERIAL_PROCS_H

#include "impl_interface/serial_procs_impl.h"
#include "include/core/SkSerialProcs.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class SkiaSerialProcs : public SerialProcsImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaSerialProcs() noexcept;
    ~SkiaSerialProcs() override
    {
        delete skSerialProcs_;
    }

    AdapterType GetType() const override
    {
        return AdapterType::SKIA_ADAPTER;
    }

    SkSerialProcs* GetSkSerialProcs() const;

    void SetHasTypefaceProc(bool flag) override;
    bool HasTypefaceProc()
    {
        return hasTypefaceProcs_;
    }

private:
    SkSerialProcs* skSerialProcs_;
    bool hasTypefaceProcs_ = false;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif