/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef UIEFFECT_FILTER_FLY_OUT_PARA_H
#define UIEFFECT_FILTER_FLY_OUT_PARA_H
#include <iostream>
#include "filter_para.h"
#include "common/rs_vector2.h"
 
namespace OHOS {
namespace Rosen {
class FlyOutPara : public FilterPara {
public:
    FlyOutPara()
    {
        this->type_ = FilterPara::ParaType::FLY_OUT;
    }
    ~FlyOutPara() override = default;
    
    void SetDegree(float degree)
    {
        degree_ = degree;
    }
 
    float GetDegree() const
    {
        return degree_;
    }
 
    void SetFlyMode(uint32_t flyMode)
    {
        flyMode_ = flyMode;
    }
 
    uint32_t GetFlyMode () const
    {
        return flyMode_;
    }
 
private:
    float degree_ = 0.0f;
    uint32_t flyMode_ = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // UIEFFECT_FILTER_FLY_OUT_PARA_H