#pragma once

#define CURL_STATICLIB
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <curl/curl.h>
#include <errno.h>
#include <json-c/json.h>
#include <math.h>

//-----------------------------------------------------------
// 常量定义
#define KEY       "a5a0b276466e253c0ab0ea5ed527fdeb"  // 这里填入你的Key
#define SIZE      512   // URL大小
#define STR       4096  // 字符串大小  

//-----------------------------------------------------------
// **枚举类型定义**
typedef enum { SMALL, MEDIUM, LARGE, NONESIZE } PackageSize;
typedef enum { NORMAL, FRAGILE, FRESH, VALUABLE, DANGEROUS, NONETYPE } PackageType;
typedef enum { STORED, OUTBOUND, DELAY, LOST, STOLEN, REJECTED, DAMAGED, IN_TRANSIT, NONESTATUS, RETURNED } PackageStatus;
typedef enum { STUDENT, VIP, NORMALPEO, STAFF, ENTERPRISE } AccountType;
typedef enum { ZONE_1, ZONE_2, ZONE_3, ZONE_4 } ZoneType;

//-----------------------------------------------------------
// **结构体定义**
typedef struct {
    char account[50];
    char name[50];
    char gender[10];
    char idCard[20];
    char contact[50];
    char address[100];
    char password[50];
    AccountType accountType;
} PersonInfo;

typedef struct ParcelNode {
    char tracking_num[20];
    char company[50];
    PackageType type;
    PackageSize size;
    float weight;
    float price;
    PersonInfo sender;
    PersonInfo receiver;
    PersonInfo collector;
    char pickup_code[6];
	char send_address[4096];
    char address[4096];
    time_t send_time;
    time_t store_time;
    time_t pickup_time;
    PackageStatus status;
    char shelf_id[10];
    int rating;
    int reminder_sent;
    struct ParcelNode* next;
} ParcelNode;

typedef struct {
    char company[50];
    float base_price;
    float weight_rate;
    float package_rate[5];
    float size_rate[3];
    float account_discount[5];
    float zone_rate[4];
} PriceRule;

typedef struct {
    int target_status;
    char sender_keyword[50];
    char receiver_keyword[50];
    char collector_keyword[50];
} ParcelFilter;

typedef struct POIInfo {
    char name[STR];
    char address[STR];
    char location[STR];
    char province[STR];
    char city[STR];
    char district[STR];
    char type[STR];
    struct POIInfo* next;
} POIInfo;

typedef struct {
    char* memory;
    size_t size;
} Data;

//-----------------------------------------------------------
// **C 兼容的函数声明**
#ifdef __cplusplus
extern "C" {
    #endif

    void generate_tracking_num(char* num, int size);
    ParcelNode* create_parcel();
    void insert_sorted(ParcelNode** head, ParcelNode* new_node);
    void delete_parcel(ParcelNode** head, char* tracking_num);
    ParcelNode* searchbytracking_num(ParcelNode* head, char* tracking_num);
    void update_parcel(ParcelNode* node);
    void display_parcel(ParcelNode* node);
    void save_to_txt(const char* filename);
    void load_from_txt(const char* filename);
    void clear_input_buffer();
    void time_to_str(time_t t, char* buf, size_t size);
    time_t str_to_time(const char* str);
    void setBufferState(ParcelNode* new_node);
    ParcelNode* create_parcel_without_state();
    char* get_time();
    void initialize_price_rule(PriceRule* rule, const char* company, float base_price, float weight_rate,
        const float* package_rate, const float* size_rate,
        const float* account_discount, const float* zone_rate);
    float calculate_price(ParcelNode* parcel, PriceRule* rule, int zone_choice);
    int validate_time_format(const char* time_str);
    PriceRule* find_rule_by_name(const char* name);
    void initialize_missing_data(ParcelNode* node);
    const char* get_package_size_str(PackageSize size);
    const char* get_package_type_str(PackageType type);
    const char* get_package_status_str(PackageStatus status);
    char* strcasestr(const char* haystack, const char* needle);
    void batch_search_by_numbers(ParcelNode* head, const char* input);
    ParcelNode** search_parcels(ParcelNode* head, ParcelFilter filter, int* count);
    void my_position(ParcelNode* node);
    char* ConvertToUTF8(const char* input);
    char* ConvertToGBK(const char* utf8_str);
    POIInfo* CreatePOINode();
    void AppendToLinkedList(POIInfo** head, POIInfo* new_node);
    size_t got_data(char* buffer, size_t size, size_t nmemb, void* userp);
    void ParsePOIData(json_object* json, POIInfo** head);
    void PrintLinkedList(const POIInfo* head);
    void FreeLinkedList(POIInfo** head);
    POIInfo* FindPOI(const POIInfo* head, const char* name);

    #ifdef __cplusplus
}
#endif

//-----------------------------------------------------------
// **全局变量声明**
extern ParcelNode* parcel_list;
extern PriceRule rule;
extern PriceRule price_table[10];
extern int price_count;
