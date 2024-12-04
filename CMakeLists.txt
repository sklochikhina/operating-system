cmake_minimum_required(VERSION 3.27)
project(operating_system)

set(CMAKE_C_STANDARD 17)

#[[ ---------- 1st semester ---------- ]]

add_executable(first.lab1.1 "semester_one/compiling/lab1.1.c"
        semester_two/sync/2.2/spinlock/spinlock-queue.h)
add_executable(first.lab1.2.1 "semester_one/address_space/lab4.2.1.c")
add_executable(first.lab1.2.2 "semester_one/address_space/lab4.2.2.c")

add_executable(first.lab3.1 "semester_one/file_systems/lab3.1.c")
add_executable(first.lab3.2 "semester_one/file_systems/lab3.2.c")

add_executable(first.lab4.1 "semester_one/address_space/lab4.1.c")
add_executable(first.lab4.2.1 "semester_one/address_space/lab4.2.1.c")
add_executable(first.lab4.2.2 "semester_one/address_space/lab4.2.2.c")

add_executable(first.lab5.1 "semester_one/processes/lab5.1.c")
add_executable(first.lab5.2.1 "semester_one/processes/lab5.2.1.c")
add_executable(first.lab5.2.2 "semester_one/processes/lab5.2.2.c")

add_executable(first.lab6.1.1 "semester_one/interprocess_communications/lab6.1.1.c")
add_executable(first.lab6.1.2 "semester_one/interprocess_communications/lab6.1.2.c")
add_executable(first.lab6.2 "semester_one/interprocess_communications/lab6.2.c")

add_executable(first.lab7.1.1 "semester_one/socket/lab7.1.server.c")
add_executable(first.lab7.1.2 "semester_one/socket/lab7.1.client.c")
add_executable(first.lab7.2.1 "semester_one/socket/lab7.2.server.c")
add_executable(first.lab7.2.2 "semester_one/socket/lab7.2.client.c")

add_executable(first.lab8.1 "semester_one/users/lab8.1.c")
add_executable(first.lab8.2 "semester_one/users/lab8.2.c")

#[[ ---------- 2nd semester ---------- ]]

add_executable(second.lab1.1.1 "semester_two/threads/lab1.1/5_threads.c")
add_executable(second.lab1.1.2 "semester_two/threads/lab1.1/thread.c")

add_executable(second.lab1.2.1 "semester_two/threads/lab1.2/inf_cycle.c")
add_executable(second.lab1.2.2 "semester_two/threads/lab1.2/join.c")

add_executable(second.lab1.3 "semester_two/threads/lab1.3/parameters.c")

add_executable(second.lab1.4.1 "semester_two/threads/lab1.4/cancel.c")
add_executable(second.lab1.4.2 "semester_two/threads/lab1.4/cleanup.c")

add_executable(second.lab1.5 "semester_two/threads/lab1.5/signals.c")

add_executable(second.lab1.6.1 "semester_two/threads/lab1.6/create_mythread.c")
add_executable(second.lab1.6.2 "semester_two/threads/lab1.6/mythreads.c")

add_executable(second.lab1.7.1 "semester_two/threads/lab1.7/create_user_thread.c")
add_executable(second.lab1.7.2 "semester_two/threads/lab1.7/user_threads.c")

add_executable(second.lab2.1.1 "semester_two/sync/2.1/queue.c")
add_executable(second.lab2.1.2 "semester_two/sync/2.1/queue-example.c")
add_executable(second.lab2.1.3 "semester_two/sync/2.1/queue-threads.c")

add_executable(second.lab2.2_1.1 "semester_two/sync/2.2/spinlock/main.c")
add_executable(second.lab2.2_1.2 "semester_two/sync/2.2/spinlock/spinlock-queue.c")

add_executable(second.lab2.2_2.1 "semester_two/sync/2.2/mutex/main.c")
add_executable(second.lab2.2_2.2 "semester_two/sync/2.2/mutex/mutex-queue.c")

add_executable(second.lab2.2_3.1 "semester_two/sync/2.2/cond/main.c")
add_executable(second.lab2.2_3.2 "semester_two/sync/2.2/cond/cond-queue.c")

add_executable(second.lab2.2_4.1 "semester_two/sync/2.2/semaphore/main.c")
add_executable(second.lab2.2_4.2 "semester_two/sync/2.2/semaphore/sem-queue.c")

add_executable(second.lab3 "semester_two/proxy/proxy.c")