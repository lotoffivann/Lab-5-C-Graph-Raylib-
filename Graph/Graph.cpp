// Подключаем заголовочный файл класса Graph с объявлением класса
#include "Graph.h"
// Подключаем библиотеку для ввода-вывода (cout, cerr)
#include <iostream>
// Подключаем библиотеку для алгоритмов (find)
#include <algorithm>

// Конструктор класса Graph - создает граф с заданным количеством вершин
// Параметр vertices - количество вершин в графе
Graph::Graph(int vertices) : numVertices(vertices) {
    // Изменяем размер вектора списков смежности под количество вершин
    // Каждая вершина получит свой пустой список смежных вершин
    adjList.resize(vertices);
}

// Функция добавления ребра между вершинами u и v
void Graph::addEdge(int u, int v) {
    // Проверяем, что номера вершин находятся в допустимом диапазоне [0, numVertices)
    if (u < 0 || u >= numVertices || v < 0 || v >= numVertices) {
        // Выводим сообщение об ошибке в поток ошибок
        std::cerr << "Ошибка: неверные номера вершин (" << u << ", " << v << ")" << std::endl;
        // Прерываем выполнение функции
        return;
    }
    
    // Проверка на дублирование рёбер
    // Ищем вершину v в списке смежности вершины u
    if (std::find(adjList[u].begin(), adjList[u].end(), v) == adjList[u].end()) {
        // Если ребро не найдено, добавляем v в список смежности u
        adjList[u].push_back(v);
        // Добавляем u в список смежности v (граф неориентированный)
        adjList[v].push_back(u);
        // Сохраняем ребро в список всех рёбер для визуализации
        edges.push_back({u, v});
    }
}

// Функция возвращает количество вершин в графе
// Модификатор const означает, что функция не изменяет состояние объекта
int Graph::getNumVertices() const {
    // Возвращаем значение поля numVertices
    return numVertices;
}

// Функция возвращает константную ссылку на список смежности заданной вершины
// Возвращаем ссылку (&), чтобы не копировать весь вектор
const std::vector<int>& Graph::getAdjList(int vertex) const {
    // Возвращаем вектор смежных вершин для заданной вершины
    return adjList[vertex];
}

// Функция возвращает константную ссылку на список всех рёбер графа
const std::vector<std::pair<int, int>>& Graph::getEdges() const {
    // Возвращаем вектор пар вершин (каждая пара - это ребро)
    return edges;
}

// Функция возвращает степень вершины (количество смежных вершин)
int Graph::getDegree(int vertex) const {
    // Проверяем корректность номера вершины
    if (vertex < 0 || vertex >= numVertices) {
        // Возвращаем -1 в случае ошибки
        return -1;
    }
    // Возвращаем размер списка смежности (количество смежных вершин)
    return adjList[vertex].size();
}

// Функция обхода графа в глубину (Depth First Search - DFS)
// Начинаем обход с вершины start
std::vector<int> Graph::depthFirstSearch(int start) const {
    // Вектор для хранения результата обхода (порядок посещения вершин)
    std::vector<int> result;
    // Вектор для отметки посещённых вершин (изначально все false - не посещены)
    std::vector<bool> visited(numVertices, false);
    // Стек для хранения вершин, которые нужно посетить (LIFO - Last In First Out)
    std::stack<int> stack;
    
    // Помещаем стартовую вершину в стек
    stack.push(start);
    
    // Пока стек не пуст, продолжаем обход
    while (!stack.empty()) {
        // Берём верхнюю вершину из стека
        int vertex = stack.top();
        // Удаляем её из стека
        stack.pop();
        
        // Если вершина ещё не посещена
        if (!visited[vertex]) {
            // Отмечаем вершину как посещённую
            visited[vertex] = true;
            // Добавляем вершину в результат
            result.push_back(vertex);
            
            // Добавляем смежные вершины в стек (в обратном порядке)
            // Обратный порядок нужен для обхода в правильной последовательности
            for (int i = adjList[vertex].size() - 1; i >= 0; i--) {
                // Получаем соседнюю вершину
                int neighbor = adjList[vertex][i];
                // Если сосед не посещён, добавляем его в стек
                if (!visited[neighbor]) {
                    stack.push(neighbor);
                }
            }
        }
    }
    
    // Возвращаем вектор с порядком посещения вершин
    return result;
}

// Функция обхода графа в ширину (Breadth First Search - BFS)
// Начинаем обход с вершины start
std::vector<int> Graph::breadthFirstSearch(int start) const {
    // Вектор для хранения результата обхода
    std::vector<int> result;
    // Вектор для отметки посещённых вершин
    std::vector<bool> visited(numVertices, false);
    // Очередь для хранения вершин (FIFO - First In First Out)
    // BFS использует очередь (в отличие от DFS, который использует стек)
    std::queue<int> queue;
    
    // Отмечаем стартовую вершину как посещённую
    visited[start] = true;
    // Помещаем стартовую вершину в очередь
    queue.push(start);
    
    // Пока очередь не пуста
    while (!queue.empty()) {
        // Берём первую вершину из очереди
        int vertex = queue.front();
        // Удаляем её из очереди
        queue.pop();
        // Добавляем вершину в результат
        result.push_back(vertex);
        
        // Проходим по всем смежным вершинам
        for (int neighbor : adjList[vertex]) {
            // Если сосед не посещён
            if (!visited[neighbor]) {
                // Отмечаем соседа как посещённого
                visited[neighbor] = true;
                // Добавляем соседа в очередь
                queue.push(neighbor);
            }
        }
    }
    
    // Возвращаем порядок посещения вершин
    return result;
}

