#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TASK_FILE "tasks.json"

int get_next_id();
void add_task(char *description);
void list_task();
void delete_task(int id);
void update_task(int id, char *description);
void mark_progress(int id);
void mark_done(int id);
void list_task_arg(char *status);

void time_parser(time_t timestamp, char *buffer, size_t buffer_size) {
  struct tm *t = localtime(&timestamp);
  strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", t);
}

cJSON *read_task_file() {
  FILE *fp = fopen(TASK_FILE, "r");
  if (fp == NULL) {
    return cJSON_CreateArray();
  }

  fseek(fp, 0, SEEK_END);
  int file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  if (file_size == 0) {
    fclose(fp);
    return cJSON_CreateArray();
  }

  char *buffer = malloc(file_size + 1);
  fread(buffer, 1, file_size, fp);
  buffer[file_size] = '\n';
  fclose(fp);

  cJSON *root = cJSON_Parse(buffer);
  free(buffer);

  if (!root || !cJSON_IsArray(root)) {
    if (root)
      cJSON_Delete(root);
    return cJSON_CreateArray();
  }

  return root;
}

int write_task_file(cJSON *root) {
  if (root == NULL) {
    return 1;
  }

  FILE *fp = fopen(TASK_FILE, "w");
  if (fp == NULL) {
    return 1;
  }

  char *json_str = cJSON_Print(root);
  if (!json_str) {
    fclose(fp);
    return 1;
  }

  fputs(json_str, fp);
  fclose(fp);
  cJSON_free(json_str);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: task-cli [add] [description]");
    printf("Usage: task-cli [list] [description]");
    return 1;
  }
  if (strcmp(argv[1], "add") == 0) {
    if ((argc < 3 || argc > 3) || argv[2] == NULL) {
      printf("Error: No task description");
      return 1;
    }
    add_task(argv[2]);
  } else if (strcmp(argv[1], "list") == 0) {
    if (argc == 2) {
      list_task();
    } else if (argc == 3) {

      if ((strcmp(argv[2], "done") == 0) || (strcmp(argv[2], "todo") == 0) ||
          (strcmp(argv[2], "in-progress") == 0)) {

        list_task_arg(argv[2]);
      } else {
        printf(
            "Error: only [done/todo/in-progress] arguments can be applied\n");
        return 1;
      }
    } else {
      printf("Error: wrong command\n");
      printf("Usage: task-cli list [status]\n");
      return 1;
    }
  } else if (strcmp(argv[1], "delete") == 0) {
    if (argc != 3) {
      printf("Error: enter a number\n");
      return 1;
    }
    int id = atoi(argv[2]);

    delete_task(id);
  } else if (strcmp(argv[1], "update") == 0) {
    if (argc != 4) {
      printf("Error: Usage [update] [id] [description]\n");
      return 1;
    }
    int id = atoi(argv[2]);

    update_task(id, argv[3]);
  } else if (strcmp(argv[1], "mark-in-progress") == 0) {
    if (argc != 3) {
      printf("Error: enter a number\n");
      return 1;
    }
    int id = atoi(argv[2]);

    mark_progress(id);
  } else if (strcmp(argv[1], "mark-done") == 0) {
    if (argc != 3) {
      printf("Error: enter a number\n");
      return 1;
    }
    int id = atoi(argv[2]);

    mark_done(id);
  }

  return 0;
}

int get_next_id() {
  cJSON *root = read_task_file();
  if (!root) {
    return 1;
  }

  int max_id = 0;
  int array_size = cJSON_GetArraySize(root); // get the cJSON array size
  if (array_size == 0) {
    printf("No Valid tasks array was found. Starting with id 1\n");
    cJSON_Delete(root);
    return 1;
  }

  // loop through all the json arrays and get their id's, compare them and
  // take the last or max id
  for (int i = 0; i < array_size; i++) {
    cJSON *task =
        cJSON_GetArrayItem(root, i); // get the pointer of i'th number array
    cJSON *id_item =
        cJSON_GetObjectItem(task, "id"); // get the id item from the i'th array

    if (id_item &&
        cJSON_IsNumber(id_item)) { // if id_item != NULL and id_item is a
                                   // cJSON number then run the if block
      int current_id =
          (int)id_item->valuedouble; // get the id_item value and cast it to
                                     // (int) and store it to current_id
      if (current_id > max_id) {
        max_id = current_id;
      }
    }
  }

  cJSON_Delete(root); // delete the root

  int next_id = max_id + 1; // make the next id
  return next_id;           // return the next id
}

