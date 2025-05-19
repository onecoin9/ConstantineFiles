import os
import shutil

def copy_db_files_to_current_directory(source_logs_dir):
    """
    将源日志目录中的所有 .db 文件拷贝到当前脚本运行的目录下。

    Args:
        source_logs_dir (str): 包含 .db 文件的源日志目录路径。
    """
    # 获取当前脚本所在的目录作为目标目录
    destination_dir = os.getcwd() 
    
    print(f"源目录: {source_logs_dir}")
    print(f"目标目录: {destination_dir}")

    if not os.path.exists(source_logs_dir):
        print(f"错误: 源目录 '{source_logs_dir}' 不存在。")
        return

    copied_files_count = 0
    for filename in os.listdir(source_logs_dir):
        if filename.endswith(".db"):
            source_file_path = os.path.join(source_logs_dir, filename)
            destination_file_path = os.path.join(destination_dir, filename)
            
            try:
                shutil.copy2(source_file_path, destination_file_path)
                print(f"已拷贝: '{source_file_path}' 到 '{destination_file_path}'")
                copied_files_count += 1
            except Exception as e:
                print(f"拷贝文件 '{source_file_path}' 时发生错误: {e}")
    
    if copied_files_count == 0:
        print(f"在源目录 '{source_logs_dir}' 中没有找到 .db 文件。")
    else:
        print(f"\n成功拷贝 {copied_files_count} 个 .db 文件。")

if __name__ == '__main__':
    source_directory = r'D:\qtProject\acroViewTester\acroViewTester\acroViewTester\logs'
    copy_db_files_to_current_directory(source_directory)