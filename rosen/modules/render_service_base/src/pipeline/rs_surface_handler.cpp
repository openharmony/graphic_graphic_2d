/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_surface_handler.h"

namespace OHOS {
namespace Rosen {
#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
void RSSurfaceHandler::SetConsumer(const sptr<Surface>& consumer)
{
    consumer_ = consumer;
}
#endif

void RSSurfaceHandler::IncreaseAvailableBuffer()
{
    bufferAvailableCount_++;
}

int32_t RSSurfaceHandler::ReduceAvailableBuffer()
{
    return --bufferAvailableCount_;
}

void RSSurfaceHandler::SetGlobalZOrder(float globalZOrder)
{
    globalZOrder_ = globalZOrder;
}

float RSSurfaceHandler::GetGlobalZOrder() const
{
    return globalZOrder_;
}
}
}
