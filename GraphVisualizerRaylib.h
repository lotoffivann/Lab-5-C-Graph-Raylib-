#pragma once

#include "Graph.h"
#include <raylib.h>
#include <vector>
#include <cmath>

/**
 * Структура для хранения позиции вершины
 */
struct VertexRaylib {
    float x, y;      // Координаты
    float vx, vy;    // Скорость (для force-directed алгоритма)
    
    VertexRaylib() : x(0), y(0), vx(0), vy(0) {}
    VertexRaylib(float x, float y) : x(x), y(y), vx(0), vy(0) {}
};

/**
 * Класс GraphVisualizerRaylib - визуализация неориентированного графа с RayLib
 * Использует force-directed алгоритм для минимизации пересечений рёбер
 */
class GraphVisualizerRaylib {
private:
    const Graph& graph;
    std::vector<VertexRaylib> vertices;
    
    // Параметры окна
    int windowWidth;
    int windowHeight;
    
    // Параметры визуализации
    float vertexRadius;
    Color vertexColor;
    Color edgeColor;
    Color textColor;
    Color backgroundColor;
    
    // Параметры force-directed алгоритма
    float repulsionForce;     // Сила отталкивания между вершинами
    float attractionForce;    // Сила притяжения вдоль рёбер
    float damping;            // Демпфирование (для затухания колебаний)
    float optimalDistance;    // Оптимальное расстояние между вершинами
    
    /**
     * Инициализировать позиции вершин
     * Размещает вершины по кругу
     */
    void initializePositions();
    
    /**
     * Применить force-directed алгоритм для одной итерации
     */
    void applyForceDirectedStep();
    
    /**
     * Рассчитать силу отталкивания между двумя вершинами
     */
    void calculateRepulsion(int i, int j);
    
    /**
     * Рассчитать силу притяжения вдоль ребра
     */
    void calculateAttraction(int i, int j);
    
    /**
     * Обновить позиции вершин на основе скоростей
     */
    void updatePositions();
    
    /**
     * Центрировать граф в окне
     */
    void centerGraph();

public:
    /**
     * Конструктор визуализатора
     * @param g - граф для визуализации
     * @param width - ширина окна
     * @param height - высота окна
     */
    GraphVisualizerRaylib(const Graph& g, int width = 1200, int height = 800);
    
    /**
     * Оптимизировать расположение вершин
     * @param iterations - количество итераций
     */
    void optimize(int iterations = 1000);
    
    /**
     * Отобразить граф в окне RayLib
     */
    void display();
    
    /**
     * Отрисовать граф
     */
    void draw();
    
    /**
     * Установить параметры визуализации
     */
    void setVertexRadius(float radius);
    void setVertexColor(Color color);
    void setEdgeColor(Color color);
    void setTextColor(Color color);
    void setBackgroundColor(Color color);
    
    /**
     * Установить параметры force-directed алгоритма
     */
    void setRepulsionForce(float force);
    void setAttractionForce(float force);
    void setDamping(float d);
    void setOptimalDistance(float distance);
};

