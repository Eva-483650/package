#define _CRT_SECURE_NO_WARNINGS
#include "package.h"
//#include <windows.h>

// 将多字节字符串转换为 UTF-8
char* ConvertToUTF8(const char* input)
{
    // Step 1: 多字节（GBK）→ 宽字符（UTF-16）
    int wc_len = MultiByteToWideChar(CP_ACP, 0, input, -1, NULL, 0);
    wchar_t* wstr = (wchar_t*)malloc(wc_len * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, input, -1, wstr, wc_len);

    // Step 2: 宽字符（UTF-16）→ UTF-8
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* utf8_str = (char*)malloc(utf8_len);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8_str, utf8_len, NULL, NULL);

    free(wstr);
    return utf8_str;
}

// 将UTF-8转换为多字节字符串
char* ConvertToGBK(const char* utf8_str)
{
    if (!utf8_str || *utf8_str == '\0')
    {
        char* empty = (char*)malloc(1);
        *empty = '\0';
        return empty;
    }

    // Step 1: UTF-8 → UTF-16
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
    wchar_t* wstr = (wchar_t*)malloc(wlen * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, wstr, wlen);

    // Step 2: UTF-16 → GBK
    int glen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* gbk_str = (char*)malloc(glen);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, gbk_str, glen, NULL, NULL);
    free(wstr);
    if (!gbk_str)
    {
        fprintf(stderr, "GBK转换失败: %s\n", utf8_str);
        return _strdup(""); // 返回空字符串避免崩溃
    }
    return gbk_str; // 需要调用者释放内存
}

// 创建新节点（改为返回节点指针）
POIInfo* CreatePOINode()
{
    POIInfo* new_node = (POIInfo*)calloc(1, sizeof(POIInfo));
    if (!new_node) {
        fprintf(stderr, "Failed to allocate POI node\n");
        return NULL;
    }
    return new_node;
}

// 添加节点到链表（通过双重指针修改链表头）
void AppendToLinkedList(POIInfo** head, POIInfo* new_node)
{
    if (!new_node) return;

    if (*head == NULL) {
        *head = new_node;
    }
    else {
        POIInfo* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}


// 回调函数用于存储 API 返回的数据
size_t got_data(char* buffer, size_t size, size_t nmemb, void* userp)
{
    size_t bytes = size * nmemb;
    Data* data = (Data*)userp;
    data->memory = (char*)realloc(data->memory, data->size + bytes + 1);
    if (data->memory == NULL) {
        printf("内存分配失败！\n");
        return 0;
    }
    memcpy(&(data->memory[data->size]), buffer, bytes);
    data->size += bytes;
    data->memory[data->size] = 0;
    return bytes;
}

// 解析JSON并填充链表（通过参数传递链表头）
void ParsePOIData(json_object* json, POIInfo** head)
{
    if (!json || !head) return;

    struct json_object* pois;
    if (!json_object_object_get_ex(json, "pois", &pois)) return;

    int len = json_object_array_length(pois);
    for (int i = 0; i < len; i++)
    {
        POIInfo* node = CreatePOINode();
        if (!node) continue;

        struct json_object* poi = json_object_array_get_idx(pois, i);
        struct json_object* field;

        // 使用安全拷贝函数
        #define SAFE_COPY(field_name, target) do { \
            if (json_object_object_get_ex(poi, field_name, &field)) { \
                const char* utf8_value = json_object_get_string(field); \
                char* gbk_value = ConvertToGBK(utf8_value); \
                strncpy(target, gbk_value, STR-1); \
                target[STR-1] = '\0'; \
                free(gbk_value); \
            } \
        } while(0)

        SAFE_COPY("name", node->name);
        SAFE_COPY("address", node->address);
        SAFE_COPY("location", node->location);
        SAFE_COPY("pname", node->province);
        SAFE_COPY("cityname", node->city);
        SAFE_COPY("adname", node->district);
        SAFE_COPY("type", node->type);

        AppendToLinkedList(head, node);
    }
}

// 打印链表（通过参数传递链表头）
void PrintLinkedList(const POIInfo* head)
{
    int i = 0;
    printf("=====================================\n");
    for (const POIInfo* current = head; current != NULL; current = current->next)
    {
        i++;
        printf("[POI %d]\n"
            "名称: %s\n"
            "详细地址: %s\n"
            "经纬度: %s\n"
            "所属省份: %s\n"
            "所属城市: %s\n"
            "所属区县: %s\n"
            "所属类型: %s\n\n"
            , i, current->name, current->address, current->location,
            current->province, current->city, current->district, current->type);
    }
    printf("=====================================\n");

}

// 释放链表（通过双重指针确保头指针置空）
void FreeLinkedList(POIInfo** head)
{
    POIInfo* current = *head;
    while (current != NULL)
    {
        POIInfo* temp = current;
        current = current->next;
        free(temp);
    }
    *head = NULL;
}

// 根据name查找POI
POIInfo* FindPOI(const POIInfo* head, const char* name)
{
    for (const POIInfo* current = head; current != NULL; current = current->next)
    {
        if (strcmp(current->name, name) == 0)
        {
            return (POIInfo*)current;
        }
    }
    return NULL;
}

