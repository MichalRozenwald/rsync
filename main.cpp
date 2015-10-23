#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string>
#include <algorithm>

#include "md5.h"

using namespace std;

int main(int argc, char *argv[]) {
//TAKING HASHES: cout << "md5 of 'Michal': " << md5("Michal") << endl;

// to check hash only in ONE file:    std::FILE* f =  fopen("/home/michal/rsync/file1.txt", "r");
    cout << "Find different hashes in files :" << endl;

// writing all the pathes of the files in folder files /home/michal/rsync/files/
    const char * pathname = "/home/michal/rsync/files/";
    string str_pathname = string(pathname);
    DIR *d = opendir(pathname); //Возвращает указатель в случае успеха или NULL в случае ошибки.
    struct dirent *dd; // Возвращает указатель в случае успеха, NULL в случае достижения конца каталога или ошибки

    vector<string> file_names;

    while ((dd = readdir(d)) != NULL ) {
        const char *f_name = dd->d_name;
        if (f_name[0] != '.') {
            string full_path = str_pathname + string(f_name);
            //const char * file_path = (str_pathname + string(f_name)).c_str(); //gives wrong answer because of same const char *

            file_names.push_back(full_path);
            cout << full_path << endl;
        }
    }

    vector< vector <string> > files_hashes;

    for (string file_path : file_names) {
        //cout << file_path << endl;
        std::FILE *f =  std::fopen(file_path.c_str(), "r");

        int block_size = 5; // size of blocks for taking hashes
        vector<char> buf(block_size);
        size_t read_num_b =  std::fread(&buf[0], sizeof buf[0], buf.size(), f);

        vector< string> block_hash_vec(0);
        while (read_num_b) {
             string cur_str_block = "";
            for (size_t elem = 0; elem < read_num_b; elem++) {
                cur_str_block += buf[elem];
            }
            //cout << "md5 of 'str_block' =  " << cur_str_block << " is " << md5(cur_str_block) << endl;
            string cur_block_hash = md5(cur_str_block);
            block_hash_vec.push_back(cur_block_hash);

            read_num_b =  fread(&buf[0], sizeof buf[0], buf.size(), f);
        }

        //for (string n : block_hash_vec)
        //    cout << n << endl;

        files_hashes.push_back(block_hash_vec);
    }

    size_t min_num_bl = 0;
    bool first = true;
    for (vector<string> file : files_hashes) {
        if (first) {
            min_num_bl = file.size();
            first = false;
        }
        if ( file.size() < min_num_bl) {
            min_num_bl = file.size();
        }
    }

    cout << endl << "num file: HASH of current block" << endl << endl;

    vector< vector <string> > hashes_by_blocks(min_num_bl, vector <string> (files_hashes.size()));
    for (size_t block_num = 0; block_num < min_num_bl; block_num++) {
        for (size_t file = 0; file < files_hashes.size(); file++) {
            hashes_by_blocks[block_num][file] = files_hashes[file][block_num];
            if( file > 0 && hashes_by_blocks[block_num][file] != hashes_by_blocks[block_num][file-1]){
                cout << "DIFFER HASHES number: " << block_num << " in files: " << file - 1 << " and " << file << endl;
            }
            cout << file << " : " << files_hashes[file][block_num] << endl;
        }
        cout << endl;
    }

    return 0;
}
