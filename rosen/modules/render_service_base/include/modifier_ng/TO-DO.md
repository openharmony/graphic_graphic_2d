1. 补充RSRenderNode / RSNode 的 AddModifier/RemoveModifier/GetModifier逻辑
    1.1 RSRenderNode新增如下接口和结构
    ```
        std::array<std::vector<std::shared_ptr<ModifierNG::RSRenderModifier>>, 46> modifiersNG_;
        std::map<PropertyId, std::shared_ptr<RSRenderPropertyBase>> properties_;
    ```
2. 重新构建当Client侧property发生变化时，通过IPC修改对应Server侧property的通路 (client侧生成command, command传输+执行，修改对应Server侧property)
    2.1 Server通过`GetProperty`获取`std::shared_ptr<RSRenderPropertyBase>`，需要适配 rs_render_path_animation和rs_render_transition
3. Server渲染时调用新modifier的Apply/Sync/Draw接口