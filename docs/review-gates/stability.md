# 稳定性检视

适用于涉及 C/C++ 代码行为、BUILD、接口/IPC 或跨模块逻辑的任务。
目标是在 Agent 交付代码前看护异常处理、并发、资源、生命周期、内存和图形稳定性风险。

## 工具

`stability-code-review` 是团队外部检查工具/skill，不是 Agent 内置能力。
执行前先确认本地版本是否可用且不是旧版本：

```sh
npm view @ohos-graphics/stability-code-review version
npm ls @ohos-graphics/stability-code-review --depth=0
```

若本地未安装，或本地版本低于 registry 最新版本，再安装/升级：

```sh
npm i @ohos-graphics/stability-code-review@latest
```

安装后按工具说明执行检视。
若网络、内网 registry 或权限问题导致无法确认最新版本，需要在最终回复说明版本缺口。
如果命令实际只是安装器或不可直接扫描仓库，需要在最终回复说明工具状态和替代检视方式。

## 触发范围

需要稳定性检视：

- 修改 C/C++ 源码行为。
- 修改 `BUILD.gn`、编译依赖、target 或构建开关。
- 修改接口/IPC、proxy/stub、Parcel、Command 或 transaction 编码。
- 修改跨模块、跨进程、跨线程或生命周期相关逻辑。

通常不适用：

- 纯文档、注释、知识路由、格式调整。
- 不涉及代码行为的路径核对或说明性改动。

## 检视范围

默认检视本次修改的 C/C++ 文件、对应头文件、直接调用方、被调用方和
IPC/proxy/stub 配对文件；不要默认全仓扫描。

涉及公开 API、IPC 协议、HWC、SurfaceBuffer、NativeBuffer、fence 或跨模块链路时，
扩大到对应数据流和调用方。

全仓或模块级稳定性扫描只在用户明确要求、风险较高或提交前专项审计时执行。

## 输出要求

最终回复说明稳定性检视状态：

- 已执行：写清检视范围和结果。
- 失败：写清失败原因和是否阻塞提交/push。
- 不可用：写清工具或环境缺口，并说明替代静态检查。
- 不适用：说明不涉及 C/C++ 代码行为。
