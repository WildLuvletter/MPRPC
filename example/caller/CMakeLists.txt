set(SRC_LIST calluserservice.cc ../user.pb.cc)
add_executable(consumer ${SRC_LIST})
add_executable(friconsumer ../friend.pb.cc callfriendservice.cc)


target_link_libraries(consumer mprpc protobuf)
target_link_libraries(friconsumer mprpc protobuf)
