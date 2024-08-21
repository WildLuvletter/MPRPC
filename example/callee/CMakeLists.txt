set(SRC_LIST userservice.cc ../user.pb.cc )
add_executable(provider ${SRC_LIST})

add_executable(friprovider ../friend.pb.cc friendservice.cc)

target_link_libraries(provider mprpc protobuf)
target_link_libraries(friprovider mprpc protobuf)
