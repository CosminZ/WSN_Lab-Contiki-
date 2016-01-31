/*
  Creation of an ArrayList and its functions.
  It will store the list of IP adresses of the nodes

  
  WSN Laboratory
  Uni Freiburg
  Freiburg, 25.01.2015
*/



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "udpClient6.h"
#include "localizationList.h"

/*
typedef char* value_type;

struct arraylist {
  int size;
  value_type* data;
};*/

void arraylist_initial(struct arraylist *list) {
  list->size = 0;
  list->data = NULL;
}

int arraylist_get_size(const struct arraylist list) {
  return list.size;
}

value_type* arraylist_get_data_collection(const struct arraylist list) {
  return list.data;
}

void arraylist_set_data_collection(struct arraylist *list, value_type* data) {
  list->data = data;
}

void arraylist_add(struct arraylist *list, value_type value) {
 int size = arraylist_get_size(*list);
  value_type *new_data;

  new_data = realloc(list->data, (size + 1) * sizeof new_data[0]);

  if (new_data)
  {
      new_data[size] = value;
      arraylist_set_data_collection(list, new_data);
      ++list->size;
  }
}

value_type arraylist_get(const struct arraylist list, int index) {
  if(index < arraylist_get_size(list)) {
    return list.data[index];
  }
  else {
    return NULL;
  }
}

int arraylist_indexof(const struct arraylist list, value_type value) {
  int index = 0;
  for(index = 0; index < (arraylist_get_size(list)); index++) {
    if(strcmp(list.data[index], value) == 0) {
      return index;
    }
  }

  return -1;
}


void addToListWithCheck (struct arraylist *list, value_type value) {
  printf("Inside addToListWithCheck ipaddr = %s", value);
  int index = arraylist_indexof(*list, value);
  printf("Index of Ip adress (%s) in current arrayList = %d",value, index);
  if (index != -1) {
    printf("String not added [already present], index of string in arrayList = %d. \n", index);
  } else {
    arraylist_add (list, value);
  }
}

char* getLeftOf (struct arraylist list, value_type value) {
  printf("------ Inside getLeftOf incomingIp = %s.\n ", value);
  char * leftIp = NULL;
  int index = arraylist_indexof(list, value);
  printf("------ Inside getLeftOf index of incomingIp in list = %d.\n ", index);
  if (index != -1) {
    if (index == 0) {
      leftIp = arraylist_get(list, (arraylist_get_size (list) - 1));
    }
    else if (index > 0) {
      leftIp = arraylist_get(list, (index-1));
    }
  }
  return leftIp;
}

char* getRightOf (struct arraylist list, value_type value) {
  printf("------ Inside getLeftOf incomingIp = %s.\n ", value);
  char * rightIp = NULL;
  int index = arraylist_indexof(list, value);
  printf("------ Inside getRightOf index of incomingIp in list = %d.\n ", index);
  int size = arraylist_get_size (list);
  printf("------ Inside getRightOf size of list = %d.\n ", size);
  if (index != -1) {
    if (index == (size - 1)) {
      rightIp = arraylist_get(list, 0);
    }
    else if (index >= 0) {
      rightIp = arraylist_get(list, (index+1));
    }
  }
  return rightIp;
}
