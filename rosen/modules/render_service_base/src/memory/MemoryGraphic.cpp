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
MemoryGraphic::MemoryGraphic(int32_t pid, float cpuMemSize, float gpuMemSize) : pid_(pid),
    cpuMemSize_(cpuMemSize), gpuMemSize_(gpuMemSize)
{

}

int32_t MemoryGraphic::GetPid() const
{
    return pid_;
}

float MemoryGraphic::GetGpuMemorySize() const
{
    return cpuMemSize_;
}

float MemoryGraphic::GetCpuMemorySize() const
{
    return gpuMemSize_;
}

void MemoryGraphic::SetPid(int32_t pid)
{
    pid_ = pid;
}

void MemoryGraphic::SetGpuMemorySize(float cpuMemSize)
{
    cpuMemSize_ = cpuMemSize;
}

void MemoryGraphic::SetCpuMemorySize(float gpuMemSize)
{
    gpuMemSize_ = gpuMemSize;
}

bool MemoryGraphic::Marshalling(Parcel& parcel) const
{
    return parcel.WriteInt32(pid_) && parcel.WriteFloat(cpuMemSize_) &&
        parcel.WriteFloat(gpuMemSize_);
}

MemoryGraphic* MemoryGraphic::Unmarshalling(Parcel& parcel)
{
    int32_t pid;
    float cpuMemSize;
    float gpuMemSize;
    if (!(parcel.ReadInt32(pid) && parcel.ReadFloat(cpuMemSize) && parcel.ReadFloat(gpuMemSize))) {
        return nullptr;
    }

    MemoryGraphic* mem = new MemoryGraphic(pid, cpuMemSize, gpuMemSize);
    return mem;
}

MemoryGraphic& MemoryGraphic::operator+=(const MemoryGraphic& other)
{
    cpuMemSize_ += other.GetCpuMemorySize();
    gpuMemSize_ += other.GetGpuMemorySize();
    return *this;
}

void MemoryGraphic::IncreaseCpuMemory(float cpuMemSize)
{
    cpuMemSize_ += cpuMemSize;
}

void MemoryGraphic::IncreaseGpuMemory(float gpuMemSize)
{
    gpuMemSize_ += gpuMemSize;
}

}
}