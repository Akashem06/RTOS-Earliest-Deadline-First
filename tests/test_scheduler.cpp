#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <iostream>

extern "C" {
#include "scheduler.h"
}

// Mock for task functions
static void dummy_task(void *args) {
    // Dummy task does nothing
    (void)args; // Unused parameter
    std::cout << "DUMMY TASK" << std::endl;
}

static int task1_call = 0;
static int task2_call = 0;
static int task3_call = 0;
static int task4_call = 0;

static void task1(void *args) {
    task1_call++;
}

static void task2(void *args) {
    task2_call++;
}

static void task3(void *args) {
    task3_call++;
}

static void task4(void *args) {
    task4_call++;
}

TEST_GROUP(TestScheduler) {
    void setup() {
        init_scheduler();
    }

    void teardown() {
        // Clean up tasks and scheduler state
        for (int i = 0; i < MAX_TASKS; i++) {
            task_list[i].state = TASK_SUSPENDED;
            task_list[i].period = 0;
            task_list[i].deadline = UINT32_MAX;
        }
        current_task_index = 0;
        system_time = 0;
    }
};

TEST(TestScheduler, InitScheduler) {
    CHECK_EQUAL(RTOS_OK, init_scheduler());

    // Check initial state
    CHECK_EQUAL(0, current_task_index);
    CHECK_EQUAL(0, system_time);

    for (int i = 0; i < MAX_TASKS; i++) {
        CHECK_EQUAL(TASK_SUSPENDED, task_list[i].state);
        CHECK_EQUAL(UINT32_MAX, task_list[i].deadline);
        CHECK_EQUAL(0, task_list[i].period);
    }
}

TEST(TestScheduler, CreateTask) {
    CHECK_EQUAL(RTOS_OK, create_task(dummy_task, NULL, 100, 100, "Dummy Task"));

    // Check task properties after creation
    CHECK_EQUAL(TASK_READY, task_list[0].state);
    CHECK_EQUAL(100, task_list[0].period);
    CHECK_EQUAL(100, task_list[0].deadline);
    STRCMP_EQUAL("Dummy Task", task_list[0].taskName);
}

TEST(TestScheduler, StartScheduler) {
    CHECK_EQUAL(RTOS_OK, create_task(dummy_task, NULL, 100, 100, "Dummy Task"));
    CHECK_EQUAL(RTOS_OK, start_scheduler());

    // Check initial state after scheduler starts
    CHECK_EQUAL(TASK_RUNNING, task_list[current_task_index].state);
    FUNCTIONPOINTERS_EQUAL(dummy_task, task_list[current_task_index].taskFunction);
}

TEST(TestScheduler, TickHandler) {
    CHECK_EQUAL(RTOS_OK, create_task(dummy_task, NULL, 100, 100, "Dummy Task"));
    CHECK_EQUAL(RTOS_OK, start_scheduler());

    SysTick_Handler();
    CHECK_EQUAL(1, system_time);  // Ensure system time has incremented

    SysTick_Handler();
    CHECK_EQUAL(2, system_time);  // Ensure system time increments correctly
}

TEST(TestScheduler, TwoTasksConcurrency) {
    CHECK_EQUAL(RTOS_OK, create_task(task1, NULL, 100, 0, "Task 1"));
    CHECK_EQUAL(RTOS_OK, create_task(task2, NULL, 30, 0, "Task 2"));
    CHECK_EQUAL(RTOS_OK, create_task(task3, NULL, 50, 10, "Task 3"));
    CHECK_EQUAL(RTOS_OK, create_task(task4, NULL, 25, 0, "Task 4"));
    CHECK_EQUAL(RTOS_OK, start_scheduler());

    // Simulate 201 ticks to observe task execution
    for (int i = 0; i < 200; i++) {
        SysTick_Handler();
    }

    // Assert task executions
    CHECK_EQUAL(3, task1_call);
    CHECK_EQUAL(7, task2_call);
    CHECK_EQUAL(4, task3_call);
    CHECK_EQUAL(8, task4_call);
}
