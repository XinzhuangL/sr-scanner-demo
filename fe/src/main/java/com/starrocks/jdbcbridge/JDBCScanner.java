package com.starrocks.jdbcbridge;

import com.zaxxer.hikari.HikariConfig;
import com.zaxxer.hikari.HikariDataSource;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.lang.reflect.Array;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.ResultSetMetaData;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.List;

public class JDBCScanner {

    private static final Logger logger = LogManager.getLogger(JDBCScanner.class);

    private String driverLocation;

    private HikariDataSource dataSource;

    private JDBCScanContext scanContext;

    private Connection connection;

    private Statement statement;

    private ResultSet resultSet;

    private ResultSetMetaData resultSetMetaData;

    private List<String> resultColumnClassNames;

    private List<Object[]> resultChunk;

    private int resultNumRows = 0;

    public JDBCScanner(String driverLocation, JDBCScanContext scanContext) {
        this.driverLocation = driverLocation;
        this.scanContext = scanContext;
    }

    public void open() throws Exception {
        logger.info("this is open start");

        try {
            String key = scanContext.getUser() + "/" + scanContext.getJdbcURL();
            logger.info("get key");

            dataSource = DataSourceCache.getInstance().getSource(key, () -> {
                HikariConfig config = new HikariConfig();
                config.setDriverClassName(scanContext.getDriverClassName());
                config.setJdbcUrl(scanContext.getJdbcURL());
                config.setUsername(scanContext.getUser());
                config.setPassword(scanContext.getPassword());
                config.setMaximumPoolSize(scanContext.getConnectionPoolSize());
                config.setIdleTimeout(scanContext.getConnectionIdleTimeoutMs());
                dataSource = new HikariDataSource(config);
                return dataSource;
            });
            logger.info("create datasource");
            connection = dataSource.getConnection();
            statement = connection.createStatement();
            statement.setFetchSize(scanContext.getStatementFetchSize());
            logger.info("pre exec sql: {}", scanContext.getSql());
            statement.execute(scanContext.getSql());
            logger.info("exec sql: {}", scanContext.getSql());
            resultSet = statement.getResultSet();
            resultSetMetaData = resultSet.getMetaData();
            resultColumnClassNames = new ArrayList<>(resultSetMetaData.getColumnCount());
            resultChunk = new ArrayList<>(resultSetMetaData.getColumnCount());
            for (int i = 1; i <= resultSetMetaData.getColumnCount(); i++) {
                resultColumnClassNames.add(resultSetMetaData.getColumnClassName(i));
                Class<?> clazz = Class.forName(resultSetMetaData.getColumnClassName(i));
                resultChunk.add((Object[]) Array.newInstance(clazz, scanContext.getStatementFetchSize()));
            }
        } catch (Exception e) {
            logger.error("exec sql cache exception: {}", e.getMessage());
            throw e;
        }
        logger.info("this is open finish");
    }

    public List<String> getResultColumnClassNames() {
        return resultColumnClassNames;
    }

    public boolean hasNext() throws SQLException {
        return resultSet.next();
    }

    public List<Object[]> getNextChunk() throws Exception {

        int chunkSize = scanContext.getStatementFetchSize();
        int columnCount = resultSetMetaData.getColumnCount();
        resultNumRows = 0;
        do {
            for (int i = 0; i < columnCount; i++) {
                Object[] dataColumn = resultChunk.get(i);
                Object resultObject = resultSet.getObject(i + 1);
                // in some cases, the real java class type of result is not consistent with the type from
                // resultSetMetadata,
                // for example, FLOAT type in oracle gives java.langDouble type in resultSetMetaData,
                // but the result type is BigDecimal when we getObject from resultSet.
                // so we choose to convert the value to the target type here.
                // dataColumn是从 resultSetMetaData中获取的
                // resultObject是真实的数据 从resultSet中获取
                // 将结果转换的与resultSetMetaData一致
                if (resultObject == null) {
                    dataColumn[resultNumRows] = null;
                } else if (dataColumn instanceof Short[]) {
                    dataColumn[resultNumRows] = ((Number) resultObject).shortValue();
                } else if (dataColumn instanceof Integer[]) {
                    dataColumn[resultNumRows] = ((Number) resultObject).intValue();
                } else if (dataColumn instanceof Long[]) {
                    dataColumn[resultNumRows] = ((Number) resultObject).longValue();
                } else if (dataColumn instanceof Float[]) {
                    dataColumn[resultNumRows] = ((Number) resultObject).floatValue();
                } else if (dataColumn instanceof Double[]) {
                    dataColumn[resultNumRows] = ((Number) resultObject).doubleValue();
                } else {
                    dataColumn[resultNumRows] = resultObject;
                }
            }
            resultNumRows++;
        } while (resultNumRows < chunkSize && resultSet.next());
        return resultChunk;
    }

    public int getResultNumRows() {
        return resultNumRows;
    }

    public void close() throws SQLException {
        if (resultSet != null) {
            resultSet.close();
        }
        if (statement != null) {
            statement.close();
        }
        if (connection != null) {
            connection.close();
        }
    }

}
