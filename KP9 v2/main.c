#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#define RED "\x1b[31m"
#define BLUE "\x1b[36m"
#define RESET "\x1b[0m"
#define GREEN "\033[0;32m"
#define DARK_BLUE "\x1b[34m"
#define ESC 27
#define ENTER 13
#define A_CODE 97
#define D_CODE 100
#define R_CODE 114
#define S_CODE 115
#define P_CODE 112
#define MAX_REC 10
#define MAX_CHARS 100
#define NAME_LEN 11
#define ERROR -1
#define SGN "program9"
#define SGN_LEN 8
#define EXTENSION_LEN 4

char current_filename[NAME_LEN + EXTENSION_LEN] = "";
int max_len = NAME_LEN + EXTENSION_LEN;
enum main_menu {FILE_MENU = 1, RECORD};
enum file_menu { CREATE_FILE = 1, READ_FILE, DELETE_FILE }; 
enum rec_menu { CREATE_RECORD = 1, READ_RECORD, EDIT_RECORD, SORT_RECORD, INSERT_RECORD, DELETE_RECORD};

typedef struct {
    char name[NAME_LEN];
    double square;
    double population;
} record;

bool is_esc() {
    printf(BLUE"\nPress ESC key to exit, or any other key to go back to the main menu: "RESET);
    int escChoice = _getch();
    printf("\n");
    system("cls");
    return escChoice == ESC;
}

bool is_input_valid(void* inp_ptr, const char* format) {
    int start_space = 0;
    int newline = 0;
    int result = scanf(format, &start_space, inp_ptr, &newline);
    rewind(stdin);
    if (start_space == 0 && result == 2 && newline == '\n') {
        return true;
    }
    else {
        printf(RED"Error! Invalid input format!\n\n"RESET);
    }
    return false;
}

bool is_recordnum_valid(int rec_num) {
    if (rec_num <= 0) {
        printf(RED"Error! Enter a valid number for the records!\n"RESET);
        return false;
    }
    return true;
}

bool is_restriction_valid(int num, int low_lim, int up_lim) {
    if (!(num >= low_lim && num <= up_lim)) {
        printf(RED"Invalid option! Try again!\n"RESET);
        return false;
    }
    return true;
}

int main_menu() {
    int option = 0;
    do {
        system("cls");
        printf(DARK_BLUE"%d. File menu    |\n", FILE_MENU);
        printf("________________|\n");
        printf("%d. Records menu |\n", RECORD);
        printf("________________|\n"RESET);
    } while (!is_input_valid(&option, " %n%d%c") || !is_restriction_valid(option, FILE_MENU, RECORD));
    return option;
}

int file_menu() {
    int option = 0;
    do {
        system("cls");
        printf(BLUE"%d. Create a file|\n", CREATE_FILE);
        printf("________________|\n");
        printf("%d. Read a file  |\n", READ_FILE);
        printf("________________|\n");
        printf("%d. Delete a file|\n", DELETE_FILE);
        printf("________________|\n"RESET);
    } while (!is_input_valid(&option, " %n%d%c") || !is_restriction_valid(option, CREATE_FILE, DELETE_FILE));
    return option;
}

int record_menu() {
    int option = 0;
    do {
        system("cls");
        printf(BLUE"%d. Create a record |\n", CREATE_RECORD);
        printf("___________________|\n");
        printf("%d. Read a record   |\n", READ_RECORD);
        printf("___________________|\n");
        printf("%d. Edit a record   |\n", EDIT_RECORD);
        printf("___________________|\n");
        printf("%d. Sort records    |\n", SORT_RECORD);
        printf("___________________|\n");
        printf("%d. Insert in record|\n", INSERT_RECORD);
        printf("___________________|\n");
        printf("%d. Delete a record |\n", DELETE_RECORD);
        printf("___________________|\n"RESET);
    } while (!is_input_valid(&option, " %n%d%c") || !is_restriction_valid(option, CREATE_RECORD, DELETE_RECORD));
    return option;
}

bool has_sgn(FILE* file) {
    char firstLine[SGN_LEN + 1];
    return (fgets(firstLine, sizeof(firstLine), file) != NULL && strcmp(firstLine, SGN) == 0);
}

bool is_exist(const char* filename) {
    struct stat file;
    return (stat(filename, &file) == 0);
}

