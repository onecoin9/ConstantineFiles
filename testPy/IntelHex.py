from intelhex import IntelHex

# 读取NBF文件内容
with open("d:/qtProject/testPy/mtp20.nbf", "r") as f:
    lines = f.readlines()

# 提取所有以:开头的HEX行
hex_lines = [line.strip() for line in lines if line.strip().startswith(":")]

# 保存为临时HEX文件
with open("temp.hex", "w") as f:
    for line in hex_lines:
        f.write(line + "\n")

# 用intelhex解析
ih = IntelHex("temp.hex")

# 获取数据的起始和结束地址
start_addr = ih.minaddr()
end_addr = ih.maxaddr()

# 提取并合并所有数据为字节数组
data = ih.tobinarray(start=start_addr, end=end_addr)

# 打印合并后的数据长度
print(f"合并后数据长度: {len(data)}")

# 以HEX字符串输出所有数据（每16字节一行）
print("所有数据的HEX表示（每16字节一行）：")
for i in range(0, len(data), 16):
    line = data[i:i+16]
    print(' '.join(f"{b:02X}" for b in line))