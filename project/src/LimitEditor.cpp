#include <GameSystem.h>


void processInput(Window &windows) {
    if (glfwGetKey(windows.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        windows.SetWindowShouldClose();
    }
}


int main() {
    //Init window
    Window LimitEditor(2200, 1400, const_cast<char *>("Limit Editor"), const_cast<char *>("../project/icon/icon.png"));
    Shader shader("../project/shader/rectangleShader/vertexShader.glsl",
                  "../project/shader/rectangleShader/fragmentShader.glsl");
    Texture texture("../project/texture/icon.png");
    float vertices[] = {
            // positions          // colors           // texture coords
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
            0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
            -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
            -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };
    VBO vbo;
    vbo.Data(sizeof(vertices), vertices);
    EBO ebo;
    ebo.Data(sizeof(indices), indices);
    VAO vao;
    vao.Bind();
    vbo.Bind();
    ebo.Bind();
    vao.Layout(0, 3, GL_FLOAT, 8 * sizeof(float), 0);
    vao.Layout(1, 3, GL_FLOAT, 8 * sizeof(float), 3 * sizeof(float));
    vao.Layout(2, 2, GL_FLOAT, 8 * sizeof(float), 6 * sizeof(float));
    shader.use();
    shader.setInt("texture1", 0);
    float deltaTime;
    float lastFrame = 0.0f;
    ImGui::FileBrowser fileDialog;
    // (optional) set browser properties
    fileDialog.SetTitle("File browser");
    std::filesystem::path pwd("../");
    fileDialog.SetPwd(pwd);
    bool openFileDialog = false;
    int time = 0;
    //gui font
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("../core/libraries/JetBrainsMono/fonts/ttf/JetBrainsMono-LightItalic.ttf", 25.0f);
    ImGui::LoadIniSettingsFromDisk("../project/EditorLayout/GuiLayout.ini");
    ImGui::GetIO().IniFilename = nullptr;
    //fbo
    FBO fbo;
    fbo.Bind();
    // create a color attachment texture
    FrameTexture frame;
    frame.Bind();
    frame.Data(2200, 1400);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    RBO rbo;
    rbo.Bind();
    rbo.Storage(2200, 1400); // use a single renderbuffer object for both a depth AND stencil buffer.
    rbo.Attach(); // now actually attach it
    fbo.Unbind();
    while (!LimitEditor.ShouldClose()) {
        if (time == 1)
            openFileDialog = false;
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fbo.Bind();
        processInput(LimitEditor);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //draw
        glActiveTexture(GL_TEXTURE0);
        texture.Bind();
        shader.use();
        vao.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        vao.Unbind();
        fbo.Unbind();
        //Gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //gui DockSpace
        static ImGuiDockNodeFlags DockSpace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        if (DockSpace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);
        ImGui::PopStyleVar(2);
        ImGuiID DockSpace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(DockSpace_id, ImVec2(0.0f, 0.0f), DockSpace_flags);
        //menu
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Add")) {
                if (ImGui::MenuItem("Model", nullptr, false, true)) {
                    fileDialog.SetPwd(pwd);
                    openFileDialog = true;
                    time = 1;

                }
                if (ImGui::MenuItem("Light", nullptr, false, true)) {

                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Shader")) {
                if (ImGui::MenuItem("New", nullptr, false, true)) {
                    fileDialog.SetPwd(pwd);
                    openFileDialog = true;
                    time = 1;
                }
                if (ImGui::MenuItem("Set Default", nullptr, false, true)) {

                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();
        //viewport window
        ImGui::Begin("Viewport");
        //render frame
        ImGui::GetWindowDrawList()->AddImage(
                (void *) frame.ID,
                ImVec2(ImGui::GetCursorScreenPos()),
                ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth(),
                       ImGui::GetCursorScreenPos().y + ImGui::GetWindowHeight()), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        //object editor panel
        ImGui::Begin("Object Editor");
        ImGui::End();
        //file browser
        if (openFileDialog) {
            fileDialog.Open();
        }
        fileDialog.Display();
        if (fileDialog.HasSelected()) {
            std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
            fileDialog.ClearSelected();
            fileDialog.Close();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(LimitEditor.window);
        glfwPollEvents();
    }
}