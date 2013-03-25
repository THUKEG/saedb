# Copyright (C)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# @file create_app.py
# @brief a script to automatically initialize sae apps
# @author Yutao Zhang
# @version 0.1
# @date 2013-03-21



import os

def main():
   apps = []
   curdir = os.path.abspath(os.path.curdir)
   for parent, dirnames, filenames in os.walk(curdir):
       for app in dirnames:
           apps.append(app)
           cmake_file = open(os.path.join(app,"CMakeLists.txt"),'w')
           cmake_file.write("project(%s)\n\n"%app)
           cmake_file.write("file(GLOB %s_SOURCE\n"%app.upper())
           cmake_file.write('\t"*.hpp"\n\t"*.cpp"\n\t)\n\n')
           cmake_file.write("add_executable(%s ${%s_SOURCE})"%(app,app.upper()))
           cmake_file.close()
   cmake_file = open("CMakeLists.txt",'w')
   for app in apps:
       cmake_file.write("add_subdirectory(%s)\n"%app)
   cmake_file.close()



if __name__ == "__main__":
    main()
