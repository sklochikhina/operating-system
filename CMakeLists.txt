cmake_minimum_required(VERSION 3.27)
project(lab3)

set(CMAKE_C_STANDARD 17)

add_executable(lab3.1 "file_systems/lab3.1.c")
add_executable(lab3.2 "file_systems/lab3.2.c")

add_executable(lab4.1 "address_space/lab4.1.c")
add_executable(lab4.2.1 "address_space/lab4.2.1.c")
add_executable(lab4.2.2 "address_space/lab4.2.2.c")

add_executable(lab5.1 "processes/lab5.1.c")
add_executable(lab5.2.1 "processes/lab5.2.1.c")
add_executable(lab5.2.2 "processes/lab5.2.2.c")

add_executable(lab6.1.1 "interprocess_communications/lab6.1.1.c")
add_executable(lab6.1.2 "interprocess_communications/lab6.1.2.c")
add_executable(lab6.2 "interprocess_communications/lab6.2.c")