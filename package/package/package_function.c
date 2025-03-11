//packageFunction
#define _CRT_SECURE_NO_WARNINGS
#include "package.h"
ParcelNode* parcel_list = NULL;  // 实现全局链表初始化

//清理缓冲区
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//生成单号
void generate_tracking_num(char* num, int size)
{
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    snprintf(num, size, "%04d%02d%02d%02d%02d%02d",
        tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
        tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
}

//取件码的生成
void generate_pickup_code(char* code) {
    ASSERT_NOT_NULL(code);

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

    //计算价格
    // 查询规则并计算价格
    printf("=== 计算价格 ===\n");
    PriceRule* rule = find_rule_by_name(new_node->company);
    if (!rule)
    {
        printf("未找到%s的规则！\n", new_node->company);
        return NULL;
    }

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
        printf("请选择包裹当前状态: STORED(0), OUT(1), DELAY(2), LOST(3), STOLEN(4), REJECTED(5), DAMAGED(6)，IN_TRANSIT(7)\n");
        scanf("%d", &state);
        if (state >= 0 && state <= 7) {
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
        new_node->status = OUT;
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
    default:
        break;
    }
}

//插入包裹到链表
void insert_sorted(ParcelNode** head, ParcelNode* new_node) {
    ASSERT_NOT_NULL(head);
    ASSERT_NOT_NULL(new_node);
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
    ASSERT_NOT_NULL(head);

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
        {
            return current;
        }
        current = current->next; // 完整遍历链表
    }
    return NULL;
}


//更新包裹信息
void update_parcel(ParcelNode* node) {
    ASSERT_NOT_NULL(node);
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

    //计算价格
    // 查询规则并计算价格
    printf("=== 计算价格 ===\n");
    PriceRule* rule = find_rule_by_name(node->company);
    if (!rule)
    {
        printf("未找到%s的规则！\n", node->company);
        return NULL;
    }

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
    // 修改满意度评价
    printf("\n是否修改满意度评价? (Y/N): ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] == 'Y' || buffer[0] == 'y') {
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
    case OUT: return "出库";
    case DELAY: return "滞留";
    case LOST: return "丢失";
    case STOLEN: return "冒领";
    case REJECTED: return "拒收";
    case DAMAGED: return "损坏";
    case IN_TRANSIT: return "运输中";
    case NONESTATUS: return "未知状态";
	case RETURNED: return "已退货";
    default: return "无效状态";
    }
}


//显示包裹详情
void display_parcel(ParcelNode* node) {
    ASSERT_NOT_NULL(node);
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
    printf("\n寄件人: %s", node->sender.name);
    printf("\n收件人: %s", node->receiver.name);
    printf("\n取件人: %s", node->collector.name);
    printf("\n取件码: %s", node->pickup_code);
    printf("\n货架编号: %s", node->shelf_id);
    printf("\n寄件时间: %s", send_time_str);
    printf("\n入库时间: %s", store_time_str);
    printf("\n出库时间: %s", pickup_time_str);
    printf("\n当前状态: %s", get_package_status_str(node->status));
    printf("\n满意度评价: %d", node->rating);
    printf("\n是否提醒: %d", node->reminder_sent);
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
void save_to_txt(const char* filename) {
    ASSERT_NOT_NULL(filename);
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "保存失败: %s\n", strerror(errno));
        return;
    }
    // 写入表头
    fprintf(fp, "快递单号,物流公司,包裹类型,包裹尺寸,重量，价格，寄件人，收件人，取件人，取件码，货架编号，寄件时间，入库时间，出库时间，当前状态，满意度评价，是否提醒\n");
    ParcelNode* current = parcel_list;
    while (current) {
        char send_time_str[20], store_time_str[20], pickup_time_str[20];
        time_to_str(current->send_time, send_time_str, sizeof(send_time_str));
        time_to_str(current->store_time, store_time_str, sizeof(store_time_str));
        time_to_str(current->pickup_time, pickup_time_str, sizeof(pickup_time_str));
        fprintf(fp, "%s,%s,%d,%d,%.2f,%.2f,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,%d\n",
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
            send_time_str,
            store_time_str,
            pickup_time_str,
            current->status,
            current->rating,
            current->reminder_sent
        );
        current = current->next;
    }
    fclose(fp);
}


//文件加载
void load_from_txt(const char* filename) {
    ASSERT_NOT_NULL(filename);
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "加载失败: %s\n", strerror(errno));
        return;
    }
    char line[256];

    fgets(line, sizeof(line), fp); // 跳过表头
    while (fgets(line, sizeof(line), fp)) {
        ParcelNode* new_node = (ParcelNode*)malloc(sizeof(ParcelNode));
        ASSERT_NOT_NULL(new_node);
        memset(new_node, 0, sizeof(ParcelNode));
        char send_time_str[20], store_time_str[20], pickup_time_str[20];
        // 解析每一行数据 赋值给time的时候需要转换
        int parsed = sscanf(line, "%19[^,],%49[^,],%d,%d,%f,%f,%49[^,],%49[^,],%49[^,],%5[^,],%9[^,],%19[^,],%19[^,],%19[^,],%d,%d,%d",
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
            send_time_str,
            store_time_str,
            pickup_time_str,
            &new_node->status,
            &new_node->rating,
            &new_node->reminder_sent);
        if (parsed != 17) {
            free(new_node);
            fprintf(stderr, "解析错误: %s", line);
            continue;
        }
        new_node->send_time = str_to_time(send_time_str);
        new_node->store_time = str_to_time(store_time_str);
        new_node->pickup_time = str_to_time(pickup_time_str);
        if (new_node->send_time == -1 || new_node->store_time == -1 || new_node->pickup_time == -1) {
            free(new_node);
            fprintf(stderr, "时间解析错误: %s", line);
            continue;
        }
        insert_sorted(&parcel_list, new_node);  // 保持链表有序
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
    ASSERT_NOT_NULL(str);
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
    // ---------- 应用账户折扣 ----------
    float discount = rule->account_discount[parcel->sender.accountType];
    if (discount < 0.0f || discount > 1.0f) {
        printf("警告: 折扣值 %.2f 超出合理范围，已自动修正！\n", discount);
        discount = (discount < 0.0f) ? 0.0f : 1.0f;  // 强制在 0~1 之间
    }
    price *= (1.0f - discount);
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
    ASSERT_NOT_NULL(rule);
    ASSERT_NOT_NULL(company);
    ASSERT_NOT_NULL(package_rate);
    ASSERT_NOT_NULL(size_rate);
    ASSERT_NOT_NULL(account_discount);
    ASSERT_NOT_NULL(zone_rate);

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