import sqlite3
import os

def open_and_inspect_db(db_filepath):
    """
    连接到指定的 SQLite 数据库文件，列出表名，并打印每个表的内容。

    Args:
        db_filepath (str): SQLite 数据库文件的路径。
    """
    if not os.path.exists(db_filepath):
        print(f"错误: 数据库文件 '{db_filepath}' 不存在。")
        return

    try:
        conn = sqlite3.connect(db_filepath)
        cursor = conn.cursor()
        print(f"成功连接到数据库: {db_filepath}")

        # 获取所有表名
        cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
        tables = cursor.fetchall()

        if not tables:
            print("\n数据库中没有找到表。")
            return

        print("\n数据库中的表及其内容:")
        for table_info in tables:
            table_name = table_info[0]
            print(f"\n--- 表: {table_name} ---")

            # 获取表结构以打印列名
            cursor.execute(f"PRAGMA table_info({table_name})")
            columns_info = cursor.fetchall()
            column_names = [col[1] for col in columns_info]
            
            if not column_names:
                print("无法获取列信息。")
                continue
            
            print("列名:", ", ".join(column_names))
            print("-" * (len(table_name) + 10)) # 分隔线

            # 获取并打印表中的所有数据
            cursor.execute(f"SELECT * FROM {table_name}")
            rows = cursor.fetchall()

            if rows:
                for row_index, row in enumerate(rows):
                    print(f"行 {row_index + 1}: {row}")
            else:
                print("表中没有数据。")

    except sqlite3.Error as e:
        print(f"连接或操作数据库时发生错误: {e}")
    finally:
        if 'conn' in locals() and conn:
            conn.close()
            print("\n数据库连接已关闭。")

if __name__ == '__main__':
    database_file = 'acro_tester_logs.db'
    
    # 脚本现在会在函数内部检查文件是否存在
    open_and_inspect_db(database_file)