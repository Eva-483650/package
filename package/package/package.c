//main
#define _CRT_SECURE_NO_WARNINGS
#include "package.h"

// ȫ�ֱ���
PriceRule rule;
PriceRule price_table[10];
int price_count = 0;

static void delBuffer(void);
static void modifyBuffer(void);
static void pickup(void);
static void searchBuffer(void);
static void stored();
static void send();
static void returnParcel();


void menu()
{
    printf("1. ��ӿ��\n");
    printf("2. ɾ�����\n");
    printf("3. �޸Ŀ��\n");
    printf("4. ��ѯ���\n");
    printf("5. ��ʾȫ��\n");

    printf("6. ȡ��\n");
    printf("7. �ļ�\n");
    printf("8. ���\n");
    printf("9. �˻�\n");

    printf("10. ��������\n");
    printf("11. �˳�ϵͳ\n");
    printf("��ѡ�����: ");
}

void initialize_all_rules() {
    
    // ��ʼ��˳�����
    PriceRule sf_rule;
    float sf_package_rate[5] = { 1.0f, 1.5f, 2.0f, 2.5f, 3.0f };
    float sf_size_rate[3] = { 1.0f, 1.2f, 1.5f };
    float sf_account_discount[5] = { 0.10f, 0.15f, 0.0f, 0.05f, 0.20f };
    float sf_zone_rate[4] = { 10.0f, 20.0f, 40.0f, 80.0f };
    initialize_price_rule(&sf_rule, "˳����", 10.0f, 0.5f,
        sf_package_rate, sf_size_rate,
        sf_account_discount, sf_zone_rate);
    price_table[price_count++] = sf_rule;

    // ��ʼ����ͨ����
    PriceRule zto_rule;
    float zto_package_rate[5] = { 0.9f, 1.2f, 1.8f, 2.2f, 2.8f };
    float zto_size_rate[3] = { 1.0f, 1.1f, 1.3f };
    float zto_account_discount[5] = { 0.05f, 0.10f, 0.0f, 0.0f, 0.15f };
    float zto_zone_rate[4] = { 8.0f, 15.0f, 30.0f, 60.0f };
    initialize_price_rule(&zto_rule, "��ͨ���", 8.0f, 0.6f,
        zto_package_rate, zto_size_rate,
        zto_account_discount, zto_zone_rate);
    price_table[price_count++] = zto_rule;

    // ��ʼ��Բͨ����
    PriceRule yto_rule;
    float yto_package_rate[5] = { 1.0f, 1.4f, 1.7f, 2.0f, 2.5f };
    float yto_size_rate[3] = { 1.0f, 1.15f, 1.3f };
    float yto_account_discount[5] = { 0.0f, 0.08f, 0.0f, 0.10f, 0.12f };
    float yto_zone_rate[4] = { 7.0f, 12.0f, 25.0f, 50.0f };
    initialize_price_rule(&yto_rule, "Բͨ���", 7.0f, 0.7f,
        yto_package_rate, yto_size_rate,
        yto_account_discount, yto_zone_rate);
    price_table[price_count++] = yto_rule;

    // ��ʼ����������
    PriceRule jd_rule;
    float jd_package_rate[5] = { 1.2f, 1.6f, 2.0f, 2.4f, 3.0f };
    float jd_size_rate[3] = { 1.0f, 1.3f, 1.6f };
    float jd_account_discount[5] = { 0.15f, 0.20f, 0.05f, 0.10f, 0.25f };
    float jd_zone_rate[4] = { 15.0f, 25.0f, 45.0f, 90.0f };
    initialize_price_rule(&jd_rule, "�������", 12.0f, 0.4f,
        jd_package_rate, jd_size_rate,
        jd_account_discount, jd_zone_rate);
    price_table[price_count++] = jd_rule;

    // ��ʼ��EMS����
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
    //�����ļ�
    char filename[256] = { 0 };
    printf("���������ļ�·��������: data.txt��: ");
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
            printf("\n=== ���п����Ϣ ===\n");
            ParcelNode* current = parcel_list;
            while (current) {
                display_parcel(current);
                current = current->next;
            }
            break;
        case 6: // ȡ��
            pickup();
            break;
        case 7:// �ļ�
            send();
        case 8: // ���
            stored();
            break;
        case 9: //�˻�
            returnParcel();
			break;
        case 10:
            save_to_txt(filename);
            printf("�����ѱ�����: %s\n", filename);
            break;
        case 11:
            printf("�˳���������\n");
            break;
        default:
            printf("ѡ�����\n");
            break;
        }
    } while (choice != 10);
	printf("�Ƿ񱣴����ݣ�(Y/N): ");
	char confirm = getchar();
    if (confirm == 'y' || confirm == 'Y')
    {
        save_to_txt(filename);  // ����ΪTXT��ʽ
        printf("�����ѱ�����: %s\n", filename);
    }
    // �ͷ��ڴ�
    ParcelNode* current = parcel_list;
    while (current) {
        ParcelNode* temp = current;
        current = current->next;
        free(temp);
    }
    return 0;
}

