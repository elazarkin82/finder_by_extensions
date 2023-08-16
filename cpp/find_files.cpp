#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <memory.h>
#include <sys/time.h>
#include <stdint.h>
#include <Python.h>

#include <vector>
#include <string>

uint64_t get_useconds() 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

inline bool endsWith(const char* str, const std::vector<std::string>& endings) 
{
    for (const std::string& ending : endings) 
    {
        std::size_t endingLength = ending.length();
        std::size_t strLength = strlen(str);
        if (strLength >= endingLength && strcmp(str + strLength - endingLength, ending.c_str()) == 0) 
            return true;
    }
    return false;
}
void _find_files(
    const char *root_dir, 
    std::vector<std::string> &filtered_dirs,
    std::vector<std::string> &extensions,
    std::vector<std::string> &result_files
)
{
    std::vector<std::string> dirs_to_check;
    int curr_index = 0;
    uint32_t num_of_files = 0;

    dirs_to_check.push_back(root_dir);

    while(curr_index < dirs_to_check.size())
    {
        const char *curr_dir = dirs_to_check[curr_index].c_str();
        DIR* dir = opendir(curr_dir);
        if (dir != NULL) 
        {
            struct dirent* entry;
            while ((entry = readdir(dir)) != NULL) 
            {
                struct stat st;
                const char *name = entry->d_name;
                
                if (!S_ISLNK(st.st_mode) && strcmp(name, ".") != 0 && strcmp(name, "..") != 0)
                {
                    if (entry->d_type == DT_DIR)
                    {
                        std::string next_dir = std::string(curr_dir) + "/" + name;
                        if(!endsWith(next_dir.c_str(), filtered_dirs))
                            dirs_to_check.push_back(next_dir);
                    }
                    else if (entry->d_type == DT_UNKNOWN && endsWith(name, extensions))
                        dirs_to_check.push_back(std::string(curr_dir) + "/" + name);
                    else if (entry->d_type == DT_REG)
                    {
                        if(endsWith(name, extensions))
                            result_files.push_back(std::string(curr_dir) + "/" + name);
                        num_of_files++;
                    }
                }
            }
            closedir(dir);
        }
        curr_index++;
    }
    printf(
        "search in %lu dirs, found %lu files. (total files found %u)\n", 
        dirs_to_check.size(), result_files.size(), num_of_files
    );
}

extern "C" void find_files(
    const char *root_dir, 
    const char **_filtered_dirs, size_t num_dirs, 
    const char **_extensions, size_t num_extensions, 
    void (*callback)(const char*)
) 
{
    std::vector<std::string> result_files, filtered_dirs, extensions;
    
    for(int i = 0; i < num_dirs; i++)
    {
        std::string dir_path(_filtered_dirs[i]);
        while(dir_path.back() == '/')
            dir_path.pop_back();
        filtered_dirs.push_back(dir_path);
    }
    for(int i = 0; i < num_extensions; i++)
        extensions.push_back(_extensions[i]);

    _find_files(root_dir, filtered_dirs, extensions, result_files);

    for (const std::string &file : result_files)
        callback(file.c_str());
}

int main() 
{
    const char *root_dir = "/home/";
    std::vector<std::string> filtered_dirs = {".git"};
    std::vector<std::string> extensions = {".pdf", ".jpg"};
    std::vector<std::string> result_files;

    uint64_t t0 = get_useconds();
    _find_files(root_dir, filtered_dirs, extensions, result_files);
    printf("time taken found files: %3.4f seconds\n", (get_useconds() - t0)/1000000.0);

    for (std::string &file : result_files) 
    {
        printf("%s\n", file.c_str());
    }

    return 0;
}