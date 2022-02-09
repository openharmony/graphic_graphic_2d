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

#ifndef FILTER_H
#define FILTER_H

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GLES3/gl32.h>
#include "cJSON.h"
#include "program.h"
#include "ec_log.h"

namespace OHOS {
namespace Rosen {
enum class FILTER_TYPE { INPUT, ALGOFILTER, MEGERFILTER, OUTPUT };

struct ProcessData {
    GLuint srcTextureID;
    GLuint dstTextureID;
    GLuint frameBufferID;
    int textureWidth;
    int textureHeight;
};

class Filter {
public:
    Filter() {};
    virtual ~Filter() {};
    virtual FILTER_TYPE GetFilterType() = 0;
    virtual void Process(ProcessData& data);
    virtual void DoProcess(ProcessData& data) = 0;
    virtual void AddNextFilter(std::shared_ptr<Filter> next);
    virtual void AddPreviousFilter(std::shared_ptr<Filter> previous);
    virtual std::shared_ptr<Filter> GetNextFilter();
    virtual std::shared_ptr<Filter> GetPreviousFilter();
    virtual int GetInputNumber();
    virtual int GetOutputNumber();
    virtual int GetMaxInputNumber();
    virtual int GetMaxOutputNumber();
    virtual void SetValue(const std::string& key, void* value, int size) {};

protected:
    int nextNum_ = 0;
    int preNum_ = 0;
    int nextPtrMax_ = 1;
    int prePtrMax_ = 1;
    std::shared_ptr<Filter> previous_ = nullptr;
    std::shared_ptr<Filter> next_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // FILTER_H