void add_task(char *description) {
  // set new id
  int new_id = get_next_id();

  cJSON *root = read_task_file();

  time_t now = time(NULL);

  cJSON *task = cJSON_CreateObject();
  cJSON_AddNumberToObject(task, "id", new_id);
  cJSON_AddStringToObject(task, "description", description);
  cJSON_AddStringToObject(task, "status", "todo");
  cJSON_AddNumberToObject(task, "createdAt", (double)now);
  cJSON_AddNumberToObject(task, "updatedAt", (double)now);

  cJSON_AddItemToArray(root, task);

  if (write_task_file(root) == 0) {
    printf("Added task %d: %s\n", new_id, description);
  } else {
    printf("Error: Failed to save task!\n");
  }
  cJSON_Delete(root);
}

void list_task() {
  cJSON *root = read_task_file();
  if (!root) {
    return;
  }

  int array_size = cJSON_GetArraySize(root);
  if (array_size == 0) {
    printf("No task found\n");
    cJSON_Delete(root);
    return;
  }

  printf("Your tasks:\n");
  printf("\n");
  for (int i = 0; i < array_size; i++) {
    cJSON *task = cJSON_GetArrayItem(root, i);
    cJSON *id = cJSON_GetObjectItem(task, "id");
    cJSON *description = cJSON_GetObjectItem(task, "description");
    cJSON *status = cJSON_GetObjectItem(task, "status");
    cJSON *Crtime_object = cJSON_GetObjectItem(task, "createdAt");
    cJSON *Uptime_object = cJSON_GetObjectItem(task, "updatedAt");
    time_t ctimestamp = (time_t)Crtime_object->valuedouble;
    time_t utimestamp = (time_t)Uptime_object->valuedouble;

    char cr_buffer[50];
    char up_buffer[50];
    time_parser(ctimestamp, cr_buffer, sizeof(cr_buffer));
    time_parser(utimestamp, up_buffer, sizeof(up_buffer));
    printf("%d [%s] %s createdAt: [%s] updatedAt: [%s\n", (int)id->valuedouble,
           status->valuestring, description->valuestring, cr_buffer, up_buffer);
  }

  cJSON_Delete(root);
}

void delete_task(int id) {

  cJSON *root = read_task_file();
  if (!root) {
    return;
  }

  int array_size = cJSON_GetArraySize(root);
  int found = 0;
  for (int i = 0; i < array_size; i++) {
    cJSON *task = cJSON_GetArrayItem(root, i);
    cJSON *id_object = cJSON_GetObjectItem(task, "id");

    if (id_object && cJSON_IsNumber(id_object)) {
      int current_id = (int)id_object->valuedouble;

      if (current_id == id) {
        cJSON_DeleteItemFromArray(root, i);
        found = 1;
        break;
      }
    }
  }

  if (found) {
    if (write_task_file(root) == 0) {
      printf("Task %d has been deleted\n", id);
    } else {
      printf("Error: Failed to save file\n");
    }
  } else {
    printf("Error: no task with id: %d\n", id);
  }

  cJSON_Delete(root);
}

