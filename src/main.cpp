#include "viewer.hpp"

int main(int argc, char **argv) {
    std::string solution;
    bool is_first = true;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (!is_first) {
                solution += " ";
            }
            else {
                is_first = false;
            }
            solution += argv[i];
        }
    }

    MachineViewer viewer;
    viewer.initViewer(solution);

    while(viewer.isActive())
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        viewer.updateCamera();
        viewer.makeTrans();
        viewer.drawMachine();
        viewer.swapBuffers();

        glfwPollEvents();
    }
}