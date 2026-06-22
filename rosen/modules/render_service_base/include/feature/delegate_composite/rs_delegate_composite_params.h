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
#ifndef RS_DELEGATE_COMPOSITE_PARAMS
#define RS_DELEGATE_COMPOSITE_PARAMS

#include <atomic>
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RsDelegateCompositeParams {
public:
    RsDelegateCompositeParams(std::string name, NodeId nodeId);

    void SetDelegateDstRect(float positionX, float positionY, float positionZ, float positionW);
    Vector4f GetDelegateDstRect();

    void SetDelegateSrcRect(float positionX, float positionY, float positionZ, float positionW);
    Vector4f GetDelegateSrcRect();

    void SetDelegateMode(bool isSetDelegateMode);
    bool GetDelegateMode();

private:
    Vector4f delegateDstRect_ = { 0.0f, 0.0f, 0.0f, 0.0f };
    Vector4f delegateSrcRect_ = { 0.0f, 0.0f, 9999.0f, 9999.0f };
    std::atomic<bool> isSetDelegateMode_ = false;
    std::string name_ = "Unnamed";
    NodeId nodeId_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_DELEGATE_COMPOSITE_PARAMS