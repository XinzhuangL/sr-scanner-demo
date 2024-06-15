package com.starrocks.jdbcbridge;

import com.zaxxer.hikari.HikariDataSource;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.Supplier;

public class DataSourceCache {

    private final Map<String, HikariDataSource> sources = new ConcurrentHashMap<>();

    private static final DataSourceCache INSTANCE = new DataSourceCache();

    public static DataSourceCache getInstance() {
        return INSTANCE;
    }

    public HikariDataSource getSource(String driverId, Supplier<HikariDataSource> provider) {
        return sources.computeIfAbsent(driverId, k -> provider.get());
    }
}
