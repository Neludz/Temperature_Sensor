#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)
#add_compile_definitions(ZB_DEV_REJOIN_TIMEOUT_MS=60000)
list(APPEND
  DTS_ROOT
  ${APPLICATION_SOURCE_DIR}
  ${BOARD_DIR}
  ${ZEPHYR_BASE}
  )

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project("Light Switch")

# NORDIC SDK APP START
FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
FILE(GLOB app_sources ../com_ws_src/*.c)
target_sources(app PRIVATE ${app_sources})
#zephyr_include_directories(src)
target_include_directories(app PRIVATE ../com_ws_src)
target_include_directories(app PRIVATE include)
target_include_directories(app PRIVATE src)
#target_include_directories(app PRIVATE ${ZEPHYR_BASE}/subsys/fs/nvs)
# NORDIC SDK APP END


