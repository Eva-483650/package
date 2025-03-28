//main
#define _CRT_SECURE_NO_WARNINGS
#include "package.h"

// 全局变量
PriceRule rule;
PriceRule price_table[10];
int price_count = 0;
PromotionNode* promotion_list = NULL;  // 定义链表头 优惠活动初始化为空链表（新增）


static void delBuffer(void);
static void modifyBuffer(void);
static void pickup(void);
static void stored();
static void sendpackage();
static void returnParcel();
static void search_menu(ParcelNode* head);
static void promotion_menu();
static int validate_full_reduction(float full_amount, float reduction_amount);
static void schedule_instant_pickup();
static void sendmenu();

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
    printf("9. 退货\n");
    printf("10. 管理优惠活动\n");
    printf("11. 保存数据\n");
    printf("12. 退出系统\n");
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
    load_from_txt("data.txt");

    initialize_all_rules();
    initialize_promotions("promotions.txt"); // 新增：从文件初始化优惠

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
			display_parcel(new_parcel);
            break;
        case 2:
            delBuffer();
            break;
        case 3:
            modifyBuffer();
            break;
        case 4:
            ParcelNode * current_parcel = parcel_list;
            if (current_parcel)
                search_menu(current_parcel);
            else
				printf("无快递信息\n");
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
            sendmenu();
            break;
        case 8: // 入库
            stored();
            break;
        case 9: //退货
            returnParcel();
			break;
        case 10://优惠管理
            promotion_menu();
            break;
        case 11:
            save_to_txt("data.txt");
            save_promotions_to_file("promotions.txt");
            printf("数据已保存至: data.txt\n");
            break;
        case 12:
            printf("退出包裹界面\n");
            break;
        default:
            printf("选择错误\n");
            break;
        }
    } while (choice != 12);
    printf("是否保存数据？(Y/N): ");
    char confirm = getchar();
    if (confirm == 'y' || confirm == 'Y')
    {
        save_to_txt("data.txt");  // 保存为TXT格式
        save_promotions_to_file("promotions.txt");
        printf("数据已保存至: data.txt");
    }
    // 释放内存
    ParcelNode* current = parcel_list;
    while (current) {
        ParcelNode* temp = current;
        current = current->next;
        free(temp);
    }
    // 在程序退出时释放优惠链表内存（新增）
    void free_promotion_list();
    //退出时保存优惠链表到文件
    return 0;
}

void delBuffer()
{
    char num[20];
    printf("输入要删除的单号: ");
    scanf("%14s", num);// 限制输入长度为14
    clear_input_buffer();
	display_parcel(searchbytracking_num(parcel_list, num));
	if (searchbytracking_num(parcel_list, num) == NULL)
	{
		printf("未找到该快递!\n");
		return;
	}
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
    scanf("%14s", num);
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    if (target) update_parcel(target);
    else printf("未找到该快递!\n");
}

void pickup()
{
    char num[20];
    printf("输入要取件的单号: ");
    scanf("%14s", num);
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
		target->status = OUTBOUND;
        
        // 输入取件人信息
        printf("\n请输入取件人信息\n");
        printf("\n姓名: ");
        fgets(target->collector.name, sizeof(target->collector.name), stdin);
        target->collector.name[strcspn(target->collector.name, "\n")] = '\0';
        printf("取件成功!\n");
		// 提醒用户包裹已取出
        printf("是否提醒用户包裹已取出？(Y/N):");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') 
        {
            printf("已成功发送提醒\n");
            target->reminder_sent = 1;
        }
        else 
        {
            printf("暂不发送提醒\n");
            target->reminder_sent = 0;
        }
        // 满意度评价
        printf("邀请用户对本次服务进行评价(Y/N): ");
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y')
        {
            int rating;
            do {
                printf("请输入1到10之间的评分: ");
                scanf("%d", &rating);
                if (rating < 1 || rating > 10) {
                    printf("评分不合法，请重新输入。\n");
                }
            } while (rating < 1 || rating > 10);
            target->rating = rating;
            printf("本次服务评价为%d分\n", target->rating);
        }
        else
        {
            printf("用户暂未评价\n");
        }
        display_parcel(target);
    }
    else printf("未找到该快递!\n");
}

