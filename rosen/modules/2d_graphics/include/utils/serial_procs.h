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

#ifndef SERIAL_PROCS_H
#define SERIAL_PROCS_H

#include "impl_interface/serial_procs_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

class SerialProcs {
public:
    SerialProcs() noexcept;
    virtual ~SerialProcs() {}
    template <typename T>
    T* GetImpl() const
    {
        return serialProcsImplPtr_->DowncastingTo<T>();
    }

    void SetHasTypefaceProc(bool flag = false)
    {
        hasTypefaceProc_ = flag;
        if (serialProcsImplPtr_ != nullptr) {
            serialProcsImplPtr_->SetHasTypefaceProc(hasTypefaceProc_);
        }
    }

    bool HasTypefaceProc()
    {
        return hasTypefaceProc_;
    }
    
private:
    std::shared_ptr<SerialProcsImpl> serialProcsImplPtr_;
    bool hasTypefaceProc_ = false;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif