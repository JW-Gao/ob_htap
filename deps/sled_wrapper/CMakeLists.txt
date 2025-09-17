# 设置项目名称
project(sled_wrapper)

# 1. 创建一个真正的 STATIC 库
# 这个库会由 DelsDB.cpp 编译生成一个 libsled_wrapper.a 文件。

set(SRC_FILES 
    sled_db.cpp
)

add_library(sled_wrapper STATIC 
    ${SRC_FILES}
)

# 2. 为 sled_wrapper 目标设置头文件搜索路径
# 我们使用 PUBLIC 关键字，这意味着：
#   - 在编译 sled_wrapper 自身时 (即编译 DelsDB.cpp)，这个路径生效。
#   - 任何链接到 sled_wrapper 的目标 (如 miniob_server)，也会自动获得这个路径。
target_include_directories(sled_wrapper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 找到我们的Rust静态库文件
find_library(SLED_FFI_LIBRARY 
    NAMES sled_ffi
    PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../third_dels/sled_ffi/target/release
    NO_DEFAULT_PATH
)

if (NOT SLED_FFI_LIBRARY)
    message(FATAL_ERROR "libsled_ffi.a not found! Please build the Rust project first.")
endif()

message(STATUS "Found sled_ffi static library: ${SLED_FFI_LIBRARY}")

# 3. 将底层的依赖项作为 INTERFACE 链接到 sled_wrapper
# 这意味着 "任何链接到我(sled_wrapper)的人，也必须链接这些东西"。
# sled_wrapper 本身在编译时不需要它们，但它的使用者需要。
target_link_libraries(sled_wrapper INTERFACE
    ${SLED_FFI_LIBRARY}
    pthread
    dl
    m
)
