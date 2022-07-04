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

#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include "algo_filter.h"
#include "image_source.h"
#include "image_packer.h"

namespace OHOS {
namespace Rosen {
class Output : public AlgoFilter {
public:
    static constexpr int COLOR_CHANNEL = 4;
    Output();
    virtual ~Output() {};
    void SetValue(const std::string& key, std::shared_ptr<void> value, int size) override;
    std::string GetVertexShader() override;
    std::string GetFragmentShader() override;
    virtual FILTER_TYPE GetFilterType() override;
    std::unique_ptr<OHOS::Media::PixelMap> GetPixelMap();
    std::shared_ptr<uint8_t> GetColorBuffer();

protected:
    void DoProcess(ProcessData& data) override;
    void EncodeToFile(ProcessData& data);
    void EncodeToPixelMap(ProcessData& data);
    void WriteToBuffer(ProcessData& data);
    std::string format_;
    std::string dstImagePath_;
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap_ = nullptr;
    std::shared_ptr<uint8_t> colorBuffer_ = nullptr;

private:
    void LoadFilterParams() override {};
};
} // namespace Rosen
} // namespace OHOS
#endif // OUTPUT_H