void update_task(int id, char *description) {
  cJSON *root = read_task_file();
  if (!root) {
    return;
  }

  int array_size = cJSON_GetArraySize(root);
  int found = 0;

  time_t now = time(NULL);

  for (int i = 0; i < array_size; i++) {
    cJSON *task = cJSON_GetArrayItem(root, i);
    cJSON *id_object = cJSON_GetObjectItem(task, "id");

    if (id_object && cJSON_IsNumber(id_object)) {
      int current_id = (int)id_object->valuedouble;

      if (id == current_id) {
        cJSON *name_object = cJSON_GetObjectItem(task, "description");
        cJSON *time_object = cJSON_GetObjectItem(task, "updatedAt");
        cJSON_SetValuestring(name_object, description);
        cJSON_SetNumberValue(time_object, (double)now);
        found = 1;
        break;
      }
    }
  }

  if (found) {
    if (write_task_file(root) == 0) {
      printf("Task %d has been deleted\n", id);
    } else {
      printf("Error: Failed to save file\n");
    }
  } else {
    printf("Error: No task with id: %d\n", id);
  }

  cJSON_Delete(root);
}

void mark_progress(int id) {
  cJSON *root = read_task_file();
  if (!root) {
    return;
  }
  int array_size = cJSON_GetArraySize(root);
  int found = 0;

  for (int i = 0; i < array_size; i++) {
    cJSON *task = cJSON_GetArrayItem(root, i);
    cJSON *id_object = cJSON_GetObjectItem(task, "id");

    if (id_object && cJSON_IsNumber(id_object)) {
      int current_id = (int)id_object->valuedouble;

      if (id == current_id) {
        cJSON *name_object = cJSON_GetObjectItem(task, "status");
        cJSON_SetValuestring(name_object, "in-progress");
        found = 1;
        break;
      }
    }
  }

  if (found) {

    if (write_task_file(root) == 0) {
      printf("Task %d has been deleted\n", id);
    } else {
      printf("Error: Failed to save file\n");
    }
  } else {
    printf("Error: No task with id: %d\n", id);
  }

  cJSON_Delete(root);
}

void mark_done(int id) {
  cJSON *root = read_task_file();
  if (!root) {
    return;
  }

  int array_size = cJSON_GetArraySize(root);
  int found = 0;

  for (int i = 0; i < array_size; i++) {
    cJSON *task = cJSON_GetArrayItem(root, i);
    cJSON *id_object = cJSON_GetObjectItem(task, "id");

    if (id_object && cJSON_IsNumber(id_object)) {
      int current_id = (int)id_object->valuedouble;

      if (id == current_id) {
        cJSON *name_object = cJSON_GetObjectItem(task, "status");
        cJSON_SetValuestring(name_object, "done");
        found = 1;
        break;
      }
    }
  }

  if (found) {
    if (write_task_file(root) == 0) {
      printf("Task %d has been deleted\n", id);
    } else {
      printf("Error: Failed to save file\n");
    }
  } else {
    printf("Error: No task with id: %d\n", id);
  }

  cJSON_Delete(root);
}

void list_task_arg(char *status) {
  cJSON *root = read_task_file();
  if (!root) {
    return;
  }

  int array_size = cJSON_GetArraySize(root);
  int found = 0;

  for (int i = 0; i < array_size; i++) {
    cJSON *task = cJSON_GetArrayItem(root, i);
    cJSON *status_objet = cJSON_GetObjectItem(task, "status");

    if (strcmp((status_objet->valuestring), status) == 0) {
      found = 1;
      cJSON *id = cJSON_GetObjectItem(task, "id");
      cJSON *description = cJSON_GetObjectItem(task, "description");
      cJSON *status = cJSON_GetObjectItem(task, "status");
      cJSON *Crtime_object = cJSON_GetObjectItem(task, "createdAt");
      cJSON *Uptime_object = cJSON_GetObjectItem(task, "updatedAt");
      time_t ctimestamp = (time_t)Crtime_object->valuedouble;
      time_t utimestamp = (time_t)Uptime_object->valuedouble;

      char cr_buffer[50];
      char up_buffer[50];
      time_parser(ctimestamp, cr_buffer, sizeof(cr_buffer));
      time_parser(utimestamp, up_buffer, sizeof(up_buffer));
      printf("%d [%s] %s createdAt: [%s] updatedAt: [%s\n",
             (int)id->valuedouble, status->valuestring,
             description->valuestring, cr_buffer, up_buffer);
    }
  }

  if (!found) {
    printf("No task with status [%s]\n", status);
  }
  cJSON_Delete(root);
}
