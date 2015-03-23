#ifndef __NTP_H__
#define __NTP_H__

#include <stdint.h>

#define NTP_ERR_NO_NIC 0
#define NTP_ERR_SOCKET 1
#define NTP_ERR_SEND   2
#define NTP_ERR_RECV   3
#define NTP_ERR_LOOKUP 4
#define NTP_ERR_SET_TIMEOUT 5

uint32_t get_ntp_utc(int *err);

#endif
