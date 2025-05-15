import struct
import base64

def calculate_crc(data, polynomial=0x04C11DB7):
    """计算数据的 CRC-32/MPEG-2 校验值"""
    crc = 0xFFFFFFFF  # 初始值

    for byte in data:
        crc ^= byte << 24  # 将当前字节移到最高位
        for _ in range(8):  # 处理每一位
            if crc & 0x80000000:  # 检查最高位是否为 1
                crc = (crc << 1) ^ polynomial
            else:
                crc <<= 1
            crc &= 0xFFFFFFFF  # 保持 CRC 为 32 位
    return crc  # 不取反

def base64_decode(encoded_string):
    """解码 Base64 字符串"""
    try:
        decoded_data = base64.b64decode(encoded_string)
        print("Base64 解码后的数据 (Hex):", decoded_data.hex())  # 打印解码后的数据
        print("Base64 解码后的数据 (Bytes):", list(decoded_data))  # 打印字节列表
        return decoded_data
    except Exception as e:
        print(f"Base64 解码失败: {e}")
        return None

def parse_data(data):
    """解析二进制数据"""
    # 解析标识符 (4 字节，char[4])
    identifier = data[:4].decode('ascii')
    
    # 解析套件版本 (4 字节，Integer，大端)
    suite_version = struct.unpack('>I', data[4:8])[0]
    
    # 解析保留区域 (7 字节，Byte[7])
    reserved = data[8:15]
    
    # 解析密钥类型 (1 字节，Byte)
    key_type = data[15]
    
    # 解析加密的密钥长度 (4 字节，Integer，大端)
    encrypted_key_length = struct.unpack('>I', data[16:20])[0]
    
    # 解析 W-UFPK (36 字节，Byte[36])
    w_ufpk = data[20:56]
    shared_key_number = w_ufpk[:4]
    w_ufpk_value = w_ufpk[4:]
    
    # 解析初始向量 (16 字节，Byte[16])
    initial_vector = data[56:72]
    
    # 解析加密的密钥 (N 字节，Byte[N])
    encrypted_key = data[72:72 + encrypted_key_length]
    
    # 解析数据 CRC (4 字节，Byte[4])
    crc = struct.unpack('>I', data[72 + encrypted_key_length:72 + encrypted_key_length + 4])[0]
    
    # 计算除 CRC 字段外的所有数据的 CRC
    data_without_crc = data[:-4]  # 去掉最后 4 个字节
    print("去掉最后 4 个字节的数据 (Hex):", data_without_crc.hex())  # 打印数据

    # 使用 CRC-32/MPEG-2 计算 CRC
    calculated_crc = calculate_crc(data_without_crc)
    print("CRC-32/MPEG-2 计算的 CRC (Calculated CRC):", hex(calculated_crc))

    # 打印解析结果
    print("标识符 (Identifier):", identifier)
    print("套件版本 (Suite Version):", suite_version)
    print("保留区域 (Reserved):", reserved.hex())
    print("密钥类型 (Key Type):", key_type)
    print("加密的密钥长度 (Encrypted Key Length):", encrypted_key_length)
    print("W-UFPK (Shared Key Number):", shared_key_number.hex())
    print("W-UFPK (WUFPK Value):", w_ufpk_value.hex())
    print("初始向量 (Initial Vector):", initial_vector.hex())
    print("加密的密钥 (Encrypted Key):", encrypted_key.hex())
    print("数据 CRC (CRC from data):", hex(crc))
    print("计算的 CRC (Calculated CRC):", hex(calculated_crc))

    # 校验 CRC
    if calculated_crc == crc:
        print("CRC 校验通过")
    else:
        print("CRC 校验失败")

if __name__ == "__main__":
    # 输入 Base64 编码字符串
    base64_input = "UkVLMQAAAAEAAAAAAAAAAAAAACAAAAAAUZpL+laVBsiRkvlyeTh/wMAD+QN7wU5U7CAsrDEF840SNFZ4EjRWeBI0VngSNFZ4XRmOa3ptjYgdMGw4w7c84F4Rg5e4JNVcYjH2s3Q7jX+lqNPX"
    
    # 解码 Base64
    binary_data = base64_decode(base64_input)
    if binary_data is None:
        print("Base64 解码失败，无法继续解析。")
    else:
        # 调用解析函数
        parse_data(binary_data)