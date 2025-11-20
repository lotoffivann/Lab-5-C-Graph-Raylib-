// Подключаем заголовочный файл класса визуализатора
#include "GraphVisualizerRaylib.h"
// Подключаем библиотеку для ввода-вывода
#include <iostream>
// Подключаем библиотеку для алгоритмов (min, max)
#include <algorithm>

// Определение математической константы Pi (если не определена)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Конструктор визуализатора графа
// Параметры: g - ссылка на граф, width - ширина окна, height - высота окна
GraphVisualizerRaylib::GraphVisualizerRaylib(const Graph& g, int width, int height)
    : graph(g), windowWidth(width), windowHeight(height) {
    
    // Изменяем размер вектора вершин под количество вершин в графе
    vertices.resize(graph.getNumVertices());
    
    // Устанавливаем параметры визуализации по умолчанию
    vertexRadius = 25.0f;                    // Радиус вершины в пикселях
    vertexColor = {100, 150, 255, 255};      // Цвет вершины (голубой) RGBA
    edgeColor = {80, 80, 80, 255};           // Цвет рёбер (серый) RGBA
    textColor = {255, 255, 255, 255};        // Цвет текста (белый) RGBA
    backgroundColor = {245, 245, 245, 255};  // Цвет фона (светло-серый) RGBA
    
    // Устанавливаем параметры force-directed алгоритма
    // Force-directed - физическая модель для красивого расположения графа
    repulsionForce = 50000.0f;    // Сила отталкивания между вершинами (как заряженные частицы)
    attractionForce = 0.01f;      // Сила притяжения вдоль рёбер (как пружины)
    damping = 0.85f;              // Коэффициент затухания (для стабилизации)
    optimalDistance = 150.0f;     // Оптимальная длина ребра в пикселях
    
    // Инициализируем начальные позиции вершин
    initializePositions();
}

// Функция инициализации начальных позиций вершин
// Размещаем вершины по кругу для красивого начального вида
void GraphVisualizerRaylib::initializePositions() {
    // Получаем количество вершин в графе
    int n = graph.getNumVertices();
    
    // Если вершин нет, ничего не делаем
    if (n == 0) return;
    
    // Размещаем вершины по кругу в центре окна
    float centerX = windowWidth / 2.0f;   // X-координата центра окна
    float centerY = windowHeight / 2.0f;  // Y-координата центра окна
    // Радиус круга - 35% от минимального размера окна
    float radius = std::min(windowWidth, windowHeight) * 0.35f;
    
    // Проходим по каждой вершине
    for (int i = 0; i < n; i++) {
        // Вычисляем угол для i-той вершины (равномерно распределяем по кругу)
        float angle = 2.0f * M_PI * i / n;
        // Вычисляем X-координату вершины по формуле окружности
        vertices[i].x = centerX + radius * std::cos(angle);
        // Вычисляем Y-координату вершины по формуле окружности
        vertices[i].y = centerY + radius * std::sin(angle);
        // Обнуляем начальную скорость по X
        vertices[i].vx = 0;
        // Обнуляем начальную скорость по Y
        vertices[i].vy = 0;
    }
}

// Функция расчёта силы отталкивания между двумя вершинами
// Работает по закону Кулона: F = k / r^2 (сила обратно пропорциональна квадрату расстояния)
void GraphVisualizerRaylib::calculateRepulsion(int i, int j) {
    // Вычисляем разницу координат (вектор от i к j)
    float dx = vertices[j].x - vertices[i].x;
    float dy = vertices[j].y - vertices[i].y;
    // Вычисляем квадрат расстояния между вершинами
    float distSq = dx * dx + dy * dy;
    
    // Защита от деления на ноль (если вершины совпадают)
    if (distSq < 1.0f) distSq = 1.0f;
    
    // Вычисляем расстояние (корень из квадрата расстояния)
    float dist = std::sqrt(distSq);
    // Вычисляем силу отталкивания (обратно пропорциональна квадрату расстояния)
    float force = repulsionForce / distSq;
    
    // Применяем силу к скорости вершины i (отталкиваем от j)
    // Направление силы: противоположно вектору dx, dy (от j к i)
    vertices[i].vx -= (dx / dist) * force;  // Компонента скорости по X
    vertices[i].vy -= (dy / dist) * force;  // Компонента скорости по Y
}