// Функция проверки связности графа
// Граф связный, если из любой вершины можно добраться до любой другой
bool Graph::isConnected() const {
    // Пустой граф считаем связным
    if (numVertices == 0) return true;
    
    // Запускаем BFS от вершины 0 и получаем все достижимые вершины
    std::vector<int> reachable = breadthFirstSearch(0);
    // Если количество достижимых вершин равно общему количеству,
    // то граф связный (можем добраться до всех вершин)
    return static_cast<int>(reachable.size()) == numVertices;
}

// Функция нахождения компонент связности графа
// Компонента связности - максимальное подмножество вершин,
// в котором любые две вершины соединены путём
std::vector<std::vector<int>> Graph::getConnectedComponents() const {
    // Вектор компонент (каждая компонента - вектор вершин)
    std::vector<std::vector<int>> components;
    // Вектор отметок посещённых вершин
    std::vector<bool> visited(numVertices, false);
    
    // Проходим по всем вершинам графа
    for (int i = 0; i < numVertices; i++) {
        // Если вершина ещё не посещена (принадлежит новой компоненте)
        if (!visited[i]) {
            // Создаём новую компоненту
            std::vector<int> component;
            // Очередь для BFS внутри компоненты
            std::queue<int> queue;
            
            // Отмечаем текущую вершину как посещённую
            visited[i] = true;
            // Добавляем её в очередь
            queue.push(i);
            
            // Запускаем BFS для нахождения всех вершин компоненты
            while (!queue.empty()) {
                // Берём вершину из очереди
                int vertex = queue.front();
                queue.pop();
                // Добавляем её в текущую компоненту
                component.push_back(vertex);
                
                // Проходим по всем соседям
                for (int neighbor : adjList[vertex]) {
                    // Если сосед не посещён
                    if (!visited[neighbor]) {
                        // Отмечаем его как посещённого
                        visited[neighbor] = true;
                        // Добавляем в очередь
                        queue.push(neighbor);
                    }
                }
            }
            
            // Добавляем найденную компоненту в список всех компонент
            components.push_back(component);
        }
    }
    
    // Возвращаем вектор всех компонент связности
    return components;
}

// Статическая функция загрузки графа из файла
// Формат файла: первая строка - количество вершин,
// остальные строки - пары вершин (рёбра)
Graph Graph::loadFromFile(const std::string& filename) {
    // Открываем файл для чтения
    std::ifstream file(filename);
    // Проверяем, удалось ли открыть файл
    if (!file.is_open()) {
        // Выводим сообщение об ошибке
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        // Возвращаем пустой граф
        return Graph(0);
    }
    
    // Переменная для количества вершин
    int numVertices;
    // Считываем количество вершин из первой строки
    file >> numVertices;
    
    // Создаём граф с заданным количеством вершин
    Graph graph(numVertices);
    
    // Переменные для номеров вершин ребра
    int u, v;
    // Читаем пары вершин до конца файла
    while (file >> u >> v) {
        // Добавляем ребро в граф
        graph.addEdge(u, v);
    }
    
    // Закрываем файл
    file.close();
    // Возвращаем созданный граф
    return graph;
}

// Функция сохранения графа в файл
void Graph::saveToFile(const std::string& filename) const {
    // Открываем файл для записи
    std::ofstream file(filename);
    // Проверяем, удалось ли открыть файл
    if (!file.is_open()) {
        // Выводим сообщение об ошибке
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return;
    }
    
    // Записываем количество вершин в первую строку
    file << numVertices << std::endl;
    
    // Проходим по всем рёбрам графа
    for (const auto& edge : edges) {
        // Записываем пару вершин (ребро) в файл
        file << edge.first << " " << edge.second << std::endl;
    }
    
    // Закрываем файл
    file.close();
}

// Функция вывода информации о графе в консоль
void Graph::printInfo() const {
    // Выводим заголовок
    std::cout << "=== Информация о графе ===" << std::endl;
    // Выводим количество вершин
    std::cout << "Количество вершин: " << numVertices << std::endl;
    // Выводим количество рёбер (размер вектора рёбер)
    std::cout << "Количество рёбер: " << edges.size() << std::endl;
    
    // Выводим степень каждой вершины
    std::cout << "\nСтепени вершин:" << std::endl;
    for (int i = 0; i < numVertices; i++) {
        // Для каждой вершины выводим её номер и степень
        std::cout << "Вершина " << i << ": степень " << getDegree(i) << std::endl;
    }
    
    // Проверяем и выводим информацию о связности
    // Тернарный оператор: условие ? значение_если_true : значение_если_false
    std::cout << "\nСвязность: " << (isConnected() ? "граф связный" : "граф несвязный") << std::endl;
    
    // Если граф несвязный, выводим информацию о компонентах связности
    if (!isConnected()) {
        // Получаем все компоненты связности
        auto components = getConnectedComponents();
        // Выводим количество компонент
        std::cout << "Количество компонент связности: " << components.size() << std::endl;
        // Проходим по каждой компоненте
        for (size_t i = 0; i < components.size(); i++) {
            // Выводим номер компоненты
            std::cout << "Компонента " << i + 1 << ": ";
            // Выводим вершины компоненты
            for (int vertex : components[i]) {
                std::cout << vertex << " ";
            }
            std::cout << std::endl;
        }
    }
}

