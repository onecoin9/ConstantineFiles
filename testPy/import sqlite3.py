import sqlite3
import shutil
import os

def read_table_structure(db_name):
    conn = sqlite3.connect(db_name)
    cursor = conn.cursor()
    
    # 获取所有表名
    cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
    tables = cursor.fetchall()
    
    for table in tables:
        table_name = table[0]
        print(f"\n表名: {table_name}")
        
        # 获取表结构
        cursor.execute(f"PRAGMA table_info({table_name})")
        columns = cursor.fetchall()
        
        print("列结构:")
        for col in columns:
            print(f"列名: {col[1]}, 类型: {col[2]}, 是否可空: {not col[3]}, 是否主键: {col[5]}")
            
    cursor.close()
    conn.close()

def insert_sample_data(db_name):
    conn = sqlite3.connect(db_name)
    cursor = conn.cursor()
    
    # 创建用户表（如果不存在）
    cursor.execute('''
    CREATE TABLE IF NOT EXISTS users (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT NOT NULL,
        age INTEGER,
        email TEXT
    )
    ''')
    
    # 准备示例数据
    sample_users = [
        ('王小明', 22, 'xiaoming@example.com'),
        ('李娜', 28, 'lina@example.com'),
        ('张伟', 35, 'zhangwei@example.com'),
        ('刘静', 26, 'liujing@example.com'),
        ('陈强', 31, 'chenqiang@example.com')
    ]
    
    # 插入数据
    cursor.executemany('''
    INSERT INTO users (name, age, email) 
    VALUES (?, ?, ?)
    ''', sample_users)
    
    # 提交更改
    conn.commit()
    
    # 验证插入的数据
    cursor.execute('SELECT * FROM users')
    all_users = cursor.fetchall()
    
    print("\n已插入的用户数据:")
    for user in all_users:
        print(f"ID: {user[0]}, 姓名: {user[1]}, 年龄: {user[2]}, 邮箱: {user[3]}")
    
    cursor.close()
    conn.close()

if __name__ == '__main__':
    db_name = 'user.db'
    target_path = r'D:\qtProject\acroViewTester\sqlite'
    
    print("=== 数据库结构 ===")
    read_table_structure(db_name)
    
    print("\n=== 插入示例数据 ===")
    insert_sample_data(db_name)
    
    # 确保目标文件夹存在
    if not os.path.exists(target_path):
        os.makedirs(target_path)
    
    # 拷贝数据库文件
    target_file = os.path.join(target_path, db_name)
    try:
        shutil.copy2(db_name, target_file)
        print(f"\n数据库文件已成功拷贝到: {target_file}")
    except Exception as e:
        print(f"\n拷贝文件时发生错误: {str(e)}")