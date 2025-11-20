#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <queue>
#include <stack>

/**
 * Класс Graph - представление неориентированного графа
 * Использует список смежности для хранения рёбер
 */
class Graph {
private:
    int numVertices;                              // Количество вершин
    std::vector<std::vector<int>> adjList;        // Список смежности
    std::vector<std::pair<int, int>> edges;       // Список рёбер для визуализации

public:
    /**
     * Конструктор графа
     * @param vertices - количество вершин
     */
    Graph(int vertices);

    /**
     * Добавить ребро между вершинами u и v
     * @param u - первая вершина
     * @param v - вторая вершина
     */
    void addEdge(int u, int v);

    /**
     * Получить количество вершин
     * @return количество вершин
     */
    int getNumVertices() const;

    /**
     * Получить список смежности для вершины
     * @param vertex - номер вершины
     * @return вектор смежных вершин
     */
    const std::vector<int>& getAdjList(int vertex) const;

    /**
     * Получить все рёбра графа
     * @return вектор пар вершин (рёбер)
     */
    const std::vector<std::pair<int, int>>& getEdges() const;

    /**
     * Получить степень вершины
     * @param vertex - номер вершины
     * @return степень вершины
     */
    int getDegree(int vertex) const;

    /**
     * Обход графа в глубину (DFS)
     * @param start - начальная вершина
     * @return вектор вершин в порядке обхода
     */
    std::vector<int> depthFirstSearch(int start) const;

    /**
     * Обход графа в ширину (BFS)
     * @param start - начальная вершина
     * @return вектор вершин в порядке обхода
     */
    std::vector<int> breadthFirstSearch(int start) const;

    /**
     * Проверка связности графа
     * @return true если граф связный, false иначе
     */
    bool isConnected() const;

    /**
     * Получить компоненты связности
     * @return вектор компонент связности (каждая компонента - вектор вершин)
     */
    std::vector<std::vector<int>> getConnectedComponents() const;

    /**
     * Загрузить граф из файла
     * Формат файла:
     * Первая строка: количество вершин
     * Последующие строки: пары вершин (рёбра)
     * @param filename - имя файла
     * @return true если загрузка успешна, false иначе
     */
    static Graph loadFromFile(const std::string& filename);

    /**
     * Сохранить граф в файл
     * @param filename - имя файла
     */
    void saveToFile(const std::string& filename) const;

    /**
     * Вывести информацию о графе
     */
    void printInfo() const;
};

