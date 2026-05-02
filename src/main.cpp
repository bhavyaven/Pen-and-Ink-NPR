#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <stb/stb_image.h>
#include "camera.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
using namespace std;

int WIDTH = 1280;
int HEIGHT = 720;

GLFWwindow* window;
Camera camera;
Mesh* mesh = nullptr;

GLuint gBufferFBO, gNormalTex, gDepthRBO, gPosTex;
GLuint edgeFBO, edgeTex;
GLuint hatchFBO, hatchTex;
GLuint paperTex;

GLuint quadVAO, quadVBO;

Shader* geoShader;
Shader* edgeShader;
Shader* hatchShader;
Shader* compShader;

struct NamedModel {
    const char* label;
    const char* path;
};

struct NamedTexture {
    const char* label;
    const char* path;
    GLuint id = 0;
	// for identifying ink wash textures that need special handling in the shader
    // true = ink, false = hatch
    bool   isInk = false;
};

vector<NamedTexture> hatchTextures = {
    // {"Hatch1 - Light", "assets/textures/hatch1-light.PNG", 0, false},
    {"Hatch1 - Medium", "assets/textures/hatch1-med.PNG", 0, false},
    {"Hatch1 - Dark", "assets/textures/hatch1-dark.PNG", 0, false},
    {"Hatch2 - Light", "assets/textures/hatch2-light.PNG", 0, false},
    {"Hatch2 - Medium", "assets/textures/hatch2-med.PNG", 0, false},
    {"Hatch2 - Dark", "assets/textures/hatch2-dark.PNG", 0, false},
	{"Hatch3", "assets/textures/hatch-3.png", 0, false}, 
	{"Hatch4", "assets/textures/hatch-4.png", 0, false}, 
    {"Ink1", "assets/textures/ink1.png", 0, true},
    {"Ink2", "assets/textures/ink2.png", 0, true},
};

vector<NamedModel> models = {
    {"Bunny", "assets/models/bunny.obj"},
	{"Bunny2", "assets/models/bunny2.obj"},
	{"Cow", "assets/models/cow.obj"},
	{"Cube", "assets/models/cube.obj"}, 
	{"DLamp", "assets/models/dlamp.obj"}, 
	{"Leaves", "assets/models/leaves.obj"}, 
	{"Pyramid", "assets/models/pyramid.obj"},
    {"Sphere", "assets/models/sphere.obj"},
	{"Teapot", "assets/models/teapot.obj"},
	{"Trunk", "assets/models/trunk.obj"},
};

int   activeModel = 0;
int   activeHatch = 0;
float edgeThreshold = 1.2f;
float edgeBoost = 1.6f;
float hatchAlpha = 0.7f;
float hatchScale = 5.0f;
float paperScale = 1.0f;
bool showDarknessOnly = false;

GLuint loadTex(const char* path) {
    GLuint id;
    glGenTextures(1, &id);
    int w, h, ch;
    unsigned char* data = stbi_load(path, &w, &h, &ch, 0);
    if (!data) {
        cerr << "Failed to load texture: " << path << "\n";
        return 0;
    }
    GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return id;
}

void loadModel(int index) {
    cout << "Loading model: " << models[index].path << endl;
    delete mesh;
    mesh = new Mesh(models[index].path);
    if (!mesh) cout << "ERROR: mesh is null after load!\n";
}

void createQuad() {
    float verts[] = { -1,-1, 1,-1, -1,1, 1,1 };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glBindVertexArray(0);
}

void renderQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void initGBuffer() {
    glGenFramebuffers(1, &gBufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

    glGenTextures(1, &gNormalTex);
    glBindTexture(GL_TEXTURE_2D, gNormalTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gNormalTex, 0);

    glGenTextures(1, &gPosTex);
    glBindTexture(GL_TEXTURE_2D, gPosTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPosTex, 0);

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glGenRenderbuffers(1, &gDepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gDepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gDepthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "GBuffer not complete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initSimpleFBO(GLuint& fbo, GLuint& tex) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "FBO not complete!\n";
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GeometryPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    geoShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(45.0f),
        (float)WIDTH / HEIGHT, 0.1f, 100.0f);
    geoShader->setUniform("model", model);
    geoShader->setUniform("view", view);
    geoShader->setUniform("projection", proj);
    mesh->draw();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void EdgePass() {
    glBindFramebuffer(GL_FRAMEBUFFER, edgeFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    edgeShader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gNormalTex);
    glUniform1i(glGetUniformLocation(edgeShader->ID, "normalTex"), 0);

    glUniform1f(glGetUniformLocation(edgeShader->ID, "uEdgeThreshold"), edgeThreshold);

    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HatchingPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, hatchFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    hatchShader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gNormalTex);
    glUniform1i(glGetUniformLocation(hatchShader->ID, "normalTex"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, hatchTextures[activeHatch].id);
    glUniform1i(glGetUniformLocation(hatchShader->ID, "hatchTex"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gPosTex);
    glUniform1i(glGetUniformLocation(hatchShader->ID, "posTex"), 2);

    glUniform1f(glGetUniformLocation(hatchShader->ID, "uHatchScale"), hatchScale);
    glUniform1i(glGetUniformLocation(hatchShader->ID, "uIsInk"),
        hatchTextures[activeHatch].isInk ? 1 : 0);

    renderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CompositePass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    compShader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, edgeTex);
    glUniform1i(glGetUniformLocation(compShader->ID, "edgeTex"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, hatchTex);
    glUniform1i(glGetUniformLocation(compShader->ID, "hatchTex"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, paperTex);
    glUniform1i(glGetUniformLocation(compShader->ID, "paperTex"), 2);

    glUniform1f(glGetUniformLocation(compShader->ID, "uEdgeBoost"), edgeBoost);
    glUniform1f(glGetUniformLocation(compShader->ID, "uHatchAlpha"), hatchAlpha);
    glUniform1f(glGetUniformLocation(compShader->ID, "uPaperScale"), paperScale);
    glUniform1i(glGetUniformLocation(compShader->ID, "uDebug"), showDarknessOnly ? 1 : 0);

    renderQuad();
}

void renderGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(280, 0), ImGuiCond_Always);
    ImGui::Begin("NPR Controls", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::SeparatorText("Model");
    for (int i = 0; i < (int)models.size(); i++) {
        if (ImGui::RadioButton(models[i].label, activeModel == i)) {
            activeModel = i;
            loadModel(i);
        }
    }
    ImGui::SeparatorText("Debug");
    ImGui::Checkbox("Show darkness mask only", &showDarknessOnly);
   

    ImGui::SeparatorText("Hatch texture");
    for (int i = 0; i < (int)hatchTextures.size(); i++) {
        if (ImGui::RadioButton(hatchTextures[i].label, activeHatch == i))
            activeHatch = i;
    }

    ImGui::SeparatorText("Edge");
    ImGui::SliderFloat("Threshold", &edgeThreshold, 0.1f, 3.0f);
    ImGui::SliderFloat("Boost", &edgeBoost, 0.5f, 3.0f);

    ImGui::SeparatorText("Hatching");
    ImGui::SliderFloat("Scale", &hatchScale, 1.0f, 15.0f);
    ImGui::SliderFloat("Alpha", &hatchAlpha, 0.0f, 1.0f);

    ImGui::SeparatorText("Paper");
    ImGui::SliderFloat("Paper scale", &paperScale, 0.25f, 4.0f);

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

double lastX = 0, lastY = 0;
bool rotating = false;

void mouse_button_callback(GLFWwindow* w, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(w, button, action, mods);
    if (ImGui::GetIO().WantCaptureMouse) {
        rotating = false;
        return;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        rotating = (action == GLFW_PRESS);
}

void cursor_pos_callback(GLFWwindow* w, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(w, xpos, ypos);
    if (rotating) {
        glm::vec2 delta((float)(xpos - lastX), (float)(ypos - lastY));
        camera.rotate(delta);
    }
    lastX = xpos;
    lastY = ypos;
}

void scroll_callback(GLFWwindow* w, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(w, xoffset, yoffset);
    if (ImGui::GetIO().WantCaptureMouse) return;
    camera.zoom((float)-yoffset);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Pen & Ink NPR", NULL, NULL);
    if (!window) { cerr << "Failed to create window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD\n"; return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);

    for (auto& ht : hatchTextures) {
        ht.id = loadTex(ht.path);
        if (ht.id == 0)
            cerr << "CRITICAL: hatch texture failed: " << ht.path
            << " — check assets/textures/ folder and filename case\n";
        else
            cout << "Loaded hatch texture: " << ht.path
            << " (id=" << ht.id << ")\n";
    }

    paperTex = loadTex("assets/textures/paper.jpg");
    loadModel(activeModel);

    geoShader = new Shader("shaders/geo.vert", "shaders/geo.frag");
    edgeShader = new Shader("shaders/quad.vert", "shaders/edge.frag");
    hatchShader = new Shader("shaders/quad.vert", "shaders/hatch.frag");
    compShader = new Shader("shaders/quad.vert", "shaders/comp.frag");

    createQuad();
    initGBuffer();
    initSimpleFBO(edgeFBO, edgeTex);
    initSimpleFBO(hatchFBO, hatchTex);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        GeometryPass();
        EdgePass();
        HatchingPass();
        CompositePass();
        renderGUI();

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}