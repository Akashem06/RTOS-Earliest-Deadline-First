#ifndef RTOS_STATUS_H
#define RTOS_STATUS_H

typedef enum {
  RTOS_OK = 0,
  RTOS_UNKNOWN,
  RTOS_INVALID_ARGS,
  RTOS_RESOURCE_EXHAUSTED,
  RTOS_UNREACHABLE,
  RTOS_TIMEOUT,
  RTOS_EMPTY,
  RTOS_OUT_OF_RANGE,
  RTOS_UNIMPLEMENTED,
  RTOS_UNINITIALIZED,
  RTOS_INTERNAL_ERROR,
  RTOS_INCOMPLETE,
  RTOS_NUM_STATUS_CODES,
} RTOSStatus;

#define status_ok_or_return(code)                 \
    ({                                            \
        __typeof__(code) status_expr = (code);    \
        if (status_expr) return status_expr;      \
    })                                            \

#endif