void search_menu(ParcelNode* head) 
{
    ParcelFilter filter = { -1, "", "", "" };
    int choice = -1;
    do {
        printf("\n=== 快递查询菜单 ===");
        printf("\n1. 按单号查询");
        printf("\n2. 批量单号查询");
        printf("\n3. 设置状态筛选");
        printf("\n4. 设置寄件人关键词");
        printf("\n5. 设置收件人关键词");
        printf("\n6. 设置取件人关键词");
        printf("\n7. 执行组合查询");
        printf("\n0. 返回主菜单");
        printf("\n请输入选项: ");
        if (scanf("%d", &choice) != 1) {
            // 输入非数字时清理缓冲区
            clear_input_buffer();
            choice = -1; // 重置为无效选项
            continue;
        }
        clear_input_buffer();
        switch (choice) {
        case 1: {
            char num[20];
            printf("请输入快递单号: ");
            fgets(num, sizeof(num), stdin);
            num[strcspn(num, "\n")] = '\0';
            ParcelNode* result = searchbytracking_num(head, num);
            if (result != NULL)
                display_parcel(result);
            else
                printf("未找到单号为 %s 的快递！\n", num);
            break;
        }
        case 2: { // 新增批量查询
            char input[200];
            printf("请输入多个单号（用逗号分隔）: ");
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = '\0'; // 去换行符
            batch_search_by_numbers(head, input);
            break;
        }
        case 3: {
            printf("\n可选状态:STORED(0), OUTBOUND(1), DELAY(2), LOST(3), STOLEN(4), REJECTED(5), DAMAGED(6)，IN_TRANSIT(7),WAITING_PICKUP(8)");
            printf("\n输入状态编号 (-1取消): ");
            int status;
            scanf("%d", &status);
            getchar();
            filter.target_status = (status >= 0 && status <= 8) ? status : -1;
            break;
        }
        case 4: {
            printf("输入寄件人关键词: ");
            fgets(filter.sender_keyword, 50, stdin);
            filter.sender_keyword[strcspn(filter.sender_keyword, "\n")] = '\0';
            break;
        }
        case 5: {
            printf("输入收件人关键词: ");
            fgets(filter.receiver_keyword, 50, stdin);
            filter.receiver_keyword[strcspn(filter.receiver_keyword, "\n")] = '\0';
            break;
        }
        case 6: {
            printf("输入取件人关键词: ");
            fgets(filter.collector_keyword, 50, stdin);
            filter.collector_keyword[strcspn(filter.collector_keyword, "\n")] = '\0';
            break;
        }
        case 7: {
            int count;
            ParcelNode** results = search_parcels(head, filter, &count);
            if (count == 0) {
                printf("\n未找到匹配的快递！");
            }
            else {
                printf("\n=== 找到 %d 个匹配快递 ===", count);
                for (int i = 0; i < count; i++) {
                    display_parcel(results[i]);
                }
            }
            free(results);
            break;
        }
        }
    } while (choice != 0);
}

void stored()//入库
{
    char num[20];
    printf("输入要入库的单号: ");
    scanf("%14s", num);
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

        printf("是否提醒用户取件？(Y/N)\n");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') 
        {
            printf("已成功发送提醒\n");
            target->reminder_sent = 1;
        }
        else {
            printf("暂不发送提醒\n");
            target->reminder_sent = 0;
        }
        display_parcel(target);
    }
    else printf("未找到该快递!\n");
}

void sendmenu()//选择普通寄件还是上门取件
{
    int choice = -1;
	do {
		printf("\n=== 寄件方式 ===\n");
		printf("1. 普通寄件\n");
		printf("2. 上门取件\n");
		printf("0. 返回主菜单\n");
		printf("请选择操作: ");
		scanf("%d", &choice);
		clear_input_buffer();
		switch (choice) {
		case 1:
			sendpackage();
			break;
		case 2:
			schedule_instant_pickup();
			break;
        case 0:
			break;
		default:
			printf("选择错误\n");
			break;
		}
	} while (choice != 0);
}

