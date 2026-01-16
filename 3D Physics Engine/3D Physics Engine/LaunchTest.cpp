#include <GLFW/glfw3.h>
#include <iostream>

// 윈도우 크기
const int WIDTH = 800;
const int HEIGHT = 600;

// 키 입력 콜백
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // ESC 누르면 종료
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    std::cout << "Step 1: GLFW Window" << std::endl;

    // === 1. GLFW 초기화 ===
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // OpenGL 버전 설정 (3.3 Core)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // === 2. 창 생성 ===
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Step 1: Window", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // OpenGL 컨텍스트 설정
    glfwMakeContextCurrent(window);

    // 키 입력 콜백 설정
    glfwSetKeyCallback(window, key_callback);

    std::cout << "Window created! Press ESC to exit." << std::endl;

    // === 3. 메인 루프 ===
    while (!glfwWindowShouldClose(window)) {
        // 화면 지우기 (검은색)
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 화면 업데이트
        glfwSwapBuffers(window);

        // 이벤트 처리
        glfwPollEvents();
    }

    // === 4. 정리 ===
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "Goodbye!" << std::endl;

    return 0;
}