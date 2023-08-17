import os
import ctypes
import time
from typing import List

cpp_lib = ctypes.CDLL(os.path.join(os.path.dirname(__file__), '..', 'build/libfindfiles.so'))

def find_files(root_dir: str, extension: List[str], filtered_dirs: List[str]):
    result = []
    def py_callback(filepath):
        result.append(filepath)

    def list2clist(items):
        ret = (ctypes.c_char_p * len(items))()
        ret[:] = [name.encode('utf-8') for name in items]
        return ret

    cpp_callback = ctypes.CFUNCTYPE(None, ctypes.c_char_p)(py_callback)
    cpp_lib.find_files(
        root_dir.encode('utf-8'), 
        list2clist(filtered_dirs), len(filtered_dirs), 
        list2clist(extension), len(extension),
        cpp_callback
    )
    return result


if __name__ == '__main__':
    t0 = time.time()
    files = find_files('/home/', ['.cpp'], ['.git'])
    print("time: {:3.2f}".format(time.time() - t0))