#include "viewer.hpp"

int main()
{
    MachineViewer viewer;
    viewer.initTrackball();
    viewer.initViewer();

    while(viewer.isActive())
    {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        viewer.updateCamera();
        viewer.drawLight();
        viewer.drawCube();
        viewer.swapBuffers();

        glfwPollEvents();
    }
}