void my_position(ParcelNode* node)
{
    // 设置终端输出为 UTF-8
    //SetConsoleOutputCP(CP_UTF8);
    if (node == NULL)
    {
        printf("包裹指针为空!请重新添加\n");
        return;
    }
    char keywords[STR] = { 0 };
    char region[STR] = { 0 };

    printf("请输入所属市(县):");
    fgets(region, sizeof(region), stdin);
    region[strcspn(region, "\n")] = '\0'; // 去除末尾换行符
    printf("请输入地点名字:");
    fgets(keywords, sizeof(keywords), stdin);
    keywords[strcspn(keywords, "\n")] = '\0'; // 去除末尾换行符

    // 转换为 UTF-8
    char* utf8_region = ConvertToUTF8(region);
    char* utf8_keywords = ConvertToUTF8(keywords);

    // 生成 API 请求 URL
    char url[SIZE];
    snprintf(url, SIZE, "https://restapi.amap.com/v5/place/text?keywords=%s&region=%s&key=%s",
        utf8_keywords, utf8_region, KEY);

    //printf("Request URL: %s\n", url);
    free(utf8_region);
    free(utf8_keywords);

    Data main_data;
    main_data.memory = (char*)malloc(1);
    main_data.size = 0;

    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "CURL 初始化失败！\n");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, got_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&main_data);

    CURLcode ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        fprintf(stderr, "CURL 发送请求失败: %s\n", curl_easy_strerror(ret));
        return;
    }

    curl_easy_cleanup(curl);

    // 打印 API 返回数据
   //printf("API response data:\n%s\n", main_data.memory);

    // 解析 JSON 数据
    //json_object* json = json_tokener_parse(main_data.memory);
    //PrintPOIData(json);
   // json_object_put(json);

    POIInfo* poi_list = NULL;  // 本地链表头指针

    json_object* json = json_tokener_parse(main_data.memory);
    if (json)
    {
        ParsePOIData(json, &poi_list);  // 通过参数传递链表头地址
        PrintLinkedList(poi_list);
        //选择POI name
        char poi_name[STR] = { 0 };
        printf("请输入你想添加的地点:");
        fgets(poi_name, sizeof(poi_name), stdin);
        poi_name[strcspn(poi_name, "\n")] = '\0'; // 去除末尾换行符
        //查找POI
        POIInfo* current = FindPOI(poi_list, poi_name);
        if (current == NULL)
        {
            printf("找不到该地点!\n");
            FreeLinkedList(&poi_list);// 安全释放并置空
            json_object_put(json);
            return;
        }
        printf("查找到该地点!\n");
        //打印POI
        printf("[POI]\n"
            "名称: %s\n"
            "详细地址: %s\n"
            "经纬度: %s\n"
            "所属省份: %s\n"
            "所属城市: %s\n"
            "所属区县: %s\n"
            "所属类型: %s\n\n"
            , current->name, current->address, current->location,
            current->province, current->city, current->district, current->type);
        // 拼接地址信息
        char combined[STR]; 
        snprintf(combined, sizeof(combined), "%s（%s）", current->name, current->address);
        strncpy(node->address, combined, sizeof(node->address) - 1);
        node->address[sizeof(node->address) - 1] = '\0'; // 确保字符串以空字符结尾
        printf("地址添加成功: %s\n", node->address);
        FreeLinkedList(&poi_list);// 安全释放并置空
    }
    free(main_data.memory);

    return;
}

// 解析 JSON 并打印 POI 信息
//void PrintPOIData(json_object* json) 
//{
//    struct json_object* status, * pois;
//
//    // 检查 status 字段
//    if (!json_object_object_get_ex(json, "status", &status) ||
//        strcmp(json_object_get_string(status), "1") != 0) {
//        printf("API request failed, returned status = %s\n", json_object_get_string(status));
//        return;
//    }
//
//    // 检查 pois 字段
//    if (!json_object_object_get_ex(json, "pois", &pois)) {
//        printf("No pois field found. Please check the API response format!\n");
//        return;
//    }
//
//    int len = json_object_array_length(pois);
//    printf("Found a total of %d POI results\n", len);
//
//    for (int i = 0; i < len; i++) 
//    {
//        struct json_object* poi = json_object_array_get_idx(pois, i);
//        struct json_object* name, * address, * location,* province,* city,* district,* type;
//
//        json_object_object_get_ex(poi, "name", &name);
//        json_object_object_get_ex(poi, "address", &address);
//        json_object_object_get_ex(poi, "location", &location);
//        json_object_object_get_ex(poi, "pname", &province);
//        json_object_object_get_ex(poi, "cityname", &city);
//        json_object_object_get_ex(poi, "adname", &district);
//        json_object_object_get_ex(poi, "type", &type);
//
//
//        printf("Name: %s\nAddress: %s\nCoordinates: %s\nProvince:%s\nCity: %s\nDistrict: %s\nType: %s\n\n",
//            json_object_get_string(name),
//            json_object_get_string(address),
//            json_object_get_string(location),
//			json_object_get_string(province),
//            json_object_get_string(city),
//            json_object_get_string(district),
//            json_object_get_string(type));
//    }
//}