// Функция расчёта силы притяжения вдоль ребра между вершинами
// Работает как пружина: F = k * (текущая_длина - оптимальная_длина)
void GraphVisualizerRaylib::calculateAttraction(int i, int j) {
    // Вычисляем разницу координат (вектор от i к j)
    float dx = vertices[j].x - vertices[i].x;
    float dy = vertices[j].y - vertices[i].y;
    // Вычисляем расстояние между вершинами
    float dist = std::sqrt(dx * dx + dy * dy);
    
    // Защита от деления на ноль
    if (dist < 1.0f) dist = 1.0f;
    
    // Вычисляем силу притяжения (пропорциональна отклонению от оптимального расстояния)
    // Если вершины дальше оптимального расстояния - притягиваются
    // Если ближе - отталкиваются
    float force = attractionForce * (dist - optimalDistance);
    
    // Применяем силу к скорости вершины i (притягиваем к j)
    vertices[i].vx += (dx / dist) * force;  // Компонента скорости по X
    vertices[i].vy += (dy / dist) * force;  // Компонента скорости по Y
}

// Функция выполнения одной итерации force-directed алгоритма
// На каждой итерации пересчитываем силы и обновляем позиции вершин
void GraphVisualizerRaylib::applyForceDirectedStep() {
    // Получаем количество вершин
    int n = graph.getNumVertices();
    
    // Обнуляем скорости всех вершин перед новым расчётом
    for (int i = 0; i < n; i++) {
        vertices[i].vx = 0;  // Скорость по X
        vertices[i].vy = 0;  // Скорость по Y
    }
    
    // Рассчитываем силы отталкивания между всеми парами вершин
    // Каждая вершина отталкивает все остальные (как заряженные частицы)
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            // Вершина i отталкивается от j
            calculateRepulsion(i, j);
            // Вершина j отталкивается от i (симметрично)
            calculateRepulsion(j, i);
        }
    }
    
    // Рассчитываем силы притяжения вдоль рёбер
    // Рёбра работают как пружины, притягивая соединённые вершины
    const auto& edges = graph.getEdges();  // Получаем список всех рёбер
    for (const auto& edge : edges) {
        int u = edge.first;   // Первая вершина ребра
        int v = edge.second;  // Вторая вершина ребра
        // Вершина u притягивается к v
        calculateAttraction(u, v);
        // Вершина v притягивается к u (симметрично)
        calculateAttraction(v, u);
    }
    
    // Обновляем позиции вершин на основе накопленных скоростей
    updatePositions();
}

// Функция обновления позиций вершин на основе их скоростей
void GraphVisualizerRaylib::updatePositions() {
    // Получаем количество вершин
    int n = graph.getNumVertices();
    // Вычисляем отступ от границ окна (2 радиуса вершины)
    float margin = vertexRadius * 2;
    
    // Проходим по каждой вершине
    for (int i = 0; i < n; i++) {
        // Применяем демпфирование к скорости (затухание для стабилизации)
        // Без демпфирования вершины будут бесконечно колебаться
        vertices[i].vx *= damping;  // Уменьшаем скорость по X
        vertices[i].vy *= damping;  // Уменьшаем скорость по Y
        
        // Обновляем позиции вершины на основе скорости
        vertices[i].x += vertices[i].vx;  // Новая позиция X = старая + скорость
        vertices[i].y += vertices[i].vy;  // Новая позиция Y = старая + скорость
        
        // Ограничиваем позиции границами окна (чтобы вершины не убегали за экран)
        // std::max выбирает максимум (не даёт уйти влево/вверх)
        // std::min выбирает минимум (не даёт уйти вправо/вниз)
        vertices[i].x = std::max(margin, std::min((float)windowWidth - margin, vertices[i].x));
        vertices[i].y = std::max(margin, std::min((float)windowHeight - margin, vertices[i].y));
    }
}

