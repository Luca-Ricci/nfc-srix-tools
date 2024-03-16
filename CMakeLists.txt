cmake_minimum_required(VERSION 3.14)
project(nfc-srix-tools C)
set(CMAKE_C_STANDARD 99)

# Use PkgConfig to find libnfc
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNFC REQUIRED libnfc)
include_directories(${LIBNFC_INCLUDE_DIRS})
link_directories(${LIBNFC_LIBRARY_DIRS})
add_definitions(${LIBNFC_CFLAGS_OTHER})

#. srix-readTag
add_executable(srix-readTag read_tag.c logging.c nfc_utils.c)
target_link_libraries(srix-readTag ${LIBNFC_LIBRARIES})

#. srix-readDump
add_executable(srix-readDump read_dump.c logging.c nfc_utils.c)
target_link_libraries(srix-readDump ${LIBNFC_LIBRARIES})

# srix-writeTag
add_executable(srix-writeTag write_tag.c logging.c nfc_utils.c)
target_link_libraries(srix-writeTag ${LIBNFC_LIBRARIES})

# srix-resetTag
add_executable(srix-resetTag otp_reset.c logging.c nfc_utils.c)
target_link_libraries(srix-resetTag ${LIBNFC_LIBRARIES})

# srix-updateCdown
add_executable(srix-updateCdown update_countdown.c logging.c nfc_utils.c)
target_link_libraries(srix-updateCdown ${LIBNFC_LIBRARIES})