FILE* open_file(const char* mode) {
    char new_name[NAME_LEN];
    if (strlen(current_filename) == 0 || !is_exist(current_filename)) {
        do {
            printf(BLUE"Enter file name (no spaces allowed): "RESET);
        } while (!is_input_valid(&new_name, " %n%10s%c"));
        snprintf(current_filename, max_len, "%s.txt", new_name);
    }
    else {
        printf(BLUE"Current file: %s\n", current_filename);
        printf("Do you want to stay in the current file? ENTER/any other key\n"RESET);
        int choice = _getch();
        if (choice != ENTER) {
            do {
                printf(BLUE"Enter new file name (no spaces allowed): "RESET);
            } while (!is_input_valid(&new_name, " %n%10s%c"));
            snprintf(current_filename, max_len, "%s.txt", new_name);
        }
    }
    if (!is_exist(current_filename)) {
        printf(RED"The file with this name does not exist\n"RESET);
        return NULL;
    }
    FILE* file = fopen(current_filename, mode);
    if (file == NULL) {
        printf(RED"Error!\n"RESET);
        return NULL;
    }
    return file;
}

int read_parameter_choice() {
    int ch = 0, choice = 0;
    do {
        printf(BLUE"R/r - region, S/s - square, P/p - population\n"RESET);
        ch = _getch();
        choice = tolower(ch);
    } while (choice != R_CODE && choice != S_CODE && choice != P_CODE);
    return choice;
}

int read_sorting_choice() {
    int ch = 0, choice = 0;
    do {
        printf(BLUE"A/a - ascending, D/d - descending\n"RESET);
        ch = _getch();
        choice = tolower(ch);
    } while (choice != A_CODE && choice != D_CODE);
    return choice;
}

void swap(record* a, record* b) {
    record temp = *a;
    *a = *b;
    *b = temp;
}

bool compare_region(record* a, record* b, int ch) {
    switch (ch) {
    case A_CODE: return (strcmp(a->name, b->name) > 0);
    case D_CODE: return (strcmp(a->name, b->name) < 0);
    default:
        printf(RED"Error while sorting!\n"RESET);
        return false;
    }
}

bool compare_square(record* a, record* b, int ch) {
    switch (ch) {
    case A_CODE: return (a->square > b->square);
    case D_CODE: return (a->square < b->square);
    default:
        printf(RED"Error while sorting!\n"RESET);
        return false;
    }
}

bool compare_population(record* a, record* b, int ch) {
    switch (ch) {
    case A_CODE: return (a->population > b->population);
    case D_CODE: return (a->population < b->population);
    default:
        printf(RED"Error while sorting!\n"RESET);
        return false;
    }
}

