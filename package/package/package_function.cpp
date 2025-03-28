//packageFunction
#define _CRT_SECURE_NO_WARNINGS
#include "package.h"
ParcelNode* parcel_list = NULL;  // 实现全局链表初始化
//清理缓冲区
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 生成并验证单号
void generate_tracking_num(char* num, int size)
{
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    // 生成16位单号：年月日时分秒+2位随机数
    int random_num = rand() % 100; // 添加随机数保证唯一性
    snprintf(num, size, "%04d%02d%02d%02d%02d%02d%02d",
        tm_info->tm_year + 1900,
        tm_info->tm_mon + 1,
        tm_info->tm_mday,
        tm_info->tm_hour,
        tm_info->tm_min,
        tm_info->tm_sec,
        random_num); // 总长度=4+2+2+2+2+2+2=16位

    // 立即验证生成结果
    if (!is_valid_tracking(num)) 
    {
        fprintf(stderr, "致命错误：单号生成失败 %s\n", num);
        exit(EXIT_FAILURE);
    }
}

// 单号验证函数
int is_valid_tracking(const char* num)
{
    // 验证长度是否为16位
    if (strlen(num) != 16) return 0;

    // 验证是否为纯数字
    for (int i = 0; i < 16; i++) {
        if (!isdigit(num[i])) return 0;
    }
    return 1;
}


//取件码的生成
void generate_pickup_code(char* code)
{
    srand((unsigned int)time(NULL)); //用时间作为随机种子
    for (int i = 0; i < 5; i++) {
        // 生成包含字母和数字的混合验证码
        code[i] = rand() % 2 ?
            'A' + rand() % 26 :  // 生成随机字母
            '0' + rand() % 10;   // 生成随机数字
    }
    code[5] = '\0';  // 字符串终止符
}

//创建包裹节点
ParcelNode* create_parcel()
{
    ParcelNode* new_node = create_parcel_without_state();
    if (new_node == NULL) {
        printf("创建失败，请重新创建快递\n");
        return NULL;
    }
    setBufferState(new_node);
    new_node->next = NULL;
    printf("创建成功\n");

    return new_node;
}

void initialize_missing_data(ParcelNode* node)
{
	strcpy(node->company, "未知");
    strcpy(node->sender.name, "未知");
    strcpy(node->receiver.name, "未知");
	strcpy(node->collector.name, "未知");
	strcpy(node->pickup_code, "00000");
	strcpy(node->shelf_id, "未知");
    strcpy(node->promo_code, "无优惠券");
	strcpy(node->address, "未知");
	strcpy(node->send_address, "未知");

	node->rating = -1;
	node->reminder_sent = 0;
	node->price = 0.0f;
	node->weight = 0.0f;
	node->size = NONESIZE;
	node->type = NONETYPE;
    node->status = NONESTATUS;
    node->send_time = (time_t)0;
    node->store_time = (time_t)0;
    node->pickup_time = (time_t)0;
}