void sendpackage()//普通寄件
{
    ParcelNode* new_node = create_parcel_without_state();//无收件人信息，无取件人信息，无状态
    insert_sorted(&parcel_list, new_node);
    //自动定义当下为寄件时间
    if (new_node) {
        //自动定义当下为入库时间
        char* current_time = get_time();
        if (current_time != NULL) {
            printf("当前时间: %s\n", current_time);
            new_node->send_time= time(NULL); // 储存取件时间
            free(current_time);  // 记得释放内存
        }
        else {
            printf("获取时间失败\n");
            return;
        }
        // 修改状态为在途
        new_node->status = IN_TRANSIT;
        printf("寄件成功!\n");
        display_parcel(new_node);
    }
    else printf("未找到该快递!\n");
}

void returnParcel()
{
    char num[20];
    printf("输入要退货的单号: ");
    if (scanf("%14s", num) != 1) {
        printf("输入错误，请重试。\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    display_parcel(target);
    if (target==NULL) 
    {
        printf("未找到该快递!\n");
    }
    else 
    {
        // 修改状态为已退货
        target->status = RETURNED;
        printf("退货费用: %.2f\n", target->price + 2);
        printf("退货成功!\n");
        display_parcel(target);
    }
}

//新增：优惠管理子菜单
void promotion_menu() {
    int choice;
    do {
        printf("\n=== 优惠管理 ===\n");
        printf("1. 添加优惠活动\n");
        printf("2. 删除优惠活动\n");
        printf("3. 修改优惠活动\n");
        printf("4. 查看所有优惠\n");
        printf("5. 查找优惠活动\n");
        printf("0. 返回主菜单\n");
        printf("请选择操作: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: {
            Promotion p = { 0 };
            printf("输入优惠码: ");
            fgets(p.code, sizeof(p.code), stdin);
            p.code[strcspn(p.code, "\n")] = '\0';
            // 检查重复
            PromotionNode* curr = promotion_list;
            while (curr) {
                if (strcmp(curr->promotion.code, p.code) == 0) {
                    printf("优惠码已存在！\n");
                    return;
                }
                curr = curr->next;
            }
            printf("选择优惠类型 (0-打折 1-代金券 2-满减): ");
            scanf("%d", (int*)&p.type);
            while (p.type != 0 && p.type != 1 && p.type != 2) {
                printf("输入有误，请重新输入");
                scanf("%d", (int*)&p.type);
            }
            clear_input_buffer();

            // 根据类型输入参数
            switch (p.type) {
            case DISCOUNT:
                printf("输入折扣率 (例如0.9): ");
                scanf("%f", &p.discount_rate);
                //验证输入
                while (p.discount_rate <= 0 || p.discount_rate >= 1) {
                    printf("输入有误！请输入0-1之间的数：");
                    scanf("%f", &p.discount_rate);
                }

                break;
            case COUPON:
                printf("输入代金券金额: ");
                scanf("%f", &p.coupon_amount);
                //验证输入
                while (p.coupon_amount <= 0) {
                    printf("请输入大于0的数字！：");
                    scanf("%f", &p.coupon_amount);
                }
                break;
            case FULL_REDUCTION:
                printf("输入满减条件（用空格分隔 满 减）: ");
                scanf("%f %f", &p.full_amount, &p.reduction_amount);
                while (!validate_full_reduction(p.full_amount, p.reduction_amount)) {
                    printf("请重新输入满减条件: ");
                    scanf("%f %f", &p.full_amount, &p.reduction_amount);
                }
                break;
            }

            // 输入时间
            char start_time[20], end_time[20];
            while (1) {
                printf("输入开始时间 (格式: YYYY-MM-DD HH:MM:SS): ");
                char ch = getchar();
                fgets(start_time, sizeof(start_time), stdin);
                start_time[strcspn(start_time, "\n")] = '\0';
                if (validate_time_format(start_time)) {
                    p.start_time = str_to_time(start_time);
                    break;
                }
                else {
                    printf("时间格式错误，请重新输入。\n");
                }
            }
            while (1) {
                printf("输入结束时间 (格式: YYYY-MM-DD HH:MM:SS): ");
                char ch = getchar();
                fgets(end_time, sizeof(end_time), stdin);
                end_time[strcspn(end_time, "\n")] = '\0';
                if (validate_time_format(end_time)) {
                    p.end_time = str_to_time(end_time);
                    break;
                }
                else {
                    printf("时间格式错误，请重新输入。\n");
                }
            }
            if (p.end_time <= p.start_time) {
                printf("结束时间必须晚于开始时间！\n");
                break;
            }
            add_promotion(p);
            break;
        }
        case 2: {
            char code[20];
            printf("输入要删除的优惠码: ");
            fgets(code, sizeof(code), stdin);
            code[strcspn(code, "\n")] = '\0';
            delete_promotion(code);
            break;
        }
        case 3: {
            char code[20];
            printf("输入要修改的优惠码: ");
            fgets(code, sizeof(code), stdin);
            code[strcspn(code, "\n")] = '\0';
            modify_promotion(code);
            break;
        }
        case 4:
            display_promotions();
            break;
        case 5:
            search_promotion();
            break;
        case 0:
            return; // 返回主菜单
        default:
            printf("无效选择!\n");
        }
    } while (choice != 0);
}
// 新增：添加优惠活动
void add_promotion(Promotion p) {

    PromotionNode* node = malloc(sizeof(PromotionNode));
    node->promotion = p;
    node->next = promotion_list;
    promotion_list = node;
    printf("添加成功！\n");
}
// 新增：删除优惠活动
void delete_promotion(const char* code) {
    PromotionNode* curr = promotion_list, * prev = NULL;
    while (curr) {
        if (strcmp(curr->promotion.code, code) == 0) {
            if (prev) prev->next = curr->next;
            else promotion_list = curr->next;
            free(curr);
            printf("删除成功！\n");
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("未找到该优惠活动！\n");
}
// 新增：查找优惠活动
Promotion* find_promotion(const char* code) {
    PromotionNode* curr = promotion_list;
    while (curr) {
        if (strcmp(curr->promotion.code, code) == 0)
            return &curr->promotion;
        curr = curr->next;
    }
    return NULL;
}
// 新增：显示优惠活动信息
void display_promotions() {
    printf("\n=== 优惠活动详细信息 ===\n");
    PromotionNode* curr = promotion_list;

    while (curr != NULL) {
        Promotion p = curr->promotion;
        char start[30], end[30];

        // 转换时间格式
        time_to_str(p.start_time, start, sizeof(start));
        time_to_str(p.end_time, end, sizeof(end));

        // 显示基础信息
        printf(" 优惠码: %-23s \n", p.code);

        // 根据类型显示详细信息
        switch (p.type) {
        case DISCOUNT:
            printf(" 类型  : 折扣活动（%.0f折）          \n", p.discount_rate * 10);
            break;
        case COUPON:
            printf(" 类型  : 代金券（立减%.2f元）       \n", p.coupon_amount);
            break;
        case FULL_REDUCTION:
            printf(" 类型  : 满减（满%.2f减%.2f）      \n", p.full_amount, p.reduction_amount);
            break;
        default:
            printf(" 类型  : 未知类型                  \n");
        }

        // 显示时间范围
        printf(" 有效期: %s ~ %s \n", start, end);
        printf("\n");
        curr = curr->next;
    }

    if (promotion_list == NULL) {
        printf("当前没有可用的优惠活动\n");
    }
}
//新增：修改优惠活动
void modify_promotion(const char* code) {
    // 查找要修改的优惠活动
    PromotionNode* curr = promotion_list;
    while (curr != NULL) {
        if (strcmp(curr->promotion.code, code) == 0) {
            break;
        }
        curr = curr->next;
    }

    if (curr == NULL) {
        printf("未找到优惠码为 %s 的活动！\n", code);
        return;
    }

    Promotion* p = &curr->promotion;
    int choice;
    char buffer[100];

    do {
        printf("\n=== 正在修改优惠活动 [%s] ===\n", code);
        printf("1. 修改优惠类型\n");
        printf("2. 修改折扣率\n");
        printf("3. 修改代金券金额\n");
        printf("4. 修改满减条件\n");
        printf("5. 修改时间范围\n");
        printf("6. 修改优惠码\n");
        printf("0. 完成修改\n");
        printf("请选择要修改的项: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch (choice) {
        case 1: { // 修改类型
            printf("\n当前类型: ");
            switch (p->type) {
            case DISCOUNT: printf("折扣"); break;
            case COUPON: printf("代金券"); break;
            case FULL_REDUCTION: printf("满减"); break;
            default: printf("未知类型");
            }
            printf("\n选择新类型 (0-折扣 1-代金券 2-满减): ");
            int new_type;
            scanf("%d", &new_type);
            clear_input_buffer();

            if (new_type < 0 || new_type > 2) {
                printf("无效类型！\n");
                break;
            }

            /* 重置所有参数 */
            p->discount_rate = 0;
            p->coupon_amount = 0;
            p->full_amount = 0;
            p->reduction_amount = 0;

            /* 根据新类型要求输入参数 */
            switch (new_type) {
            case DISCOUNT:
                printf("▌ 已切换为折扣类型，请设置折扣率\n");
                printf("输入折扣率（例如0.9）: ");
                scanf("%f", &p->discount_rate);
                //验证输入
                while (p->discount_rate <= 0 || p->discount_rate >= 1) {
                    printf("输入有误！请输入0-1之间的数：");
                    scanf("%f", &p->discount_rate);
                }
                break;

            case COUPON:
                printf("▌ 已切换为代金券类型，请设置金额\n");
                printf("输入代金券金额: ");
                scanf("%f", &p->coupon_amount);
                break;

            case FULL_REDUCTION:
                printf("▌ 已切换为满减类型，请设置条件\n");
                printf("输入满减条件（用空格分隔 满 减）: ");
                scanf("%f %f", &p->full_amount, &p->reduction_amount);
                while (!validate_full_reduction(p->full_amount, p->reduction_amount)) {
                    printf("请重新输入满减条件: ");
                    scanf("%f %f", &p->full_amount, &p->reduction_amount);
                }
                break;
            }

            p->type = (PromotionType)new_type;
            clear_input_buffer();
            printf("类型修改成功！\n");
            break;
        }

        case 2: {
            if (p->type != DISCOUNT) {
                printf("当前活动类型不是折扣，无法修改！\n");
                break;
            }
            printf("当前折扣率: %.2f\n", p->discount_rate);
            printf("输入新折扣率（例如0.9）: ");
            float rate;
            scanf("%f", &rate);
            //验证输入
            while (rate <= 0 || rate >= 1) {
                printf("输入有误！请输入0-1之间的数：");
                scanf("%f", rate);
            }
            p->discount_rate = rate;
            break;
        }

        case 3: {
            if (p->type != COUPON) {
                printf("当前活动类型不是代金券，无法修改！\n");
                break;
            }
            printf("当前代金券金额: %.2f\n", p->coupon_amount);
            printf("输入新金额: ");
            float amount;
            scanf("%f", &amount);
            clear_input_buffer();

            if (amount < 0) {
                printf("金额不能为负数！\n");
                break;
            }
            p->coupon_amount = amount;
            printf("代金券金额已更新\n");
            break;
        }

        case 4: {
            if (p->type != FULL_REDUCTION) {
                printf("当前活动类型不是满减，无法修改！\n");
                break;
            }
            printf("当前满减条件: 满%.2f减%.2f\n",
                p->full_amount, p->reduction_amount);

            printf("输入新满减条件（用空格分隔 满 减）：");
            float full, reduction;
            scanf("%f %f", &full, &reduction);
            clear_input_buffer();
            //使用验证函数
            while (!validate_full_reduction(full, reduction)) {
                printf("请重新输入满减条件: ");
                scanf("%f %f", &full, &reduction);
            }

            p->full_amount = full;
            p->reduction_amount = reduction;
            printf("满减条件已更新\n");
            break;
        }

        case 5: {
            char start[20], end[20];
            printf("当前时间范围:\n");
            time_to_str(p->start_time, start, sizeof(start));
            time_to_str(p->end_time, end, sizeof(end));
            printf("%s 至 %s\n", start, end);
            time_t new_start;
            time_t new_end;
            while (1) {
                printf("输入新开始时间 (格式: YYYY-MM-DD HH:MM:SS): ");
                char ch = getchar();
                fgets(start, sizeof(start), stdin);
                start[strcspn(start, "\n")] = '\0';
                if (validate_time_format(start)) {
                    new_start = str_to_time(start);
                    break;
                }
                else {
                    printf("时间格式错误，请重新输入。\n");
                }
            }
            while (1) {
                printf("输入新结束时间 (格式: YYYY-MM-DD HH:MM:SS): ");
                char ch = getchar();
                fgets(end, sizeof(end), stdin);
                end[strcspn(end, "\n")] = '\0';
                if (validate_time_format(end)) {
                    new_end = str_to_time(end);
                    break;
                }
                else {
                    printf("时间格式错误，请重新输入。\n");
                }
            }
            if (new_end <= new_start) {
                printf("结束时间必须晚于开始时间！\n");
                break;
            }

            p->start_time = new_start;
            p->end_time = new_end;
            printf("时间范围已更新\n");
            break;
        }

        case 6: {
            printf("当前优惠码: %s\n", p->code);
            printf("输入新优惠码（最多19字符）: ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';

            if (strlen(buffer) == 0) {
                printf("优惠码不能为空！\n");
                break;
            }

            // 检查是否重复
            PromotionNode* check = promotion_list;
            while (check != NULL) {
                if (check != curr && strcmp(check->promotion.code, buffer) == 0) {
                    printf("优惠码 %s 已存在！\n", buffer);
                    return;
                }
                check = check->next;
            }

            strncpy(p->code, buffer, sizeof(p->code) - 1);
            p->code[sizeof(p->code) - 1] = '\0';
            printf("优惠码已更新\n");
            break;
        }

        case 0:
            printf("修改完成！\n");
            break;

        default:
            printf("无效选项！\n");
        }

    } while (choice != 0);
}
//新增：判断满减规则是否合法
int validate_full_reduction(float full_amount, float reduction_amount) {
    // 检查基础数值有效性
    if (full_amount <= 0) {
        printf("错误：满减条件金额必须大于0！\n");
        return 0;
    }

    if (reduction_amount <= 0) {
        printf("错误：减免金额必须大于0！\n");
        return 0;
    }

    // 计算最大允许减免金额（25%）
    float max_reduction = full_amount * 0.25f;

    // 精确到小数点后两位的浮点数比较
    if (fabs(reduction_amount - max_reduction) < 0.005f) {
        return 1; // 允许等于25%
    }

    if (reduction_amount > max_reduction) {
        printf("错误：减免金额不能超过满减条件的25%%\n");
        printf("当前满额: %.2f → 最大可减免: %.2f\n",
            full_amount, max_reduction);
        return 0;
    }

    return 1;
}

void schedule_instant_pickup(){
    int choice = -1;
    do {
        printf("\n=== 上门取件 ===\n");
        printf("1. 添加取件订单\n");
        printf("2. 确认取件完成\n");
        printf("3. 查看取件订单\n");
        printf("0. 返回主菜单\n");
        printf("请选择操作: ");
        scanf("%d", &choice);
        clear_input_buffer();

        switch (choice){
        case 1:
            ParcelNode * new_node = create_parcel_without_send_address();
            insert_sorted(&parcel_list, new_node);
            printf("上门取件预约成功!\n");
            display_parcel(new_node);
            break;
        case 2:
            char num[20];
            printf("输入要确认取件的单号: ");
            scanf("%14s", num);
            clear_input_buffer();
            ParcelNode* target = searchbytracking_num(parcel_list, num);
            if (target) {
                //自动定义当下为寄件时间
                char* current_time = get_time();
                if (current_time != NULL) {
                    printf("当前时间: %s\n", current_time);
                    target->send_time = time(NULL); // 储存取件时间
                    free(current_time);  // 记得释放内存
                }
                else {
                    printf("获取时间失败\n");
                    return;
                }
                // 修改状态为在途
                target->status = IN_TRANSIT;
                printf("寄件成功!\n");
                display_parcel(target);
            }
            else printf("未找到该快递!\n");
            break;
        case 3:
            printf("\n=== 等待上门取件的订单 ===\n");
            ParcelNode* current = parcel_list;
            int found = 0;
            while (current) {
                if (current->status == WAITING_PICKUP) {
                    display_parcel(current);
                    found = 1;
                }
                current = current->next;
            }
            if (!found) {
                printf("当前没有等待上门取件的订单\n");
            }
            break;
        case 0:
            break;
        default:
            printf("无效选择!\n");
            break;
        }
    } while (choice != 0);
}
