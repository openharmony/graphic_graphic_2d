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

#ifndef ROSEN_MODULE_FRAME_LOAD_RS_FRAME_DEADLINE_PREDICT_H
#define ROSEN_MODULE_FRAME_LOAD_RS_FRAME_DEADLINE_PREDICT_H

#include <string>

#include "hgm_core.h"
#include "rs_frame_report.h"
#include "rs_trace.h"
#include "vsync_generator.h"

namespace OHOS::Rosen {
class RsFrameDeadlinePredict {
public:
	static RsFrameDeadlinePredict& GetInstance();
	void ReportRsFrameDeadline(OHOS::Rosen::HgmCore& hgmCore, bool forceRefreshFlag);

private:
	RsFrameDeadlinePredict();
	~RsFrameDeadlinePredict() = default;

	int64_t preIdealPeriod_ = 0;
	int64_t preExtraReserve_ = 0;
};
} // namespace OHOS::Rosen
#endif // ROSEN_MODULE_FRAME_LOAD_RS_FRAME_DEADLINE_PREDICT_H