ParcelNode* create_parcel_without_state()
{
    ParcelNode* new_node = (ParcelNode*)malloc(sizeof(ParcelNode));
    memset(new_node, 0, sizeof(ParcelNode));
    char buffer[100];
    printf("\n=== 新建快递 ===\n");
    initialize_missing_data(new_node);
    // 单号生成
    generate_tracking_num(new_node->tracking_num, sizeof(new_node->tracking_num));
    printf("生成快递单号: %s\n", new_node->tracking_num);
    // 公司名称输入
    printf("物流公司: ");
    fgets(new_node->company, sizeof(new_node->company), stdin);
    new_node->company[strcspn(new_node->company, "\n")] = '\0';
    int valid = 0;
    // 类型输入验证
    valid = 0;
    while (!valid) {
        printf("包裹类型 (0-普通 1-易碎 2-生鲜 3-贵重 4-危险): ");
        fgets(buffer, sizeof(buffer), stdin);
        int input = atoi(buffer);
        if (input >= 0 && input <= 4) {
            new_node->type = (PackageType)input;
            valid = 1;
        }
    }
    // 尺寸输入验证
    valid = 0;
    while (!valid) {
        printf("包裹尺寸 (0-小 1-中 2-大): ");
        fgets(buffer, sizeof(buffer), stdin);
        int input = atoi(buffer);
        if (input >= 0 && input <= 2) {
            new_node->size = (PackageSize)input;
            valid = 1;
        }
    }
    
    // 重量输入验证
    valid = 0;
    while (!valid) {
        printf("重量(kg): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%f", &new_node->weight) == 1 && new_node->weight > 0) {
            valid = 1;
        }
    }
    // 输入寄件人信息
    printf("\n=== 寄件人信息 ===");
    printf("\n姓名: ");
    fgets(new_node->sender.name, sizeof(new_node->sender.name), stdin);
    new_node->sender.name[strcspn(new_node->sender.name, "\n")] = '\0';
    printf("请选择寄件人账号类型(0-学生 1-VIP 2-普通 3-教职工 4-企业):");
    scanf("%d", (int*)&new_node->sender.accountType);
    clear_input_buffer();
    strcpy(new_node->send_address, "吉大大学城快递驿站（博文路与学海街交叉口西南120米）");
    printf("寄件地址: %s\n", new_node->send_address);
    // 输入收件人信息
    printf("\n=== 收件人信息 ===");
    printf("\n姓名: ");
    fgets(new_node->receiver.name, sizeof(new_node->receiver.name), stdin);
    new_node->receiver.name[strcspn(new_node->receiver.name, "\n")] = '\0';

    int zone_choice;
    printf("\n=== 选择寄送区域 ===\n");
    printf("0: 省内 (ZONE_1)\n");
    printf("1: 邻省 (ZONE_2)\n");
    printf("2: 远程 (ZONE_3)\n");
    printf("3: 偏远地区 (ZONE_4)\n");
    printf("请输入区域编号: ");
    scanf("%d", &zone_choice);
    clear_input_buffer();
    // 确保用户输入有效
    if (zone_choice < 0 || zone_choice > 3) {
        printf("输入错误，默认使用偏远地区 (ZONE_4)!\n");
        zone_choice = 3;
    }
    // 输入地址
	printf("\n=== 收件地址 ===\n");
    my_position(new_node,0);

    //计算价格
    // 查询规则并计算价格
    printf("=== 计算价格 ===\n");
    PriceRule* rule = find_rule_by_name(new_node->company);
    if (!rule)
    {
        printf("未找到%s的规则！\n", new_node->company);
        return NULL;
    }
    printf("请输入优惠码（若无则直接回车）: ");
    char promo_code[20];

    // 使用fgets获取完整输入行
    fgets(promo_code, sizeof(promo_code), stdin);
    // 去除换行符
    size_t len = strcspn(promo_code, "\n");
    promo_code[len] = '\0';
    // 判断逻辑
    if (strlen(promo_code) == 0)
    {
        strcpy(new_node->promo_code, "无优惠券");
    }
    //（新增）
    float price = calculate_price(new_node, rule, zone_choice);
    if (price < 0.0f) {
        printf("计算价格失败，请检查输入！\n");
    }
    else {
        printf("运费为: %.2f\n", price);
        new_node->price = price;
    }
    return new_node;
}
void setBufferState(ParcelNode* new_node)
{
    //设定包裹状态
    printf("\n=== 包裹状态 ===\n");
    int state = -1;
    while (1) {
        printf("请选择包裹当前状态: STORED(0), OUTBOUND(1), DELAY(2), LOST(3), STOLEN(4), REJECTED(5), DAMAGED(6),IN_TRANSIT(7)\n");
        scanf("%d", &state);
        if (state >= 0 && state <= 8) {
            break;
        }
        printf("输入错误,请重新输入\n");
    }
    //当前时间
    char* current_time = get_time();
    switch (state) {
    case 0:
        //自动定义当下为入库时间
        //取件码生成
        generate_pickup_code(new_node->pickup_code);
        if (current_time != NULL) {
            printf("当前时间: %s\n", current_time);
            new_node->store_time = time(NULL);
            free(current_time);
        }
        else {
            printf("获取时间失败\n");
            return;
        }
        // 修改状态为已入库
        new_node->status = STORED;
        printf("已标记为入库\n");
        display_parcel(new_node);
        break;
    case 1:
        // 自动定义当下为取件时间
        if (current_time != NULL) {
            printf("当前时间: %s\n", current_time);
            new_node->pickup_time = time(NULL); // 储存取件时间
            free(current_time);  // 记得释放内存
        }
        else {
            printf("获取时间失败\n");
            return;
        }
        // 输入取件人信息
        printf("\n请输入取件人信息\n");
        printf("\n姓名: ");
        fgets(new_node->collector.name, sizeof(new_node->collector.name), stdin);
        new_node->collector.name[strcspn(new_node->collector.name, "\n")] = '\0';
        // 修改状态为已取件
        new_node->status = OUTBOUND;
        printf("已标记为出库\n");
        printf("是否提醒用户包裹已取出？(Y/N):");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') {
            printf("已成功发送提醒\n");
            new_node->reminder_sent = 1;
        }
        else {
            printf("暂不发送提醒\n");
            new_node->reminder_sent = 0;
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
            new_node->rating = rating;
            printf("本次服务评价为%d分\n", new_node->rating);
        }
        else
        {
            printf("用户暂未评价\n");
        }
        display_parcel(new_node);
        break;
    case 2:
        new_node->status = DELAY;
        printf("已标记滞留状态\n");
        printf("是否提醒用户取件？(Y/N)\n");
        choice = 0;
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') {
            printf("已成功发送提醒\n");
            new_node->reminder_sent = 1;
        }
        else {
            printf("暂不发送提醒\n");
            new_node->reminder_sent = 0;
        }
        break;
    case 3:
        new_node->status = LOST;
        printf("已标记丢失状态\n");
        printf("是否提醒用户包裹丢失？(Y/N)\n");
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') {
            printf("已成功发送提醒\n");
            new_node->reminder_sent = 1;
        }
        else {
            printf("暂不发送提醒\n");
            new_node->reminder_sent = 0;
        }
        break;
    case 4:
        new_node->status = STOLEN;
        printf("已标记冒领状态\n");
        printf("是否提醒用户包裹被冒领？(Y/N)\n");
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') {
            printf("已成功发送提醒\n");
            new_node->reminder_sent = 1;
        }
        else {
            printf("暂不发送提醒\n");
            new_node->reminder_sent = 0;
        }
        break;
    case 5:
        new_node->status = REJECTED;
        printf("已标记拒收状态\n");
        break;
    case 6:
        new_node->status = DAMAGED;
        printf("已标记损坏状态\n");
        printf("是否提醒用户包裹损坏？(Y/N)\n");
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') {
            printf("已成功发送提醒\n");
        }
        else {
            printf("暂不发送提醒\n");
        }
        break;
    case 7:
        new_node->status = IN_TRANSIT;
        // 自动定义当下为寄件时间
        if (current_time != NULL) {
            printf("当前时间: %s\n", current_time);
            new_node->send_time = time(NULL); // 储存取件时间
            free(current_time);  // 记得释放内存
        }
        else {
            printf("获取时间失败\n");
            return;
        }
        printf("已标记运输中状态\n");
        break;
	case 8:
		new_node->status = WAITING_PICKUP;
    default:
        break;
    }
}
ParcelNode* create_parcel_without_send_address()
{
    ParcelNode* new_node = (ParcelNode*)malloc(sizeof(ParcelNode));
    memset(new_node, 0, sizeof(ParcelNode));
    char buffer[100];
    printf("\n=== 新建快递 ===\n");
    initialize_missing_data(new_node);
    // 单号生成
    generate_tracking_num(new_node->tracking_num, sizeof(new_node->tracking_num));
    printf("生成快递单号: %s\n", new_node->tracking_num);
    // 公司名称输入
    printf("物流公司: ");
    fgets(new_node->company, sizeof(new_node->company), stdin);
    new_node->company[strcspn(new_node->company, "\n")] = '\0';
    int valid = 0;
    // 类型输入验证
    valid = 0;
    while (!valid) {
        printf("包裹类型 (0-普通 1-易碎 2-生鲜 3-贵重 4-危险): ");
        fgets(buffer, sizeof(buffer), stdin);
        int input = atoi(buffer);
        if (input >= 0 && input <= 4) {
            new_node->type = (PackageType)input;
            valid = 1;
        }
    }
    // 尺寸输入验证
    valid = 0;
    while (!valid) {
        printf("包裹尺寸 (0-小 1-中 2-大): ");
        fgets(buffer, sizeof(buffer), stdin);
        int input = atoi(buffer);
        if (input >= 0 && input <= 2) {
            new_node->size = (PackageSize)input;
            valid = 1;
        }
    }

    // 重量输入验证
    valid = 0;
    while (!valid) {
        printf("重量(kg): ");
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%f", &new_node->weight) == 1 && new_node->weight > 0) {
            valid = 1;
        }
    }
    // 输入寄件人信息
    printf("\n=== 寄件人信息 ===");
    printf("\n姓名: ");
    fgets(new_node->sender.name, sizeof(new_node->sender.name), stdin);
    new_node->sender.name[strcspn(new_node->sender.name, "\n")] = '\0';
    printf("请选择寄件人账号类型(0-学生 1-VIP 2-普通 3-教职工 4-企业):");
    scanf("%d", (int*)&new_node->sender.accountType);
    clear_input_buffer();
    // 输入地址
    printf("\n=== 上门取件地址 ===\n");
    my_position(new_node,1);
	new_node->status = WAITING_PICKUP;

    // 输入收件人信息
    printf("\n=== 收件人信息 ===");
    printf("\n姓名: ");
    fgets(new_node->receiver.name, sizeof(new_node->receiver.name), stdin);
    new_node->receiver.name[strcspn(new_node->receiver.name, "\n")] = '\0';

    int zone_choice;
    printf("\n=== 选择寄送区域 ===\n");
    printf("0: 省内 (ZONE_1)\n");
    printf("1: 邻省 (ZONE_2)\n");
    printf("2: 远程 (ZONE_3)\n");
    printf("3: 偏远地区 (ZONE_4)\n");
    printf("请输入区域编号: ");
    scanf("%d", &zone_choice);
    clear_input_buffer();
    // 确保用户输入有效
    if (zone_choice < 0 || zone_choice > 3) {
        printf("输入错误，默认使用偏远地区 (ZONE_4)!\n");
        zone_choice = 3;
    }
    // 输入地址
    printf("\n=== 收件地址 ===\n");
    my_position(new_node,0);

    //计算价格
    // 查询规则并计算价格
    printf("=== 计算价格 ===\n");
    PriceRule* rule = find_rule_by_name(new_node->company);
    if (!rule)
    {
        printf("未找到%s的规则！\n", new_node->company);
        return NULL;
    }
	printf("请输入优惠码（若无则直接回车）: ");
	char promo_code[20];
    // 使用fgets获取完整输入行
    fgets(promo_code, sizeof(promo_code), stdin);
    // 去除换行符
    size_t len = strcspn(promo_code, "\n");
    promo_code[len] = '\0';
    // 判断逻辑
    if (strlen(promo_code) == 0)
    {
        strcpy(new_node->promo_code, "无优惠券");
    }
    //（新增）
    float price = calculate_price(new_node, rule, zone_choice);
    if (price < 0.0f) {
        printf("计算价格失败，请检查输入！\n");
    }
    else {
        printf("运费为: %.2f\n", price);
        new_node->price = price;
    }
	new_node->status = WAITING_PICKUP;
	printf("已标记等待上门取件状态\n");
    return new_node;
}
//插入包裹到链表
void insert_sorted(ParcelNode** head, ParcelNode* new_node) {
	if (!head || !new_node) return;
    // 空链表或新节点应放在开头
    if (*head == NULL || strcmp(new_node->tracking_num, (*head)->tracking_num) < 0) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    ParcelNode* current = *head;
    while (current->next != NULL &&
        strcmp(current->next->tracking_num, new_node->tracking_num) < 0) {
        current = current->next;
    }
    new_node->next = current->next;
    current->next = new_node;
}

