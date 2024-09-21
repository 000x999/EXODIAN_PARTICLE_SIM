#pragma once
#ifndef HASHGRID_H
#define HASHGRID_H
#include "raylib.h"
#include <vector>
#include <memory>
#include <array>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <unordered_map>

template <typename T>

struct CollisionCell {
    //Increased capacity for fewer cache misses..
    //8 is good enough, can go all the way down to 2 even.
    static constexpr uint8_t m_cellCapacity = 8;
    uint8_t m_objectsCount = 0;
    std::array<T*, m_cellCapacity> m_objects;

    CollisionCell() : m_objectsCount(0) {}

    void addAtom(T& obj) {
        if (m_objectsCount < m_cellCapacity) {
            m_objects[m_objectsCount++] = &obj;
        }
    }

    void clear() {
        m_objectsCount = 0;
    }
};


template <typename T>
class Hash {
public:

    std::vector<CollisionCell<T>> m_data;
    float m_cellSize = 10.0f;
    uint32_t m_maxSize = 1000u;
    uint32_t m_gridWidth, m_gridHeight;

    Hash() {
        m_gridWidth  = static_cast<uint16_t>((m_maxSize) / m_cellSize);
        m_gridHeight = static_cast<uint16_t>((m_maxSize) / m_cellSize);
        m_data.resize(m_gridWidth * m_gridHeight);
    }

    uint32_t HashFunction(const Vec2& pos) const {
        //Hashes the position of a single particle
        uint32_t x = static_cast<uint32_t>(pos.x / m_cellSize);
        uint32_t y = static_cast<uint32_t>(pos.y / m_cellSize);
        x = std::min(x, m_gridWidth  - 1);
        y = std::min(y, m_gridHeight - 1);
        return (x + y * m_gridWidth);
    }

    void AddToHash(T& obj) {
        uint32_t key = HashFunction(obj.GetPos());
        if (key < m_data.size()) {
            m_data[key].addAtom(obj);
        }
        else {
            std::cerr << "***INDEX OOR***" << key << std::endl;
        }
    }

    std::vector<std::reference_wrapper<T>> GetPotentialCollisions(const T& obj) const {
        std::vector<std::reference_wrapper<T>> potentialCollisions;
        //Preallocate to avoid frequent reallocations
        //I can't seem to find the perfect balance but 100 works well, 20 works good as well
        potentialCollisions.reserve(100);  
        Vec2 pos = obj.GetPos();
        //Create the hashing for each cell respectively
        //Could be done with another hashing function but inlining it in this case seems better
        uint16_t cellX = static_cast<uint16_t>(floor(pos.x / m_cellSize));
        uint16_t cellY = static_cast<uint16_t>(floor(pos.y / m_cellSize));
        //Defines the starting posistions for the iterators
        //Adds an extra neighboring cell on each side to check for as well 
        //Increased collision accuracy at the cost of some performance
        uint16_t minX = std::max(cellX - 1, 0);
        uint16_t maxX = std::min(cellX + 1, static_cast<uint16_t>(m_gridWidth)  - 1);
        uint16_t minY = std::max(cellY - 1, 0);
        uint16_t maxY = std::min(cellY + 1, static_cast<uint16_t>(m_gridHeight) - 1);
        //Fetches all the potential collisions between particles within a valid range
        for (uint16_t x = minX; x <= maxX; x++) {
            for (uint16_t y = minY; y <= maxY; y++) {
                uint16_t key = x + y * m_gridWidth;
                if (key < m_data.size()) {
                    const auto& cell = m_data[key];
                    for (uint8_t i = 0; i < cell.m_objectsCount; ++i) {
                        //returns a vector of all potential collisions
                        potentialCollisions.push_back(*cell.m_objects[i]);
                    }
                }
                else {
                    std::cerr << "***COLLISION OOR***" << key << std::endl;
                }
            }
        }

        return potentialCollisions;
    }

    void Clear() {
        for (auto& cell : m_data) {
            cell.clear();
        }
    }

    void DrawGrid() const {
        for (uint32_t x = 0; x < m_gridWidth; ++x) {
            for (uint32_t y = 0; y < m_gridHeight; ++y) {
                float startX = x * m_cellSize;
                float startY = y * m_cellSize;
                DrawRectangleLinesEx({ startX, startY, m_cellSize, m_cellSize }, 0.5f, WHITE);
            }
        }
    }
};
#endif