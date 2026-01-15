#pragma once
#include "Vec2.h"
#include "SFML/Graphics.hpp"
#include <vector>
#include <unordered_map>
#include <functional>

template<typename T>
class SpatialHash {

public:

	float cellSize;
	std::unordered_map<int, std::vector<T*>> grid;

	SpatialHash(float cellSize) : cellSize(cellSize) {}

	// === 해시 함수 (cell 좌표 -> 1D index) ===
	int hash(int x, int y) const {

		// Cantor pairing function
		return x * 73856093 ^ y * 19349663;

	}

	// === 위치를 cell 좌표로 변환 ===
	void getCellCoords(Vec2 position, int& cellX, int& cellY) const {

		cellX = (int)std::floor(position.x / cellSize);
		cellY = (int)std::floor(position.y / cellSize);

	}

	// === 객체가 차지하는 셀 ===
	std::vector<int> getObjectCells(Vec2 position, float radius) const {

		std::vector<int> cells;

		// AABB
		int minX = (int)std::floor((position.x - radius) / cellSize);
		int maxX = (int)std::floor((position.x + radius) / cellSize);
		int minY = (int)std::floor((position.y - radius) / cellSize);
		int maxY = (int)std::floor((position.y + radius) / cellSize);

		for (int x = minX; x <= maxX; x++) {

			for (int y = minY; y <= maxY; y++) {

				cells.push_back(hash(x, y));

			}

		}

		return cells;

	}

	// === grid 초기화 ===
	void clear() {

		grid.clear();

	}

	// === 객체 삽입 ===
	void insert(T* object, Vec2 position, float radius) {

		std::vector<int> cells = getObjectCells(position, radius);

		for (int cellHash : cells) {

			grid[cellHash].push_back(object);

		}

	}

	// === 근처 객체 찾기 ===
	std::vector<T*> getNearby(Vec2 position, float radius) const {

		std::vector<int> cells = getObjectCells(position, radius);
		std::vector<T*> nearby;

		for (int cellHash : cells) {

			auto it = grid.find(cellHash);
			if (it != grid.end()) {

				nearby.insert(nearby.end(), it->second.begin(), it->second.end());

			}

		}

		// 중복 제거
		std::sort(nearby.begin(), nearby.end());
		nearby.erase(std::unique(nearby.begin(), nearby.end()), nearby.end());

		return nearby;

	}

	// === 모든 충돌 쌍 찾기 ===
	template<typename GetPosFunc, typename GetRadiusFunc>
	std::vector<std::pair<T*, T*>> getPotentialCollisions(
		const std::vector<T>& objects,
		GetPosFunc getPos,
		GetRadiusFunc getRadius
	) {

		clear();

		// 모든 object 삽입
		for (auto& obj : objects) {

			insert(const_cast<T*>(&obj), getPos(obj), getRadius(obj));

		}

		// 충돌 쌍 찾기
		std::vector<std::pair<T*, T*>> pairs;
		std::set<std::pair<T*, T*>> uniquePairs; // 중복 방지

		for (auto& obj : objects) {

			std::vector<T*> nearby = getNearby(getPos(obj), getRadius(obj));

			for (T* other : nearby) {

				if (&obj < other) {

					auto pair = std::make_pair(const_cast<T*>(&obj), other);
					if (uniquePairs.insert(pair).second) {

						pairs.push_back(pair);

					}

				}

			}

		}

		return pairs;

	}

	void drawGrid(sf::RenderWindow& window, float width, float height) const {

		int numCellsX = (int)(width / cellSize) + 1;
		int numCellsY = (int)(height / cellSize) + 1;

		for (int x = 0; x <= numCellsX; x++) {

			float posX = x * cellSize;
			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f(posX, 0), sf::Color(50, 50, 50)),
				sf::Vertex(sf::Vector2f(posX, height), sf::Color(50, 50, 50))
			};
			window.draw(line, 2, sf::Lines);
		}

		for (int y = 0; y <= numCellsY; y++) {

			float posY = y * cellSize;
			sf::Vertex line[] = {
				sf::Vertex(sf::Vector2f(0, posY), sf::Color(50, 50, 50)),
				sf::Vertex(sf::Vector2f(width, posY), sf::Color(50, 50, 50))
			};
			window.draw(line, 2, sf::Lines);
		}

	}

};