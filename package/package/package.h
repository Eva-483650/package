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
#define DELAY_THRESHOLD_SEC 172800 // 48小时后触发滞留状态

//-----------------------------------------------------------
// **枚举类型定义**
typedef enum { SMALL, MEDIUM, LARGE, NONESIZE } PackageSize;
typedef enum { NORMAL, FRAGILE, FRESH, VALUABLE, DANGEROUS, NONETYPE } PackageType;
typedef enum { STORED, OUTBOUND, DELAY, LOST, STOLEN, REJECTED, DAMAGED, IN_TRANSIT, NONESTATUS, RETURNED, WAITING_PICKUP} PackageStatus;
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
    char promo_code[20];    // 使用的优惠码/
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

// 优惠类型枚举（保持不变）
typedef enum {
    DISCOUNT,      // 打折
    COUPON,        // 代金券
    FULL_REDUCTION // 满减
} PromotionType;

// 优惠活动结构体定义
typedef struct {
    char code[20];          // 优惠码（用户输入的字符串，如 "DOUBLE11"）
    PromotionType type;     // 优惠类型（枚举值：DISCOUNT=打折，COUPON=代金券，FULL_REDUCTION=满减）
    float discount_rate;    // 折扣率（仅当 type=DISCOUNT 时有效，例如 0.9 表示 9 折）
    float coupon_amount;    // 代金券金额（仅当 type=COUPON 时有效，例如 15 表示立减 15 元）
    float full_amount;      // 满减条件金额（仅当 type=FULL_REDUCTION 时有效，例如 100 表示满 100 元）
    float reduction_amount; // 满减减免金额（仅当 type=FULL_REDUCTION 时有效，例如 20 表示减 20 元）
    time_t start_time;      // 活动开始时间（Unix 时间戳，通过 str_to_time 函数生成）
    time_t end_time;        // 活动结束时间（Unix 时间戳，活动仅在 [start_time, end_time] 内有效）
} Promotion;

// 定义优惠活动链表节点
typedef struct PromotionNode {
    Promotion promotion;          // 活动数据
    struct PromotionNode* next;   // 指向下一个节点
} PromotionNode;

//-----------------------------------------------------------
// **C 兼容的函数声明**
#ifdef __cplusplus
extern "C" {
    #endif

	void generate_tracking_num(char* num, int size);//生成快递单号
	int is_valid_tracking(const char* num);//验证快递单号
	ParcelNode* create_parcel();//创建包裹节点
	void insert_sorted(ParcelNode** head, ParcelNode* new_node);//按顺序插入链表
	void delete_parcel(ParcelNode** head, char* tracking_num);//删除包裹
	ParcelNode* searchbytracking_num(ParcelNode* head, char* tracking_num);//按单号查找包裹
	void update_parcel(ParcelNode* node);//更新包裹信息
	void display_parcel(ParcelNode* node);//显示包裹信息
	void save_to_txt(const char* filename);//保存到文件
	void load_from_txt(const char* filename);//从文件加载
	void clear_input_buffer();//清空输入缓冲区
	void time_to_str(time_t t, char* buf, size_t size);//时间转字符串
	time_t str_to_time(const char* str);//字符串转时间
	void setBufferState(ParcelNode* new_node);//设置包裹状态
	ParcelNode* create_parcel_without_state();//创建包裹节点（不包含状态）
	char* get_time();//获取当前时间
    void initialize_price_rule(PriceRule* rule, const char* company, float base_price, float weight_rate,
        const float* package_rate, const float* size_rate,
		const float* account_discount, const float* zone_rate);//初始化计费规则
	float calculate_price(ParcelNode* parcel, PriceRule* rule, int zone_choice);//计算价格
	int validate_time_format(const char* time_str);//验证时间格式
	PriceRule* find_rule_by_name(const char* name);//根据公司名称查找计费规则
	void initialize_missing_data(ParcelNode* node);//初始化缺失数据
	const char* get_package_size_str(PackageSize size);//获取包裹尺寸字符串
	const char* get_package_type_str(PackageType type);//获取包裹类型字符串
	const char* get_package_status_str(PackageStatus status);//获取包裹状态字符串
	char* strcasestr(const char* haystack, const char* needle);//忽略大小写的字符串查找
    void batch_search_by_numbers(ParcelNode* head, const char* input);
    ParcelNode** search_parcels(ParcelNode* head, ParcelFilter filter, int* count);
    void my_position(ParcelNode* node, int choice);
    char* ConvertToUTF8(const char* input);
    char* ConvertToGBK(const char* utf8_str);
    POIInfo* CreatePOINode();
    void AppendToLinkedList(POIInfo** head, POIInfo* new_node);
    size_t got_data(char* buffer, size_t size, size_t nmemb, void* userp);
    void ParsePOIData(json_object* json, POIInfo** head);
    void PrintLinkedList(const POIInfo* head);
    void FreeLinkedList(POIInfo** head);
    POIInfo* FindPOI(const POIInfo* head, const char* name);
    //优惠有关函数
    void add_promotion(Promotion p); // 添加优惠活动
    void delete_promotion(const char* code); // 删除优惠活动
    Promotion* find_promotion(const char* code); // 查找优惠活动
    void modify_promotion(const char* code); // 修改优惠活动
    void display_promotions(); // 显示所有优惠
    void save_promotions_to_file(const char* filename); // 保存到文件
    void search_promotion();
	void initialize_promotions(const char* filename); // 从文件初始化优惠活动
    float apply_promotions(float price, const char* promo_code);
    ParcelNode* create_parcel_without_send_address();
    int is_parcel_delayed(ParcelNode* p);//判断是否滞留
	void auto_update_status(ParcelNode* p);//自动更新状态
	void sanitize_field(char* str);//清理字符串


    #ifdef __cplusplus
}
#endif

//-----------------------------------------------------------
// **全局变量声明**
extern ParcelNode* parcel_list;
extern PriceRule rule;
extern PriceRule price_table[10];
extern int price_count;
extern PromotionNode* promotion_list;

