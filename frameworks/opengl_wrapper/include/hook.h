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
#ifndef FRAMEWORKS_OPENGL_WRAPPER_HOOK_H
#define FRAMEWORKS_OPENGL_WRAPPER_HOOK_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl31.h>
#include <GLES3/gl32.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#ifdef OPENGL_WRAPPER_ENABLE_GL4
#include <GL/gl.h>
#include <GL/glcorearb.h>
#include <GL/glext.h>
#endif

namespace OHOS {
#undef CALL_HOOK_API
#define CALL_HOOK_API(...)
#undef CALL_HOOK_API_RET
#define CALL_HOOK_API_RET CALL_HOOK_API
#undef HOOK_API_ENTRY
#define HOOK_API_ENTRY(r, api, ...) r (*(api))(__VA_ARGS__);

constexpr int32_t EGL_API_NUM = 100;
constexpr int32_t GLES_API_NUM = 1000;
#ifdef OPENGL_WRAPPER_ENABLE_GL4
constexpr int32_t OPENGL_API_NUM = 3000;
#endif

// for compatibility, if xxx_entries.in file modified, the value of g_entriesFilesVersion must be increased by 1.
constexpr uint64_t ENTRIES_FILES_VERSION = 1;

struct WrapperHookTable {
    #include "wrapper_hook_entries.in"
};

struct EglHookTable {
    #include "egl_hook_entries.in"
};

struct GlHookTable1 {
    #include "gl1_hook_entries.in"
};

struct GlHookTable2 {
    #include "gl2_hook_entries.in"
};

struct GlHookTable3 {
    #include "gl3_hook_entries.in"
};

#ifdef OPENGL_WRAPPER_ENABLE_GL4
struct GlHookTable4 {
    #include "gl4_hook_entries.in"
};
#endif


struct GlHookTable {
    GlHookTable1 table1;
    GlHookTable2 table2;
    GlHookTable3 table3;
#ifdef OPENGL_WRAPPER_ENABLE_GL4
    GlHookTable4 table4;
#endif
};
} // namespace OHOS
#endif // FRAMEWORKS_OPENGL_WRAPPER_HOOK_H
