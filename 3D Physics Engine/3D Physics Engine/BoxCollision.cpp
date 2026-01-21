//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <vector>
//#include "Vec3.h"
//#include "Mat4.h"
//#include "RigidBody3D.h"
//#include "Collision3D.h"
//
//const int WIDTH = 800;
//const int HEIGHT = 600;
//
//// 카메라
//float cameraAngleX = 0.4f;
//float cameraAngleY = 0.0f;
//float cameraDistance = 20.0f;
//float autoRotateSpeed = 0.2f;
//
//std::vector<RigidBody3D> boxes;
//
//// Vertex Shader
//const char* vertexShaderSource = R"(
//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//
//out vec3 vertexColor;
//uniform mat4 mvp;
//
//void main() {
//
//	gl_Position = mvp * vec4(aPos, 1.0);
//	vertexColor = aColor;
//
//}
//
//)";
//
//// Fragment Shader
//const char* fragmentShaderSource = R"(
//#version 330 core
//in vec3 vertexColor;
//out vec4 FragColor;
//
//void main() {
//
//	FragColor = vec4(vertexColor, 1.0);
//	
//}
//
//)";
//
//void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//
//	float centerX = WIDTH / 2.0f;
//	float centerY = HEIGHT / 2.0f;
//
//	float offsetX = (float)(xpos - centerX) / WIDTH;
//	float offsetY = (float)(ypos - centerY) / HEIGHT;
//
//	autoRotateSpeed = 0.2f + offsetX * 1.5f;
//	cameraAngleX = 0.4f - offsetY * 0.5f;
//
//	if (cameraAngleX > 1.2f) cameraAngleX = 1.2f;
//	if (cameraAngleX < 0.0f) cameraAngleX = 0.0f;
//
//}
//
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
//
//	cameraDistance -= (float)yoffset;
//	if (cameraDistance < 8.0f) cameraDistance = 8.0f;
//	if (cameraDistance > 40.0f) cameraDistance = 40.0f;
//
//}
//
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
//
//		glfwSetWindowShouldClose(window, true);
//
//	}
//
//	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
//
//		boxes.clear();
//
//		// 바닥 (static)
//		RigidBody3D ground = RigidBody3D::createStatic(Vec3(0, -0.5f, 0), Vec3(20, 1, 20));
//		ground.color = Vec3(0.2f, 0.3f, 0.2f);
//		boxes.push_back(ground);
//
//		// 박스들
//		for (int i = 0; i < 5; i++) {
//
//			RigidBody3D box(Vec3((i - 2) * 2.5f, 8.0f + i * 3.0f, 0), Vec3(1, 1, 1), 1.0f);
//			box.angularVelocity = Vec3(
//				(rand() % 200 - 100) / 50.0f,
//				(rand() % 200 - 100) / 50.0f,
//				(rand() % 200 - 100) / 50.0f
//			);
//
//			float hue = i / 5.0f;
//			box.color = Vec3(
//				0.5f + 0.5f * cos(hue * 6.28f),
//				0.5f + 0.5f * cos(hue * 6.28f + 2.09f),
//				0.5f + 0.5f * cos(hue * 6.28f + 4.18f)
//			);
//
//			boxes.push_back(box);
//
//		}
//		
//		std::cout << "Reset" << boxes.size() << " boxes" << std::endl;
//
//	}
//
//	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
//
//		// 새 박스 추가
//		RigidBody3D box(Vec3(0, 15, 0), Vec3(1, 1, 1), 1.0f);
//		box.angularVelocity = Vec3(
//			(rand() % 200 - 100) / 50.0f,
//			(rand() % 200 - 100) / 50.0f,
//			(rand() % 200 - 100) / 50.0f
//		);
//		box.color = Vec3(
//			(rand() % 100) / 100.0f,
//			(rand() % 100) / 100.0f,
//			(rand() % 100) / 100.0f
//		);
//		boxes.push_back(box);
//		std::cout << "Added box! Total: " << boxes.size() - 1 << std::endl;
//	}
//
//}
//
//int main() {
//
//	if (!glfwInit()) {
//
//		std::cerr << "Failed to initialize GLFW!" << std::endl;
//		return -1;
//
//	}
//
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Box Collision", NULL, NULL);
//	if (!window) {
//
//		std::cerr << "Failed to create window" << std::endl;
//		glfwTerminate();
//		return -1;
//
//	}
//
//	glfwMakeContextCurrent(window);
//	glfwSetKeyCallback(window, key_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//
//		std::cerr << "Failed to initialize GLAD" << std::endl;
//		return -1;
//
//	}
//
//	std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
//
//	glEnable(GL_DEPTH_TEST);
//
//	// Shader Compile
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
//	glCompileShader(vertexShader);
//
//	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
//	glCompileShader(fragmentShader);
//
//	unsigned int shaderProgram = glCreateProgram();
//	glAttachShader(shaderProgram, vertexShader);
//	glAttachShader(shaderProgram, fragmentShader);
//	glLinkProgram(shaderProgram);
//
//	glDeleteShader(vertexShader);
//	glDeleteShader(fragmentShader);
//
//	// 박스 정점
//	float boxVertices[] = {
//
//		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
//		 0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
//		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
//		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
//		-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
//		-0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
//
//		-0.5f, -0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
//		 0.5f,  0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
//		 0.5f, -0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
//		-0.5f,  0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
//		 0.5f,  0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
//		-0.5f, -0.5f, -0.5f,  0.8f, 0.8f, 0.8f,
//
//		-0.5f,  0.5f,  0.5f,  0.9f, 0.9f, 0.9f,
//		-0.5f,  0.5f, -0.5f,  0.9f, 0.9f, 0.9f,
//		-0.5f, -0.5f, -0.5f,  0.9f, 0.9f, 0.9f,
//		-0.5f, -0.5f, -0.5f,  0.9f, 0.9f, 0.9f,
//		-0.5f, -0.5f,  0.5f,  0.9f, 0.9f, 0.9f,
//		-0.5f,  0.5f,  0.5f,  0.9f, 0.9f, 0.9f,
//
//		 0.5f,  0.5f,  0.5f,  0.7f, 0.7f, 0.7f,
//		 0.5f, -0.5f, -0.5f,  0.7f, 0.7f, 0.7f,
//		 0.5f,  0.5f, -0.5f,  0.7f, 0.7f, 0.7f,
//		 0.5f, -0.5f, -0.5f,  0.7f, 0.7f, 0.7f,
//		 0.5f,  0.5f,  0.5f,  0.7f, 0.7f, 0.7f,
//		 0.5f, -0.5f,  0.5f,  0.7f, 0.7f, 0.7f,
//
//		-0.5f,  0.5f, -0.5f,  0.95f, 0.95f, 0.95f,
//		 0.5f,  0.5f,  0.5f,  0.95f, 0.95f, 0.95f,
//		 0.5f,  0.5f, -0.5f,  0.95f, 0.95f, 0.95f,
//		 0.5f,  0.5f,  0.5f,  0.95f, 0.95f, 0.95f,
//		-0.5f,  0.5f, -0.5f,  0.95f, 0.95f, 0.95f,
//		-0.5f,  0.5f,  0.5f,  0.95f, 0.95f, 0.95f,
//
//		-0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.6f,
//		 0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.6f,
//		 0.5f, -0.5f,  0.5f,  0.6f, 0.6f, 0.6f,
//		 0.5f, -0.5f,  0.5f,  0.6f, 0.6f, 0.6f,
//		-0.5f, -0.5f,  0.5f,  0.6f, 0.6f, 0.6f,
//		-0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.6f
//
//	};
//
//	unsigned int VAO, VBO;
//	glGenVertexArrays(1, &VAO);
//	glGenBuffers(1, &VBO);
//
//	glBindVertexArray(VAO);
//	glBindBuffer(GL_ARRAY_BUFFER, VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
//
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//	glEnableVertexAttribArray(1);
//	
//	int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
//
//	// 초기 박스
//	RigidBody3D ground = RigidBody3D::createStatic(Vec3(0, -0.5f, 0), Vec3(20, 1, 20));
//	ground.color = Vec3(0.2f, 0.3f, 0.2f);
//	boxes.push_back(ground);
//
//	srand((unsigned int)glfwGetTime());
//	for (int i = 0; i < 5; i++) {
//
//		RigidBody3D box(Vec3((i - 2) * 2.5f, 8.0f + i * 3.0f, 0), Vec3(1, 1, 1), 1.0f);
//		box.angularVelocity = Vec3(
//			(rand() % 200 - 100) / 50.0f,
//			(rand() % 200 - 100) / 50.0f,
//			(rand() % 200 - 100) / 50.0f
//		);
//
//		float hue = i / 5.0f;
//		box.color = Vec3(
//			0.5f + 0.5f * cos(hue * 6.28f),
//			0.5f + 0.5f * cos(hue * 6.28f + 2.09f),
//			0.5f + 0.5f * cos(hue * 6.28f + 4.18f)
//		);
//
//		boxes.push_back(box);
//
//	}
//
//	Mat4 projection = Mat4::perspective(
//		3.14159f / 4.0f,
//		(float)WIDTH / HEIGHT,
//		0.1f,
//		100.0f
//	);
//
//	float lastTime = (float)glfwGetTime();
//	int collisionCount = 0;
//
//	// Rendering Loop
//	while (!glfwWindowShouldClose(window)) {
//
//		float currentTime = (float)glfwGetTime();
//		float deltaTime = currentTime - lastTime;
//		lastTime = currentTime;
//
//		if (deltaTime > 0.02f) deltaTime = 0.02f;
//
//		cameraAngleY += autoRotateSpeed * deltaTime;
//
//		Vec3 gravity(0, -9.8f, 0);
//
//		// Physics update
//		for (size_t i = 1; i < boxes.size(); i++) {
//
//			if (boxes[i].velocity.lengthSq() < 0.05f && boxes[i].position.y < 5.0f) {
//
//				boxes[i].isSleeping = true;
//				boxes[i].velocity = Vec3(0, 0, 0);
//				boxes[i].angularVelocity *= 0.5f;
//
//			}
//			else {
//				boxes[i].isSleeping = false;
//			}
//
//			if (!boxes[i].isSleeping) {
//
//				boxes[i].applyForce(gravity* boxes[i].mass, deltaTime);
//				boxes[i].integrate(deltaTime);
//
//			}
//
//		}
//
//		// 충돌 검사 
//		collisionCount = 0;
//		for (int iter = 0; iter < 3; iter++) {
//
//			for (size_t i = 0; i < boxes.size(); i++) {
//
//				if (boxes[i].isSleeping) continue;
//
//				for (size_t j = i + 1; j < boxes.size(); j++) {
//
//					Vec3 diff(
//						boxes[i].position.x - boxes[j].position.x,
//						boxes[i].position.y - boxes[j].position.y,
//						boxes[i].position.z - boxes[j].position.z
//					);
//
//					float distSq = diff.lengthSq();
//					float maxDist = (boxes[i].halfSize.length() + boxes[j].halfSize.length()) * 3.0f;
//					if (distSq > maxDist * maxDist) continue;
//
//					CollisionInfo info = checkBoxCollision(boxes[i], boxes[j]);
//					if (info.hasCollision) {
//
//						resolveCollision(boxes[i], boxes[j], info);
//						collisionCount++;
//
//						boxes[i].isSleeping = false;
//						boxes[j].isSleeping = false;
//
//					}
//
//				}
//
//			}
//
//		}
//
//		// 카메라
//		float camX = sin(cameraAngleY) * cos(cameraAngleX) * cameraDistance;
//		float camY = sin(cameraAngleX) * cameraDistance;
//		float camZ = cos(cameraAngleY) * cos(cameraAngleX) * cameraDistance;
//
//		Vec3 cameraPos(camX, camY + 5.0f, camZ);
//		Vec3 cameraTarget(0, 3, 0);
//		Vec3 cameraUp(0, 1, 0);
//
//		Mat4 view = Mat4::lookAt(cameraPos, cameraTarget, cameraUp);
//
//		glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		glUseProgram(shaderProgram);
//		glBindVertexArray(VAO);
//
//		// 박스 그리기
//		for (size_t i = 0; i < boxes.size(); i++) {
//
//			Mat4 model = boxes[i].getModelMatrix();
//
//			// 크기 적용
//			model = model * Mat4::scale(
//				boxes[i].halfSize.x * 2.0f,
//				boxes[i].halfSize.y * 2.0f,
//				boxes[i].halfSize.z * 2.0f
//			);
//
//			Mat4 mvp = projection * view * model;
//			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp.data());
//
//			glDrawArrays(GL_TRIANGLES, 0, 36);
//
//		}
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//
//	}
//
//	glDeleteVertexArrays(1, &VAO);
//	glDeleteBuffers(1, &VBO);
//	glDeleteProgram(shaderProgram);
//
//	glfwTerminate();
//
//	return 0;
//
//}