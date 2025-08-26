#pragma once

#include <cmath>
#include <type_traits>

/// @brief DaneJoe namespace
namespace DaneJoe
{
    /**
     * @struct Vector2D
     * @brief 2D vector
     */
    template<typename T = int>
    struct Vector2D
    {
        /**
         * @brief Quadrant
         * @brief 象限
         */
        enum class Quadrant
        {
            FIRST,
            SECOND,
            THIRD,
            FOURTH,
            POSITIVE_X,
            NEGATIVE_X,
            POSITIVE_Y,
            NEGATIVE_Y,
            CENTER
        };
        /**
         * @brief Direction
         * @brief 方向
         */
        enum class Direction
        {
            NORTH,
            SOUTH,
            EAST,
            WEST,
            NORTH_EAST,
            NORTH_WEST,
            SOUTH_EAST,
            SOUTH_WEST,
            CENTER,
        };
        /**
         * @brief 获取点所在象限
         * @return 象限
         */
        Quadrant quadrant()const
        {
            constexpr double EPSILON = std::is_same<T, double>::value ? 1e-10 : 1e-6;
            if (x > 0 && y > 0)
            {
                return Quadrant::FIRST;
            }
            else if (x > 0 && y < 0)
            {
                return Quadrant::SECOND;
            }
            else if (x < 0 && y < 0)
            {
                return Quadrant::THIRD;
            }
            else if (x < 0 && y > 0)
            {
                return Quadrant::FOURTH;
            }
            else if (std::abs(x - 0) <= EPSILON && y > 0)
            {
                return Quadrant::POSITIVE_Y;
            }
            else if (std::abs(x - 0) <= EPSILON && y < 0)
            {
                return Quadrant::NEGATIVE_Y;
            }
            else if (x > 0 && std::abs(y - 0) <= EPSILON)
            {
                return Quadrant::POSITIVE_X;
            }
            else if (x < 0 && std::abs(y - 0) <= EPSILON)
            {
                return Quadrant::NEGATIVE_X;
            }
            else
                return Quadrant::CENTER;

        }
        /**
         * @brief 获取点到目标点的方向
         * @param dest_point 目标点
         * @return 方向
         */
        Direction direction(const Vector2D<T>& dest_point) const
        {
            if (x == dest_point.x && y == dest_point.y)
            {
                return Direction::CENTER; // 当前点与目标点重合
            }

            // 计算相对位置
            int deltaX = dest_point.x - x;
            int deltaY = dest_point.y - y;

            if (deltaY > 0)
            {
                if (deltaX > 0)
                {
                    return Direction::NORTH_EAST; // 东北
                }
                else if (deltaX < 0)
                {
                    return Direction::NORTH_WEST; // 西北
                }
                return Direction::NORTH; // 北
            }
            else if (deltaY < 0)
            {
                if (deltaX > 0)
                {
                    return Direction::SOUTH_EAST; // 东南
                }
                else if (deltaX < 0)
                {
                    return Direction::SOUTH_WEST; // 西南
                }
                return Direction::SOUTH; // 南
            }
            else
            { // deltaY == 0
                if (deltaX > 0)
                {
                    return Direction::EAST; // 东
                }
                else if (deltaX < 0)
                {
                    return Direction::WEST; // 西
                }
            }

            return Direction::CENTER; // 默认情况（不应该到达这里）
        }
        /**
         * @brief 获取点到目标点的距离
         */
        double get_distance(const Vector2D<T>& dest_point)const
        {
            return std::sqrt(std::pow(x - dest_point.x, 2) + std::pow(y - dest_point.y, 2));
        }

        /**
         * @brief Vector2D::operator==
         * @param rhs
         * @return 相等为true
         */
        bool operator==(const Vector2D& rhs)const
        {
            return x == rhs.x && y == rhs.y;
        }
        /**
         * @brief Vector2D::operator!=
         * @param rhs
         * @return 不相等为true
         */
        bool operator!=(const Vector2D<T>& rhs)const
        {
            return !(*this == rhs);
        }
        /**
         * @brief Vector2D::operator+
         * @param rhs
         * @return 两个向量的和
         */
        Vector2D<T>& operator+=(const Vector2D<T>& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }
        /**
         * @brief Vector2D::operator-
         * @param rhs
         * @return 两个向量的差
         */
        Vector2D<T>& operator-=(const Vector2D<T>& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }
        Vector2D<T> operator+(const Vector2D<T>& rhs) const
        {
            return Vector2D<T>(x + rhs.x, y + rhs.y);
        }

        Vector2D<T> operator-(const Vector2D<T>& rhs) const
        {
            return Vector2D<T>(x - rhs.x, y - rhs.y);
        }

        /// @brief 横坐标
        int x;
        /// @brief 纵坐标
        int y;
    };
    /// @brief 尺寸
    template<typename T = int>
    using Size = Vector2D<T>;
    /// @brief 位置
    template<typename T = int>
    using Pos = Vector2D<T>;
}