/*
    This file is part of Magnum.

    Original authors — credit is appreciated but not required:

        2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019 —
            Vladimír Vondruš <mosra@centrum.cz>

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or distribute
    this software, either in source code form or as a compiled binary, for any
    purpose, commercial or non-commercial, and by any means.

    In jurisdictions that recognize copyright laws, the author or authors of
    this software dedicate any and all copyright interest in the software to
    the public domain. We make this dedication for the benefit of the public
    at large and to the detriment of our heirs and successors. We intend this
    dedication to be an overt act of relinquishment in perpetuity of all
    present and future rights to this software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <imgui.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/ImGuiIntegration/Context.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Context.h>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Trade/MeshData2D.h>


using namespace Magnum;

int main(int argc, char** argv) {
    /* Initialize the library */
    if(!glfwInit()) return -1;

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* const window0 = glfwCreateWindow(
        800, 600, "window0", nullptr, nullptr);

    GLFWwindow* const window1 = glfwCreateWindow(
        640, 480, "window1", nullptr, window0);

    GLFWwindow* const window2 = glfwCreateWindow(
        640, 480, "window2", nullptr, window0);

    if(!window0 || !window1 || !window2) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window0);

    {
        /* Create Magnum context in an isolated scope */
        Platform::GLContext ctx{ argc, argv };
        ImGuiIntegration::Context _imgui{ NoCreate };

        _imgui = ImGuiIntegration::Context(Vector2{ GL::defaultFramebuffer.viewport().size() },
            GL::defaultFramebuffer.viewport().size(), GL::defaultFramebuffer.viewport().size());

        GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
            GL::Renderer::BlendEquation::Add);
        GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
            GL::Renderer::BlendFunction::OneMinusSourceAlpha);

        using namespace Math::Literals;

        bool hideImGui = false;

        glfwSetWindowUserPointer(window0, &hideImGui);

        glfwSetKeyCallback(window0, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            bool* hideImGui = static_cast<bool*>(glfwGetWindowUserPointer(window));
            if (action == GLFW_PRESS) {
                *hideImGui = !*hideImGui;
            }
        });


        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window0)) {

            {
                glfwMakeContextCurrent(window0);
                GL::defaultFramebuffer.clear(GL::FramebufferClear::Color);
                if (!hideImGui) {
                    _imgui.newFrame();

                    {
                        ImGui::Text("Hello, world!");
                        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                            1000.0 / Double(ImGui::GetIO().Framerate), Double(ImGui::GetIO().Framerate));
                    }

                    /* Set appropriate states. If you only draw imgui UI, it is sufficient to
                       do this once in the constructor. */
                    GL::Renderer::enable(GL::Renderer::Feature::Blending);
                    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
                    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
                    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

                    _imgui.drawFrame();

                    /* Reset state. Only needed if you want to draw something else with
                       different state next frame. */
                    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
                    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
                    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
                    GL::Renderer::disable(GL::Renderer::Feature::Blending);
                }

                glfwSwapBuffers(window0);
            }
            {
                glfwMakeContextCurrent(window1);
                GL::Renderer::setClearColor(0xa5c9ea_rgbf);
                GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
                MeshTools::compile(Primitives::squareSolid())
                    .draw(Shaders::Flat2D{}
                        .setTransformationProjectionMatrix(Matrix3::scaling({ 0.2f, 0.3f }))
                        .setColor(0xff0000_rgbf));
                glfwSwapBuffers(window1);
            }

            {
                glfwMakeContextCurrent(window2);
                GL::Renderer::setClearColor(0xa5c9ea_rgbf);
                GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
                MeshTools::compile(Primitives::squareSolid())
                    .draw(Shaders::Flat2D{}
                        .setTransformationProjectionMatrix(Matrix3::scaling({ 0.2f, 0.3f }))
                        .setColor(0x00ff00_rgbf));
                glfwSwapBuffers(window2);
            }

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
}
