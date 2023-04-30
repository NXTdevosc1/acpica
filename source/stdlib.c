#include <nosdef.h>
typedef unsigned long long size_t;

#define EXPORT


EXPORT void *memset(
   void *dest,
   char c,
   unsigned long long count
) {
    if(!(count & 7)) __stosq(dest, c, count >> 3);
    else if(!(count & 3)) __stosd(dest, c, count >> 2);
    else __stosb(dest, c, count);
    return dest;
}
EXPORT void *memcpy(
   void *dest,
   const void *src,
   unsigned long long count
) {
    while(count) {
        *(char*)dest = *(char*)src;
        ((char*)dest)++;
        ((char*)src)++;
        count--;
    }
    return dest;
}

int memcmp(
   const void *buffer1,
   const void *buffer2,
   unsigned long long count
) {
    while(count) {
        char b1 = *(char*)buffer1, b2 = *(char*)buffer2;
        if(b1 < b2) {
            return -1;
        } else if(b1 > b2) {
            return 1;
        }
        ((char*)buffer1)++;
        ((char*)buffer2)++;
        count--;
    }
    return 0;
}

char *strncpy(
   char *strDest,
   const char *strSource,
   size_t count
) {
    int endzero = 0;
    while(count) {
        if(*strDest == 0) endzero = 1;

        *strDest = *strSource;
        strDest++;
        strSource++;
        count--;
    }
    if(endzero) *strDest = 0;
    return strDest;
}

int isdigit(char c) {
    return c >= '0' && c <= '9';
}

int isupper(
   int c
) {
    return c >= 'A' && c<='Z';
}

int isalpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

int isspace(
   int c
) {
    return c == 9 || c == 0xD || c == 0x20;
}

int toupper(
   int c
) {
    if(c >= 'a' && c <= 'z') return c - 32;
    
    return c;
}

int tolower(
   int c
) {
    if(c >= 'A' && c <= 'Z') return c + 32;
    
    return c;
}


#define ISDIGIT(c) isdigit(c)
#define ISUPPER(c) isupper(c)
#define ISALPHA(c) isalpha(c)
#define ISSPACE(c) isspace(c)


int
strncmp(const char *s1, const char *s2, register size_t n)
{
  register unsigned char u1, u2;

  while (n-- > 0)
    {
      u1 = (unsigned char) *s1++;
      u2 = (unsigned char) *s2++;
      if (u1 != u2)
	return u1 - u2;
      if (u1 == '\0')
	return 0;
    }
  return 0;
}

size_t strlen(char* str) {
    size_t len = 0;
    while(*str) {
        str++;
        len++;
    }
    return len;
}

int isprint(
   int c
) {
    return c>=0x20 && c<=0x7E;
}

int isxdigit(
   int c
) {
    return (c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a'&&c<='f');
}

char* strncat(char* destination, const char* source, size_t num)
{
    // make `ptr` point to the end of the destination string
    char* ptr = destination + strlen(destination);
 
    // Appends characters of the source to the destination string
    while (*source != '\0' && num--) {
        *ptr++ = *source++;
    }
 
    // null terminate destination string
    *ptr = '\0';
 
    // destination string is returned by standard `strncat()`
    return destination;
}

char *
strchr (register const char *s, int c)
{
  do {
    if (*s == c)
      {
	return (char*)s;
      }
  } while (*s++);
  return (0);
}
#define ULONG_MAX ((UINT32)-1)

char *
strrchr (register const char *s, int c)
{
  char *rtnval = 0;

  do {
    if (*s == c)
      rtnval = (char*) s;
  } while (*s++);
  return (rtnval);
}

char *
strstr (const char *s1, const char *s2)
{
  const char *p = s1;
  const size_t len = strlen ((char*)s2);

  if (!len)
    return (char*)s1;

  for (; (p = strchr (p, *s2)) != 0; p++)
    {
      if (strncmp (p, s2, len) == 0)
	return (char *)p;
    }
  return NULL;
}


unsigned long
strtoul(const char *nptr, char **endptr, register int base)
{
	register const char *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	do {
		c = *s++;
	} while (ISSPACE(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (ISDIGIT(c))
			c -= '0';
		else if (ISALPHA(c))
			c -= ISUPPER(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = ULONG_MAX;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);
	return (acc);
}

