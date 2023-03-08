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
#include "memory/MemoryGraphic.h"

namespace OHOS {
namespace Rosen {
MemoryGraphic::MemoryGraphic(int32_t pid, float glSize, float graphicSize) : pid_(pid),
    glSize_(glSize), graphicSize_(graphicSize)
{

}

int32_t MemoryGraphic::GetPid() const
{
    return pid_;
}

float MemoryGraphic::GetGLMemorySize() const
{
    return glSize_;
}

float MemoryGraphic::GetGraphicMemorySize() const
{
    return graphicSize_;
}

void MemoryGraphic::SetPid(int32_t pid)
{
    pid_ = pid;
}

void MemoryGraphic::SetGLMemorySize(float glSize)
{
    glSize_ = glSize;
}

void MemoryGraphic::SetGraphicMemorySize(float graphicSize)
{
    graphicSize_ = graphicSize;
}

bool MemoryGraphic::Marshalling(Parcel& parcel) const
{
    return parcel.WriteInt32(pid_) && parcel.WriteFloat(glSize_) &&
        parcel.WriteFloat(graphicSize_);
}

MemoryGraphic* MemoryGraphic::Unmarshalling(Parcel& parcel)
{
    int32_t pid;
    float glSize;
    float graphicSize;
    if (!(parcel.ReadInt32(pid) && parcel.ReadFloat(glSize) && parcel.ReadFloat(graphicSize))) {
        return nullptr;
    }

    MemoryGraphic* mem = new MemoryGraphic(pid, glSize, graphicSize);
    return mem;
}

MemoryGraphic& MemoryGraphic::operator+=(const MemoryGraphic& other)
{
    glSize_ += other.GetGLMemorySize();
    graphicSize_ += other.GetGraphicMemorySize();
    return *this;
}

void MemoryGraphic::IncreaseGLMemory(float glSize)
{
    glSize_ += glSize;
}

void MemoryGraphic::IncreaseGraphicMemory(float graphicSize)
{
    graphicSize_ += graphicSize;
}

}
}