#include "cpu8080_common.h"

#include <ctype.h>
#include <stdio.h>


void sv_print(string_view_t sv) {
	for(int i = 0; i < sv.len; i++) {
		putchar(sv.str[i]);
	}
}
bool sv_cmp(string_view_t sv1, string_view_t sv2) {
	if(sv1.len != sv2.len) {
		return false;
	}
	if(sv1.len == 0) {
		return sv1.str == sv2.str;
	}
	for(int i=0;i<sv1.len;i++) {
		if(sv1.str[i] != sv2.str[i]) {
			return false;
		}
	}
	return true;
}
bool sv_cmp_weak(string_view_t sv1, string_view_t sv2) {
	if(sv1.len != sv2.len) {
		return false;
	}
	if(sv1.len == 0) {
		return sv1.str == sv2.str;
	}
	for(int i=0;i<sv1.len;i++) {
		if(tolower(sv1.str[i]) != tolower(sv2.str[i])) {
			return false;
		}
	}
	return true;
}
bool sv_str_cmp(string_view_t s1, char* s2) {
	int i;
	for(i = 0; i < s1.len && s2[i] != '\0'; i++) {
		if(s1.str[i] != s2[i]) {
			return false;
		}
	}
	return i == s1.len && s2[i] == '\0';
}
bool sv_str_cmp_weak(string_view_t s1, char* s2) {
	int i;
	for(i = 0; i < s1.len && s2[i] != '\0'; i++) {
		if(tolower(s1.str[i]) != tolower(s2[i])) {
			return false;
		}
	}
	return i == s1.len && s2[i] == '\0';
}