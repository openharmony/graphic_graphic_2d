# 新增 BUILD.gn target

适用于新增库、测试、fuzz、sample 或其它 GN target。

执行要点：

- 复用附近 `part_name`、`subsystem_name`、`module_out_path`、deps 和 external_deps。
- 先查同目录或相邻目录已有 target，不要新造不一致的分组和输出路径。
- 新增测试 target 时，确认是否需要挂到上层 `test`、`unittest`、`fuzztest` group
  或 `bundle.json` test 列表。
- 引用本仓 target 时使用完整 `//foundation/graphic/graphic_2d/...` 前缀。

发现命令：

```sh
rg -n 'group\("test|group\("unittest|ohos_unittest|ohos_fuzztest' <dir>
```

验证建议：

- 做 `BUILD.gn` 引用检查、路径/符号核对和 `git diff --check`。
