#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "global_context.h"

void launch_access_point();
void check_hostapd_with_start_dnsmasq();
void connect_access_point();

#endif // WIFI_MANAGER_H
