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

file(GLOB HEADER_FILES flom/*.hpp)
add_custom_target(flom_headers SOURCES ${HEADER_FILES})
enable_clang_format(flom_headers)
enable_clang_tidy(flom_headers)

install(DIRECTORY . DESTINATION include FILES_MATCHING PATTERN "*.hpp")
