//
// Created by Michal Přikryl on 07.08.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <cmath>
#include <algorithm>
#include <vector.h>

namespace TemplatePlugin::VectorExtends
{
    inline const Vector VectorZero{0.0f, 0.0f, 0.0f};
    inline const QAngle RotationZero{0.0f, 0.0f, 0.0f};
    inline const Vector StaticTeleportVector{9999999.0f, 8888888.0f, 777777.0f};

    inline Vector Clone(const Vector& vector)
    {
        return Vector{vector.x, vector.y, vector.z};
    }

    inline QAngle QClone(const QAngle& angle)
    {
        return QAngle{angle.x, angle.y, angle.z};
    }

    inline Vector Adds(const Vector& vector, const Vector& other)
    {
        return Vector{vector.x + other.x, vector.y + other.y, vector.z + other.z};
    }

    inline Vector Scale(const Vector& vector, float scale)
    {
        return Vector{vector.x * scale, vector.y * scale, vector.z * scale};
    }

    inline Vector Normalize(const Vector& vector)
    {
        float length = vector.Length();
        return Vector{vector.x / length, vector.y / length, vector.z / length};
    }

    inline float Distance(const Vector& vector, const Vector& other)
    {
        return std::sqrt(
            std::pow(vector.x - other.x, 2.0f) +
            std::pow(vector.y - other.y, 2.0f) +
            std::pow(vector.z - other.z, 2.0f)
        );
    }

    inline float DistanceSquared(const Vector& vector, const Vector& other)
    {
        return
            std::pow(vector.x - other.x, 2.0f) +
            std::pow(vector.y - other.y, 2.0f) +
            std::pow(vector.z - other.z, 2.0f);
    }

    inline Vector MakeVectorFromPoints(const Vector& pt1, const Vector& pt2)
    {
        return Vector{pt2.x - pt1.x, pt2.y - pt1.y, pt2.z - pt1.z};
    }

    inline Vector SubtractVectors(const Vector& vec1, const Vector& vec2)
    {
        return Vector{vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z};
    }

    inline Vector NegateVector(const Vector& vec)
    {
        return Vector{-vec.x, -vec.y, -vec.z};
    }

    inline float DegToRad(float degrees)
    {
        return degrees * (static_cast<float>(M_PI) / 180.0f);
    }

    inline float RadToDeg(float radians)
    {
        return radians * (180.0f / static_cast<float>(M_PI));
    }

    inline bool IsInsideBox(const Vector& playerVector, const Vector& corner1, const Vector& corner2, float height = 0.0f)
    {
        float minX = std::min(corner1.x, corner2.x);
        float minY = std::min(corner1.y, corner2.y);
        float minZ = std::min(corner1.z, corner2.z);

        float maxX = std::max(corner1.x, corner2.x);
        float maxY = std::max(corner1.y, corner2.y);
        float maxZ = std::max(corner1.z, corner2.z);

        return playerVector.x >= minX && playerVector.x <= maxX &&
               playerVector.y >= minY && playerVector.y <= maxY &&
               playerVector.z >= minZ && playerVector.z <= maxZ + height;
    }

    inline bool IsInsideField(const Vector& playerVector, const Vector& corner1, const Vector& corner2)
    {
        float minX = std::min(corner1.x, corner2.x);
        float minY = std::min(corner1.y, corner2.y);

        float maxX = std::max(corner1.x, corner2.x);
        float maxY = std::max(corner1.y, corner2.y);

        return playerVector.x >= minX && playerVector.x <= maxX &&
               playerVector.y >= minY && playerVector.y <= maxY;
    }

    inline float ArcCosine(float value)
    {
        return std::acos(value);
    }

    inline float GetVectorDotProduct(const Vector& vec1, const Vector& vec2)
    {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    inline float Dot(const Vector& vec1, const Vector& vec2)
    {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }
}