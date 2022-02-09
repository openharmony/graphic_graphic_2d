/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "algo_filter.h"

namespace OHOS {
namespace Rosen {
AlgoFilter::AlgoFilter()
{
    mesh_ = new Mesh();
    mesh_->Use();
}

AlgoFilter::~AlgoFilter()
{
    delete program_;
    delete mesh_;
}

void AlgoFilter::Prepare(ProcessData& data)
{
    glBindTexture(GL_TEXTURE_2D, data.dstTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.textureWidth, data.textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindFramebuffer(GL_FRAMEBUFFER, data.frameBufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data.dstTextureID, 0);
    glViewport(0, 0, data.textureWidth, data.textureHeight);
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void AlgoFilter::Draw(ProcessData& data)
{
    Use();
    glBindVertexArray(mesh_->VAO_);
    glBindTexture(GL_TEXTURE_2D, data.srcTextureID);
    glDrawElements(GL_TRIANGLES, DRAW_ELEMENTS_NUMBER, GL_UNSIGNED_INT, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void AlgoFilter::CreateProgram(const std::string& vertexString, const std::string& fragmentString)
{
    program_ = new Program();
    program_->Compile(vertexString, fragmentString);
}

void AlgoFilter::Use()
{
    if (program_ != nullptr) {
        program_->UseProgram();
    }
}

FILTER_TYPE AlgoFilter::GetFilterType()
{
    return FILTER_TYPE::ALGOFILTER;
}

void AlgoFilter::DoProcess(ProcessData& data)
{
    Prepare(data);
    LoadFilterParams();
    Draw(data);
}
} // namespcae Rosen
} // namespace OHOS