// Функция центрирования графа в окне
// Находит центр масс всех вершин и перемещает его в центр окна
void GraphVisualizerRaylib::centerGraph() {
    // Получаем количество вершин
    int n = graph.getNumVertices();
    // Если вершин нет, ничего не делаем
    if (n == 0) return;
    
    // Находим центр масс графа (среднее всех координат)
    float centerX = 0, centerY = 0;  // Инициализируем центр
    for (int i = 0; i < n; i++) {
        centerX += vertices[i].x;  // Суммируем X-координаты
        centerY += vertices[i].y;  // Суммируем Y-координаты
    }
    centerX /= n;  // Делим на количество вершин для получения среднего
    centerY /= n;  // Делим на количество вершин для получения среднего
    
    // Вычисляем смещение для перемещения центра масс в центр окна
    float offsetX = windowWidth / 2.0f - centerX;   // Смещение по X
    float offsetY = windowHeight / 2.0f - centerY;  // Смещение по Y
    
    // Применяем смещение ко всем вершинам
    for (int i = 0; i < n; i++) {
        vertices[i].x += offsetX;  // Сдвигаем вершину по X
        vertices[i].y += offsetY;  // Сдвигаем вершину по Y
    }
}

// Функция оптимизации расположения вершин
// Выполняет заданное количество итераций алгоритма для улучшения вида графа
void GraphVisualizerRaylib::optimize(int iterations) {
    // Выводим сообщение о начале оптимизации
    std::cout << "Оптимизация расположения вершин..." << std::endl;
    
    // Выполняем заданное количество итераций
    for (int i = 0; i < iterations; i++) {
        // Применяем один шаг force-directed алгоритма
        applyForceDirectedStep();
        
        // Выводим прогресс каждые 100 итераций
        if ((i + 1) % 100 == 0) {
            std::cout << "Итерация " << (i + 1) << "/" << iterations << std::endl;
        }
    }
    
    // После оптимизации центрируем граф в окне
    centerGraph();
    // Выводим сообщение о завершении
    std::cout << "Оптимизация завершена!" << std::endl;
}

// Функция отрисовки графа на экране
// Сначала рисуем рёбра, потом вершины (чтобы вершины были поверх рёбер)
void GraphVisualizerRaylib::draw() {
    // Отрисовка рёбер графа
    const auto& edges = graph.getEdges();  // Получаем список всех рёбер
    for (const auto& edge : edges) {
        int u = edge.first;   // Первая вершина ребра
        int v = edge.second;  // Вторая вершина ребра
        
        // Рисуем линию между вершинами u и v
        DrawLineEx(
            {vertices[u].x, vertices[u].y},  // Начальная точка (вершина u)
            {vertices[v].x, vertices[v].y},  // Конечная точка (вершина v)
            2.0f,                            // Толщина линии в пикселях
            edgeColor                        // Цвет линии
        );
    }
    
    // Отрисовка вершин графа
    int n = graph.getNumVertices();  // Получаем количество вершин
    for (int i = 0; i < n; i++) {
        // Рисуем заполненный круг для вершины
        DrawCircleV({vertices[i].x, vertices[i].y}, vertexRadius, vertexColor);
        
        // Рисуем обводку вокруг вершины (чёрный контур)
        DrawCircleLines(vertices[i].x, vertices[i].y, vertexRadius, BLACK);
        
        // Рисуем номер вершины в центре круга
        const char* text = TextFormat("%d", i);      // Форматируем номер вершины
        int textWidth = MeasureText(text, 20);       // Измеряем ширину текста
        DrawText(
            text,                            // Текст для отрисовки
            vertices[i].x - textWidth / 2,   // X-позиция (центрируем текст)
            vertices[i].y - 10,              // Y-позиция (центрируем по вертикали)
            20,                              // Размер шрифта
            textColor                        // Цвет текста
        );
    }
}

