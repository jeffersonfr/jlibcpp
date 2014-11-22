// stdafx.cpp : source file that includes just the standard includes
// win32.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

char *strndup(const char *s, size_t size)
{
	char *r;
	char *end = (char *)memchr(s, 0, size);
	
	if (end) {
		/* Length + 1 */
		size = end - s + 1;
	}
	
	r = (char *)malloc(size);
	
	if (size) {
		memcpy(r, s, size - 1);
		r[size - 1] = '\0';
	}

	return r;
}