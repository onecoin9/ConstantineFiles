import re
from datetime import datetime

class LogParser:
    def __init__(self, file_path):
        self.file_path = file_path
        self.log_data = {}

    def read_file(self):
        # 尝试不同的编码方式读取文件
        encodings = ['utf-8', 'gbk', 'gb2312', 'ansi']
        content = None
        
        for encoding in encodings:
            try:
                with open(self.file_path, 'r', encoding=encoding) as f:
                    content = f.read()
                    print(f"成功使用 {encoding} 编码读取文件")
                    break
            except UnicodeDecodeError:
                continue
            except Exception as e:
                print(f"读取文件失败: {e}")
                return None
                
        if content is None:
            print("无法使用任何编码方式读取文件")
            return None
            
        return content

    def parse_log(self):
        content = self.read_file()
        if not content:
            return None

        # 初始化数据字典
        self.log_data = {
            'batch_start': '',
            'task_data': '',
            'tray_data': '',
            'adapter_data': '',
            'tape_data': '',
            'tube_data': '',
            'program_data': '',
            'target_count': 0,
            'batch_end': '',
            'total_count': 0,
            'pass_count': 0,
            'fail_count': 0,
            'remove_count': 0,
            'remove_before': 0,
            'remove_after': 0,
            'run_time': '',
            'alarm_count': 0,
            'alarm_time': '',
            'suspend_count': 0,
            'suspend_time': '',
            'fail_rate': '',
            'efficiency': '',
            'uph': '',
            'function_name': '',
            'checksum': '',
            'device_name': '',
            'file_info': '',
            'cycle_time': '',
            'items': [''] * 7,
            'lot_vid': '',
            'alarm_report': ''
        }

        # 解析每一行
        lines = content.split('\n')
        for line in lines:
            if not line.strip():
                continue

            # 使用正则表达式分割键值对
            parts = re.split(r':\t+', line.strip())
            if len(parts) < 2:
                continue

            key, value = parts[0], parts[1].strip()

            # 根据不同的键处理不同的值
            if '批量开始日志' in key:
                self.log_data['batch_start'] = value
            elif '任务数据' in key:
                self.log_data['task_data'] = value
            elif '托盘数据' in key:
                self.log_data['tray_data'] = value
            elif '适配数据' in key:
                self.log_data['adapter_data'] = value
            elif '卷带数据' in key:
                self.log_data['tape_data'] = value
            elif '管装数据' in key:
                self.log_data['tube_data'] = value
            elif '烧录数据' in key:
                self.log_data['program_data'] = value
            elif '目标数量' in key:
                self.log_data['target_count'] = int(value or 0)
            elif '供给数量' in key:
                self.log_data['total_count'] = int(value.replace('pcs', '').strip() or 0)
            elif '良品数量' in key:
                self.log_data['pass_count'] = int(value.replace('pcs', '').strip() or 0)
            elif '不良数量' in key:
                self.log_data['fail_count'] = int(value.replace('pcs', '').strip() or 0)
            elif 'Item' in key:
                index = int(key.replace('Item', '')) - 1
                self.log_data['items'][index] = value

        return self.log_data

    def print_data(self):
        """打印解析后的数据"""
        for key, value in self.log_data.items():
            print(f"{key}: {value}")

# 使用示例
if __name__ == "__main__":
    log_file = r"c:\IPS\testlog\20241211173154_1.log"
    parser = LogParser(log_file)
    data = parser.parse_log()
    parser.print_data()