void delBuffer()
{
    char num[20];
    printf("����Ҫɾ���ĵ���: ");
    scanf("%14s", num);// �������볤��Ϊ14
    clear_input_buffer();
    printf("ȷ��ɾ����� %s ��(y/n): ", num);
    char confirm = getchar();
    if (confirm == 'y' || confirm == 'Y') {
        delete_parcel(&parcel_list, num);
    }
    clear_input_buffer();
}

void modifyBuffer()
{
    char num[20];
    printf("����Ҫ�޸ĵĵ���: ");
    scanf("%14s", num);
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    if (target) update_parcel(target);
    else printf("δ�ҵ��ÿ��!\n");
}


void pickup(void)
{
    char num[20];
    printf("����Ҫȡ���ĵ���: ");
    scanf("%14s", num);
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    if (target) {
        // �Զ����嵱��Ϊȡ��ʱ��
        char* current_time = get_time();
        if (current_time != NULL) {
            printf("��ǰʱ��: %s\n", current_time);
            target->pickup_time = time(NULL); // ����ȡ��ʱ��
            free(current_time);  // �ǵ��ͷ��ڴ�

        }
        else {
            printf("��ȡʱ��ʧ��\n");
            return;
        }
        // �޸�״̬Ϊ��ȡ��
		target->status = OUT;
        
        // ����ȡ������Ϣ
        printf("\n������ȡ������Ϣ\n");
        printf("\n����: ");
        fgets(target->collector.name, sizeof(target->collector.name), stdin);
        target->collector.name[strcspn(target->collector.name, "\n")] = '\0';
        printf("ȡ���ɹ�!\n");
		// �����û�������ȡ��
        printf("�Ƿ������û�������ȡ����(Y/N):");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') 
        {
            printf("�ѳɹ���������\n");
            target->reminder_sent = 1;
        }
        else 
        {
            printf("�ݲ���������\n");
            target->reminder_sent = 0;
        }
        // ���������
        printf("�����û��Ա��η����������(Y/N): ");
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y')
        {
            int rating;
            do {
                printf("������1��10֮�������: ");
                scanf("%d", &rating);
                if (rating < 1 || rating > 10) {
                    printf("���ֲ��Ϸ������������롣\n");
                }
            } while (rating < 1 || rating > 10);
            target->rating = rating;
            printf("���η�������Ϊ%d��\n", target->rating);
        }
        else
        {
            printf("�û���δ����\n");
        }
        display_parcel(target);
    }
    else printf("δ�ҵ��ÿ��!\n");
}

void searchBuffer()
{
    char num[20];
    printf("�����ѯ����: ");
    scanf("%14s", num);
    clear_input_buffer();
    ParcelNode* result = searchbytracking_num(parcel_list, num);
    if (result) display_parcel(result);
    else printf("δ�ҵ��ÿ��!\n");
}

void stored()//���
{
    char num[20];
    printf("����Ҫ���ĵ���: ");
    scanf("%14s", num);
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    if (target) {
        //�Զ����嵱��Ϊ���ʱ��
        char* current_time = get_time();
        if (current_time != NULL) {
            printf("��ǰʱ��: %s\n", current_time);
            target->store_time = time(NULL); // ����ȡ��ʱ��
            free(current_time);  // �ǵ��ͷ��ڴ�
        }
        else {
            printf("��ȡʱ��ʧ��\n");
            return;
        }
        // �޸�״̬Ϊ�����
		target->status = STORED;
        printf("���ɹ�!\n");
        printf("�Ƿ������û�ȡ����(Y/N)\n");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'Y' || choice == 'y') 
        {
            printf("�ѳɹ���������\n");
            target->reminder_sent = 1;
        }
        else {
            printf("�ݲ���������\n");
            target->reminder_sent = 0;
        }
        display_parcel(target);
        return;
    }
    else printf("δ�ҵ��ÿ��!\n");
}

void send()
{
    ParcelNode* new_node = create_parcel_without_state();//���ռ�����Ϣ����ȡ������Ϣ����״̬
    insert_sorted(&parcel_list, new_node);
    new_node->status = IN_TRANSIT;
    //�Զ����嵱��Ϊ�ļ�ʱ��
    printf("�ļ��ɹ�!\n");
}

void returnParcel()
{
    char num[20];
    printf("����Ҫ�˻��ĵ���: ");
    scanf("%14s", num);
    clear_input_buffer();
    ParcelNode* target = searchbytracking_num(parcel_list, num);
    if (target) {
        // �޸�״̬Ϊ���˻�
        target->status = RETURNED;
        printf("�˻�����: %.2f\n", target->price+2);
        printf("�˻��ɹ�!\n");
        display_parcel(target);
    }
    else {
        printf("δ�ҵ��ÿ��!\n");
    }
}
