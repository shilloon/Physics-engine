#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Vec3.h"
#include "Mat4.h"

const int WIDTH = 800;
const int HEIGHT = 600;

// 상태 변수
bool isPaused = false;
float rotationSpeedY = 1.0f;
float rotationSpeedX = 0.5f;

// Vertex Shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;

uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(aPos, 1.0);
	vertexColor = aColor;
}

)";

// Fragment Shader
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

void main() {
	FragColor = vec4(vertexColor, 1.0);
}

)";

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {

		isPaused = !isPaused;
		std::cout << (isPaused ? "Paused" : "Playing") << std::endl;

	}

	// 회전 속도 조절
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {

		rotationSpeedY += 0.5f;
		std::cout << "Y Speed: " << rotationSpeedY << std::endl;

	}

	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {

		rotationSpeedY -= 0.5f;
		std::cout << "Y Speed: " << rotationSpeedY << std::endl;

	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {

		rotationSpeedX += 0.5f;
		std::cout << "X Speed: " << rotationSpeedX << std::endl;

	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {

		rotationSpeedX -= 0.5f;
		std::cout << "X Speed: " << rotationSpeedX << std::endl;

	}

}

int main() {

	// GLFW 초기화
	if (!glfwInit()) {

		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;

	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Rotating Box", NULL, NULL);
	if (!window) {

		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;

	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;

	}

	std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;

	// Depth Test
	glEnable(GL_DEPTH_TEST);

	// Shader Complie
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {

		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "Vertex Shader ErrorL\n" << infoLog << std::endl;

	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {

		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "Fragment Shader Error:\n" << infoLog << std::endl;

	}

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {

		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Program Link Error:\n" << infoLog << std::endl;

	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 박스 정점 데이터 (36개 정점)
	float vertices[] = {

		// 앞면 (빨강)
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		// 뒷면 (초록)
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

		// 왼쪽면 (파랑)
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

		// 오른쪽면 (노랑)
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

		 // 윗면 (자홍)
		 -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		  0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		  0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		 -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

		 // 아랫면 (청록)
		 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f

	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 위치
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 색상
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Uniform 위치
	int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");

	// 카메라 설정
	Vec3 cameraPos(0, 2, 5); // 카메라 위치
	Vec3 cameraTarget(0, 0, 0); // 바라보는 곳 
	Vec3 cameraUp(0, 1, 0); // 위 방향

	// View & Projection 행렬
	Mat4 view = Mat4::lookAt(cameraPos, cameraTarget, cameraUp);
	Mat4 projection = Mat4::perspective(
		3.14159f / 4.0f, // FOV 45도
		(float)WIDTH / HEIGHT, // Aspect ratio
		0.1f, // Near
		100.0f // Far
	);

	float angleY = 0.0f;
	float angleX = 0.0f;
	float lastTime = (float)glfwGetTime();

	// Rendering Loop
	while (!glfwWindowShouldClose(window)) {

		// Delta Time
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// 회전 업데이트
		if (!isPaused) {

			angleY += rotationSpeedY * deltaTime;
			angleX += rotationSpeedX * deltaTime;

		}

		// 화면 지우기
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Model 행렬
		Mat4 model = Mat4::identity();
		model = model * Mat4::rotateY(angleY);
		model = model * Mat4::rotateX(angleX);

		// MVP 조합
		Mat4 mvp = projection * view * model;

		// Shader 사용
		glUseProgram(shaderProgram);
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data());

		// 박스 그리기
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();

	return 0;

}