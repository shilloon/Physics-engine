#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Vec3.h"
#include "Mat4.h"

const int WIDTH = 800;
const int HEIGHT = 600;
const int NUM_BOXES = 5;

// 마우스 상태
float cameraAngleX = 0.3f; // 위아래
float cameraAngleY = 0.0f; // 좌우
float cameraDistance = 10.0f;
float autoRotateSpeed = 0.3f; // 자동 회전

// 박스 데이터
struct Box {

	Vec3 position;
	Vec3 velocity;
	Vec3 angularVelocity; // 회전 속도
	float angleX;
	float angleY;
	float angleZ;
	Vec3 color;

};

std::vector<Box> boxes;

// Vertex Shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vertexColor;
uniform mat4 mvp;

void main(){

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


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

	// 마우스 위치로 회전 속도 조절
	float centerX = WIDTH / 2.0f;
	float centerY = HEIGHT / 2.0f;

	float offsetX = (float)(xpos - centerX) / WIDTH;
	float offsetY = (float)(ypos - centerY) / HEIGHT;

	// 마우스가 중심에서 멀수록 빠르게 회전
	autoRotateSpeed = 0.3f + offsetX * 2.0f;

	// 위아래는 마우스 위치에 따라 살짝 조절
	cameraAngleX = 0.3f - offsetY * 0.5f;

	// 제한
	if (cameraAngleX > 1.2f) cameraAngleX = 1.2f;
	if (cameraAngleX < -0.3f) cameraAngleX = -0.3f;
	
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	cameraDistance -= (float)yoffset * 0.5f;
	if (cameraDistance < 2.0f) cameraDistance = 2.0f;
	if (cameraDistance > 20.0f) cameraDistance = 20.0f;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {

		glfwSetWindowShouldClose(window, true);

	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {

		for (int i = 0; i < NUM_BOXES; i++) {

			boxes[i].position = Vec3((i - 2) * 2.0f, 5.0f + i * 2.0f, 0);
			boxes[i].velocity = Vec3(0, 0, 0);
			boxes[i].angularVelocity = Vec3(
				(rand() % 200 - 100) / 100.0f,
				(rand() % 200 - 100) / 100.0f,
				(rand() % 200 - 100) / 100.0f
			);
			boxes[i].angleX = 0;
			boxes[i].angleY = 0;
			boxes[i].angleZ = 0;
		
		}

		std::cout << "Reset" << std::endl;

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

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Physics Boxes", NULL, NULL);
	if (!window) {

		std::cerr << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;

	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
		
	}

	std::cout << "OpenGL" << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);

	// Shader Compile
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);


	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// 박스 정점 데이터
	float boxVertices[] = {
		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
		 0.5f,  0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
		 0.5f, -0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
		-0.5f,  0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
		 0.5f,  0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
		-0.5f, -0.5f, -0.5f,  0.8f, 0.8f, 0.8f,

		-0.5f,  0.5f,  0.5f,  0.9f, 0.9f, 0.9f,
		-0.5f,  0.5f, -0.5f,  0.9f, 0.9f, 0.9f,
		-0.5f, -0.5f, -0.5f,  0.9f, 0.9f, 0.9f,
		-0.5f, -0.5f, -0.5f,  0.9f, 0.9f, 0.9f,
		-0.5f, -0.5f,  0.5f,  0.9f, 0.9f, 0.9f,
		-0.5f,  0.5f,  0.5f,  0.9f, 0.9f, 0.9f,

		 0.5f,  0.5f,  0.5f,  0.7f, 0.7f, 0.7f,
		 0.5f, -0.5f, -0.5f,  0.7f, 0.7f, 0.7f,
		 0.5f,  0.5f, -0.5f,  0.7f, 0.7f, 0.7f,
		 0.5f, -0.5f, -0.5f,  0.7f, 0.7f, 0.7f,
		 0.5f,  0.5f,  0.5f,  0.7f, 0.7f, 0.7f,
		 0.5f, -0.5f,  0.5f,  0.7f, 0.7f, 0.7f,

		-0.5f,  0.5f, -0.5f,  0.95f, 0.95f, 0.95f,
		 0.5f,  0.5f,  0.5f,  0.95f, 0.95f, 0.95f,
		 0.5f,  0.5f, -0.5f,  0.95f, 0.95f, 0.95f,
		 0.5f,  0.5f,  0.5f,  0.95f, 0.95f, 0.95f,
		-0.5f,  0.5f, -0.5f,  0.95f, 0.95f, 0.95f,
		-0.5f,  0.5f,  0.5f,  0.95f, 0.95f, 0.95f,

		-0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.6f,
		 0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.6f,
		 0.5f, -0.5f,  0.5f,  0.6f, 0.6f, 0.6f,
		 0.5f, -0.5f,  0.5f,  0.6f, 0.6f, 0.6f,
		-0.5f, -0.5f,  0.5f,  0.6f, 0.6f, 0.6f,
		-0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.6f
	};

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 바닥 정점 데이터
	float groundVertices[] = {
		-10.0f, 0.0f, -10.0f, 0.3f, 0.3f, 0.3f,
		10.0f, 0.0f, -10.0f, 0.3f, 0.3f, 0.3f,
		10.0f, 0.0f, 10.0f, 0.3f, 0.3f, 0.3f,
		10.0f, 0.0f, 10.0f, 0.3f, 0.3f, 0.3f,
		-10.0f, 0.0f, 10.0f, 0.3f, 0.3f, 0.3f,
		-10.0f, 0.0f, -10.0f, 0.3f, 0.3f, 0.3f
	};

	unsigned int groundVAO, groundVBO;
	glGenVertexArrays(1, &groundVAO);
	glGenBuffers(1, &groundVBO);

	glBindVertexArray(groundVAO);
	glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");

	// 박스 초기화
	srand((unsigned int)glfwGetTime());
	for (int i = 0; i < NUM_BOXES; i++) {

		Box box;
		box.position = Vec3((i - 2) * 2.0f, 5.0f + i * 2.0f, 0);
		box.velocity = Vec3(0, 0, 0);
		box.angularVelocity = Vec3(
			(rand() % 200 - 100) / 100.0f,
			(rand() % 200 - 100) / 100.0f,
			(rand() % 200 - 100) / 100.0f
		);
		box.angleX = 0;
		box.angleY = 0;
		box.angleZ = 0;

		// 색상
		float hue = i / (float)NUM_BOXES;
		box.color = Vec3(
			0.5f + 0.5f * cos(hue * 6.28f),
			0.5f + 0.5f * cos(hue * 6.28f + 2.09f),
			0.5f + 0.5f * cos(hue * 6.28f + 4.18f)
		);

		boxes.push_back(box);

	}

	// Projection 행렬
	Mat4 projection = Mat4::perspective(
		3.14159f / 4.0f,
		(float)WIDTH / HEIGHT,
		0.1f,
		100.0f
	);

	float lastTime = (float)glfwGetTime();

	// Rendering Loop
	while (!glfwWindowShouldClose(window)) {

		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		// Physics Update
		Vec3 gravity(0, -9.8f, 0);

		cameraAngleY += autoRotateSpeed * deltaTime;

		for (int i = 0; i < NUM_BOXES; i++) {

			// 중력 작용
			boxes[i].velocity += gravity * deltaTime;

			// 위치 업데이트
			boxes[i].position += boxes[i].velocity * deltaTime;

			// 회전 업데이트
			boxes[i].angleX += boxes[i].angularVelocity.x * deltaTime;
			boxes[i].angleY += boxes[i].angularVelocity.y * deltaTime;
			boxes[i].angleZ += boxes[i].angularVelocity.z * deltaTime;

			// 바닥 충돌
			if (boxes[i].position.y < 0.5f) {

				boxes[i].position.y = 0.5f;
				boxes[i].velocity.y *= -0.7f; // 팅김 (에너지 손실)

				// 바닥에 닿을 시 회전 감속
				boxes[i].angularVelocity *= 0.98f;

			}

			// 범위 밖으로 나가면 되돌리기
			if (boxes[i].position.x < -9.0f) {

				boxes[i].position.x = -9.0f;
				boxes[i].velocity.x *= -0.5f;

			}

			if (boxes[i].position.x > 9.0f) {

				boxes[i].position.x = 9.0f;
				boxes[i].velocity.x *= -0.5f;

			}

			if (boxes[i].position.z < -9.0f) {

				boxes[i].position.z = -9.0f;
				boxes[i].velocity.z *= -0.5f;

			}

			if (boxes[i].position.z > 9.0f) {

				boxes[i].position.z = 9.0f;
				boxes[i].velocity.z *= -0.5f;

			}

		}

		// 카메라 위치 계산
		float camX = sin(cameraAngleY) * cos(cameraAngleX) * cameraDistance;
		float camY = sin(cameraAngleX) * cameraDistance;
		float camZ = cos(cameraAngleY) * cos(cameraAngleX) * cameraDistance;

		Vec3 cameraPos(camX, camY + 2.0f, camZ);
		Vec3 cameraTarget(0, 1, 0);
		Vec3 cameraUp(0, 1, 0);

		Mat4 view = Mat4::lookAt(cameraPos, cameraTarget, cameraUp);

		// 화면 지우기
		glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);

		// 바닥 그리기
		Mat4 groundModel = Mat4::identity();
		Mat4 groundMVP = projection * view * groundModel;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, groundMVP.data());
		glBindVertexArray(groundVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 박스 그리기
		glBindVertexArray(VAO);
		for (int i = 0; i < NUM_BOXES; i++) {

			Mat4 model = Mat4::identity();
			model = model * Mat4::translate(boxes[i].position);
			model = model * Mat4::rotateX(boxes[i].angleX);
			model = model * Mat4::rotateY(boxes[i].angleY);
			model = model * Mat4::rotateZ(boxes[i].angleZ);

			Mat4 mvp = projection * view * model;
			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data());

			glDrawArrays(GL_TRIANGLES, 0, 36);

		}

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// 정리
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &groundVAO);
	glDeleteBuffers(1, &groundVBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();

	return 0;

}