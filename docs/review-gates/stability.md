# 稳定性检视

适用于涉及 C/C++ 代码行为、BUILD、接口/IPC 或跨模块逻辑的任务。
目标是在 Agent 交付代码前看护异常处理、并发、资源、生命周期、
内存和图形稳定性风险。

## 工具

`stability-code-review` 是团队外部检查工具/skill，不是 Agent 内置能力。
默认由 OpenCode 加载，持久安装目录为：

```sh
${OPENCODE_SKILLS_DIR:-$HOME/.config/opencode/skills}/ohos-dev-graphics-stability-code-review
```

npm 包只是技能安装器，不作为仓库依赖。禁止在待检视仓库中执行 `npm i`，
也不使用仓库的 `npm ls` 判断技能是否已安装。

### 版本检查

执行前查询 registry 最新版本，并读取技能目录中的 `.npm-version`：

```sh
stability_registry_version=$(npm view @ohos-graphics/stability-code-review version)
stability_opencode_root="${OPENCODE_SKILLS_DIR:-$HOME/.config/opencode/skills}"
stability_skill_dir="$stability_opencode_root/ohos-dev-graphics-stability-code-review"
stability_version_file="$stability_skill_dir/.npm-version"
```

同时存在 `SKILL.md` 和 `.npm-version`，且记录版本等于 registry 最新版本时，
直接使用已安装技能，不重复安装。

技能目录或版本文件缺失、记录版本较旧时，才执行临时安装。
旧安装没有 `.npm-version` 时允许一次迁移安装，之后按版本文件判断。

若网络、内网 registry 或权限问题导致无法确认最新版本，需要在最终回复说明版本缺口。
若 Agent 负责 commit 或 push，版本无法确认属于门禁不可用，不继续 push。

### 临时安装与升级

安装包只落在任务专用临时目录。安装器将技能复制到 OpenCode 持久目录后，
写入版本文件：

```sh
stability_review_tmp=$(mktemp -d /tmp/stability-code-review.XXXXXX)
npm install --prefix "$stability_review_tmp" \
    "@ohos-graphics/stability-code-review@$stability_registry_version"
"$stability_review_tmp/node_modules/.bin/stability-code-review" install \
    --target "$stability_opencode_root" --force
printf '%s\n' "$stability_registry_version" > "$stability_version_file"
```

临时目录必须通过 `/tmp/stability-code-review.*` 前缀校验后再递归删除。
无论安装、检视或报告生成成功还是失败，都要执行清理：

```sh
case "$stability_review_tmp" in
  /tmp/stability-code-review.*)
    rm -rf -- "$stability_review_tmp"
    ;;
  *)
    echo "Refuse to remove unexpected path: $stability_review_tmp"
    ;;
esac
```

不得删除 `$stability_skill_dir`。它是 OpenCode 后续检视复用的持久技能，
不属于单次任务残留。仅 registry 版本更新时才重新临时安装并覆盖升级。

安装完成后由 OpenCode 加载该技能，按技能说明执行检视。
如果安装包实际不可用或技能无法加载，需要说明工具状态和替代检视方式。

## 触发范围

稳定性检视采用询问制：下列业务代码改动时，Agent 先询问用户是否执行稳定性检视，
用户确认后才按本规范检视；用户未确认或拒绝时不执行。

需要询问（业务代码）：

- 修改 C/C++ 源码行为。
- 修改 `BUILD.gn`、编译依赖、target 或构建开关。
- 修改接口/IPC、proxy/stub、Parcel、Command 或 transaction 编码。
- 修改跨模块、跨进程、跨线程或生命周期相关逻辑。

无需询问（非业务代码，直接不适用）：

- 纯测试、文档、注释、知识路由、格式调整。
- 不涉及代码行为的路径核对或说明性改动。

## 检视范围

默认检视本次修改的 C/C++ 文件、对应头文件、直接调用方、被调用方和
IPC/proxy/stub 配对文件；不要默认全仓扫描。

涉及公开 API、IPC 协议、HWC、SurfaceBuffer、NativeBuffer、fence 或跨模块链路时，
扩大到对应数据流和调用方。

全仓或模块级稳定性扫描只在用户明确要求、风险较高或提交前专项审计时执行。

## 生成物清理

检视前记录 `git status --porcelain --untracked-files=all`，并记录 `report/`
是否已存在及其原有文件。稳定性技能每次检视生成一份 Markdown 和一份 CSV 报告。

报告内容被读取并汇总结论后，立即删除本次新生成的两个报告文件。
只允许删除本次记录的精确路径，不使用 `report/*` 等通配符，不删除既有报告。
报告生成失败时，也要删除能确认由本次任务创建的残缺报告。

如果 `report/` 由本次检视创建，且删除两个报告后目录为空，使用 `rmdir` 删除空目录。
原本存在的 `report/` 目录必须保留。

清理结束后再次检查工作区：

- 状态应恢复到检视前基线，只保留用户原有改动。
- 临时 npm 目录、npm 包、lockfile 和 `.bin` 链接不得残留在仓库。
- OpenCode 持久技能目录和 `.npm-version` 必须保留。
- npm 共享缓存不属于仓库残留，不得为单次检视清空。
- 工具若修改 tracked 文件或产生无法确认归属的文件，门禁判定为失败。
- 对无法确认归属的文件不得自动删除或恢复，等待人工确认。

## 输出要求

最终回复说明稳定性检视状态：

- 已执行：写清检视范围和结果。
- 未执行：已询问但用户未确认或拒绝，说明未执行原因。
- 失败：写清失败原因和是否阻塞提交/push。
- 不可用：写清工具或环境缺口，并说明替代静态检查。
- 不适用：说明仅涉及测试、文档、注释等非业务代码。

已执行时还要说明本次报告和临时 npm 内容已清理，
以及清理后的工作区是否与检视前基线一致。