// Функция отображения графа в окне RayLib
// Главный цикл приложения с обработкой событий и отрисовкой
void GraphVisualizerRaylib::display() {
    // Инициализируем окно RayLib с заданными размерами и заголовком
    InitWindow(windowWidth, windowHeight, "Визуализация графа - Lab5 (RayLib)");
    // Устанавливаем целевую частоту кадров (60 FPS)
    SetTargetFPS(60);
    
    // Флаг для включения/выключения автоматической оптимизации
    bool isOptimizing = false;
    // Счётчик выполненных шагов оптимизации
    int optimizationSteps = 0;
    // Количество шагов оптимизации за один кадр
    const int stepsPerFrame = 5;
    
    // Главный игровой цикл (выполняется пока окно не закрыто)
    while (!WindowShouldClose()) {
        // ===== ОБРАБОТКА ВВОДА =====
        
        // Если нажата клавиша ПРОБЕЛ - переключаем режим оптимизации
        if (IsKeyPressed(KEY_SPACE)) {
            isOptimizing = !isOptimizing;  // Инвертируем флаг
            if (isOptimizing) {
                std::cout << "Оптимизация запущена (нажмите Пробел для остановки)" << std::endl;
            } else {
                std::cout << "Оптимизация остановлена" << std::endl;
            }
        }
        
        // Если нажата клавиша R - сбрасываем позиции вершин
        if (IsKeyPressed(KEY_R)) {
            std::cout << "Сброс позиций вершин" << std::endl;
            initializePositions();       // Размещаем вершины по кругу
            optimizationSteps = 0;       // Сбрасываем счётчик шагов
        }
        
        // Если нажата клавиша C - центрируем граф
        if (IsKeyPressed(KEY_C)) {
            centerGraph();
        }
        
        // ===== ПРИМЕНЕНИЕ ОПТИМИЗАЦИИ =====
        
        // Если включена оптимизация и не достигнут лимит шагов
        if (isOptimizing && optimizationSteps < 1000) {
            // Выполняем несколько шагов оптимизации за кадр
            for (int i = 0; i < stepsPerFrame; i++) {
                applyForceDirectedStep();  // Применяем force-directed алгоритм
                optimizationSteps++;       // Увеличиваем счётчик
            }
        }
        
        // ===== ОТРИСОВКА =====
        
        // Начинаем отрисовку кадра
        BeginDrawing();
        // Очищаем фон заданным цветом
        ClearBackground(backgroundColor);
        
        // Рисуем граф (рёбра и вершины)
        draw();
        
        // Отрисовка текстовых инструкций внизу экрана
        DrawText(
            "Пробел - оптимизация | R - сброс | C - центр | ESC - выход",
            10,                    // X-позиция
            windowHeight - 30,     // Y-позиция (внизу экрана)
            16,                    // Размер шрифта
            BLACK                  // Цвет текста
        );
        
        // Показываем счётчик FPS в правом верхнем углу
        DrawText(
            TextFormat("FPS: %d", GetFPS()),  // Форматируем текст с FPS
            windowWidth - 100,                 // X-позиция (справа)
            10,                                // Y-позиция (сверху)
            16,                                // Размер шрифта
            DARKGRAY                           // Цвет текста
        );
        
        // Завершаем отрисовку кадра и выводим на экран
        EndDrawing();
    }
    
    // Закрываем окно и освобождаем ресурсы RayLib
    CloseWindow();
}

// ===== СЕТТЕРЫ ДЛЯ НАСТРОЙКИ ПАРАМЕТРОВ ВИЗУАЛИЗАЦИИ =====

// Установить радиус вершин в пикселях
void GraphVisualizerRaylib::setVertexRadius(float radius) {
    vertexRadius = radius;
}

// Установить цвет вершин (формат RGBA)
void GraphVisualizerRaylib::setVertexColor(Color color) {
    vertexColor = color;
}

// Установить цвет рёбер (формат RGBA)
void GraphVisualizerRaylib::setEdgeColor(Color color) {
    edgeColor = color;
}

// Установить цвет текста (номеров вершин)
void GraphVisualizerRaylib::setTextColor(Color color) {
    textColor = color;
}

// Установить цвет фона окна
void GraphVisualizerRaylib::setBackgroundColor(Color color) {
    backgroundColor = color;
}

// ===== СЕТТЕРЫ ДЛЯ ПАРАМЕТРОВ FORCE-DIRECTED АЛГОРИТМА =====

// Установить силу отталкивания между вершинами
// Большие значения - вершины отталкиваются сильнее
void GraphVisualizerRaylib::setRepulsionForce(float force) {
    repulsionForce = force;
}

// Установить силу притяжения вдоль рёбер
// Большие значения - рёбра сильнее притягивают вершины
void GraphVisualizerRaylib::setAttractionForce(float force) {
    attractionForce = force;
}

// Установить коэффициент затухания (0-1)
// Близко к 1 - медленное затухание, близко к 0 - быстрое
void GraphVisualizerRaylib::setDamping(float d) {
    damping = d;
}

// Установить оптимальное расстояние между соединёнными вершинами
void GraphVisualizerRaylib::setOptimalDistance(float distance) {
    optimalDistance = distance;
}

