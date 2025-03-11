//main
#define _CRT_SECURE_NO_WARNINGS
#include "package.h"

// 全局变量
PriceRule rule;
PriceRule price_table[10];
int price_count = 0;

static void delBuffer(void);
static void modifyBuffer(void);
static void pickup(void);
static void searchBuffer(void);
static void stored();
static void send();


void menu()
{
    printf("1. 添加快递\n");
    printf("2. 删除快递\n");
    printf("3. 修改快递\n");
    printf("4. 查询快递\n");
    printf("5. 显示全部\n");

    printf("6. 取件\n");
    printf("7. 寄件\n");
    printf("8. 入库\n");

    printf("9. 保存数据\n");
    printf("10. 退出系统\n");
    printf("请选择操作: ");
}

void initialize_all_rules() {
    
    // 初始化顺丰规则
    PriceRule sf_rule;
    float sf_package_rate[5] = { 1.0f, 1.5f, 2.0f, 2.5f, 3.0f };
    float sf_size_rate[3] = { 1.0f, 1.2f, 1.5f };
    float sf_account_discount[5] = { 0.10f, 0.15f, 0.0f, 0.05f, 0.20f };
    float sf_zone_rate[4] = { 10.0f, 20.0f, 40.0f, 80.0f };
    initialize_price_rule(&sf_rule, "顺丰快递", 10.0f, 0.5f,
        sf_package_rate, sf_size_rate,
        sf_account_discount, sf_zone_rate);
    price_table[price_count++] = sf_rule;

    // 初始化中通规则
    PriceRule zto_rule;
    float zto_package_rate[5] = { 0.9f, 1.2f, 1.8f, 2.2f, 2.8f };
    float zto_size_rate[3] = { 1.0f, 1.1f, 1.3f };
    float zto_account_discount[5] = { 0.05f, 0.10f, 0.0f, 0.0f, 0.15f };
    float zto_zone_rate[4] = { 8.0f, 15.0f, 30.0f, 60.0f };
    initialize_price_rule(&zto_rule, "中通快递", 8.0f, 0.6f,
        zto_package_rate, zto_size_rate,
        zto_account_discount, zto_zone_rate);
    price_table[price_count++] = zto_rule;

    // 初始化圆通规则
    PriceRule yto_rule;
    float yto_package_rate[5] = { 1.0f, 1.4f, 1.7f, 2.0f, 2.5f };
    float yto_size_rate[3] = { 1.0f, 1.15f, 1.3f };
    float yto_account_discount[5] = { 0.0f, 0.08f, 0.0f, 0.10f, 0.12f };
    float yto_zone_rate[4] = { 7.0f, 12.0f, 25.0f, 50.0f };
    initialize_price_rule(&yto_rule, "圆通快递", 7.0f, 0.7f,
        yto_package_rate, yto_size_rate,
        yto_account_discount, yto_zone_rate);
    price_table[price_count++] = yto_rule;

    // 初始化京东规则
    PriceRule jd_rule;
    float jd_package_rate[5] = { 1.2f, 1.6f, 2.0f, 2.4f, 3.0f };
    float jd_size_rate[3] = { 1.0f, 1.3f, 1.6f };
    float jd_account_discount[5] = { 0.15f, 0.20f, 0.05f, 0.10f, 0.25f };
    float jd_zone_rate[4] = { 15.0f, 25.0f, 45.0f, 90.0f };
    initialize_price_rule(&jd_rule, "京东快递", 12.0f, 0.4f,
        jd_package_rate, jd_size_rate,
        jd_account_discount, jd_zone_rate);
    price_table[price_count++] = jd_rule;

    // 初始化EMS规则
    PriceRule ems_rule;
    float ems_package_rate[5] = { 0.8f, 1.0f, 1.5f, 2.0f, 2.5f };
    float ems_size_rate[3] = { 1.0f, 1.1f, 1.2f };
    float ems_account_discount[5] = { 0.0f, 0.05f, 0.10f, 0.0f, 0.15f };
    float ems_zone_rate[4] = { 20.0f, 35.0f, 60.0f, 120.0f };
    initialize_price_rule(&ems_rule, "EMS", 15.0f, 0.3f,
        ems_package_rate, ems_size_rate,
        ems_account_discount, ems_zone_rate);
    price_table[price_count++] = ems_rule;

}

