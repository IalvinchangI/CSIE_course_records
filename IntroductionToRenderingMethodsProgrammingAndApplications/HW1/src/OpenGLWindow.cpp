#include "OpenGLWindow.hpp"

#include "Model/TextureFactory.hpp"
#include "OpenGL/OpenGLException.hpp"
#include "Utils/Compilers.hpp"
#include "Utils/Global.hpp"
#include "Utils/StringFormat/StringFormat.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"

#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "tiny_obj_loader.h"

#include <iostream>
#include <utility>
#include <vector>

namespace Detail
{

bool compileShaders(OpenGL::OpenGLShaderProgram &program,
                    const char *vertexShaderFile,
                    const char *fragmentShaderFile = nullptr,
                    const char *geometryShaderFile = nullptr);
void frameBufferSizeCallback(GLFWwindow *window, int width, int height);

bool compileShaders(OpenGL::OpenGLShaderProgram &program,
                    const char *vertexShaderFile,
                    const char *fragmentShaderFile,
                    const char *geometryShaderFile)
{
    if (!program.addShaderFromFile(OpenGL::OpenGLShader::Type::Vertex,
                                   vertexShaderFile))
    {
        return false;
    }

    if (fragmentShaderFile &&

        !program.addShaderFromFile(OpenGL::OpenGLShader::Type::Fragment,
                                   fragmentShaderFile))
    {
        return false;
    }

    if (geometryShaderFile &&
        !program.addShaderFromFile(OpenGL::OpenGLShader::Type::Geometry,
                                   geometryShaderFile))
    {
        return false;
    }

    program.link();
    if (!program.linkStatus())
    {
        return false;
    }

    return true;
}

void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    if (window)
    {
    }
    glViewport(0, 0, width, height);
}

} // namespace Detail

OpenGLWindow::OpenGLWindow(glm::ivec2 windowSize, std::string title,
                           glm::ivec2 openglVersion)
    : window_{nullptr}, size_{windowSize}, title_{title},
      version_{openglVersion}, models_{}, renderMode_{RenderMode::Fill},
      backgroundColor_{0}, lookAt_{0}, cameraPosition_{lookAt_ + glm::vec3{8}}, 
      cameraFront_{0.0f, 0.0f, -1.0f}, cameraUp_{0.0f, 1.0f, 0.0f},
      yaw_{-90.0f}, pitch_{0.0f}, lastX_{windowSize.x / 2.0f},
      lastY_{windowSize.y / 2.0f}, firstMouse_{true}
{
    create();
}

OpenGLWindow::~OpenGLWindow() { destroy(); }

bool OpenGLWindow::addModel(const char *modelSource, const char *textureSource,
                            OpenGL::OpenGLShaderProgram &program)
{
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string errorMessage;

    auto success =
        tinyobj::LoadObj(shapes, materials, errorMessage, modelSource);

    if (!success)
    {
        std::cerr << "[Error]" << errorMessage.c_str();

        return false;
    }

    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<float> textureCoordinates;
    std::vector<unsigned int> indices;

    for (auto &shape : shapes)
    {
        positions.insert(positions.end(), shape.mesh.positions.begin(),
                         shape.mesh.positions.end());
        if (!shape.mesh.normals.empty())
        {
            normals.insert(normals.end(), shape.mesh.normals.begin(),
                           shape.mesh.normals.end());
        }

        if (!shape.mesh.texcoords.empty())
        {
            textureCoordinates.insert(textureCoordinates.end(),
                                      shape.mesh.texcoords.begin(),
                                      shape.mesh.texcoords.end());
        }

        indices.insert(indices.end(), shape.mesh.indices.begin(),
                       shape.mesh.indices.end());
    }

    std::unique_ptr<OpenGL::OpenGLTexture> texture;
    std::unique_ptr<Model::Mesh> mesh;

    if (textureSource)
    {
        texture = Model::TextureFactory::loadFromFile(textureSource);
        mesh.reset(new Model::Mesh{positions, normals, textureCoordinates,
                                   indices, program, texture.get()});

        textures.push_back(std::move(texture));
    }
    else
    {
        mesh.reset(new Model::Mesh{positions, normals, textureCoordinates,
                                   indices, program});
    }

    models_.push_back(std::move(mesh));

    return true;
}

OpenGL::OpenGLShaderProgram *
OpenGLWindow::addShader(const char *vertexShaderSource,
                        const char *fragmentShaderSource,
                        const char *geometryShaderSource)
{
    std::unique_ptr<OpenGL::OpenGLShaderProgram> program{
        new OpenGL::OpenGLShaderProgram{}};
    if (!Detail::compileShaders(*program, vertexShaderSource,
                                fragmentShaderSource, geometryShaderSource))
    {
        return nullptr;
    }
    shaders_.push_back(std::move(program));

    return shaders_.back().get();
}

float OpenGLWindow::aspectRatio() const noexcept
{
    return static_cast<float>(width()) / static_cast<float>(height());
}

void OpenGLWindow::create()
{
    if (!initializeOpenGL())
    {
        throw OpenGL::OpenGLException{"Failed to initialize OpenGL"};
    }

    if (!createWindow())
    {
        glfwTerminate();
        throw OpenGL::OpenGLException{"Failed to Create GLFW window"};
    }

    if (!initializeGLAD())
    {
        glfwTerminate();
        throw OpenGL::OpenGLException{"Failed to initialize GLAD"};
    }

    initializeImgui();

    glEnable(GL_DEPTH_TEST);
}

