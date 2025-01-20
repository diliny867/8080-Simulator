#pragma once

#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

typedef struct {
    char* ptr;
    int64_t size;
    HANDLE h_file;
    HANDLE h_map;
} file_mm_data;

typedef enum {
    FMM_READONLY = 1,
    FMM_READWRITE = 2,
} file_mm_mode;

extern file_mm_data file_mm_open(char* path, file_mm_mode mode);
extern file_mm_data file_mm_wopen(wchar_t* path, file_mm_mode mode);
extern file_mm_data file_mm_fopen(FILE* file, file_mm_mode mode);
extern void file_mm_free(file_mm_data* fd);


#ifdef FILE_MM_UTILS_IMPLEMENTATION

#include <stdlib.h>
#include <io.h>

static inline wchar_t* char_to_wchar(const char* str) {
    size_t str_len = strlen(str);
    size_t wstr_len;
    mbstowcs_s(&wstr_len, NULL, 0, str, str_len); // get number of wchars
    wchar_t* wstr = malloc((wstr_len+1) * sizeof(wchar_t));
    mbstowcs_s(&str_len, wstr, wstr_len, str, str_len); // convert
    return wstr;
}

file_mm_data file_mm_wopen(wchar_t* path, file_mm_mode mode) {
    file_mm_data out = {0,0,0,0};

    out.h_file = CreateFile(path, GENERIC_READ | (GENERIC_WRITE * (mode == FMM_READWRITE)), 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (out.h_file == INVALID_HANDLE_VALUE) {
        //printf("CreateFile error %lu\n", GetLastError());
        return out;
    }

    if (!GetFileSizeEx(out.h_file, &out.size)) {
        //printf("GetFileSize error %lu\n", GetLastError());
        //CloseHandle(out.h_file);
        return out;
    }

    out.h_map = CreateFileMapping(out.h_file, NULL, (PAGE_READONLY * (mode == FMM_READONLY)) | (PAGE_READWRITE * (mode == FMM_READWRITE)), 0, 0, NULL);
    if (out.h_map == NULL) {
        //printf("CreateFileMapping error %lu\n", GetLastError());
        //CloseHandle(out.h_file);
        return out;
    }

    out.ptr = MapViewOfFile(out.h_map, FILE_MAP_READ | (FILE_MAP_WRITE * (mode == FMM_READWRITE)) , 0, 0, 0);
    if (out.ptr == NULL) {
        //printf("MapViewOfFile error %lu\n", GetLastError());
        //CloseHandle(out.h_map);
        //CloseHandle(out.h_file);
        return out;
    }

    return out;
}
file_mm_data file_mm_open(char* path, file_mm_mode mode) {
    WCHAR* wpath;
    wpath = char_to_wchar(path);
    //wprintf(L"%ls\n\n", wpath);
    file_mm_data out = file_mm_wopen(wpath, mode);
    free(wpath);
    return out;
}
file_mm_data file_mm_fopen(FILE* file, file_mm_mode mode) {
    file_mm_data out = {0,0,0,0};

    out.h_file = (HANDLE)_get_osfhandle(_fileno(file));
    if (out.h_file == INVALID_HANDLE_VALUE) {
        //printf("_get_osfhandle error %lu\n", errno);
        return out;
    }
    
    if (!GetFileSizeEx(out.h_file, &out.size)) {
        //printf("GetFileSize error %lu\n", GetLastError());
        out.h_file = INVALID_HANDLE_VALUE; // so it wont call CloseHandle
        return out;
    }

    out.h_map = CreateFileMapping(out.h_file, NULL, (PAGE_READONLY * (mode == FMM_READONLY)) | (PAGE_READWRITE * (mode == FMM_READWRITE)), 0, 0, NULL);
    out.h_file = INVALID_HANDLE_VALUE; // so it wont call CloseHandle
    if (out.h_map == NULL) {
        //printf("CreateFileMapping error %lu\n", GetLastError());
        return out;
    }

    out.ptr = MapViewOfFile(out.h_map, FILE_MAP_READ | (FILE_MAP_WRITE * (mode == FMM_READWRITE)) , 0, 0, 0);
    if (out.ptr == NULL) {
        //printf("MapViewOfFile error %lu\n", GetLastError());
        //CloseHandle(out.h_map);
        return out;
    }

    return out;
}

inline void file_mm_free(file_mm_data* fd) {
    //if(fd->h_file != INVALID_HANDLE_VALUE){
    //    if(fd->h_map != NULL){
    //        if(fd->ptr != NULL){
    //            UnmapViewOfFile(fd->ptr);
    //        }
    //        CloseHandle(fd->h_map);
    //    }
    //    CloseHandle(fd->h_file);
    //}
    if(fd->ptr != NULL){
        UnmapViewOfFile(fd->ptr);
    }
    if(fd->h_map != NULL){
        CloseHandle(fd->h_map);
    }
    if(fd->h_file != INVALID_HANDLE_VALUE){
        CloseHandle(fd->h_file);
    }
}

#endif