int main() {
    //载入文件
    char filename[256] = { 0 };
    printf("输入数据文件路径（例如: data.txt）: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = '\0';
    load_from_txt(filename);

    initialize_all_rules();

    int choice = 0;
    do {
        menu();
        scanf("%d", &choice);
        clear_input_buffer();
        switch (choice) {
        case 1:
            ParcelNode * new_parcel;
            do {
                new_parcel = create_parcel();
            } while (new_parcel == NULL);
            insert_sorted(&parcel_list, new_parcel);
            break;
        case 2:
            delBuffer();
            break;
        case 3:
            modifyBuffer();
            break;
        case 4:
            searchBuffer();
            break;
        case 5:
            printf("\n=== 所有快递信息 ===\n");
            ParcelNode* current = parcel_list;
            while (current) {
                display_parcel(current);
                current = current->next;
            }
            break;
        case 6: // 取件
            pickup();
            break;
        case 7:// 寄件
            send();
        case 8: // 入库
            stored();
            break;
        case 9:
            save_to_txt(filename);
            printf("数据已保存至: %s\n", filename);
            break;
        case 10:
            printf("退出包裹界面\n");
            break;
        default:
            printf("选择错误\n");
            break;
        }
    } while (choice != 10);

    // 释放内存
    ParcelNode* current = parcel_list;
    while (current) {
        ParcelNode* temp = current;
        current = current->next;
        free(temp);
    }

    save_to_txt(filename);  // 保存为TXT格式
    printf("数据已保存至: %s\n", filename);
    return 0;
}

void delBuffer()
{
    char num[20];
    printf("输入要删除的单号: ");
    scanf("%12s", num);// 限制输入长度为12
    clear_input_buffer();
    printf("确认删除快递 %s ？(y/n): ", num);
    char confirm = getchar();
    if (confirm == 'y' || confirm == 'Y') {
        delete_parcel(&parcel_list, num);
    }
    clear_input_buffer();
}

void modifyBuffer()
{
    char num[20];
    printf("输入要修改的单号: ");
    scanf("%12s", num);
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    if (target) update_parcel(target);
    else printf("未找到该快递!\n");
}


void pickup(void)
{
    char num[20];
    printf("输入要取件的单号: ");
    scanf("%12s", num);
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    if (target) {
        // 自动定义当下为取件时间
        char* current_time = get_time();
        if (current_time != NULL) {
            printf("当前时间: %s\n", current_time);
            target->pickup_time = time(NULL); // 储存取件时间
            free(current_time);  // 记得释放内存
        }
        else {
            printf("获取时间失败\n");
            return;
        }
        // 修改状态为已取件
		target->status = OUT;

        // 输入取件人信息
        printf("\n请输入取件人信息\n");
        printf("\n姓名: ");
        fgets(target->collector.name, sizeof(target->collector.name), stdin);
        target->collector.name[strcspn(target->collector.name, "\n")] = '\0';
        printf("取件成功!\n");
        display_parcel(target);
    }
    else printf("未找到该快递!\n");
}

void searchBuffer()
{
    char num[20];
    printf("输入查询单号: ");
    scanf("%12s", num);
    clear_input_buffer();
    ParcelNode* result = searchbytracking_num(parcel_list, num);
    if (result) display_parcel(result);
    else printf("未找到该快递!\n");
}

void stored()//入库
{
    char num[20];
    printf("输入要入库的单号: ");
    scanf("%12s", num);
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    if (target) {
        //自动定义当下为入库时间
        char* current_time = get_time();
        if (current_time != NULL) {
            printf("当前时间: %s\n", current_time);
            target->store_time = time(NULL); // 储存取件时间
            free(current_time);  // 记得释放内存
        }
        else {
            printf("获取时间失败\n");
            return;
        }
        // 修改状态为已入库
		target->status = STORED;
        printf("入库成功!\n");
        display_parcel(target);
        return;
    }
    else printf("未找到该快递!\n");
}

void send()
{
    ParcelNode* new_node = create_parcel_without_state();//无收件人信息，无取件人信息，无状态
    insert_sorted(&parcel_list, new_node);
    new_node->status = IN_TRANSIT;
    //自动定义当下为寄件时间

    printf("寄件成功!\n");
}