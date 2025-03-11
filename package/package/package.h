//package.h
#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>

//-----------------------------------------------------------
//属性定义
typedef enum {
	SMALL, MEDIUM, LARGE, NONESIZE
} PackageSize;

typedef enum {
	NORMAL, FRAGILE, FRESH, VALUABLE, DANGEROUS, NONETYPE
} PackageType;


typedef enum {
	STORED, OUT, DELAY, LOST, STOLEN, REJECTED, DAMAGED, IN_TRANSIT, NONESTATUS, RETURNED
} PackageStatus;

// 定义账号类型枚举
typedef enum {
    STUDENT,  // 学生特惠用户
    VIP,      // VIP用户
    NORMALPEO,   // 普通个人用户
    STAFF,    // 教职工
    ENTERPRISE // 企业协议用户
} AccountType;

// 定义用户结构体
typedef struct {
    char account[50];       // 账号
    char name[50];          // 名称
    char gender[10];        // 性别
    char idCard[20];        // 身份证号
    char contact[50];       // 联系方式
    char address[100];      // 常用地址
    char password[50];      // 密码
    AccountType accountType;// 账号类型
} PersonInfo;

typedef enum { ZONE_1, ZONE_2, ZONE_3, ZONE_4 } ZoneType;

typedef struct ParcelNode {
    char tracking_num[20];//单号
    char company[50];//物流公司
    PackageType type;//包裹类型
    PackageSize size;//大小
    float weight;//重量
    float price;//价格
    PersonInfo sender;//寄件人
    PersonInfo receiver;//收件人
    PersonInfo collector;//取件人
    char pickup_code[6];//取件码
    time_t send_time;//寄件时间
    time_t store_time;//入库时间
    time_t pickup_time;//取件时间
    PackageStatus status;//状态
    char shelf_id[10];//货架编号
    int rating;//满意度评价（满分10分）
    int reminder_sent;//是否发送提醒
    struct ParcelNode* next;
} ParcelNode;

//计费规则
typedef struct {
    char company[50];
    float base_price;        // 基础费用
    float weight_rate;       // 每公斤收费
    float package_rate[5];   // 包裹类型费用
    float size_rate[3];      // 尺寸收费
    float account_discount[5]; // 账号类型折扣
    float zone_rate[4];      // **按地区收费：省内、邻省、远程、偏远地区**
} PriceRule;



//-----------------------------------------------------------
// 函数声明
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


extern ParcelNode* parcel_list;  // 声明全局链表!!
extern PriceRule rule;
extern PriceRule price_table[10];
extern int price_count;

// 自定义断言宏，用于检查指针是否为空
#define ASSERT_NOT_NULL(ptr) do {                                       \
    if ((ptr) == NULL) {                                                \
        fprintf(stderr, "ERROR: Pointer %s is NULL\n", #ptr);           \
        fprintf(stderr, "  Location: File %s, Function %s, Line %d\n",  \
                __FILE__, __func__, __LINE__);                          \
        fflush(stderr); /* 确保错误信息立即输出 */                      \
        exit(EXIT_FAILURE); /* 以失败状态退出程序，避免abort的默认错误信息 */  \
    }                                                                   \
} while (0)