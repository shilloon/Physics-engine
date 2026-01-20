//#include <glad/glad.h>  // 반드시 GLFW보다 먼저!
//#include <GLFW/glfw3.h>
//#include <iostream>
//
//const int WIDTH = 800;
//const int HEIGHT = 600;
//
//// Vertex Shader 소스 코드
//const char* vertexShaderSource = R"(
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//
//out vec3 vertexColor;
//
//void main() {
//    gl_Position = vec4(aPos, 1.0);
//    vertexColor = aColor;
//}
//)";
//
//// Fragment Shader 소스 코드
//const char* fragmentShaderSource = R"(
//#version 330 core
//in vec3 vertexColor;
//out vec4 FragColor;
//
//void main() {
//    FragColor = vec4(vertexColor, 1.0);
//}
//)";
//
//// ESC 키로 종료
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//        glfwSetWindowShouldClose(window, true);
//    }
//}
//
//int main() {
//    std::cout << "Step 2: Triangle" << std::endl;
//
//    // === 1. GLFW 초기화 ===
//    if (!glfwInit()) {
//        std::cerr << "Failed to initialize GLFW!" << std::endl;
//        return -1;
//    }
//
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    // === 2. 창 생성 ===
//    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Step 2: Triangle", NULL, NULL);
//    if (!window) {
//        std::cerr << "Failed to create GLFW window!" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    glfwMakeContextCurrent(window);
//    glfwSetKeyCallback(window, key_callback);
//
//    // === 3. GLAD 초기화 (중요!) ===
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//        std::cerr << "Failed to initialize GLAD!" << std::endl;
//        return -1;
//    }
//
//    std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
//
//    // === 4. Vertex Shader 컴파일 ===
//    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//    glCompileShader(vertexShader);
//
//    // 컴파일 에러 체크
//    int success;
//    char infoLog[512];
//    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
//    if (!success) {
//        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
//        std::cerr << "Vertex Shader compilation failed:\n" << infoLog << std::endl;
//    }
//
//    // === 5. Fragment Shader 컴파일 ===
//    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//    glCompileShader(fragmentShader);
//
//    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
//    if (!success) {
//        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
//        std::cerr << "Fragment Shader compilation failed:\n" << infoLog << std::endl;
//    }
//
//    // === 6. Shader Program 생성 ===
//    unsigned int shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//
//    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
//    if (!success) {
//        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
//        std::cerr << "Shader Program linking failed:\n" << infoLog << std::endl;
//    }
//
//    // Shader 객체 삭제 (이제 필요 없음)
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//
//    // === 7. 삼각형 좌표 데이터 ===
//    float vertices[] = {
//        // 위치 (x, y, z)      // 색상 (r, g, b)
//         0.0f,  0.5f, 0.0f,    1.0f, 0.0f, 0.0f,  // 위 (빨강)
//        -0.5f, -0.5f, 0.0f,    0.0f, 1.0f, 0.0f,  // 왼쪽 아래 (초록)
//         0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f   // 오른쪽 아래 (파랑)
//    };
//
//    // === 8. VAO, VBO 생성 ===
//    unsigned int VAO, VBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//
//    // VAO 바인드
//    glBindVertexArray(VAO);
//
//    // VBO 바인드 & 데이터 복사
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    // 위치 속성 (location = 0)
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    // 색상 속성 (location = 1)
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//
//    // 언바인드
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//
//    std::cout << "Triangle ready! Press ESC to exit." << std::endl;
//
//    // === 9. 렌더링 루프 ===
//    while (!glfwWindowShouldClose(window)) {
//        // 화면 지우기
//        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        // Shader 프로그램 사용
//        glUseProgram(shaderProgram);
//
//        // 삼각형 그리기
//        glBindVertexArray(VAO);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
//
//        // 화면 업데이트
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    // === 10. 정리 ===
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//    glDeleteProgram(shaderProgram);
//
//    glfwDestroyWindow(window);
//    glfwTerminate();
//
//    std::cout << "Goodbye!" << std::endl;
//
//    return 0;
//}