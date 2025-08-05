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

#include "subtree_parallel_param.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {

void SubtreeParallelParam::SetSubtreeEnable(bool isSubtreeEnable)
{
    isSubtreeEnable_ = isSubtreeEnable;
}

bool SubtreeParallelParam::GetSubtreeEnable()
{
    return isSubtreeEnable_;
}

void SubtreeParallelParam::SetRBPolicyEnabled(bool isRBPolicyEnable)
{
    isRBPolicyEnable_ = isRBPolicyEnable;
}

bool SubtreeParallelParam::GetRBPolicyEnabled()
{
    return isRBPolicyEnable_;
}

void SubtreeParallelParam::SetMultiWinPolicyEnabled(bool isMultiWinPolicyEnable)
{
    isMultiWinPolicyEnable_ = isMultiWinPolicyEnable;
}

bool SubtreeParallelParam::GetMultiWinPolicyEnabled()
{
    return isMultiWinPolicyEnable_;
}

void SubtreeParallelParam::SetMutliWinSurfaceNum(int surfaceNumber)
{
    multiWinSurfaceNum_ = surfaceNumber;
}

int SubtreeParallelParam::GetMutliWinSurfaceNum()
{
    return multiWinSurfaceNum_;
}

void SubtreeParallelParam::SetRBChildrenWeight(int weight)
{
    rbChildrenWeight_ = weight;
}

int SubtreeParallelParam::GetRBChildrenWeight()
{
    return rbChildrenWeight_;
}

void SubtreeParallelParam::SetRBSubtreeWeight(int weight)
{
    rbSubTreeWeight_ = weight;
}

int SubtreeParallelParam::GetRBSubtreeWeight()
{
    return rbSubTreeWeight_;
}

void SubtreeParallelParam::SetSubtreeScene(std::string appName, std::string val)
{
    subtreeScene_[std::move(appName)] = std::move(val);
}

std::unordered_map<std::string, std::string>& SubtreeParallelParam::GetSubtreeScene()
{
    return subtreeScene_;
}
} // namespace OHOS::Rosen