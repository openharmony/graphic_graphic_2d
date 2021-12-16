/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "texture.h"

#include <GLES2/gl2ext.h>

#include <gslogger.h>

namespace {
//DEFINE_HILOG_LABEL("AnimationServer::Texture");
} // namespace

Texture::Texture(void *buffer, int32_t width, int32_t height)
    : buffer_(buffer), width_(width), height_(height)
{
    glGenTextures(1, &rendererID_);
    glBindTexture(GL_TEXTURE_2D, rendererID_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA_EXT, width_, height_, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, buffer_);
}

Texture::~Texture()
{
    glDeleteTextures(1, &rendererID_);
}

void Texture::Bind(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, rendererID_);
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