void bubblesort(bool (*compare)(record*, record*, int), record arr[], int num, int ch) {
    for (int i = 0; i < num - 1; i++) {
        for (int j = 0; j < num - i - 1; j++) {
            if (compare(&arr[j], &arr[j + 1], ch)) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}

void sort_records(record arr[], int n, int par_choice, int sort_choice) {
    switch (par_choice) {
    case R_CODE:
        bubblesort(compare_region, arr, n, sort_choice);
        break;
    case S_CODE:
        bubblesort(compare_square, arr, n, sort_choice);
        break;
    case P_CODE:
        bubblesort(compare_population, arr, n, sort_choice);
        break;
    default:
        printf(RED"Error in sorting\n"RESET);
    }
}

int get_num_of_records(FILE* file) {
    if (file == NULL) {
        printf(RED"Invalid file pointer.\n"RESET);
        return ERROR;
    }
    char buffer[MAX_CHARS];
    int num = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        num++;
    }
    int num_of_rec = (num - 1) >= 0 ? num - 1 : 0;
    return num_of_rec;
}

bool is_rec_exist(int rec_index, int num_of_rec) {
    if (rec_index > num_of_rec) {
        printf(RED"Error! This file contains only %d records\n"RESET, num_of_rec);
        return false;
    }
    return true;
}

void print_header() {
    printf("\n%-10s%-15s%-30s%-20s\n", "№", "Region", "Square", "Population");
    printf("--------------------------------------------------------------------\n");
}

void read_data(record* ptr) {
    do {
        printf(BLUE"Enter the region: "RESET);
    } while (!is_input_valid(ptr->name, " %n%10s%c"));

    do {
        printf(BLUE"Enter square: "RESET);
    } while (!is_input_valid(&(ptr->square), " %n%lf%c"));

    do {
        printf(BLUE"Enter the population: "RESET);
    } while (!is_input_valid(&(ptr->population), " %n%lf%c"));
}

void read_input(int* inp_ptr, char* message) {
    do {
        printf(BLUE"%s: "RESET, message);
    } while (!is_input_valid(inp_ptr, " %n%d%c") || !is_recordnum_valid(*inp_ptr));
}

int change_files(char* filename1, char* filename2) {
    if (remove(filename1) != 0) {
        printf(RED"Error removing the file\n"RESET);
        return ERROR;
    }

    if (rename(filename2, filename1) != 0) {
        printf(RED"Error renaming the file\n"RESET);
        return ERROR;
    }
    return 0;
}

bool is_sorted(record records[], int n, int parameter, int method) {
    for (int i = 0; i < n - 1; i++) {
        double compare = 0.0;
        switch (parameter) {
        case R_CODE:
            compare = strcmp(records[i + 1].name, records[i].name);
            break;
        case S_CODE:
            compare = records[i + 1].square - records[i].square;
            break;
        case P_CODE:
            compare = records[i + 1].population - records[i].population;
            break;
        default:
            printf(RED"An error happened while inserting!\n"RESET);
            return false;
        }
        if ((method == A_CODE && compare < 0) || (method == D_CODE && compare > 0)) {
            printf(RED"The records are not sorted this way!\n"RESET);
            return false;
        }
    }
    return true;
}

int find_ind_to_insert(record records[], record new_record, int n, int par, int m) {
    int new_ind = 0;
    switch (par) {
    case R_CODE:
        for (int i = 0; i < n; i++) {
            if ((m == A_CODE && strcmp(records[i].name, new_record.name) < 0) ||
                (m == D_CODE && strcmp(records[i].name, new_record.name) > 0)) {
                new_ind++;
            }
        }
        break;
    case S_CODE:
        for (int i = 0; i < n; i++) {
            if ((m == A_CODE && records[i].square < new_record.square) ||
                (m == D_CODE && records[i].square > new_record.square)) {
                new_ind++;
            }
        }
        break;
    case P_CODE:
        for (int i = 0; i < n; i++) {
            if ((m == A_CODE && records[i].population < new_record.population) ||
                (m == D_CODE && records[i].population > new_record.population)) {
                new_ind++;
            }
        }
        break;
    default:
        printf(RED"Error inserting records!\n"RESET);
        return ERROR;
    }
    return new_ind + 1;
}

char* set_tmp_filename() {
    char* tmp_fn = "tmp";
    int name_ln = strlen(tmp_fn) + EXTENSION_LEN;
    char* tmp_filename = (char*)calloc(name_ln, sizeof(char));
    if (tmp_filename == NULL) {
        fprintf(stderr, "Memory allocation error!\n");
        exit(EXIT_FAILURE);
    }
    sprintf(tmp_filename, "%s.txt", tmp_fn);
    return tmp_filename;
}

FILE* create_temp_file(char* tmp_name) {
    FILE* tmp_ptr = fopen(tmp_name, "w");
    if (tmp_ptr == NULL) {
        return ERROR;
    }
    fprintf(tmp_ptr, "%s\n", SGN);
    return tmp_ptr;
}

int insert_by_index(FILE* fl, record rec_arr[], int ind_arr[], int new_ind, record new_record, int n) {
    char* tmp_fl = set_tmp_filename();
    FILE* tmp_f = create_temp_file(tmp_fl);
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    bool is_inserted = false;
    for (int i = 0; i < n; i++) {
        if (ind_arr[i] != new_ind) {
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", ind_arr[i], rec_arr[i].name, rec_arr[i].square, rec_arr[i].population);
        }
        else {
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", new_ind, new_record.name, new_record.square, new_record.population);
            is_inserted = true;
            for (int j = i; j < n; j++) {
                ind_arr[j] += 1;
            }
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", ind_arr[i], rec_arr[i].name, rec_arr[i].square, rec_arr[i].population);
        }
    }
    if (!is_inserted) {
        fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", new_ind, new_record.name, new_record.square, new_record.population);
    }
    fclose(fl);
    fclose(tmp_f);
    if (change_files(current_filename, tmp_fl) != 0) {
        return ERROR;
    }
    return 0;
}

int write_to_structure(int n, FILE* fl, record rec_arr[], int num_arr[]) {
    char buffer[MAX_CHARS];
    for (int i = 0; i < n; i++) {
        if (fgets(buffer, sizeof(buffer), fl) == NULL) {
            return ERROR;
        }
        if (sscanf(buffer, "%d)%10s%lf%lf", &num_arr[i], rec_arr[i].name, &rec_arr[i].square, &rec_arr[i].population) != 4) {
            return ERROR;
        }
    }
    return 0;
}

void* safe_calloc(int num_elements, size_t element_size) {
    void* ptr = calloc(num_elements, element_size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation error!\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void finish_function(record* rec_arr, int* int_arr, FILE* f_ptr) {
    free(rec_arr);
    free(int_arr);
    fclose(f_ptr);
}

int create_file() {
    char* sign = SGN;
    char filename[NAME_LEN];
    do {
        printf(BLUE"Enter file name (no spaces allowed): "RESET);
    } while (!is_input_valid(&filename, " %n%10s%c"));
    snprintf(current_filename, max_len, "%s.txt", filename);
    FILE* file = fopen(current_filename, "w");
    if (file == NULL) {
        printf(RED"Error!\n"RESET);
        return ERROR;
    }
    else {
        size_t write_sgn = fwrite(sign, sizeof(sign), 1, file);
        if (write_sgn == NULL) {
            fclose(file);
            return ERROR;
        }
    }
    printf(GREEN"File was created successfully!\n"RESET);
    fclose(file);
    return 0;
}

int read_file() {
    FILE* fl = open_file("r");
    if (fl == NULL) {
        return ERROR;
    }
    if (!has_sgn(fl)) {
        printf(RED"Sorry, I did not create this file, I don't have a right to read it\n");
        fclose(fl);
        return 0;
    }
    char buffer[MAX_CHARS];
    if (fgets(buffer, sizeof(buffer), fl) == NULL) {
        printf(GREEN"The file is empty!\n");
        fclose(fl);
        return 0;
    }
    print_header();
    while (fgets(buffer, sizeof(buffer), fl) != NULL) {
        printf("%s", buffer);
    }
    fclose(fl);
    return 0;
}

int delete_file() {
    FILE* file = open_file("r+");
    if (file == NULL) {
        return ERROR;
    }
    else {
        if (!has_sgn(file)) {
            printf(RED"Sorry, I did not create this file, I don't have a right to delete it\n");
            fclose(file);
            return 0;
        }
    }
    fclose(file);
    if (remove(current_filename) == 0) {
        printf(GREEN"File was deleted successfully!\n"RESET);
    }
    else {
        return ERROR;
    }
    return 0;
}

int create_record() {
    FILE* fl = open_file("a+");
    if (fl == NULL) {
        return ERROR;
    }
    if (!has_sgn(fl)) {
        printf(RED"Sorry, I did not create this file, I don't have a right to write in it\n");
        fclose(fl);
        return 0;
    }
    int num = 0;
    do {
        printf(BLUE"Enter the number of records you want to add (1 - %d): "RESET, MAX_REC);
    } while (!is_input_valid(&num, " %n%d%c") || !is_restriction_valid(num, 1, MAX_REC));

    record* rec = (record*)safe_calloc(num, sizeof(record));
    int ind = get_num_of_records(fl) + 1;
    for (int i = 0; i < num; i++) {
        read_data(&rec[i]);
        fseek(fl, 0, SEEK_END);
        fprintf(fl, "\n%d)\t%-15s\t%-30.10lf\t%-10.10lf", ind, rec[i].name, rec[i].square, rec[i].population);
        ind++;
    }
    fclose(fl);
    printf(GREEN"The record was successful!\n"RESET);
    free(rec);
    return 0;
}

int read_record() {
    FILE* fl = open_file("r");
    if (fl == NULL) {
        return ERROR;
    }
    if (!has_sgn(fl)) {
        printf(RED"Sorry, I did not create this file, I don't have a right to read its records\n");
        fclose(fl);
        return 0;
    }
    int ind_rec = 0, num_rec = 0;
    read_input(&ind_rec, "Enter the index of the 1st record you want to read");
    read_input(&num_rec, "Enter the number of records you want to read");
    int end_index = ind_rec + num_rec;
    int num_of_rec = get_num_of_records(fl);
    if (!is_rec_exist(end_index - 1, num_of_rec)) {
        fclose(fl);
        return ERROR;
    }
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    record* rec = (record*)safe_calloc(num_of_rec, sizeof(record));
    int* index_arr = (int*)safe_calloc(num_of_rec, sizeof(int));
    print_header();
    if (write_to_structure(num_of_rec, fl, rec, index_arr) != 0) {
        printf(RED"Error reading records!\n"RESET);
        finish_function(rec, index_arr, fl);
        return ERROR;
    }
    for (int i = ind_rec - 1; i < end_index - 1; i++) {
        printf("%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", index_arr[i], rec[i].name, rec[i].square, rec[i].population);
    }
    finish_function(rec, index_arr, fl);
    return 0;
}

int edit_record() {
    FILE* fl = open_file("r");
    if (fl == NULL) {
        return ERROR;
    }
    if (!has_sgn(fl)) {
        printf(RED"Sorry, I did not create this file, I don't have a right to edit its records\n");
        fclose(fl);
        return 0;
    }
    char* tmp_fl = set_tmp_filename();
    FILE* tmp_f = create_temp_file(tmp_fl);
    int ind_rec = 0;
    read_input(&ind_rec, "Enter the index of the record you want to edit");
    int num_of_rec = get_num_of_records(fl);
    if (!is_rec_exist(ind_rec, num_of_rec)) {
        fclose(fl);
        return ERROR;
    }
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    record* rec = (record*)safe_calloc(num_of_rec, sizeof(record));
    int* index_arr = (int*)safe_calloc(num_of_rec, sizeof(int));
    if (write_to_structure(num_of_rec, fl, rec, index_arr) != 0) {
        printf(RED"Error reading records!\n"RESET);
        finish_function(rec, index_arr, fl);
        return ERROR;
    }
    for (int i = 0; i < num_of_rec; i++) {
        if (index_arr[i] != ind_rec) {
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", index_arr[i], rec[i].name, rec[i].square, rec[i].population);
        }
        else {
            read_data(&rec[i]);
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", index_arr[i], rec[i].name, rec[i].square, rec[i].population);
        }
    }
    finish_function(rec, index_arr, fl);
    fclose(tmp_f);
    if (change_files(current_filename, tmp_fl) != 0) {
        return ERROR;
    }
    printf(GREEN"The record was successfully edited\n"RESET);
    return 0;
}

int sort_record() {
    FILE* fl = open_file("r+");
    if (fl == NULL) {
        return ERROR;
    }
    if (!has_sgn(fl)) {
        printf(RED"Sorry, I did not create this file, I don't have a right to sort its records\n");
        fclose(fl);
        return 0;
    }
    char* tmp_fl = set_tmp_filename();
    FILE* tmp_f = create_temp_file(tmp_fl);
    int num = get_num_of_records(fl);
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    record* rec = (record*)safe_calloc(num, sizeof(record));
    int* index_arr = (int*)safe_calloc(num, sizeof(int));
    if (write_to_structure(num, fl, rec, index_arr) != 0) {
        printf(RED"Error reading records!\n"RESET);
        finish_function(rec, index_arr, fl);
        return ERROR;
    }
    int parameter = read_parameter_choice();
    int sorting = read_sorting_choice();
    sort_records(rec, num, parameter, sorting);
    for (int i = 0; i < num; i++) {
        fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", index_arr[i], rec[i].name, rec[i].square, rec[i].population);
    }
    fclose(tmp_f);
    finish_function(rec, index_arr, fl);
    if (change_files(current_filename, tmp_fl) != 0) {
        return ERROR;
    }
    printf(GREEN"Records were sorted successfully!\n"RESET);
    return 0;
}

int insert_record() {
    FILE* fl = open_file("r+");
    if (fl == NULL) {
        return ERROR;
    }
    if (!has_sgn(fl)) {
        printf(RED"Sorry, I did not create this file, I don't have a right to sort its records\n");
        fclose(fl);
        return 0;
    }
    int num = get_num_of_records(fl);
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    record* rec_arr = (record*)safe_calloc(num, sizeof(record));
    int* index_arr = (int*)safe_calloc(num, sizeof(int));
    if (write_to_structure(num, fl, rec_arr, index_arr) != 0) {
        printf(RED"Error reading records!\n"RESET);
        return ERROR;
    }
    int param = read_parameter_choice();
    int sorting_method = read_sorting_choice();
    if (is_sorted(rec_arr, num, param, sorting_method)) {
        record rec;
        read_data(&rec);
        int index_of_new_rec = find_ind_to_insert(rec_arr, rec, num, param, sorting_method);
        if (index_of_new_rec == ERROR) {
            finish_function(rec_arr, index_arr, fl);
            return ERROR;
        }
        if (insert_by_index(fl, rec_arr, index_arr, index_of_new_rec, rec, num) != 0) {
            free(rec_arr);
            free(index_arr);
            return ERROR;
        }
        printf(GREEN"The record was successfully inserted!\n"RESET);
    }
    finish_function(rec_arr, index_arr, fl);
    return 0;
}

int delete_record() {
    FILE* fl = open_file("r+");
    if (fl == NULL) {
        return ERROR;
    }
    if (!has_sgn(fl)) {
        printf(RED"Sorry, I did not create this file, I don't have a right to delete its records\n");
        fclose(fl);
        return 0;
    }

    int index = 0;
    read_input(&index, "Enter the index of the record you want to delete");
    int num = get_num_of_records(fl);
    if (!is_rec_exist(index, num)) {
        fclose(fl);
        return ERROR;
    }
    char* tmp_fl = set_tmp_filename();
    FILE* tmp_f = create_temp_file(tmp_fl);
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    int new_index = 1;
    record* rec_arr = (record*)safe_calloc(num, sizeof(record));
    int* index_arr = (int*)safe_calloc(num, sizeof(int));
    if (write_to_structure(num, fl, rec_arr, index_arr) != 0) {
        printf(RED"Error reading records!\n"RESET);
        finish_function(rec_arr, index_arr, fl);
        return ERROR;
    }
    for (int i = 0; i < num; i++) {
        if (index_arr[i] != index) {
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", new_index, rec_arr[i].name, rec_arr[i].square, rec_arr[i].population);
            new_index++;
        }
    }
    finish_function(rec_arr, index_arr, fl);
    fclose(tmp_f);
    if (change_files(current_filename, tmp_fl) != 0) {
        return ERROR;
    }
    printf(GREEN"The record was successfully deleted\n"RESET);
    return 0;
}

void record_worker(int ch) {
    switch (ch) {
    case CREATE_RECORD:
        if (create_record() == ERROR) {
            printf(RED"The record can not be made\n"RESET);
        }
        break;
    case READ_RECORD:
        if (read_record() == ERROR) {
            printf(RED"The record can not be read\n"RESET);
        }
        break;
    case EDIT_RECORD:
        if (edit_record() == ERROR) {
            printf(RED"The record can not be edited\n"RESET);
        }
        break;
    case SORT_RECORD:
        if (sort_record() == ERROR) {
            printf(RED"The records can not be sorted\n"RESET);
        }
        break;
    case INSERT_RECORD:
        if (insert_record() == ERROR) {
            printf(RED"Error while inserting record\n"RESET);
        }
        break;
    case DELETE_RECORD:
        if (delete_record() == ERROR) {
            printf(RED"The record can not be deleted\n"RESET);
        }
        break;
    default:
        printf(RED"Invalid option\n"RESET);
    }
}

void file_worker(int ch) {
    switch (ch) {
    case CREATE_FILE:
        if (create_file() == ERROR) {
            printf(RED"File creation error!\n"RESET);
        }
        break;
    case READ_FILE:
        if (read_file() == ERROR) {
            printf(RED"The file can not be read\n"RESET);
        }
        break;
    case DELETE_FILE:
        if (delete_file() == ERROR) {
            printf(RED"The file can not be deleted or does not exist at all\n"RESET);
        }
        break;
    default:
        printf(RED"Invalid option\n"RESET);
    }
}

void worker(int ch) {
    int option = 0;
    switch (ch) {
    case FILE_MENU:
        option = file_menu();
        file_worker(option);
        break;
    case RECORD:
        if ((strlen(current_filename) != 0) && is_exist(current_filename)) {
            option = record_menu();
            record_worker(option);
        }
        else {
            system("cls");
            printf(RED"Please, open the file first\n"RESET);
        }
        break;
    default:
        printf(RED"Invalid option\n"RESET);
    }
}

int main() {
    do {
        int choice = main_menu();
        worker(choice);
    } while (!is_esc());
    return 0;
}
