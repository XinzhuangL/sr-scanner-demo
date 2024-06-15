package com.starrocks.jdbcbridge;

import java.io.File;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;

public class JDBCBridge {

    private String driverLocation = null;

    public void setClassLoader(String driverLocation) throws MalformedURLException {
        URLClassLoader loader = URLClassLoader.newInstance(new URL[] {
                new File(driverLocation).toURI().toURL()
        });
        Thread.currentThread().setContextClassLoader(loader);
        this.driverLocation = driverLocation;
    }

    public JDBCScanner getScanner(JDBCScanContext scanContext) {
        return new JDBCScanner(this.driverLocation, scanContext);
    }

}
