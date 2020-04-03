cmake_minimum_required(VERSION 3.10)
project(rtsp_server)

set(CMAKE_BUILD_TYPE Debug)

if(DEFINED ENV{CROSS_COMPILE})
	set(CMAKE_SYSTEM_NAME Linux)
	set(CMAKE_SYSTEM_VERSION 1)
	set(CMAKE_C_COMPILER $ENV{CROSS_COMPILE}gcc)
	set(CMAKE_CXX_COMPILER $ENV{CROSS_COMPILE}g++)
endif()

set(CMAKE_VERBOSE_MAKEFILE true)
include(ExternalProject)

#################
### build zlib
#################
set(zlib_PREFIX ${CMAKE_BINARY_DIR}/external_library/zlib-1.2.11)
set(zlib_INSTALL ${zlib_PREFIX}-install)
if(DEFINED ENV{CROSS_COMPILE})
	set(zlib_CONFIGURE_COMMAND
			CC=$ENV{CROSS_COMPILE}gcc AR=$ENV{CROSS_COMPILE}ar RANLIB=$ENV{CROSS_COMPILE}ranlib)
endif()
set(zlib_CONFIGURE_COMMAND
		${zlib_CONFIGURE_COMMAND}
		${zlib_PREFIX}/src/zlib/configure --prefix=${zlib_INSTALL} --static)


ExternalProject_Add(zlib
    PREFIX ${zlib_PREFIX}
#    GIT_REPOSITORY "https://github.com/madler/zlib.git"
#    GIT_TAG "v1.2.11"
	URL "${CMAKE_SOURCE_DIR}/external_library/zlib-1.2.11.tar.gz"
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${zlib_CONFIGURE_COMMAND}
    INSTALL_DIR ${zlib_INSTALL}
#    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#               -DCMAKE_INSTALL_PREFIX=${zlib_INSTALL}
#               -DAMD64=ON
)

#################
### build openssl
#################
set(openssl_PREFIX ${CMAKE_BINARY_DIR}/external_library/openssl-1.1.1d)
set(openssl_INSTALL ${openssl_PREFIX}-install)
set(openssl_CONFIGURE_COMMAND
		${openssl_PREFIX}/src/openssl/Configure no-shared no-idea no-mdc2 no-rc5 --prefix=${openssl_INSTALL})
if(DEFINED ENV{CROSS_COMPILE})
	set(openssl_CONFIGURE_COMMAND
			${openssl_CONFIGURE_COMMAND} linux-generic32 --cross-compile-prefix=$ENV{CROSS_COMPILE})
else()
	set(openssl_CONFIGURE_COMMAND
			${openssl_CONFIGURE_COMMAND} linux-x86_64)
endif()

ExternalProject_Add(openssl
    PREFIX ${openssl_PREFIX}
#    GIT_REPOSITORY "https://github.com/openssl/openssl"
#    GIT_TAG "OpenSSL_1_1_1b"
	URL "${CMAKE_SOURCE_DIR}/external_library/openssl-1.1.1d.tar.gz"
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${openssl_CONFIGURE_COMMAND}
    INSTALL_DIR ${openssl_INSTALL}
)


#################
### build curl
#################
set(curl_PREFIX ${CMAKE_BINARY_DIR}/external_library/curl-7.67.0)
set(curl_INSTALL ${curl_PREFIX}-install)
set(curl_CONFIGURE_COMMAND
		${curl_PREFIX}/src/curl/configure --disable-shared --with-ssl=${openssl_INSTALL} --with-zlib=${zlib_INSTALL} --prefix=${curl_INSTALL})
if(DEFINED ENV{CROSS_COMPILE})
	set(curl_CONFIGURE_COMMAND
			${curl_CONFIGURE_COMMAND} --host=$ENV{CROSS_COMPILE_PREFIX})
endif()


ExternalProject_Add(curl
    PREFIX ${curl_PREFIX}
#    GIT_REPOSITORY "https://github.com/curl/curl"
#    GIT_TAG "curl-7.67.0"
	URL "${CMAKE_SOURCE_DIR}/external_library/curl-7.67.0.tar.gz"
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${curl_CONFIGURE_COMMAND}
    INSTALL_DIR ${curl_INSTALL}
)


#################
### build libuv
#################
set(libuv_PREFIX ${CMAKE_BINARY_DIR}/external_library/libuv-v1.34.0)
set(libuv_INSTALL ${libuv_PREFIX}-install)
set(libuv_CONFIGURE_COMMAND
		sh autogen.sh COMMAND ${libuv_PREFIX}/src/libuv/configure --disable-shared --enable-static --prefix=${libuv_INSTALL})
if(DEFINED ENV{CROSS_COMPILE})
	set(libuv_CONFIGURE_COMMAND
			${libuv_CONFIGURE_COMMAND} --host=$ENV{CROSS_COMPILE_PREFIX})
endif()


ExternalProject_Add(libuv
    PREFIX ${libuv_PREFIX}
#    GIT_REPOSITORY "https://github.com/libuv/libuv"
#    GIT_TAG "v1.34.0"
	URL "${CMAKE_SOURCE_DIR}/external_library/libuv-v1.34.0.tar.gz"
    UPDATE_COMMAND ""
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${libuv_CONFIGURE_COMMAND}
    INSTALL_DIR ${libuv_INSTALL}
)


add_executable(${CMAKE_PROJECT_NAME}
		main.c 
		livemedia/hash_table.c
		livemedia/basic_hash_table.c
		livemedia/media.c 
		livemedia/str_dup.c
		livemedia/media_source.c
		livemedia/framed_source.c
		livemedia/generic_media_server.c
		livemedia/server_media_session.c
		livemedia/rtsp_server.c
		livemedia/rtsp_common.c
		groupsock/groupsock_helper.c
		groupsock/net_address.c
		groupsock/inet.c
		)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC -g -Wall)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
		${zlib_INSTALL}/include
		${openssl_INSTALL}/include
		${curl_INSTALL}/include
		${libuv_INSTALL}/include
		.)
target_link_libraries(${CMAKE_PROJECT_NAME}
		${curl_INSTALL}/lib/libcurl.a
		${openssl_INSTALL}/lib/libssl.a
		${openssl_INSTALL}/lib/libcrypto.a
		${zlib_INSTALL}/lib/libz.a
		${libuv_INSTALL}/lib/libuv.a
		dl
		pthread)

if(DEFINED ENV{CROSS_COMPILE})
	target_link_libraries(${CMAKE_PROJECT_NAME} libgomp.a rt)
else()
	find_library(IDN2_LIB idn2)
	if(IDN2_LIB)
		target_link_libraries(${CMAKE_PROJECT_NAME} idn2)
	endif()
endif()

add_dependencies(openssl zlib)
add_dependencies(curl openssl zlib)
add_dependencies(${CMAKE_PROJECT_NAME} zlib openssl curl libuv)
