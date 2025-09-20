#include "revert_string.h"
#include <stdio.h>
#include <string.h>

void RevertString(char *str)
{    
    int len = strlen(str);
    int l = 0;
    int r = len - 1;
    
    while (l < r)
    {
        char temp = str[l];
        str[l] = str[r];
        str[r] = temp;
        
        l++;
        r--;
    }
}

