package aqcxbom.rebuildproject25;

import android.app.Application;
import android.content.Context;

/**
 * Created by AqCxBoM on 2017/4/24.
 */

public class MyApplication extends Application {
    public MyApplication(){
        super();
    }
    public native void onCreate() ;
    protected native void attachBaseContext(Context ct);
}
