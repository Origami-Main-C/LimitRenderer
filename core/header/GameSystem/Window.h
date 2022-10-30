#pragma once

#include <GameSystem/Log.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <windows.h>
#include <GameSystem/Gui.h>
#include <GameSystem/NormalFunction.h>
#include <functional>
#include <stb_image.h>

class Window {
public:
    GLFWwindow *window;
    HWND cmd = FindWindow(TEXT("ConsoleWindowClass"), nullptr);

    void SetCmd(bool show) {
        if (show) {
            ShowWindow(cmd, SW_SHOW);
            LOG_Log("WINDOW", "CONSOLE", "SHOW");
        } else {
            ShowWindow(cmd, SW_HIDE);
            LOG_Log("WINDOW", "CONSOLE", "HIDE");
        }
    }

    void SetIcon(const char *path) {
        GLFWimage icon;
        icon.pixels = stbi_load(path, &icon.width, &icon.height, 0, 4); //rgba channels
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(icon.pixels);
    }

    //Use GLFW to Create a default window and Init libraries
    Window(int width, int height, std::string title, char *iconPath = nullptr, bool hideCmd = true,
           void (*framebuffer_size_callback)(GLFWwindow *window, int width,
                                             int height) = Normal::Function::framebuffer_size_callback,
           std::function<void(ImGuiStyle *)> guiColor = ImGui::StyleColorsDark,
           ImGuiStyle *guiColor_dst = nullptr, bool guidocking = true) {
        if (hideCmd) {
            if (cmd) {
                SetCmd(false);
            }
        }
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr) {

            LOG_Error("WINDOW", "FAILED_TO_CREATE_GLFW_WINDOW", "the window pointer is nullptr");
            glfwTerminate();
            exit(-1);
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            LOG_Error("WINDOW", "INIT", "failed to init GLAD");
            exit(-1);
        }
        if (iconPath != nullptr) {
            LOG_Log("WINDOW", "SET_ICON", iconPath);
            SetIcon(iconPath);
        }
        ImGui::CreateContext();
        guiColor(guiColor_dst);
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
        if (guidocking) {
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            LOG_Log("WINDOW", "INIT_GUI", "docking enable");
        } else {
            LOG_Log("WINDOW", "INIT_GUI", "docking unable");
        }
        LOG_Log("WINDOW", "INIT", title + "    " + std::to_string(width) + " " + std::to_string(height));
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(window);
    }

    void SetWindowShouldClose(bool close = true) {
        glfwSetWindowShouldClose(window, close);
    }

    ~Window() {
        //Shutdown
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(0);
    }

};

