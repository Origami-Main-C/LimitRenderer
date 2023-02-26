#include <GameSystem.h>
Normal::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 1100.f;
float lastY = 700.f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool focus_on_viewport=true;
void processInput(Window &windows) {
    if (glfwGetKey(windows.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(windows.window, true);
    if(focus_on_viewport)
    {
        if (glfwGetKey(windows.window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(windows.window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(windows.window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(windows.window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

int main() {
    //Init window
    Window LimitEditor(2200, 1400, const_cast<char *>("Limit Editor"), const_cast<char *>("../project/icon/icon.png"));
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);

    std::vector<Model> modelList;
    ImGui::FileBrowser ModelFileDialog;
    ImGui::FileBrowser ShaderFileDialog(ImGuiFileBrowserFlags_SelectDirectory);
    // (optional) set browser properties
    ShaderFileDialog.SetTitle("Select shader dir");
    ShaderFileDialog.SetPwd(std::filesystem::path("../project/shader"));

    ModelFileDialog.SetTitle("Load model");
    ModelFileDialog.SetPwd(std::filesystem::path("../project/model"));


    bool openModelFileDialog = false;
    bool openShaderFileDialog = false;
    //gui font
    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("../core/libraries/JetBrainsMono/fonts/ttf/JetBrainsMono-LightItalic.ttf", 25.0f);
    ImGui::LoadIniSettingsFromDisk("../project/EditorLayout/GuiLayout.ini");
    ImGui::GetIO().IniFilename = nullptr;
    //fbo
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2200, 1400, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 2200, 1400); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG_Error("FRAMEBUFFER","FRAMEBUFFER_GENERATE_ERROR","Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float View_Width = 2200;
    float View_Height = 1400;
    std::string vShader="../project/shader/model/vertex.glsl";
    std::string fShader="../project/shader/model/fragment.glsl";
    while (!LimitEditor.ShouldClose()) {

        Shader shader(vShader,
                      fShader);
        openModelFileDialog=false;
        openShaderFileDialog=false;
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(LimitEditor);
        if(focus_on_viewport)
        {
            if(ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                float xpos = static_cast<float>(ImGui::GetMousePos().x);
                float ypos = static_cast<float>(ImGui::GetMousePos().y);

                if (firstMouse) {
                    lastX = xpos;
                    lastY = ypos;
                    firstMouse = false;
                }

                float xoffset = xpos - lastX;
                float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

                lastX = xpos;
                lastY = ypos;

                camera.ProcessMouseMovement(xoffset, yoffset);
            }
            else
            {
                firstMouse= true;
            }

        }
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //draw
        shader.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) View_Width / (float) View_Height,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,
                               glm::vec3(0.0f, 0.0f, -1.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));    // it's a bit too big for our scene, so scale it down
        shader.setMat4("model", model);
        for (int i = 0; i < modelList.size(); ++i) {
            modelList[i].Draw(shader);
        }
        //backpack_model.Draw(shader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
                    ModelFileDialog.SetPwd(std::filesystem::path("../project/model"));
                    openModelFileDialog = true;

                }
                if (ImGui::MenuItem("Light", nullptr, false, true)) {

                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Shader")) {
                ShaderFileDialog.SetPwd(std::filesystem::path("../project/shader"));
                openShaderFileDialog = true;
                ImGui::EndMenu();
            }
        }
        ImGui::End();
        //viewport window
        ImGui::Begin("Viewport");
        View_Width = ImGui::GetWindowWidth();
        View_Height = ImGui::GetWindowHeight();
        focus_on_viewport=ImGui::IsWindowFocused();
        //render frame
        ImGui::GetWindowDrawList()->AddImage(
                (void *) textureColorbuffer,
                ImVec2(ImGui::GetCursorScreenPos()),
                ImVec2(ImGui::GetCursorScreenPos().x + ImGui::GetWindowWidth(),
                       ImGui::GetCursorScreenPos().y + ImGui::GetWindowHeight()), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
        //object editor panel
        ImGui::Begin("Object Editor");
        ImGui::End();
        //file browser
        if (openShaderFileDialog) {
            ShaderFileDialog.Open();
        }
        if (openModelFileDialog) {
            ModelFileDialog.Open();
        }
        ShaderFileDialog.Display();
        ModelFileDialog.Display();
        if (ModelFileDialog.HasSelected()) {

            modelList.push_back(Model(ModelFileDialog.GetSelected().string()));
            ModelFileDialog.ClearSelected();
            ModelFileDialog.Close();
        }if (ShaderFileDialog.HasSelected()) {
            vShader=ShaderFileDialog.GetSelected().string()+"/vertex.glsl";
            fShader=ShaderFileDialog.GetSelected().string()+"/fragment.glsl";
            ShaderFileDialog.ClearSelected();
            ShaderFileDialog.Close();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(LimitEditor.window);
        glfwPollEvents();
    }
}