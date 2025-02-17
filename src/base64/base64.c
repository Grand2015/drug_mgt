
#include "c_std_inc.h"
#include "base64.h"

#define HWW_BASE64_TABLE_LEN		(64)
#define HWW_BINARY_LEN				(8)
#define HWW_BASE64_DATE_LEN_MAX 	(128)
#define HWW_BASE64_PRIMARY_INTERVAL	(24)
#define HWW_BASE64_SECOND_INTERVAL	(6)
#define	HWW_BASE64_PAD_CHAR			'='
#define HWW_TRUE					(1)
#define HWW_FALSE					(0)
#define HWW_OK						(0)
#define HWW_ERROR					(-1)

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define HWW_INFO(info,...)  \
do{ \
    if(g_debug_level>=HWW_DEBUG_LEVEL_INFO){\
        printf("[Info] %s,%s,%d:"info"",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)
 
 
#define HWW_HINT(info,...)  \
do{ \
    if(g_debug_level>=HWW_DEBUG_LEVEL_HINT){\
        printf("[Debug] %s,%s,%d:"info"",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)


#define HWW_ERR(info,...)  \
do{ \
    if(g_debug_level>=HWW_DEBUG_LEVEL_ERR){\
        printf("[Error] %s,%s,%d:"info"",__FILE__,__FUNCTION__,__LINE__,##__VA_ARGS__);}\
}while(0)

char base64_table[HWW_BASE64_TABLE_LEN] = { 'A','B','C','D','E','F','G','H',
											'I','J','K','L','M','N','O','P',
											'Q','R','S','T','U','V','W','X',
											'Y','Z','a','b','c','d','e','f',
											'g','h','i','j','k','l','m','n',
											'o','p','q','r','s','t','u','v',
											'w','x','y','z','0','1','2','3',
											'4','5','6','7','8','9','+','/'};

HWW_DEBUG_LEVEL_E g_debug_level = HWW_DEBUG_LEVEL_ERR;


static void hww_print_binary_data(char *str_binary, int len_binary, const char *fun, int line)
{
	if(NULL == str_binary) return;
	
	int i =0;

	if (g_debug_level > HWW_DEBUG_LEVEL_ERR){
		printf("[%s:%d]--------------PRINT BEGIN-----------------\n", fun, line);
		printf("[%s:%d]--------------data len:%d-----------------\n", fun, line, len_binary);
		printf("[%s:%d]--------------data-----------------\n", fun, line);
		for (i = 0; i < len_binary; i++)
			printf("%d ", str_binary[i]);
		printf("\n[%s:%d]--------------PRINT END-----------------\n", fun, line);
	}

	return;
}

//56->00111000
static void hww_convert_ASCII_to_binary(char c_ASCII, char *str_binary)
{
	if (NULL == str_binary) return;

	char c = c_ASCII;
	char array[HWW_BINARY_LEN] = {0};
	char i = 0;
	
	for (i = 0; i < HWW_BINARY_LEN; i++){
		if (0 == c) break;
		array[HWW_BINARY_LEN - i - 1] = c%2;
		c = c/2;
	}

	memcpy(str_binary, array, HWW_BINARY_LEN);

	return;
}

static void hww_fill_0_by_bit(char *str_binary, int *p_len_binary)
{
	if (NULL == str_binary || NULL == p_len_binary) return;
	
	int len_binary = *p_len_binary;
	int num_group = 0;
	int num_binary_miss = 0;
	int i =0;
	
	num_group = (int)ceil(len_binary*1.0/HWW_BASE64_SECOND_INTERVAL);
	HWW_INFO("num_group:%d\n", num_group);

	//2.1 补0
	num_binary_miss = num_group*HWW_BASE64_SECOND_INTERVAL - len_binary;
	HWW_INFO("num_binary_miss:%d\n", num_binary_miss);

	for (i = 0; i < num_binary_miss; i++)
		str_binary[len_binary+i] = 0;

	*p_len_binary += num_binary_miss;
	hww_print_binary_data(str_binary, *p_len_binary, __FUNCTION__, __LINE__);
	
	
	return;
}

static int hww_cal_equal_symbol_count(int len_binary)
{
	int num_group = 0;
	int num_binary_miss = 0;
	int num_equal_symbol = 0;
	
	num_group = (int)ceil(len_binary*1.0/HWW_BASE64_PRIMARY_INTERVAL);
	HWW_INFO("num_group:%d\n", num_group);

	//3.1 缺0数量
	num_binary_miss = num_group*HWW_BASE64_PRIMARY_INTERVAL - len_binary;
	HWW_INFO("num_binary_miss:%d\n", num_binary_miss);

	//3.2 缺#数量
	num_equal_symbol = num_binary_miss/HWW_BASE64_SECOND_INTERVAL;
	HWW_INFO("num_equal_symbol:%d\n", num_equal_symbol);

	return num_equal_symbol;
}

static void hww_convert_binary_to_decimalism(char *str_input, int len_input, char *out_put, int *p_len_output, int interval)
{
	if (NULL == str_input || NULL == out_put || NULL == p_len_output) return;

	int i = 0, j = 0, k = 0;
	char str_decimalism[HWW_BASE64_DATE_LEN_MAX*2] = {0};
	char decimalism = 0;
	char str_binary[HWW_BINARY_LEN] = {0};
	int group = len_input/interval;	

	for (i = 0; i < group; i++){
		memset(str_binary, 0, sizeof(str_binary));
		memcpy(str_binary, &str_input[i*interval], interval);

		decimalism = 0;
		for (j = 0; j < interval; j++){	
			decimalism += str_binary[interval-j-1]*pow(2, j);
		}
		str_decimalism[i] = decimalism;
	}

	*p_len_output = MIN(group, *p_len_output);
	memcpy(out_put, str_decimalism, *p_len_output);
	
	return;
}


static void hww_search_from_base64_table(char *str_input, int len_input)
{
	int i = 0;

	for(i = 0; i < len_input; i++){
		str_input[i] = base64_table[str_input[i]];
	}
	
	return;
}

int hww_base64_encryption(char *str_input, int len_input, char *out_put, int len_output)
{
	if (NULL == str_input || 0 == len_input) return HWW_ERROR;
	if (NULL == out_put || 0 == len_output) return HWW_ERROR;

	int i = 0;
	char str_binary[HWW_BASE64_DATE_LEN_MAX*8] = {0};
	char str_decimalism[HWW_BASE64_DATE_LEN_MAX*2] = {0};
	int num_equal_symbol = 0;
	int len_binary = 0, len_decimalism = 0;

	if (len_input > HWW_BASE64_DATE_LEN_MAX){
		HWW_ERR("len(%d) of base64 date is out of range(%d)\n", len_input, HWW_BASE64_DATE_LEN_MAX);
		return -1;
	}

	hww_print_binary_data(str_input, len_input, __FUNCTION__, __LINE__);
	
	//1.转化成二进制
	for (i = 0; i < len_input; i++){
		hww_convert_ASCII_to_binary(str_input[i], &str_binary[i*HWW_BINARY_LEN]);
		len_binary += HWW_BINARY_LEN;
	}

	//2.按照6位二进制数据进行分组，向上取整， 不足补0
	hww_fill_0_by_bit(str_binary, &len_binary);

	//3.统计补=号个数
	num_equal_symbol = hww_cal_equal_symbol_count(len_binary);

	//4.按位转化为十进制
	len_decimalism = sizeof(str_decimalism);
	hww_convert_binary_to_decimalism(str_binary, len_binary, str_decimalism, &len_decimalism, HWW_BASE64_SECOND_INTERVAL);

	//5.查表
	hww_search_from_base64_table(str_decimalism, len_decimalism);

	//5.1 补=
	//len_binary = strlen(str_decimalism);
	if (num_equal_symbol){
		for(i = 0; i < num_equal_symbol; i++){
			str_decimalism[len_decimalism+i] = HWW_BASE64_PAD_CHAR;
		}
	}
	
	len_decimalism += num_equal_symbol;

	HWW_INFO("str_decimalism:%s\n", str_decimalism);

	memcpy(out_put, str_decimalism, MIN(len_decimalism, len_output));

	return 0;
}

static void hww_base64_filter_equal_symbol(char *str_input, int *plen)
{
	if (NULL == str_input || NULL == plen) return;

	int i = 0;
	int len = *plen;
	
	HWW_INFO("str_input:%s\n", str_input);


	for (i = len-1; i >= 0; i--){
		HWW_INFO("str_input[%d]:%c\n", i, str_input[i]);
		if (str_input[i] == HWW_BASE64_PAD_CHAR){
			len--;
			
			continue;
		}
		else break;
	}
	
	HWW_INFO("len:%d\n", len);

	*plen = len;

	return;
}

static int hww_base_encrytion_data_check(char *str_input, int len_input)
{
	int i = 0, j = 0;
	int not_equal_symbol = HWW_FALSE;

	if (len_input > HWW_BASE64_DATE_LEN_MAX){
		HWW_ERR("len(%d) of base64 date is out of range(%d)\n", len_input, HWW_BASE64_DATE_LEN_MAX);
		return HWW_FALSE;
	}

	for (i = len_input-1; i >= 0; i--){
		for (j = 0; j < HWW_BASE64_TABLE_LEN; j++){
			if (str_input[i] == base64_table[j]){
				HWW_INFO("str_input[%d]:%c,base64_table[%d]:%c.\n",i, str_input[i], j, base64_table[j]);
				not_equal_symbol = HWW_TRUE;
				break;
			}
		}

		if (j < HWW_BASE64_TABLE_LEN) continue;
 
		if (str_input[i] == HWW_BASE64_PAD_CHAR){
			if (not_equal_symbol) return HWW_FALSE;
			else continue;
		}
		else
			return HWW_FALSE;
	}
	return HWW_TRUE;
}

static void hww_base64_searh_index(char *str_input, int len_input, char *out_put, int len_output)
{
	int i = 0, j = 0;
	
	for (i = 0; i < len_input; i++){
		for (j = 0; j < HWW_BASE64_TABLE_LEN; j++){
			if (str_input[i] == base64_table[j]){
				out_put[i] = j;
				break;
			}
		}
	}

	hww_print_binary_data(out_put, len_input, __FUNCTION__, __LINE__);

	return;
}

int hww_base64_decryption(char *str_input, int len_input, char *out_put, int len_output)
{
	if (NULL == str_input || 0 == len_input) return HWW_ERROR;
	if (NULL == out_put || 0 == len_output) return HWW_ERROR;

	char str_index[HWW_BASE64_DATE_LEN_MAX] = {0};
	char index_binary[HWW_BINARY_LEN] = {0};
	char str_binary[HWW_BASE64_DATE_LEN_MAX*8] = {0};
	char str_binary_res[HWW_BASE64_DATE_LEN_MAX*8] = {0};
	int len = len_input;
	int len_binary = 0;
	int i = 0;
	
	//0.入参校验
	if (HWW_FALSE == hww_base_encrytion_data_check(str_input, len)) return HWW_ERROR;

	//1. 过滤=号
	hww_base64_filter_equal_symbol(str_input, &len);

	//2.根据内容反差索引值
	hww_base64_searh_index(str_input, len, str_index, sizeof(str_index));

	//3.转化为二进制
	for (i = 0; i < len; i++){
		memset(index_binary, 0,sizeof(index_binary));
		hww_convert_ASCII_to_binary(str_index[i], index_binary);
		memcpy(&str_binary[i*HWW_BASE64_SECOND_INTERVAL], &index_binary[2], HWW_BASE64_SECOND_INTERVAL);
		len_binary += HWW_BASE64_SECOND_INTERVAL;
	}

	//4.对8取余
	int cnt = len_binary / HWW_BINARY_LEN;
	HWW_INFO("len_binary:%d, cnt:%d\n", len_binary, cnt);

	//5.去掉补位数据
	memcpy(str_binary_res, str_binary, cnt*HWW_BINARY_LEN);
	len = sizeof(str_index);
	memset(str_index,0, len);
	hww_convert_binary_to_decimalism(str_binary_res, cnt*HWW_BINARY_LEN, str_index, &len, HWW_BINARY_LEN);
	memcpy(out_put, str_index, MIN(len, len_output));

	return HWW_OK;
}

void hww_base64_init(HWW_DEBUG_LEVEL_E debug_level)
{
	if (debug_level < HWW_DEBUG_LEVEL_ERR || debug_level > HWW_DEBUG_LEVEL_HINT) return;

	g_debug_level = debug_level;

	return;
}