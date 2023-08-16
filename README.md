# finder_by_extensions
Python use c++ shared object to find quickly files in big filesystem.

# After cloning this project:

  cd <finder_by_extensions cloned path>
  mkdir build
  cd build
  cmake .. && make

# Using:

  add <finder_by_extensions cloned path>/python to PYTHONPATH
  In the code add "from cpp_find_files_wrapper import find_files"

  ...
  files = find_files('<some root dir>, ['.png', 'jpg'], ['.git'])
