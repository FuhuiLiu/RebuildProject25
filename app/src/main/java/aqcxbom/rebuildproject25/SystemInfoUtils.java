package aqcxbom.rebuildproject25;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;

public class SystemInfoUtils
{
  private static Context mContext;
  public static String packageName;
  public static String imei;
  public static String imsi;
  public static String androidid;
  public static String apkkey;

  public static void init(Context ct)
  {
    mContext = ct.getApplicationContext();
    packageName = ct.getPackageName();
    imei = getIMEI();
    imsi = getIMSI();
    androidid = getAndroidID();
    apkkey = getApkKey();
  }
  public static String getVM_Version()
  {
      String vmVersion = System.getProperty("java.vm.version");
      return vmVersion;
  }
  /**
   * 获取当前系统运行模式
   * @return 模式字符串
   */
  public static String getCurrentRuntimeValue()
  {
      String filePath = "/system/build.prop";// 系统内存信息文件
      String strRead;
      String[] arrayOfString;
      try {
          //读取/system/build.prop文件内容
          FileReader localFileReader = new FileReader(filePath);
          BufferedReader localBufferedReader = new BufferedReader(
                  localFileReader, 8192);
          do {
              //挨行判断是否包含字符串persist.sys.dalvik.vm.lib
              strRead = localBufferedReader.readLine();// 读取meminfo第一行，系统总内存大小
              if (strRead.contains("persist.sys.dalvik.vm.lib"))
                  break;
          }while(true);
          //"="号分割
          arrayOfString = strRead.split("=");
          localBufferedReader.close();
          //等号右边就是当前系统加载SO
          if(arrayOfString[1].equals("libart.so"))
            return "ART";
          else if(arrayOfString[1].equals("libdvm.so"))
            return "DALVIK";
      } catch (IOException e) {
      }
      return "未知";
  }
  public static Context getContext()
  {
    return mContext;
  }
  //获取手机SIM卡的序列号
  public static String getApkKey() {
    String gamecode = null;
    try {
        PackageManager pm = mContext.getPackageManager();
        ApplicationInfo applicationInfo = pm.getApplicationInfo(
            mContext.getPackageName(), PackageManager.GET_META_DATA);
        if (applicationInfo.metaData != null) {
            gamecode = applicationInfo.metaData.getString("gamecode");
            
        }
    } catch (PackageManager.NameNotFoundException e) {
        e.printStackTrace();
    }
    return gamecode;
  }
//获取手机SIM卡的序列号
  public static String getSimSerialNumber() {
      String v1 = "";
      try {
          TelephonyManager tm = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);
          v1 = tm.getSimSerialNumber();
      }
      catch(Exception v0) {
      }
      return v1;
  }
  //手机串号:GSM手机的 IMEI 和 CDMA手机的 MEID.
  public static String getAndroidID() {
      String android_id = "";
      try {
          android_id = android.provider.Settings.Secure.getString(mContext.getContentResolver(), "android_id");
      }
      catch(Exception v0) {
      }
      return android_id;
  }
  //手机串号:GSM手机的 IMEI 和 CDMA手机的 MEID.
  public static String getIMEI() {
      String v1 = "";
      try {
          TelephonyManager tm = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);
          v1 = tm.getDeviceId();
      }
      catch(Exception v0) {
      }
      return v1;
  }
  //获取客户id，在gsm中是imsi号
  public static String getIMSI() {
      String SubId = "";
      int v5 = 15;
      try {
          TelephonyManager tm = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);
          //获取客户id，在gsm中是imsi号
          SubId = tm.getSubscriberId();

          if(!TextUtils.isEmpty(SubId) && (SubId.startsWith("460"))) {
              return SubId;
          }
          //如果 上面的返回不足于代表手机卡的运营商状态
          //运营商名称,注意：仅当用户已在网络注册时有效,在CDMA网络中结果也许不可靠
          SubId = getNetworkOperator();
          if((TextUtils.isEmpty(SubId)) || !SubId.startsWith("460")) {
              SubId = "46000";
          }

          SubId = SubId + randomImsi();
          if(SubId.length() < v5) {
              SubId = SubId + "000000000000000";
          }

          if(SubId.length() > v5) {
              SubId = SubId.substring(0, 15);
          }

      }
      catch(Exception v0) {
      }
      return SubId;
  }
  //运营商名称,注意：仅当用户已在网络注册时有效,在CDMA网络中结果也许不可靠
  public static String getNetworkOperator() {
      String v1;
      try {
          TelephonyManager tm = (TelephonyManager)mContext.getSystemService(Context.TELEPHONY_SERVICE);
          v1 = tm.getNetworkOperator();
      }
      catch(Exception v0) {
          v1 = "";
      }

      if(v1 == null) {
          v1 = "";
      }

      return v1;
  }
  private static String randomImsi() {
    try {
        //手机串号:GSM手机的 IMEI 和 CDMA手机的 MEID.
        String v3 = getIMEI();
        StringBuffer v0 = new StringBuffer();
        int v2;
        for(v2 = 0; v2 < v3.length(); ++v2) {
            char v1 = v3.charAt(v2);
            if(!Character.isDigit(v1)) {
                v0.append("0");
            }
            else {
                v0.append(v1);
            }
        }

        String v4_1 = v0.toString();
        return v4_1;
    }
    catch(Exception v4) {
        return "2561158629";
    }
}
}