void OpenGLWindow::clearColor()
{
    glClearColor(backgroundColor_.x, backgroundColor_.y, backgroundColor_.z,
                 backgroundColor_.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool OpenGLWindow::createWindow()
{
    window_ =
        glfwCreateWindow(width(), height(), title_.c_str(), nullptr, nullptr);
    if (!window_)
    {
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, Detail::frameBufferSizeCallback);

    return true;
}

void OpenGLWindow::destroy()
{
    for (auto &model : models_)
    {
        model.reset(nullptr);
    }
    models_.clear();

    for (auto &texture : textures)
    {
        texture.reset(nullptr);
    }
    textures.clear();

    shaders_.clear();

    destroyImgui();
    destroyOpenGL();
}

void OpenGLWindow::destroyImgui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void OpenGLWindow::destroyOpenGL()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}

int OpenGLWindow::height() const noexcept { return size_.y; }

bool OpenGLWindow::initializeGLAD()
{
    return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void OpenGLWindow::initializeImgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    PROGRAM_MAYBE_UNUSED(io)

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init(
        StringFormat::StringFormat("#version %d%d0", version_.x, version_.y)
            .c_str());
}

bool OpenGLWindow::initializeOpenGL()
{
    if (!glfwInit())
    {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version_[0]);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version_[1]);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    return true;
}

void OpenGLWindow::processInput() {
    shouldExit();

    // mouse
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window_, &xpos, &ypos);

        if (firstMouse_)
        {
            lastX_ = static_cast<float>(xpos);
            lastY_ = static_cast<float>(ypos);
            firstMouse_ = false;
        }

        float xoffset = static_cast<float>(xpos) - lastX_;
        float yoffset = lastY_ - static_cast<float>(ypos);
        lastX_ = static_cast<float>(xpos);
        lastY_ = static_cast<float>(ypos);

        xoffset *= sensitivity_;
        yoffset *= sensitivity_;

        yaw_ += xoffset;
        pitch_ += yoffset;

        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;


        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        cameraFront_ = glm::normalize(front);
    }
    else
    {
        firstMouse_ = true;
    }

    // keyboard
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront_, cameraUp_));
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition_ += cameraSpeed_ * cameraFront_;
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition_ -= cameraSpeed_ * cameraFront_;
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition_ -= cameraSpeed_ * cameraRight;
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition_ += cameraSpeed_ * cameraRight;


    // final update
    lookAt_ = cameraPosition_ + cameraFront_;
}

void OpenGLWindow::shouldExit()
{
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_, true);
    }
}

void OpenGLWindow::startRender() { windowRenderLoop(); }

int OpenGLWindow::width() const noexcept { return size_.x; }

void OpenGLWindow::windowImguiGeneralSetting()
{
    ImGui::Begin("Setting");
    ImGui::ColorEdit4("Background color", glm::value_ptr(backgroundColor_));
    ImGui::SliderFloat3("Camera Position", glm::value_ptr(cameraPosition_), -20.0f, 20.0f);
    //ImGui::SliderFloat3("Look At", glm::value_ptr(lookAt_), -20.0f, 20.0f);

    const char *items[] = {"Line", "Fill"};
    int current_item = renderMode_;
    ImGui::Combo("combo", &current_item, items, IM_ARRAYSIZE(items));

    if (current_item != renderMode_)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE + current_item);
        renderMode_ = static_cast<RenderMode>(current_item);
    }

    ImGui::Separator();

    // Bonus
    ImGui::SliderFloat("Model Scale", &scale_, 0.1f, 5.0f);
    ImGui::Checkbox("Auto Rotation", &autoRotate_);
    ImGui::SliderFloat("Rotation Speed", &rotationSpeed_, 0.1f, 5.0f);
    ImGui::SliderFloat2("Texture Offset", glm::value_ptr(textureOffset_), -2.0f, 2.0f);
    ImGui::SliderFloat3("Light Position", glm::value_ptr(lightPos_), -20.0f, 20.0f);
    ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor_));

    ImGui::End();
}

void OpenGLWindow::windowRenderLateUpdate() {}

void OpenGLWindow::windowRenderImguiUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    windowImguiGeneralSetting();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLWindow::windowRenderLoop()
{
    while (!(glfwWindowShouldClose(window_)))
    {
        processInput();
        clearColor();

        windowRenderUpdate();
        windowRenderLateUpdate();

        windowRenderImguiUpdate();

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

void OpenGLWindow::windowRenderUpdate()
{
    // store time
    static float lastFrameTime = glfwGetTime();
    float currentFrameTime = glfwGetTime();
    float deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    PRAGMA_WARNING_PUSH
    PRAGMA_WARNING_DISABLE_CONSTANTCONDITIONAL
    glm::mat4 view = glm::lookAt(cameraPosition_, lookAt_, glm::vec3{0, 1, 0}) *
                     glm::mat4(1);
    PRAGMA_WARNING_POP

    glm::mat4 projection{
        glm::perspective(glm::radians(45.0f), aspectRatio(), 0.1f, 100.0f)};

    // adjust shader value
    for (auto &shader : shaders_)
    {
        shader->use();
        // texture offset
        shader->setValue("textureOffset", textureOffset_.x, textureOffset_.y);

        // light
        shader->setValue("lightPos", lightPos_);
        shader->setValue("viewPos", cameraPosition_);
        shader->setValue("lightColor", lightColor_);
    }

    // adjust model value & draw model
    for (auto &model : models_)
    {
        // rotate & scale
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        if (autoRotate_)
            currentRotationAngle_ += rotationSpeed_ * deltaTime;
        modelMatrix = glm::rotate(modelMatrix, currentRotationAngle_, glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale_));
        model->setModel(modelMatrix);

        model->draw(view, projection);
    }
}
