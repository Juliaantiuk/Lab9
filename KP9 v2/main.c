#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#define RED "\x1b[31m"
#define BLUE "\x1b[36m"
#define RESET "\x1b[0m"
#define GREEN "\033[0;32m"
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
size_t max_len = NAME_LEN + EXTENSION_LEN;
enum menu_opt {
    CREATE_FILE = 1, READ_FILE, DELETE_FILE, CREATE_RECORD, READ_RECORD, EDIT_RECORD, SORT_RECORD, INSERT_RECORD, DELETE_RECORD
};

typedef struct {
    char name[NAME_LEN];
    double square;
    double population;
} record;

bool is_esc() {
    printf(BLUE"Press ESC key to exit, or any other key to do something else: "RESET);
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

int menu() {
    int option = 0;
    printf(BLUE"Hello, dear user! This is the program for working with files\n\n"RESET);
    do {
        printf(BLUE"Choose an option:\n\n");
        printf("%d. Create a file\n", CREATE_FILE);
        printf("%d. Read a file\n", READ_FILE);
        printf("%d. Delete a file\n", DELETE_FILE);
        printf("%d. Create a record in the existing file\n", CREATE_RECORD);
        printf("%d. Read a record from the existing file\n", READ_RECORD);
        printf("%d. Edit a record\n", EDIT_RECORD);
        printf("%d. Sort records\n", SORT_RECORD);
        printf("%d. Insert in record\n", INSERT_RECORD);
        printf("%d. Delete a record\n\n"RESET, DELETE_RECORD);
    } while (!is_input_valid(&option, " %n%d%c") || !is_restriction_valid(option, CREATE_FILE, DELETE_RECORD));
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

void sort_population(record arr[], int num, int ch) {
    switch (ch) {
    case A_CODE:
        for (int i = 0; i < num - 1; i++) {
            for (int j = 0; j < num - i - 1; j++) {
                if (arr[j].population > arr[j + 1].population) {
                    swap(&arr[j], &arr[j + 1]);
                }
            }
        }
        break;
    case D_CODE:
        for (int i = 0; i < num - 1; i++) {
            for (int j = 0; j < num - i - 1; j++) {
                if (arr[j].population < arr[j + 1].population) {
                    swap(&arr[j], &arr[j + 1]);
                }
            }
        }
        break;
    default:
        printf(RED"Error! Can not sort records\n"RESET);
    }
}

void sort_square(record arr[], int num, int ch) {
    switch (ch) {
    case A_CODE:
        for (int i = 0; i < num - 1; i++) {
            for (int j = 0; j < num - i - 1; j++) {
                if (arr[j].square > arr[j + 1].square) {
                    swap(&arr[j], &arr[j + 1]);
                }
            }
        }
        break;
    case D_CODE:
        for (int i = 0; i < num - 1; i++) {
            for (int j = 0; j < num - i - 1; j++) {
                if (arr[j].square < arr[j + 1].square) {
                    swap(&arr[j], &arr[j + 1]);
                }
            }
        }
        break;
    default:
        printf(RED"Error! Can not sort records\n"RESET);
    }
}

void sort_region(record arr[], int num, int ch) {
    switch (ch) {
    case A_CODE:
        for (int i = 0; i < num - 1; i++) {
            for (int j = 0; j < num - i - 1; j++) {
                if (strcmp(arr[j].name, arr[j + 1].name) > 0) {
                    swap(&arr[j], &arr[j + 1]);
                }
            }
        }
        break;
    case D_CODE:
        for (int i = 0; i < num - 1; i++) {
            for (int j = 0; j < num - i - 1; j++) {
                if (strcmp(arr[j].name, arr[j + 1].name) < 0) {
                    swap(&arr[j], &arr[j + 1]);
                }
            }
        }
        break;
    default:
        printf(RED"Error! Can not sort records\n"RESET);
    }
}

void sort_records(record arr[], int n, int par_choice, int sort_choice) {
    switch (par_choice) {
    case R_CODE:
        sort_region(arr, n, sort_choice);
        break;
    case S_CODE:
        sort_square(arr, n, sort_choice);
        break;
    case P_CODE:
        sort_population(arr, n, sort_choice);
        break;
    default:
        printf(RED"Error while sorting!"RESET);
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

    return num - 1;
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

bool is_sorted_by_region_descending(record records[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (strcmp(records[i].name, records[i + 1].name) < 0) {
            printf(RED"The records are not sorted in descending order by name\n"RESET);
            return false;
        }
    }
    printf(GREEN"The records are sorted in descending order by name\n"RESET);
    return true;
}

bool is_sorted_by_region_ascending(record records[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (strcmp(records[i].name, records[i + 1].name) > 0) {
            printf(RED"The records are not sorted in ascending order by name\n"RESET);
            return false;
        }
    }
    printf(GREEN"The records are sorted in ascending order by name\n"RESET);
    return true;
}

bool is_sorted_by_square_descending(record records[], int n) {
    for (int i = 1; i < n - 1; i++) {
        if (records[i].square < records[i + 1].square) {
            printf(RED"The records are not sorted in descending order by square\n"RESET);
            return false;
        }
    }
    printf(GREEN"The records are sorted in descending order by square\n"RESET);
    return true;
}

bool is_sorted_by_square_ascending(record records[], int n) {
    for (int i = 1; i < n - 1; i++) {
        if (records[i].square > records[i + 1].square) {
            printf(RED"The records are not sorted in ascending order by square\n"RESET);
            return false;
        }
    }
    printf(GREEN"The records are sorted in ascending order by square\n"RESET);
    return true;
}

bool is_sorted_by_population_descending(record records[], int n) {
    for (int i = 1; i < n - 1; i++) {
        if (records[i].population < records[i + 1].population) {
            printf(GREEN"The records are not sorted in descending order by population\n"RESET);
            return false;
        }
    }
    printf(GREEN"The records are sorted in descending order by population\n"RESET);
    return true;
}

bool is_sorted_by_population_ascending(record records[], int n) {
    for (int i = 1; i < n - 1; i++) {
        if (records[i].population > records[i + 1].population) {
            printf(GREEN"The records are not sorted in ascending order by population\n"RESET);
            return false;
        }
    }
    printf(GREEN"The records are sorted in ascending order by population\n"RESET);
    return true;
}

bool is_sorted(record records[], int n, int parameter, int method) {
    switch (parameter) {
    case R_CODE:
        if (method == A_CODE) {
            return(is_sorted_by_region_ascending(records, n));
        }
        else if (method == D_CODE) {
            return(is_sorted_by_region_descending(records, n));
        }
        else {
            printf(RED"An error happened!\n");
            return false;
        }
    case S_CODE:
        if (method == A_CODE) {
            return(is_sorted_by_square_ascending(records, n));
        }
        else if (method == D_CODE) {
            return(is_sorted_by_square_descending(records, n));
        }
        else {
            printf(RED"An error happened!\n");
            return false;
        }
    case P_CODE:
        if (method == A_CODE) {
            return(is_sorted_by_population_ascending(records, n));
        }
        else if (method == D_CODE) {
            return(is_sorted_by_population_descending(records, n));
        }
        else {
            printf(RED"An error happened!\n");
            return false;
        }
    default:
        printf(RED"An error happened while inserting!\n"RESET);
        return false;
    }
}

int find_ind_insert_by_region(record records[], record new_record, int n, int parameter) {
    int new_index = 0;
    switch (parameter)
    {
    case A_CODE:
        for (int i = 0; i < n; i++) {
            if (strcmp(records[i].name, new_record.name) < 0) {
                new_index++;
            }
        }
        break;
    case D_CODE:
        for (int i = 0; i < n; i++) {
            if (strcmp(records[i].name, new_record.name) > 0) {
                new_index++;
            }
        }
        break;
    default:
        printf(RED"Error inserting records!\n"RESET);
        return ERROR;
    }
    return new_index + 1;
}

int find_ind_insert_by_square(record records[], record new_record, int n, int parameter) {
    int new_index = 0;
    switch (parameter)
    {
    case A_CODE:
        for (int i = 0; i < n; i++) {
            if (records[i].square < new_record.square) {
                new_index++;
            }
        }
        break;
    case D_CODE:
        for (int i = 0; i < n; i++) {
            if (records[i].square > new_record.square) {
                new_index++;
            }
        }
        break;
    default:
        printf(RED"Error inserting records!\n"RESET);
        return ERROR;
    }
    return new_index + 1;
}

int find_ind_insert_by_population(record records[], record new_record, int n, int parameter) {
    int new_index = 0;
    switch (parameter)
    {
    case A_CODE:
        for (int i = 0; i < n; i++) {
            if (records[i].population < new_record.population) {
                new_index++;
            }
        }
        break;
    case D_CODE:
        for (int i = 0; i < n; i++) {
            if (records[i].population > new_record.population) {
                new_index++;
            }
        }
        break;
    default:
        printf(RED"Error inserting records!\n"RESET);
        return ERROR;
    }
    return new_index + 1;
}

int insert_by_index(FILE* fl, record rec_arr[], int ind_arr[], int new_ind, record new_record, int n) {
    char* tmp_fn = "tmp";
    char tmp_fl[NAME_LEN + 4];
    sprintf(tmp_fl, "%s.txt", tmp_fn);
    FILE* tmp_f = fopen(tmp_fl, "w");
    if (tmp_f == NULL) {
        return ERROR;
    }
    fprintf(tmp_f, "%s\n", SGN);
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    for (int i = 0; i < n; i++) {
        if (ind_arr[i] != new_ind) {
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", ind_arr[i], rec_arr[i].name, rec_arr[i].square, rec_arr[i].population);
        }
        else {
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", new_ind, new_record.name, new_record.square, new_record.population);
            for (int j = i; j < n; j++) {
                ind_arr[j] += 1;
            }
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", ind_arr[i], rec_arr[i].name, rec_arr[i].square, rec_arr[i].population);
        }
    }
    fclose(fl);
    fclose(tmp_f);
    if (change_files(current_filename, tmp_fl) != 0) {
        return ERROR;
    }
    return 0;
}

FILE* create_temp_file(char* tmp_name) {
    FILE* tmp_ptr = fopen(tmp_name, "w");
    if (tmp_ptr == NULL) {
        return ERROR;
    }
    fprintf(tmp_ptr, "%s\n", SGN);
    return tmp_ptr;
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
    int index = 0;
    if (fgets(buffer, sizeof(buffer), fl) == NULL) {
        printf(GREEN"The file is empty!\n");
        fclose(fl);
        return 0;
    }
    print_header();
    while (fgets(buffer, sizeof(buffer), fl) != NULL) {
        record rec;
        if (sscanf(buffer, "%d)\t%s\t%lf\t%lf", &index, rec.name, &rec.square, &rec.population) == 4) {
            printf("%d) ", index);
            printf("   %-15s%-30.10lf%-10.10lf\n", rec.name, rec.square, rec.population);
        }
        else {
            printf(RED"Error reading file content.\n"RESET);
            fclose(fl);
            return ERROR;
        }
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
    int start_ind = get_num_of_records(fl) + 1;
    int end_ind = start_ind + num;

    for (int i = start_ind; i < end_ind; i++) {
        read_data(&rec[i]);
        fseek(fl, 0, SEEK_END);
        fprintf(fl, "\n%d)\t%-15s\t%-30.10lf\t%-10.10lf", i + 1, rec[i].name, rec[i].square, rec[i].population);
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
    do { 
        printf(BLUE"Enter the index of the 1st record you want to read: "RESET);
    } while (!is_input_valid(&ind_rec, " %n%d%c") || !is_recordnum_valid(ind_rec));
    do {
        printf(BLUE"Enter the number of records you want to read: "RESET);
    } while (!is_input_valid(&num_rec, " %n%d%c") || !is_recordnum_valid(num_rec));
    char buffer[MAX_CHARS];
    int end_index = ind_rec + num_rec;
    int num_of_rec = get_num_of_records(fl);
    if (!is_rec_exist(end_index, num_of_rec)) {
        return ERROR;
    }
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    record rec;
    int current_rec = ind_rec, index = 0;
    print_header();
    while (fgets(buffer, sizeof(buffer), fl) != NULL && current_rec < end_index) {
        if (sscanf(buffer, "%d)%10s%lf%lf", &index, rec.name, &rec.square, &rec.population) == 4) {
            if (index == current_rec) {
                printf("%d) ", index);
                printf("   %-15s%-30.10lf%-10.10lf\n", rec.name, rec.square, rec.population);
                current_rec++;
            }
        }
        else {
            printf(RED"Error reading file content.\n"RESET);
            fclose(fl);
            return ERROR;
        }
    }
    fclose(fl);
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
    char* tmp_fn = "tmp";
    char tmp_fl[NAME_LEN + 4];
    sprintf(tmp_fl, "%s.txt", tmp_fn);
    FILE* tmp_f = create_temp_file(tmp_fl);
    int ind_rec = 0;
    do {
        printf(BLUE"Enter the index of the record you want to edit: "RESET);
    } while (!is_input_valid(&ind_rec, " %n%d%c") || !is_recordnum_valid(ind_rec));
    int total = get_num_of_records(fl);
    if (!is_rec_exist(ind_rec, total)) {
        return ERROR;
    }
    int current_record = 1;
    char buffer[MAX_CHARS];
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    while (fgets(buffer, sizeof(buffer), fl) != NULL) {
        if (current_record != ind_rec) {
            fprintf(tmp_f, "%s", buffer);
        }
        else {
            record rec;
            read_data(&rec);
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", current_record, rec.name, rec.square, rec.population);
        }
        current_record++;
    }
    fclose(fl);
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
    char* tmp_fn = "tmp";
    char tmp_fl[NAME_LEN + 4];
    sprintf(tmp_fl, "%s.txt", tmp_fn);
    FILE* tmp_f = create_temp_file(tmp_fl);
    int num = get_num_of_records(fl);
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    record* rec = (record*)safe_calloc(num, sizeof(record));
    int* index_arr = (int*)safe_calloc(num, sizeof(int));
    if (write_to_structure(num, fl, rec, index_arr) != 0) {
        printf(RED"Error reading records!\n"RESET);
    }
    int parameter = read_parameter_choice();
    int sorting = read_sorting_choice();
    sort_records(rec, num, parameter, sorting);
    for (int i = 0; i < num; i++) {
        fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", index_arr[i], rec[i].name, rec[i].square, rec[i].population);
    }
    free(rec);
    free(index_arr);
    fclose(fl);
    fclose(tmp_f);
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
    }
    int param = read_parameter_choice();
    int sorting_method = read_sorting_choice();
    if (is_sorted(rec_arr, num, param, sorting_method)) {
        record rec;
        read_data(&rec);
        int index_of_new_rec = 0;
        switch (param) {
        case R_CODE:
            index_of_new_rec = find_ind_insert_by_region(rec_arr, rec, num, sorting_method);
            break;
        case S_CODE:
            index_of_new_rec = find_ind_insert_by_square(rec_arr, rec, num, sorting_method);
            break;
        case P_CODE:
            index_of_new_rec = find_ind_insert_by_population(rec_arr, rec, num, sorting_method);
            break;
        default:
            return ERROR;
        }
        if (insert_by_index(fl, rec_arr, index_arr, index_of_new_rec, rec, num) != 0) {
            fclose(fl);
            return ERROR;
        }
    }
    else {
        printf(BLUE"Please, come back to the menu and sort the records in some way!\n"RESET);
        fclose(fl);
        return 0;
    }
    printf(GREEN"The record was successfully inserted!\n"RESET);
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
    do {
        printf(BLUE"Enter the index of the record you want to delete: "RESET);
    } while (!is_input_valid(&index, " %n%d%c") || !is_recordnum_valid(index));
    int num = get_num_of_records(fl);
    if (!is_rec_exist(index, num)) {
        fclose(fl);
        return ERROR;
    }
    char* tmp_fn = "tmp";
    char tmp_fl[NAME_LEN + 4];
    sprintf(tmp_fl, "%s.txt", tmp_fn);
    FILE* tmp_f = create_temp_file(tmp_fl);
    fseek(fl, SGN_LEN + 2, SEEK_SET);
    int new_index = 1;
    record* rec_arr = (record*)safe_calloc(num, sizeof(record));
    int* index_arr = (int*)safe_calloc(num, sizeof(int));
    if (write_to_structure(num, fl, rec_arr, index_arr) != 0) {
        printf(RED"Error reading records!\n"RESET);
    }
    for (int i = 0; i < num; i++) {
        if (index_arr[i] != index) {
            fprintf(tmp_f, "%d)\t%-15s\t%-30.10lf\t%-10.10lf\n", new_index, rec_arr[i].name, rec_arr[i].square, rec_arr[i].population);
            new_index++;
        }
    }
    free(rec_arr);
    free(index_arr);
    fclose(fl);
    fclose(tmp_f);
    if (change_files(current_filename, tmp_fl) != 0) {
        return ERROR;
    }
    printf(GREEN"The record was successfully deleted\n"RESET);
    return 0;
}

void worker(int handler, int ch) {
    switch (ch) {
    case CREATE_FILE:
        handler = create_file();
        if (handler == ERROR) {
            printf(RED"File creation error!\n"RESET);
        }
        break;
    case READ_FILE:
        handler = read_file();
        if (handler == ERROR) {
            printf(RED"The file can not be read\n"RESET);
        }
        break;
    case DELETE_FILE:
        handler = delete_file();
        if (handler == ERROR) {
            printf(RED"The file can not be deleted or does not exist at all\n"RESET);
        }
        break;
    case CREATE_RECORD:
        handler = create_record();
        if (handler == ERROR) {
            printf(RED"The record can not be made\n"RESET);
        }
        break;
    case READ_RECORD:
        handler = read_record();
        if (handler == ERROR) {
            printf(RED"The record can not be read\n"RESET);
        }
        break;
    case EDIT_RECORD:
        handler = edit_record();
        if (handler == ERROR) {
            printf(RED"The record can not be edited\n"RESET);
        }
        break;
    case SORT_RECORD:
        handler = sort_record();
        if (handler == ERROR) {
            printf(RED"The records can not be sorted\n"RESET);
        }
        break;
    case INSERT_RECORD:
        handler = insert_record();
        if (handler == ERROR) {
            printf(RED"Error while inserting record\n"RESET);
        }
        break;
    case DELETE_RECORD:
        handler = delete_record();
        if (handler == ERROR) {
            printf(RED"The record can not be deleted\n"RESET);
        }
        break;
    default:
        printf(RED"Invalid option\n"RESET);
    }
}

int main() {
    int status_code = 0;
    do {
        int choice = menu();
        worker(status_code, choice);
    } while (!is_esc());
    return 0;
}
