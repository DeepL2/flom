#
# Copyright 2018 coord.e
#
# This file is part of Flom.
#
# Flom is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Flom is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Flom.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.0.2)

add_executable(test_effector effector.cpp)
flom_add_test(test_effector)

add_executable(test_location location.cpp)
flom_add_test(test_location)

add_executable(test_rotation rotation.cpp)
flom_add_test(test_rotation)

add_executable(test_frame frame.cpp)
flom_add_test(test_frame)

add_executable(test_motion_keyframe motion_keyframe.cpp)
flom_add_test(test_motion_keyframe)

add_executable(test_motion_keyframe_range motion_keyframe_range.cpp)
flom_add_test(test_motion_keyframe_range)

add_executable(test_motion_frame motion_frame.cpp)
flom_add_test(test_motion_frame)

add_executable(test_motion_frame_range motion_frame_range.cpp)
flom_add_test(test_motion_frame_range)

add_executable(test_motion_io motion_io.cpp)
if(USE_LIBCXX)
  target_link_libraries(test_motion_io PRIVATE c++fs)
else()
  target_link_libraries(test_motion_io PRIVATE stdc++fs)
endif()
flom_add_test(test_motion_io)

add_executable(test_motion_misc motion_misc.cpp)
flom_add_test(test_motion_misc)
