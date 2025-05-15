import json

def parse_nbf_and_merge(file_path):
    try:
        # 打开 .nbf 文件
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = file.readlines()
        
        merged_content = ""
        in_file_section = False

        # 逐行解析文件内容
        for line in lines:
            line = line.strip()  # 去除首尾空格

            # 检查 [File X] 段名
            if line.startswith("[File"):
                in_file_section = True
                continue  # 跳过段名

            # 处理数据行
            if in_file_section and line.startswith(":"):
                merged_content += line[1:]  # 去掉冒号并合并数据

        return merged_content
    except Exception as e:
        print(f"Error: {e}")
        return None

# 示例用法
file_path = r"C:\Users\pc\Downloads\ddFIle\烧录文件\burn_run_nvmram.nbf"  # 使用原始字符串避免转义问题
merged_content = parse_nbf_and_merge(file_path)
if merged_content:
    print("Merged Content:")
    print(merged_content)