

typedef char* value_type;

struct arraylist {
  int size;
  value_type* data;
};

value_type arraylist_get (const struct arraylist list, int index);
void arraylist_initial (struct arraylist *list);
int arraylist_get_size (const struct arraylist list);
value_type* arraylist_get_data_collection (const struct arraylist list);
void arraylist_set_data_collection (struct arraylist *list, value_type* data);
void arraylist_add (struct arraylist *list, value_type value);
value_type arraylist_get (const struct arraylist list, int index);
int arraylist_indexof (const struct arraylist list, value_type value);
void addToListWithCheck (struct arraylist *list, value_type value);
char* getLeftOf (const struct arraylist list, value_type value);
char* getRightOf (const struct arraylist list, value_type value);