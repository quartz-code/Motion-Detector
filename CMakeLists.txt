cmake_minimum_required(VERSION 3.10)
project(MotionDetector)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Ищем библиотеку OpenCV в системе
find_package(OpenCV REQUIRED)

# Добавляем директории заголовочных файлов OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Создаем исполняемый файл из нашего исходника
add_executable(MotionDetector main.cpp)

# Линкуем (связываем) OpenCV с нашей программой
target_link_libraries(MotionDetector ${OpenCV_LIBS})
