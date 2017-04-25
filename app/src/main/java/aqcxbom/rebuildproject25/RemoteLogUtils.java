package aqcxbom.rebuildproject25;

import java.io.IOException;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

import android.support.annotation.NonNull;
import android.util.Log;

import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.CoreConnectionPNames;

public class RemoteLogUtils
{
  public static boolean mIsStop = true;
  public static class WorkContext
  {
    int m_NType;
    HashMap<String, String> mMap;
  }
  private static BlockingQueue <WorkContext> mWorkQueue;

  public static void doWork()
  {
    new Thread(new Runnable()
    {
      @Override
      public void run()
      {
        try
        {
        while(!mIsStop)
        {
          //如果日志队列不为空
          if(!mWorkQueue.isEmpty() && mWorkQueue.size() > 0)
          {
            WorkContext workCt = mWorkQueue.peek();
            sendLog(workCt.m_NType, workCt.mMap);
            mWorkQueue.remove(workCt);
          }
          Thread.sleep(100);
        }
        }
        catch (Exception e)
        { e.printStackTrace(); }
      }
    }).start();
  }
  public static void sendLog(int ntype, HashMap<String, String> map)
  {
      StringBuilder sb = new StringBuilder();
      sb.append("from=0&android_id=" + SystemInfoUtils.androidid + "&imsi=" + SystemInfoUtils.imsi + "&imei=" + SystemInfoUtils.imei + "&package_name=" + SystemInfoUtils.packageName + "&apk_key=" + SystemInfoUtils.apkkey);
      sb.append("&reason=" + ntype + "&");
      String p1 = map.get("p1");
      if(!p1.isEmpty() && p1.length() != 0)
        sb.append("p1=" + p1);
      else
        sb.append("p1=");
      sb.append("&");
      String p2 = map.get("p2");
      if(!p2.isEmpty() && p2.length() != 0)
        sb.append("p2=" + p2);
      else
        sb.append("p2=");
      sb.append("&");
      String p3 = map.get("p3");
      if(!p3.isEmpty() && p3.length() != 0)
        sb.append("p3=" + p3);
      else
        sb.append("p3=");
      sb.append("&");
      long lTime = System.currentTimeMillis() / 1000;
      sb.append("timestamp=" + lTime);
      //Log.e(TAG, sb.toString());

      /*URL可以随意改*/
      String uriAPI;
      uriAPI = "http://cnsdk.zhuqueok.com/ab_api/plugin_log.php?";
      uriAPI += sb.toString();
      Log.e(TAG, uriAPI);
      /*建立HTTP Get对象*/
      HttpGet httpRequest = new HttpGet(uriAPI);
      try
      {
        /*发送请求并等待响应*/
        HttpClient hc = new DefaultHttpClient();
        hc.getParams().setParameter(CoreConnectionPNames.CONNECTION_TIMEOUT, 1000);
        HttpResponse httpResponse = hc.execute(httpRequest);
        /*若状态码为200 ok*/
        if(httpResponse.getStatusLine().getStatusCode() == 200)
        {
          /*读*/
          Log.d(TAG, "LogUpdata sussess");
        }
      }
      catch (ClientProtocolException e)
      {
        e.printStackTrace();
      }
      catch (IOException e)
      {
        e.printStackTrace();
      }
      catch (Exception e)
      {
        e.printStackTrace();
      }
  }
  public static void doLog(int ntype, HashMap<String, String> map)
  {
    if(mIsStop == true)
    {
      mWorkQueue = new ArrayBlockingQueue(128);
      mIsStop = false;
      doWork();
    }
    WorkContext wc = new WorkContext();
    wc.m_NType = ntype;
    wc.mMap = map;
    mWorkQueue.add(wc);
  }
  private static String TAG = "RemoteLogUtils";
}
