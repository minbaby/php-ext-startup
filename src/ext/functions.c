#include "functions.h"


int strsplit (const char *str, char *parts[], const char *delimiter) {
  char *pch;
  int i = 0;
  char *copy = NULL, *tmp = NULL;

  copy = strdup(str);
  if (! copy)
    goto bad;

  pch = strtok(copy, delimiter);

  tmp = strdup(pch);
  if (! tmp)
    goto bad;

  parts[i++] = tmp;

  while (pch) {
    pch = strtok(NULL, delimiter);
    if (NULL == pch) break;

    tmp = strdup(pch);
    if (! tmp)
      goto bad;

    parts[i++] = tmp;
  }

  free(copy);
  return i;

 bad:
  free(copy);
  for (int j = 0; j < i; j++)
    free(parts[j]);
  return -1;
}

bool zval_str_equal(zval *first, zval *second)
{
    if (Z_TYPE_P(first) == Z_TYPE_P(second) && zend_string_equals(Z_STR_P(first), Z_STR_P(second))) {
        return true;
    }
    return false;
}

bool m_array_set(zval *array, const char *key, zval *value)
{

}

zval m_array_get(zval *array, const char *key)
{
    zval ret;
    if (Z_TYPE_P(array) != IS_ARRAY) {
        return ret;
    }

    char *x[1];

    strsplit(key, x, ".");

    return ret;
}
