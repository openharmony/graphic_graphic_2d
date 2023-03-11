/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "render_frame_trace.h"

namespace FRAME_TRACE {

RenderFrameTrace* RenderFrameTrace::implInstance_ = nullptr;

bool RenderFrameTrace::RenderFrameTraceIsOpen()
{
    return false;
}

bool RenderFrameTrace::RenderFrameTraceOpen()
{
    return false;
}

bool RenderFrameTrace::RenderFrameTraceClose()
{
    return false;
}

} // namespace FRAME_TRACE
