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

#ifndef INPUT_H
#define INPUT_H

#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include "filter.h"
#include "mesh.h"
#include "image_source.h"

namespace OHOS {
namespace Rosen {
class Input : public Filter {
public:
    Input() {};
    virtual ~Input() {};
    void SetValue(const std::string& key, void *value, int size) override;
    void DoProcess(ProcessData& data) override;
    virtual FILTER_TYPE GetFilterType() override;

protected:
    std::string format_;
    std::string srcImagePath_;

private:
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // INPUT_H