//删除包裹节点
void delete_parcel(ParcelNode** head, char* tracking_num) {
	if (head == NULL || *head == NULL) return; // 处理空链表

    ParcelNode* current = *head;
    ParcelNode* prev = NULL;

    while (current != NULL) {
        if (strcmp(current->tracking_num, tracking_num) == 0) {
            if (prev == NULL) {  // 删除头节点
                *head = current->next;
            }
            else {  // 删除中间节点
                prev->next = current->next;
            }
            free(current);  // 释放内存
            printf("快递删除成功!\n");
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("未找到该快递!\n");
}

//查找（单号）
ParcelNode* searchbytracking_num(ParcelNode* head, char* tracking_num) 
{
    if (head == NULL) return NULL; // 处理空链表
    ParcelNode* current = head;
    while (current != NULL) 
    {
        if (strcmp(current->tracking_num, tracking_num) == 0) 
            return current;
        current = current->next; // 完整遍历链表
    }
    return NULL;
}

//批量查询单号（逗号分隔）
void batch_search_by_numbers(ParcelNode* head, const char* input) {
	if (head == NULL) return;
    char buffer[200]; // 假设最多支持200字符输入
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    printf("\n=== 批量查询结果 ===\n");
    char* token = strtok(buffer, ","); // 分割逗号
    int total = 0, found = 0;
    while (token != NULL) {
        total++;
        // 清理单号两端的空白字符
        char* num = token;
        while (isspace(*num)) num++;
        size_t len = strlen(num);
        while (len > 0 && isspace(num[len - 1])) num[--len] = '\0';
        if (strlen(num) == 0) {
            token = strtok(NULL, ",");
            continue;
        }
        ParcelNode* result = searchbytracking_num(head, num);
        if (result != NULL) {
            found++;
            display_parcel(result);
        }
        else {
            printf("\n单号[%s]未找到", num);
        }
        token = strtok(NULL, ",");
    }
    printf("\n总计查询%d个单号，成功匹配%d个\n", total, found);
}

//不区分大小写的字符串查找（无strcasestr）
char* strcasestr(const char* haystack, const char* needle)
{
    if (!*needle) return (char*)haystack;
    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;
        while (tolower(*h) == tolower(*n) && *n) { h++; n++; }
        if (!*n) return (char*)haystack;
    }
    return NULL;
}

ParcelNode** search_parcels(ParcelNode* head, ParcelFilter filter, int* count) {
	if (head == NULL) return NULL;
    ParcelNode** results = malloc(100 * sizeof(ParcelNode*)); // 假设最多100条结果
    *count = 0;

    ParcelNode* current = head;
    while (current != NULL && *count < 100) {
        // 状态过滤
        if (filter.target_status != -1 && current->status != filter.target_status) 
        {
            current = current->next;
            continue;
        }

        // 姓名关键词过滤（支持空关键词）
        if (strlen(filter.sender_keyword) > 0 &&
            !strcasestr(current->sender.name, filter.sender_keyword)) {
            current = current->next;
            continue;
        }
        if (strlen(filter.receiver_keyword) > 0 &&
            !strcasestr(current->receiver.name, filter.receiver_keyword)) {
            current = current->next;
            continue;
        }
        if (strlen(filter.collector_keyword) > 0 &&
            !strcasestr(current->collector.name, filter.collector_keyword)) {
            current = current->next;
            continue;
        }

        // 记录匹配结果
        results[*count] = current;
        (*count)++;
        current = current->next;
    }
    return results;
}

//更新包裹信息
void update_parcel(ParcelNode* node) 
{
	if (node == NULL) return;
    char buffer[100];
    int valid = 0;
    // 打印旧信息
    printf("\n=== 当前快递信息 ===");
    display_parcel(node);
    // 修改快递单号
    printf("\n是否重新生成快递单号? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y')
    {
        generate_tracking_num(node->tracking_num, sizeof(node->tracking_num));
        printf("\n生成快递单号: %s\n", node->tracking_num);
    }
    // 修改物流公司
    printf("\n是否修改物流公司? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') 
    {
        printf("新物流公司: ");
        fgets(node->company, sizeof(node->company), stdin);
        node->company[strcspn(node->company, "\n")] = '\0';
    }
    // 修改包裹类型
    printf("\n是否修改包裹类型? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') 
    {
        valid = 0;
        while (!valid) {
            printf("新包裹类型 (0-普通 1-易碎 2-生鲜 3-贵重 4-危险): ");
            fgets(buffer, sizeof(buffer), stdin);
            int input = atoi(buffer);
            if (input >= 0 && input <= 4) 
            {
                node->type = (PackageType)input;
                valid = 1;
            }
        }
    }
    // 修改包裹尺寸
    printf("\n是否修改包裹尺寸? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') 
    {
        valid = 0;
        while (!valid) {
            printf("新包裹尺寸 (0-小 1-中 2-大): ");
            fgets(buffer, sizeof(buffer), stdin);
            int input = atoi(buffer);
            if (input >= 0 && input <= 2) 
            {
                node->size = (PackageSize)input;
                valid = 1;
            }
        }
    }
    // 修改重量
    printf("\n是否修改重量? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') 
    {
        valid = 0;
        while (!valid) {
            printf("新重量(kg): ");
            fgets(buffer, sizeof(buffer), stdin);
            if (sscanf(buffer, "%f", &node->weight) == 1 && node->weight > 0) 
            {
                valid = 1;
            }
        }
    }

    // 修改寄件人信息
    printf("\n是否修改寄件人信息? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
        printf("\n新寄件人姓名: ");
        fgets(node->sender.name, sizeof(node->sender.name), stdin);
        node->sender.name[strcspn(node->sender.name, "\n")] = '\0';
        printf("请选择寄件人账号类型(0-学生 1-VIP 2-普通 3-教职工 4-企业):");
        clear_input_buffer();
        int accountType;
        scanf("%d", &accountType);
        node->sender.accountType = (AccountType)accountType;
        clear_input_buffer();
    }

	printf("\n是否修改寄件地址（仅限上门取件服务）? (Y/N): ");
	fgets(buffer, sizeof(buffer), stdin);
	if (buffer[0] == 'Y' || buffer[0] == 'y')
	{
		my_position(node,1);
	}

    //修改区域
    int zone_choice;
    printf("\n=== 选择寄送区域 ===\n");
    printf("0: 省内 (ZONE_1)\n");
    printf("1: 邻省 (ZONE_2)\n");
    printf("2: 远程 (ZONE_3)\n");
    printf("3: 偏远地区 (ZONE_4)\n");
    printf("请输入区域编号: ");
    scanf("%d", &zone_choice);
    clear_input_buffer();
    // 确保用户输入有效
    if (zone_choice < 0 || zone_choice > 3) 
    {
        printf("输入错误，默认使用偏远地区 (ZONE_4)!\n");
        zone_choice = 3;
    }

	// 修改地址
	printf("是否修改收件地址? (Y/N): ");
	fgets(buffer, sizeof(buffer), stdin);
	if (buffer[0] == 'Y' || buffer[0] == 'y')
	{
        my_position(node,0);
	}
   
    //计算价格
    // 查询规则并计算价格
    printf("=== 计算价格 ===\n");
    PriceRule* rule = find_rule_by_name(node->company);
    if (!rule)
    {
        printf("未找到%s的规则！\n", node->company);
        return;
    }
    printf("请输入优惠码（若无则直接回车）: ");
    char promo_code[20];

    // 使用fgets获取完整输入行
    fgets(promo_code, sizeof(promo_code), stdin);
    // 去除换行符
    size_t len = strcspn(promo_code, "\n");
    promo_code[len] = '\0';
    // 判断逻辑
    if (strlen(promo_code) == 0)
    {
        strcpy(node->promo_code, "无优惠券");
    }
    //（新增）
    float price = calculate_price(node, rule, zone_choice);
    if (price < 0.0f) {
        printf("计算价格失败，请检查输入！\n");
    }
    else {
        printf("运费为: %.2f\n", price);
        node->price = price;
    }


    // 修改收件人信息
    printf("\n是否修改收件人信息? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
        printf("\n新收件人姓名: ");
        fgets(node->receiver.name, sizeof(node->receiver.name), stdin);
        node->receiver.name[strcspn(node->receiver.name, "\n")] = '\0';
    }
    // 修改取件人信息
    printf("\n是否修改取件人信息? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
        printf("\n新取件人姓名: ");
        fgets(node->collector.name, sizeof(node->collector.name), stdin);
        node->collector.name[strcspn(node->collector.name, "\n")] = '\0';
    }

    // 修改寄件时间
    printf("\n是否修改寄件时间? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
        while (1) {
            printf("新寄件时间 (格式: YYYY-MM-DD HH:MM:SS): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            if (validate_time_format(buffer)) {
                node->send_time = str_to_time(buffer);
                break;
            }
            else {
                printf("时间格式错误，请重新输入。\n");
            }
        }
    }

    // 修改入库时间
    printf("\n是否修改入库时间? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
        while (1) {
            printf("新入库时间 (格式: YYYY-MM-DD HH:MM:SS): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            if (validate_time_format(buffer)) {
                node->store_time = str_to_time(buffer);
                break;
            }
            else {
                printf("时间格式错误，请重新输入。\n");
            }
        }
    }

    // 修改出库时间
    printf("\n是否修改出库时间? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
        while (1) {
            printf("新出库时间 (格式: YYYY-MM-DD HH:MM:SS): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            if (validate_time_format(buffer)) {
                node->pickup_time = str_to_time(buffer);
                break;
            }
            else {
                printf("时间格式错误，请重新输入。\n");
            }
        }
    }
	// 修改状态
	printf("\n是否修改状态? (Y/N): ");
	fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y')
    {
        //设定包裹状态
        printf("\n=== 包裹状态 ===\n");
        int state = -1;
        while (1) {
            printf("请选择包裹当前状态: STORED(0), OUTBOUND(1), DELAY(2), LOST(3), STOLEN(4), REJECTED(5), DAMAGED(6),IN_TRANSIT(7)\n");
            scanf("%d", &state);
            if (state >= 0 && state <= 8) 
            {
                break;
            }
            printf("输入错误,请重新输入\n");
        }
    }
    // 修改满意度评价
    printf("\n是否修改满意度评价? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') 
    {
        valid = 0;
        while (!valid) {
            printf("新满意度评价 (0-10): ");
            fgets(buffer, sizeof(buffer), stdin);
            int input = atoi(buffer);
            if (input >= 0 && input <= 10) {
                node->rating = input;
                valid = 1;
            }
        }
    }
    // 打印更新后的信息
    printf("\n=== 更新后的快递信息 ===");
    display_parcel(node);
}

const char* get_package_size_str(PackageSize size) {
    switch (size) {
    case SMALL: return "小";
    case MEDIUM: return "中";
    case LARGE: return "大";
    case NONESIZE: return "未知尺寸";
    default: return "无效尺寸";
    }
}

const char* get_package_type_str(PackageType type) {
    switch (type) {
    case NORMAL: return "普通";
    case FRAGILE: return "易碎";
    case FRESH: return "生鲜";
    case VALUABLE: return "贵重";
    case DANGEROUS: return "危险";
    case NONETYPE: return "未知类型";
    default: return "无效类型";
    }
}

const char* get_package_status_str(PackageStatus status) {
    switch (status) {
    case STORED: return "入库";
    case OUTBOUND: return "出库";
    case DELAY: return "滞留";
    case LOST: return "丢失";
    case STOLEN: return "冒领";
    case REJECTED: return "拒收";
    case DAMAGED: return "损坏";
    case IN_TRANSIT: return "运输中";
    case NONESTATUS: return "未知状态";
	case RETURNED: return "已退货";
	case WAITING_PICKUP: return "等待上门取件";
    default: return "无效状态";
    }
}

//显示包裹详情
void display_parcel(ParcelNode* node) {
    if (node == NULL)
    {
		printf("无效的包裹信息\n");
        return;
    }
    auto_update_status(node);
    char send_time_str[20], store_time_str[20], pickup_time_str[20];
    time_to_str(node->send_time, send_time_str, sizeof(send_time_str));
    time_to_str(node->store_time, store_time_str, sizeof(store_time_str));
    time_to_str(node->pickup_time, pickup_time_str, sizeof(pickup_time_str));
    printf("\n=== 快递详情 ===");
    printf("\n快递单号: %s", node->tracking_num);
    printf("\n物流公司: %s", node->company);
    printf("\n包裹类型: %s", get_package_type_str(node->type));
    printf("\n包裹尺寸: %s", get_package_size_str(node->size));
    printf("\n重量: %.2fkg", node->weight);
    printf("\n价格: %.2f", node->price);
	printf("\n优惠券: %s", node->promo_code);
    printf("\n寄件人: %s", node->sender.name);
    printf("\n收件人: %s", node->receiver.name);
    printf("\n取件人: %s", node->collector.name);
    printf("\n取件码: %s", node->pickup_code);
	printf("\n寄件地址: %s", node->send_address);
	printf("\n收件地址: %s", node->address);
    printf("\n货架编号: %s", node->shelf_id);
    printf("\n寄件时间: %s", send_time_str);
    printf("\n入库时间: %s", store_time_str);
    printf("\n出库时间: %s", pickup_time_str);
    printf("\n当前状态: %s", get_package_status_str(node->status));
    printf("\n满意度评价: %d", node->rating);
    printf("\n是否提醒: %d", node->reminder_sent);
	if (node->status == DELAY)
	{
        printf("\033[31m滞留包裹\033[0m"); // 红色高亮
        printf(" (滞留时长：%.1f小时)",
            (double)(time(NULL) - node->store_time) / 3600);
	}
    printf("\n=================\n");
}

//获取当前时间转换为字符串
char* get_time()
{
    char* time_str = (char*)malloc(30);  // 申请堆内存
    if (time_str == NULL) return NULL;
    time_t now = time(NULL);
    strftime(time_str, 30, "%Y-%m-%d %H:%M:%S", localtime(&now));
    return time_str;
}

//文件保存
//void save_to_txt(const char* filename)
//{
//    FILE* fp = fopen(filename, "w");
//    if (!fp) {
//        fprintf(stderr, "保存失败: %s\n", strerror(errno));
//        return;
//    }
//    // 写入表头
//    fprintf(fp, "快递单号, 物流公司, 包裹类型, 包裹尺寸, 重量, 价格, 寄件人, 收件人, 取件人, 取件码, 货架编号, 寄件地址, 收件地址, 寄件时间, 入库时间, 出库时间, 当前状态, 满意度评价, 是否提醒, 优惠券\n");
//    ParcelNode* current = parcel_list;
//    while (current) {
//        char send_time_str[20], store_time_str[20], pickup_time_str[20];
//        time_to_str(current->send_time, send_time_str, sizeof(send_time_str));
//        time_to_str(current->store_time, store_time_str, sizeof(store_time_str));
//        time_to_str(current->pickup_time, pickup_time_str, sizeof(pickup_time_str));
//        fprintf(fp, "%s,%s,%d,%d,%.2f,%.2f,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%d,%s\n",
//            current->tracking_num,
//            current->company,
//            current->type,
//            current->size,
//            current->weight,
//            current->price,
//            current->sender.name,
//            current->receiver.name,
//            current->collector.name,
//            current->pickup_code,
//            current->shelf_id,
//			current->send_address,
//            current->address,
//            send_time_str,
//            store_time_str,
//            pickup_time_str,
//            current->status,
//            current->rating,
//            current->reminder_sent,
//            current->promo_code
//        );
//        current = current->next;
//    }
//    fclose(fp);
//}

// 清理字段中的非法字符（关键！）
void sanitize_field(char* str) 
{
    char* p = str;
    while (*p) {
        // 替换换行符和逗号为下划线
        if (*p == '\n' || *p == '\r' || *p == ',') 
        {
            *p = '_';
        }
        p++;
    }
}

void save_to_txt(const char* filename) 
{
    FILE* fp = fopen(filename, "w"); // 文本模式写入
    if (!fp) {
        fprintf(stderr, "保存失败: %s\n", strerror(errno));
        return;
    }

    // 写入表头（无BOM）
    fprintf(fp, "快递单号,物流公司,类型,尺寸,重量,价格,发件人,收件人,代收人,取件码,货架号,发货地址,收货地址,发货时间,入库时间,取件时间,状态,评分,催件标记,优惠券\r\n");

    ParcelNode* current = parcel_list;
    while (current) {
        // 清理所有字符串字段
        sanitize_field(current->tracking_num);
        sanitize_field(current->company);
        sanitize_field(current->sender.name);
        sanitize_field(current->receiver.name);
        sanitize_field(current->collector.name);
        sanitize_field(current->send_address);
        sanitize_field(current->address);
        sanitize_field(current->promo_code);

        // 时间格式化
        char send_time_str[20], store_time_str[20], pickup_time_str[20];
        time_to_str(current->send_time, send_time_str, sizeof(send_time_str));
        time_to_str(current->store_time, store_time_str, sizeof(store_time_str));
        time_to_str(current->pickup_time, pickup_time_str, sizeof(pickup_time_str));

        // 写入数据行（严格按格式）
        fprintf(fp, "%s,%s,%d,%d,%.2f,%.2f,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%d,%s\r\n",
            current->tracking_num,
            current->company,
            current->type,
            current->size,
            current->weight,
            current->price,
            current->sender.name,
            current->receiver.name,
            current->collector.name,
            current->pickup_code,
            current->shelf_id,
            current->send_address,
            current->address,
            send_time_str,
            store_time_str,
            pickup_time_str,
            current->status,
            current->rating,
            current->reminder_sent,
            current->promo_code
        );

        current = current->next;
    }
    fclose(fp);
}


//文件加载
//void load_from_txt(const char* filename) 
//{
//    FILE* fp = fopen(filename, "r");
//    if (!fp) {
//        fprintf(stderr, "加载失败: %s\n", strerror(errno));
//        return;
//    }
//    char line[256];
//
//    fgets(line, sizeof(line), fp); // 跳过表头
//    while (fgets(line, sizeof(line), fp)) 
//    {
//        ParcelNode* new_node = (ParcelNode*)malloc(sizeof(ParcelNode));
//        if (new_node == NULL)
//        {
//            fprintf(stderr, "内存分配失败\n");
//            continue;
//        }
//        memset(new_node, 0, sizeof(ParcelNode));
//        char send_time_str[20], store_time_str[20], pickup_time_str[20];
//        // 解析每一行数据 赋值给time的时候需要转换
//        int parsed = sscanf(line, "%19[^,],%49[^,],%d,%d,%f,%f,%49[^,],%49[^,],%49[^,],%5[^,],%9[^,],%4095[^,],%4095[^,],%19[^,],%19[^,],%19[^,],%d,%d,%d,%19[^,]",
//            new_node->tracking_num,
//            new_node->company,
//            &new_node->type,
//            &new_node->size,
//            &new_node->weight,
//            &new_node->price,
//            new_node->sender.name,
//            new_node->receiver.name,
//            new_node->collector.name,
//            new_node->pickup_code,
//            new_node->shelf_id,
//			new_node->send_address,
//            new_node->address,
//            send_time_str,
//            store_time_str,
//            pickup_time_str,
//            &new_node->status,
//            &new_node->rating,
//            &new_node->reminder_sent,
//            new_node->promo_code);
//        if (parsed != 20) {
//            free(new_node);
//            fprintf(stderr, "解析错误: %s", line);
//            continue;
//        }
//        new_node->send_time = str_to_time(send_time_str);
//        new_node->store_time = str_to_time(store_time_str);
//        new_node->pickup_time = str_to_time(pickup_time_str);
//        if (new_node->send_time == (time_t)-1 || new_node->store_time == (time_t)-1 || new_node->pickup_time == (time_t)-1) {
//            free(new_node);
//            fprintf(stderr, "时间解析错误: %s", line);
//            continue;
//        }
//        insert_sorted(&parcel_list, new_node);  // 保持链表有序
//    }
//    fclose(fp);
//}
void load_from_txt(const char* filename) {
    FILE* fp = fopen(filename, "r"); // 文本模式读取
    if (!fp) {
        fprintf(stderr, "加载失败: %s\n", strerror(errno));
        return;
    }

    char line[4096];
    fgets(line, sizeof(line), fp); // 跳过表头

    while (fgets(line, sizeof(line), fp)) {
        // 清除所有换行符（兼容\n和\r\n）
        line[strcspn(line, "\r\n")] = '\0';

        ParcelNode* new_node = malloc(sizeof(ParcelNode));
        memset(new_node, 0, sizeof(ParcelNode));

        // 临时存储时间字符串
        char send_time_str[20], store_time_str[20], pickup_time_str[20];

        // 严格长度控制（必须与结构体定义一致！）
        int parsed = sscanf(line,
            "%19[^,],%49[^,],%d,%d,%f,%f,%49[^,],%49[^,],%49[^,],%5[^,],%9[^,],%4095[^,],%4095[^,],%19[^,],%19[^,],%19[^,],%d,%d,%d,%19[^,]",
            new_node->tracking_num,
            new_node->company,
            &new_node->type,
            &new_node->size,
            &new_node->weight,
            &new_node->price,
            new_node->sender.name,
            new_node->receiver.name,
            new_node->collector.name,
            new_node->pickup_code,
            new_node->shelf_id,
            new_node->send_address,
            new_node->address,
            send_time_str,
            store_time_str,
            pickup_time_str,
            &new_node->status,
            &new_node->rating,
            &new_node->reminder_sent,
            new_node->promo_code
        );

        if (parsed != 20) {
            fprintf(stderr, "解析失败：需要20列，实际解析到%d列\n错误行：%s\n", parsed, line);
            free(new_node);
            continue;
        }

        // 时间转换
        new_node->send_time = str_to_time(send_time_str);
        new_node->store_time = str_to_time(store_time_str);
        new_node->pickup_time = str_to_time(pickup_time_str);

        insert_sorted(&parcel_list, new_node);
    }
    fclose(fp);
}


// 将time_t类型的时间转换为格式化的字符串
void time_to_str(time_t t, char* buf, size_t size) {
    struct tm* tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// 时间字符串转换为time_t
time_t str_to_time(const char* str) {
    struct tm tm_info = { 0 };
    int parsed = sscanf(str, "%d-%d-%d %d:%d:%d",
        &tm_info.tm_year,   // 年（需要调整）
        &tm_info.tm_mon,    // 月（需要调整）
        &tm_info.tm_mday,   // 日
        &tm_info.tm_hour,   // 时
        &tm_info.tm_min,    // 分
        &tm_info.tm_sec);   // 秒

    if (parsed != 6) {
        return (time_t)-1;  // 返回错误值
    }
    // 调整结构体字段
    tm_info.tm_year -= 1900;  // 年份基准1900
    tm_info.tm_mon -= 1;      // 月份范围0-11
    tm_info.tm_isdst = -1;    // 自动判断夏令时

    return mktime(&tm_info);  // 转换为time_t
}

// 计算包裹费用的函数（带错误检查）
float calculate_price(ParcelNode* parcel, PriceRule* rule, int zone_choice) {
    if(find_promotion(parcel->promo_code) == NULL)
	{
		printf("未找到优惠活动，自动定义为无优惠\n");
        strcpy(parcel->promo_code, "无优惠券");
	}
    // ---------- 参数检查 ----------
    if (!parcel || !rule) {
        printf("错误: 包裹或计费规则指针为空！\n");
        return -1.0f;  // 返回负数表示错误
    }
    // ---------- 索引越界检查 ----------
    if (zone_choice < 0 || zone_choice >= 4) {
        printf("错误: 区域选择值 %d 不合法（应为 0~3）！\n", zone_choice);
        return -1.0f;
    }
    if (parcel->type < 0 || parcel->type >= 5) {
        printf("错误: 包裹类型 %d 不合法（应为 0~4）！\n", parcel->type);
        return -1.0f;
    }
    if (parcel->size < 0 || parcel->size >= 3) {
        printf("错误: 包裹尺寸 %d 不合法（应为 0~2）！\n", parcel->size);
        return -1.0f;
    }
    if (parcel->sender.accountType < 0 || parcel->sender.accountType >= 5) {
        printf("错误: 账户类型 %d 不合法（应为 0~4）！\n", parcel->sender.accountType);
        return -1.0f;
    }
    // ---------- 计费逻辑 ----------
    float distance_fee = rule->zone_rate[zone_choice];
    float price = rule->base_price
        + (parcel->weight * rule->weight_rate)
        + distance_fee
        + rule->package_rate[parcel->type]
        + rule->size_rate[parcel->size];
    if(parcel->status== WAITING_PICKUP)
	{
        price = price + 5;
		printf("用户选择上门取件，额外收取5元\n");
	}
    // ---------- 应用账户折扣 ----------
    float discount = rule->account_discount[parcel->sender.accountType];
    if (discount < 0.0f || discount > 1.0f) {
        printf("警告: 折扣值 %.2f 超出合理范围，已自动修正！\n", discount);
        discount = (discount < 0.0f) ? 0.0f : 1.0f;  // 强制在 0~1 之间
    }
    price *= (1.0f - discount);
    printf("原价格：%.2f\n", price);//新增
    price = apply_promotions(price, parcel->promo_code);//新增
    printf("优惠后价格：%.2f\n", price);
    return price;
}


// 验证时间格式是否正确
int validate_time_format(const char* time_str) {
    int year, mon, day, hour, min, sec;
    return sscanf(time_str, "%d-%d-%d %d:%d:%d",
        &year, &mon, &day, &hour, &min, &sec) == 6;
}

// 初始化计费规则
void initialize_price_rule(PriceRule* rule, const char* company, float base_price, float weight_rate,
    const float* package_rate, const float* size_rate,
    const float* account_discount, const float* zone_rate)
{
    strncpy(rule->company, company, sizeof(rule->company) - 1);
    rule->company[sizeof(rule->company) - 1] = '\0';
    rule->base_price = base_price;
    rule->weight_rate = weight_rate;

    memcpy(rule->package_rate, package_rate, sizeof(rule->package_rate));
    memcpy(rule->size_rate, size_rate, sizeof(rule->size_rate));
    memcpy(rule->account_discount, account_discount, sizeof(rule->account_discount));
    memcpy(rule->zone_rate, zone_rate, sizeof(rule->zone_rate));
}


// 根据公司名称查找计费规则
PriceRule* find_rule_by_name(const char* name)
{
    for (int i = 0; i < price_count; i++) {
        if (strcmp(price_table[i].company, name) == 0) {
            return &price_table[i];
        }
    }
    return NULL;  // 未找到规则
}

// 新增：从文件初始化优惠活动
void initialize_promotions(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("无法打开优惠活动文件，将使用默认配置\n");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        Promotion p = { 0 };
        char start_time_str[20], end_time_str[20];
        int parsed = sscanf(line, "%19[^,],%d,%f,%f,%f,%f,%19[^,],%19[^,]",
            p.code,
            (int*)&p.type,
            &p.discount_rate,
            &p.coupon_amount,
            &p.full_amount,
            &p.reduction_amount,
            start_time_str,
            end_time_str);
        if (parsed != 8) continue;
        p.start_time = str_to_time(start_time_str);
        p.end_time = str_to_time(end_time_str);

        // 插入链表
        PromotionNode* node = malloc(sizeof(PromotionNode));
        node->promotion = p;
        node->next = promotion_list;
        promotion_list = node;
    }
    fclose(fp);
}
// 新增：应用优惠活动
float apply_promotions(float price, const char* promo_code) {
    time_t now = time(NULL);
    PromotionNode* current = promotion_list;  // 从链表头部开始遍历
    while (current != NULL) {
        Promotion p = current->promotion;
        // 检查时间范围和优惠码
        if (now >= p.start_time && now <= p.end_time && (strcmp(promo_code, p.code) == 0 || strcmp(promo_code, "") == 0)) {

            switch (p.type) {
            case DISCOUNT:
                price *= p.discount_rate;    // 打折
                break;
            case COUPON:
                price -= p.coupon_amount;   // 代金券
                break;
            case FULL_REDUCTION:
                if (price >= p.full_amount) { // 满减
                    price -= p.reduction_amount;
                }
                else
                    printf("不满足满减条件\n");
                break;
            }
            if (price < 0) price = 0;
            break; // 仅应用第一个匹配的优惠
        }
        current = current->next;  // 移动到下一个节点
    }
    return price;
}
// 新增：在程序退出时释放优惠链表内存
void free_promotion_list() {
    PromotionNode* current = promotion_list;
    while (current != NULL) {
        PromotionNode* temp = current;
        current = current->next;
        free(temp);
    }
    promotion_list = NULL;  // 清空链表头指针
}
// 新增：保存到文件
void save_promotions_to_file(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("保存失败！\n");
        return;
    }
    PromotionNode* curr = promotion_list;
    while (curr) {
        Promotion p = curr->promotion;
        char start[20], end[20];
        time_to_str(p.start_time, start, 20);
        time_to_str(p.end_time, end, 20);
        fprintf(fp, "%s,%d,%.2f,%.2f,%.2f,%.2f,%s,%s\n",
            p.code, p.type, p.discount_rate,
            p.coupon_amount, p.full_amount,
            p.reduction_amount, start, end);
        curr = curr->next;
    }
    fclose(fp);
}
// 详细显示单个优惠活动
void display_promotions_detail(Promotion* p) {
    char start[30], end[30];
    time_to_str(p->start_time, start, sizeof(start));
    time_to_str(p->end_time, end, sizeof(end));

    printf(" 优惠码: %-23s \n", p->code);
    switch (p->type) {
    case DISCOUNT:
        printf(" 类型  : 折扣（%.0f折）%15s \n", p->discount_rate * 100, "");
        break;
    case COUPON:
        printf(" 类型  : 代金券（立减%.2f元）%8s \n", p->coupon_amount, "");
        break;
    case FULL_REDUCTION:
        printf(" 类型  : 满%.2f减%.2f%18s \n",
            p->full_amount, p->reduction_amount, "");
    }
    printf(" 有效期: %s ~ %s \n", start, end);
}
// 优惠活动搜索函数
void search_promotion() {
    int search_type;
    printf("\n=== 优惠活动搜索 ===\n");
    printf("1. 按优惠码查找\n");
    printf("2. 按类型查找\n");
    printf("3. 查找有效优惠\n");
    printf("4. 查找即将过期优惠\n");
    printf("请选择搜索方式: ");
    scanf("%d", &search_type);
    clear_input_buffer();

    time_t now = time(NULL);
    PromotionNode* curr = promotion_list;
    int found = 0;
    char buffer[50];

    switch (search_type) {
    case 1: { // 按优惠码查找
        printf("输入优惠码（支持模糊查询）: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        printf("\n=== 查找结果 ===\n");
        while (curr) {
            if (strcasestr(curr->promotion.code, buffer)) {
                display_promotions_detail(&curr->promotion);
                found++;
            }
            curr = curr->next;
        }
        break;
    }

    case 2: { // 按类型查找
        printf("选择类型 (0-折扣 1-代金券 2-满减): ");
        int type;
        scanf("%d", &type);
        clear_input_buffer();

        printf("\n=== %s类型优惠 ===\n",
            type == 0 ? "折扣" :
            type == 1 ? "代金券" : "满减");

        while (curr) {
            if (curr->promotion.type == type) {
                display_promotions_detail(&curr->promotion);
                found++;
            }
            curr = curr->next;
        }
        break;
    }

    case 3: { // 有效优惠
        printf("\n=== 当前有效优惠 ===\n");
        while (curr) {
            if (now >= curr->promotion.start_time &&
                now <= curr->promotion.end_time) {
                display_promotions_detail(&curr->promotion);
                found++;
            }
            curr = curr->next;
        }
        break;
    }

    case 4: { // 即将过期（7天内到期）
        printf("\n=== 即将过期优惠（7天内到期） ===\n");
        while (curr) {
            time_t remain = curr->promotion.end_time - now;
            if (remain > 0 && remain <= 7 * 24 * 3600) {
                display_promotions_detail(&curr->promotion);
                found++;
            }
            curr = curr->next;
        }
        break;
    }

    default:
        printf("无效的搜索类型！\n");
        return;
    }

    printf("\n共找到 %d 条匹配结果\n", found);
}

// 判断包裹是否需要标记为滞留状态
int is_parcel_delayed(ParcelNode* p)
{
    // 排除已经处于终态的情况
    if (p->status == LOST ||
        p->status == STOLEN ||
        p->status == REJECTED ||
        p->status == RETURNED) {
        return 0;
    }
    // 仅针对应检测滞留的状态
    if (p->status == STORED)
    {
        time_t now = time(NULL);
        time_t store_duration = now - p->store_time;
        return (store_duration > DELAY_THRESHOLD_SEC) ? 1 : 0;
    }
    return 0;
}

void auto_update_status(ParcelNode* p)
{
    if (is_parcel_delayed(p)) {
        p->status = DELAY; // 动态更新状态
    }
}


