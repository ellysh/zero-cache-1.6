include_directories(${COMMON_INCLUDE_DIRS})
link_directories(${COMMON_LIBDIR})

set(zero_cache_LIB_SRCS
    client.cpp
    client_base.cpp
    client_list.cpp
    client_wrap.cpp
    registrar_client.cpp)

add_library(zero_cache SHARED ${zero_cache_LIB_SRCS})

if(UNIX)
    target_link_libraries(zero_cache common ${ZMQ_LIBRARY})
endif(UNIX)

install(TARGETS zero_cache DESTINATION lib/${PROJECT_NAME})

install(FILES
            client_base.h
            client_list.h
            client_wrap.h
            registrar_client.h
        DESTINATION include/${PROJECT_NAME})
