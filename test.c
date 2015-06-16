#include <string.h>
#include <stdio.h>

int func(char *, const char *);

main()
{
    int n;
    char *str = strdup("abcabcdefghaaabcbc");
    const char *substr = "abc";
    n = func(str, substr);
    printf("%d\t%s\n", n, str);
}

int func(char *str, const char *substr)
{
  int len;
  int i = 0;
  char *p, *p2;
  if(!str || !substr)
    return -1;
  if(len = strlen(substr))
    if(p2 = p = strstr(str, substr))
      do{
	i++;
	p2 = strstr(p2 + len, substr);
	if(!p2)
	  while(*p = *(p + len*i))
	    p++;
	else
	  while(p + len*i < p2){
	    *p = *(p + len*i);
	    p++;
	  }
      }while(p2);
  return i;
}

int func2(char *str, const char *substr)
{
	char *strend, *p, *p2;
	int sublen, n = 1;
	if(!str || !substr)
		return -1;
	strend = strstr(str, substr);
	if (!strend)
		return 0;
	sublen = strlen(substr);
	p = strend + sublen;
	while (p2 = strstr(p, substr)) {
		n++;
		strncpy(strend, p, p2 - p);
		strend += p2 - p;
		p = p2 + sublen;
	}
	strcpy(strend, p);
	return n;
}
