package com.starrocks.jdbcbridge;

public class Main {
    public static void main(String[] args) throws Exception {
        JDBCScanner jdbcScanner = new JDBCScanner("", new JDBCScanContext());
        jdbcScanner.open();

    }
}
