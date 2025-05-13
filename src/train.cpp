// Copyright 2021 NNTU-CS
#include "train.h"
Train::Train() : countOp(0), first(nullptr) {}

Train::~Train() {
  if (!first) return;
  Car* current = first->next;
  while (current != first) {
    Car* temp = current;
    current = current->next;
    delete temp;
  }
  delete first;
}

void Train::addCar(bool light) {
  Car* newCar = new Car{light, nullptr, nullptr};
  if (!first) {
    first = newCar;
    first->next = first;
    first->prev = first;
  } else {
    Car* last = first->prev;
    last->next = newCar;
    newCar->prev = last;
    newCar->next = first;
    first->prev = newCar;
  }
}

int Train::getLength() {
  if (!first) return 0;

  countOp = 0;
  Car* current = first;
  int length = 0;

  // Инициализируем лампочку в текущем вагоне
  bool initialLight = current->light;
  current->light = true;
  countOp++;

  while (true) {
    // Идем вперед
    current = current->next;
    length++;
    countOp++;

    // Если нашли вагон с включенной лампочкой
    if (current->light) {
      // Выключаем лампочку
      current->light = false;
      countOp++;

      // Возвращаемся назад на length шагов
      for (int i = 0; i < length; i++) {
        current = current->prev;
        countOp++;
      }

      // Проверяем исходный вагон
      if (!current->light) {
        // Если лампочка выключена - мы сделали полный круг
        // Восстанавливаем исходное состояние лампочки
        current->light = initialLight;
        return length;
      } else {
        // Если нет - продолжаем с новым length
        length = 0;
      }
    }
  }
}

int Train::getOpCount() { return countOp; }

// src/main.cpp
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

int main() {
  std::srand(std::time(nullptr));

  const int max_n = 100;
  const int step = 5;
  const int experiments = 3;

  std::vector<int> sizes;
  std::vector<int> ops_all_off;
  std::vector<int> ops_all_on;
  std::vector<int> ops_random;
  std::vector<double> times_all_off;
  std::vector<double> times_all_on;
  std::vector<double> times_random;

  for (int n = step; n <= max_n; n += step) {
    sizes.push_back(n);

    // Все лампочки выключены
    {
      Train train;
      for (int i = 0; i < n; i++) train.addCar(false);

      auto start = std::chrono::high_resolution_clock::now();
      train.getLength();  // Результат не сохраняется, так как не используется
      auto end = std::chrono::high_resolution_clock::now();

      double time =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start)
              .count();
      times_all_off.push_back(time);
      ops_all_off.push_back(train.getOpCount());
    }

    // Все лампочки включены
    {
      Train train;
      for (int i = 0; i < n; i++) train.addCar(true);

      auto start = std::chrono::high_resolution_clock::now();
      train.getLength();  // Результат не сохраняется
      auto end = std::chrono::high_resolution_clock::now();

      double time =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start)
              .count();
      times_all_on.push_back(time);
      ops_all_on.push_back(train.getOpCount());
    }

    // Случайное распределение лампочек
    {
      int ops_sum = 0;
      double time_sum = 0;

      for (int e = 0; e < experiments; e++) {
        Train train;
        for (int i = 0; i < n; i++) train.addCar(std::rand() % 2 == 0);

        auto start = std::chrono::high_resolution_clock::now();
        train.getLength();  // Результат не сохраняется
        auto end = std::chrono::high_resolution_clock::now();

        time_sum +=
            std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                .count();
        ops_sum += train.getOpCount();
      }

      times_random.push_back(time_sum / experiments);
      ops_random.push_back(ops_sum / experiments);
    }
  }

  // Вывод результатов
  std::cout << "n\tOps(off)\tOps(on)\tOps(rand)\tTime(off)\tTime(on)\t"
            << "Time(rand)\n";
  for (size_t i = 0; i < sizes.size(); i++) {
    std::cout << sizes[i] << "\t" << ops_all_off[i] << "\t" << ops_all_on[i]
              << "\t" << ops_random[i] << "\t" << times_all_off[i] << "\t"
              << times_all_on[i] << "\t" << times_random[i] << "\n";
  }

  return 0;
}
