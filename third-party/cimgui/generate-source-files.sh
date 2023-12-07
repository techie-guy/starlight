python3 dear_bindings/dear_bindings.py -o cimgui imgui/imgui.h --imconfig-path imgui/imconfig.h
python3 dear_bindings/dear_bindings.py --backend -o cimgui_impl_glfw imgui/backends/imgui_impl_glfw.h --imconfig-path imgui/imconfig.h
python3 dear_bindings/dear_bindings.py --backend -o cimgui_impl_opengl3 imgui/backends/imgui_impl_opengl3.h --imconfig-path imgui/imconfig.h

