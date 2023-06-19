/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _MYSQL_CONN_H
#define _MYSQL_CONN_H

#include "mysql.h"
#include "mysqld_error.h"
#include <map>
#include <memory>
#include <vector>

//----------------------- row -----------------------
// 表示 MySQL 的一行记录
class MySQL_Row {
public:
    MySQL_Row();

    ~MySQL_Row();

    // 获取指定字段的值
    std::string operator[](int field_id);

    // 将指定字段的值转换为指定类型的数据
    template <typename T>
    T get_value(int field_id);

    // 将记录中的一个字段值添加到记录中
    void operator()(const std::string &value);

private:
    std::vector<std::string> m_Record;
};

//----------------------- row set -----------------------
// 查询结果集
class MySQL_Row_Set {
public:
    MySQL_Row_Set();

    ~MySQL_Row_Set();

    int field_count();

    int row_count();

    bool empty();

    // 获取指定行的对象
    MySQL_Row operator[](int row_id);

    // 将一行记录添加到结果集中
    void operator()(MySQL_Row &row);

    // 用于获取指定字段名对应的字段 ID
    int operator()(std::string field_name);

    void show_all();

    // 分别用于设置结果集中字段和行的数量
    void field_count(int count);
    void row_count(int count);

    // 设置指定字段 ID 对应的字段名
    void set_field_name(std::string field_name, int index);

private:
    // 获取指定字段名对应的字段 ID
    int field_id(std::string field_name);

private:
    int m_row;                               // 记录数量
    int m_field;                             // 字段数量
    std::vector<MySQL_Row> m_Record_Set;     // 结果集
    std::map<std::string, int> m_Field_Name; //
};

// mysql 连接
class mysql_conn : public RefCounter {
public:
    mysql_conn(int seq);

    ~mysql_conn();

    int connect_conn(const std::string &ip, unsigned int port,
                     const std::string &user, const std::string &pwd,
                     const std::string &db, const std::string &chars = "");

    // 执行查询语句
    int query_sql(const std::string sql, MySQL_Row_Set &row_set, std::string &errInfo);

    // 执行非查询语句
    int execute_sql(const std::string sql, unsigned long long &last_insert_id, unsigned long long &affected_rows, std::string &errInfo);

    // // 转义字符串
    unsigned long escape_string(char *to, const char *from, unsigned long length);

    void release_conn();

    // 通过 ping 对 mysql 长连接进行检查
    bool ping();

    // 设置是否自动提交
    int autocommit(bool open);
    int commit();
    int rollback();

public:
    MYSQL *_mysql;

    std::string _ip;
    unsigned int _port;
    std::string _user;
    std::string _pwd;
    std::string _db;
    std::string _chars;

    bool _conn;
    bool _used;
    int _seq;
};

typedef std::shared_ptr<mysql_conn> mysql_conn_Ptr; // 智能指针

#endif
