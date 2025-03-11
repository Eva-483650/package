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
//���Զ���
typedef enum {
	SMALL, MEDIUM, LARGE, NONESIZE
} PackageSize;

typedef enum {
	NORMAL, FRAGILE, FRESH, VALUABLE, DANGEROUS, NONETYPE
} PackageType;


typedef enum {
	STORED, OUT, DELAY, LOST, STOLEN, REJECTED, DAMAGED, IN_TRANSIT, NONESTATUS, RETURNED
} PackageStatus;

// �����˺�����ö��
typedef enum {
    STUDENT,  // ѧ���ػ��û�
    VIP,      // VIP�û�
    NORMALPEO,   // ��ͨ�����û�
    STAFF,    // ��ְ��
    ENTERPRISE // ��ҵЭ���û�
} AccountType;

// �����û��ṹ��
typedef struct {
    char account[50];       // �˺�
    char name[50];          // ����
    char gender[10];        // �Ա�
    char idCard[20];        // ���֤��
    char contact[50];       // ��ϵ��ʽ
    char address[100];      // ���õ�ַ
    char password[50];      // ����
    AccountType accountType;// �˺�����
} PersonInfo;

typedef enum { ZONE_1, ZONE_2, ZONE_3, ZONE_4 } ZoneType;

typedef struct ParcelNode {
    char tracking_num[20];//����
    char company[50];//������˾
    PackageType type;//��������
    PackageSize size;//��С
    float weight;//����
    float price;//�۸�
    PersonInfo sender;//�ļ���
    PersonInfo receiver;//�ռ���
    PersonInfo collector;//ȡ����
    char pickup_code[6];//ȡ����
    time_t send_time;//�ļ�ʱ��
    time_t store_time;//���ʱ��
    time_t pickup_time;//ȡ��ʱ��
    PackageStatus status;//״̬
    char shelf_id[10];//���ܱ��
    int rating;//��������ۣ�����10�֣�
    int reminder_sent;//�Ƿ�������
    struct ParcelNode* next;
} ParcelNode;

//�Ʒѹ���
typedef struct {
    char company[50];
    float base_price;        // ��������
    float weight_rate;       // ÿ�����շ�
    float package_rate[5];   // �������ͷ���
    float size_rate[3];      // �ߴ��շ�
    float account_discount[5]; // �˺������ۿ�
    float zone_rate[4];      // **�������շѣ�ʡ�ڡ���ʡ��Զ�̡�ƫԶ����**
} PriceRule;



//-----------------------------------------------------------
// ��������
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


extern ParcelNode* parcel_list;  // ����ȫ������!!
extern PriceRule rule;
extern PriceRule price_table[10];
extern int price_count;

// �Զ�����Ժ꣬���ڼ��ָ���Ƿ�Ϊ��
#define ASSERT_NOT_NULL(ptr) do {                                       \
    if ((ptr) == NULL) {                                                \
        fprintf(stderr, "ERROR: Pointer %s is NULL\n", #ptr);           \
        fprintf(stderr, "  Location: File %s, Function %s, Line %d\n",  \
                __FILE__, __func__, __LINE__);                          \
        fflush(stderr); /* ȷ��������Ϣ������� */                      \
        exit(EXIT_FAILURE); /* ��ʧ��״̬�˳����򣬱���abort��Ĭ�ϴ�����Ϣ */  \
    }                                                                   \
} while (0)