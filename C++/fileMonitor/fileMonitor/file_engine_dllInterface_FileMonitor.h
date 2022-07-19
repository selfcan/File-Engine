/* DO NOT EDIT THIS FILE - it is machine generated */
#include "jni.h"
/* Header for class file_engine_dllInterface_FileMonitor */

#ifndef _Included_file_engine_dllInterface_FileMonitor
#define _Included_file_engine_dllInterface_FileMonitor
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     file_engine_dllInterface_FileMonitor
 * Method:    monitor
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_file_engine_dllInterface_FileMonitor_monitor
  (JNIEnv *, jobject, jstring);

/*
 * Class:     file_engine_dllInterface_FileMonitor
 * Method:    stop_monitor
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_file_engine_dllInterface_FileMonitor_stop_1monitor
  (JNIEnv *, jobject);

/*
 * Class:     file_engine_dllInterface_FileMonitor
 * Method:    pop_add_file
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_file_engine_dllInterface_FileMonitor_pop_1add_1file
  (JNIEnv *, jobject);

/*
 * Class:     file_engine_dllInterface_FileMonitor
 * Method:    pop_del_file
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_file_engine_dllInterface_FileMonitor_pop_1del_1file
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
