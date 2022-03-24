package file.engine.dllInterface;

import java.nio.file.Path;

public enum FileMonitor {
    INSTANCE;

    static {
        System.load(Path.of("user/fileMonitor.dll").toAbsolutePath().toString());
    }

    public native void monitor(String path);

    public native void stop_monitor();

    public native void set_output(String path);
}
