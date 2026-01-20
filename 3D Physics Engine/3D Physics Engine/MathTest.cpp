//#include <iostream>
//#include "Vec3.h"
//#include "Mat4.h"
//
//void testVec3() {
//
//	std::cout << "=== Vec3 Test ===" << std::endl;
//
//	// 기본 연산
//	Vec3 a(1, 2, 3);
//	Vec3 b(4, 5, 6);
//
//	std::cout << "a = ";
//	a.print();
//	std::cout << "b = ";
//	b.print();
//
//	Vec3 sum = a + b;
//	std::cout << "a + b = ";
//	sum.print();
//
//	// 내적
//	float dot = a.dot(b);
//	std::cout << "a·b = " << dot << std::endl;
//
//	// 외적
//	Vec3 cross = a.cross(b);
//	std::cout << "a x b = ";
//	cross.print();
//
//	// 외적 = 두 벡터의 수직
//	std::cout << "(a x b) · a = " << cross.dot(a) << " (should be ~0)" << std::endl;
//	std::cout << "(a x b) · b = " << cross.dot(b) << " (should be ~0)" << std::endl;
//
//	// 정규화
//	Vec3 normalized = a.normalize();
//	std::cout << "normalized a = ";
//	normalized.print();
//	std::cout << "length = " << normalized.length() << " (should be ~1)" << std::endl;
//
//	std::cout << std::endl;
//
//}
//
//void testMat4() {
//
//	std::cout << "=== Mat4 Test ===" << std::endl;
//
//	// 단위 행렬
//	Mat4 identity = Mat4::identity();
//	std::cout << "Identity Matrix:" << std::endl;
//	identity.print();
//
//	// 이동 행렬
//	Mat4 translate = Mat4::translate(1, 2, 3);
//	std::cout << "\nTranslate(1, 2, 3):" << std::endl;
//	translate.print();
//
//	Vec3 point(0, 0, 0);
//	Vec3 moved = translate.transformPoint(point);
//	std::cout << "\n(0, 0, 0) moved by (1, 2, 3) = ";
//	moved.print();
//
//	// 회전 행렬(Y축 90도)
//	Mat4 rotateY = Mat4::rotateY(3.1415f / 2.0f);
//	std::cout << "\nRotate(90):" << std::endl;
//	rotateY.print();
//
//	Vec3 v(1, 0, 0);
//	Vec3 rotated = rotateY.transformPoint(v);
//	std::cout << "\n(1, 0, 0) rotated 90 around Y = ";
//	rotated.print();
//	std::cout << "(expected: (0, 0, -1))" << std::endl;
//
//	// 크기 행렬
//	Mat4 scale = Mat4::scale(2, 3, 4);
//	Vec3 v2(1, 1, 1);
//	Vec3 scaled = scale.transformPoint(v2);
//	std::cout << "\n(1, 1, 1) scaled by (2, 3, 4) = ";
//	scaled.print();
//
//	// 행렬 곱셈
//	Mat4 combined = Mat4::translate(0, 1, 0) * Mat4::rotateY(3.14159f / 4.0f);
//	std::cout << "\nCombined (Translate * Rotate):" << std::endl;
//	combined.print();
//
//	std::cout << std::endl;
//
//}
//
//void testMVP() {
//
//	std::cout << "=== MVP TEST ===" << std::endl;
//
//	// Model: 박스를 y=1 위치, Y축으로 45도 회전
//	Mat4 model = Mat4::translate(0, 1, 0) * Mat4::rotateY(3.14159f / 4.0f);
//
//	// View: 카메라가 (0, 2, 5) 위치에서 원점을 바라봄
//	Mat4 view = Mat4::lookAt(
//		Vec3(0, 2, 5), // 카메라 위치
//		Vec3(0, 0, 0), // 바라보는 지점
//		Vec3(0, 1, 0) // 업 벡터
//	);
//
//	// Projection: 원근 투영
//	Mat4 projection = Mat4::perspective(
//
//		3.14159f / 4.0f, // FOV 45도
//		800.0f / 600.0f, // Aspect ratio
//		0.1f, // Near
//		100.0f // Far
//
//	);
//
//	std::cout << "Model Matrix:" << std::endl;
//	model.print();
//
//	std::cout << "\nView Matrix:" << std::endl;
//	view.print();
//
//	std::cout << "\nProjection Matrix:" << std::endl;
//	projection.print();
//
//	// MVP 조합
//	Mat4 mvp = projection * view * model;
//	std::cout << "\nMVP Matrix:" << std::endl;
//	mvp.print();
//
//	// 정점 변환 테스트
//	Vec3 vertex(1, 0, 0);
//	Vec3 transformed = mvp.transformPoint(vertex);
//	std::cout << "\nVertex (1, 0, 0) transformed:" << std::endl;
//	std::cout << " ";
//	transformed.print();
//
//	std::cout << std::endl;
//
//}
//
//int main() {
//
//	std::cout << "============================================" << std::endl;
//	std::cout << "Math Library Test" << std::endl;
//	std::cout << "============================================" << std::endl;
//	std::cout << std::endl;
//
//	testVec3();
//	testMat4();
//	testMVP();
//
//	std::cout << "============================================" << std::endl;
//	std::cout << "All tests completed!" << std::endl;
//	std::cout << "============================================" << std::endl;
//
//	return 0;
//}