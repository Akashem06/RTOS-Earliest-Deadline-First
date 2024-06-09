#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include <iostream>

extern "C" {
#include "scheduler.h"
#include "task.h"
}


static void dummy_task() {
    static int call_count = 0;
    call_count = (call_count + 1) % 101;
    std::cout << "DUMMY TASK! " << call_count << std::endl;
}

TEST_GROUP(TestScheduler){
    void setup() {}

    void teardown() {}
};

TEST(TestScheduler, InitScheduler) { 
    CHECK_EQUAL(RTOS_OK, init_scheduler());
    CHECK_EQUAL(0, current_task_index);
    CHECK_EQUAL(0, system_time);

    for (int i = 0; i < MAX_TASKS; i++) {
        CHECK_EQUAL(TASK_SUSPENDED, task_list[i].state);
        POINTERS_EQUAL(&task_stacks[i][STACK_SIZE - 1], task_list[i].stackPointer);
        CHECK_EQUAL(UINT32_MAX, task_list[i].deadline);
        CHECK_EQUAL(0, task_list[i].period);
    }
};

TEST(TestScheduler, CreateTask) {
    init_scheduler();
    CHECK_EQUAL(RTOS_OK, create_task(dummy_task, 100, 100, "Dummy Task"));
    CHECK_EQUAL(TASK_READY, task_list[0].state);
    CHECK_EQUAL(100, task_list[0].period);
    CHECK_EQUAL(100, task_list[0].deadline);
    STRCMP_EQUAL("Dummy Task", task_list[0].taskName);
};

TEST(TestScheduler, StartScheduler) {
    init_scheduler();
    create_task(dummy_task, 100, 100, "Dummy Task");
    CHECK_EQUAL(RTOS_OK, start_scheduler());
    LONGS_EQUAL(TASK_RUNNING, task_list[current_task_index].state);
    FUNCTIONPOINTERS_EQUAL(dummy_task, task_list[current_task_index].taskFunction);
}


TEST(TestScheduler, TickHandler) {
    init_scheduler();
    create_task(dummy_task, 100, 100, "Dummy Task");
    start_scheduler();
    CHECK_EQUAL(RTOS_OK, tick_handler());
    CHECK_EQUAL(1, system_time);  // Ensure system time has incremented

    for (uint8_t i{0}; i < 4; i++) {
        CHECK_EQUAL(RTOS_OK, tick_handler());
    }
    CHECK_EQUAL(5, system_time);
}

TEST(TestScheduler, UpdateTaskDeadlines) {
    init_scheduler();
    create_task(dummy_task, 100, 100, "Dummy Task");
    start_scheduler();
    for (uint8_t i{0}; i < 50; i++) {
        CHECK_EQUAL(RTOS_OK, tick_handler());
    }
    CHECK_EQUAL(50, system_time);
    update_task_deadlines();
    CHECK_EQUAL(150, task_list[0].deadline); // Should be system_time + period
}
