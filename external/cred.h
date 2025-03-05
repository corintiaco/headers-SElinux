/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Versión simplificada de linux/cred.h para entornos de usuario/simulación.
 */

#ifndef _LINUX_CRED_H
#define _LINUX_CRED_H

#include "kernel_compat.h" // Nuestras redefiniciones de macros y funciones de compatibilidad.
#include "linux/uidgid.h"   // Aquí debes definir o incluir tipos mínimos para kuid_t y kgid_t.

/* Definiciones mínimas para los tipos necesarios */

typedef struct {
    unsigned int cap[2];
} kernel_cap_t;

/* En este ejemplo, definimos kuid_t y kgid_t como estructuras simples.
 * En un entorno real pueden ser más complejos, pero esto sirve para fines de compilación.
 */
typedef struct { int val; } kuid_t;
typedef struct { int val; } kgid_t;

/* Tipo mínimo para un contador atómico */
typedef volatile long atomic_long_t;

/* Tipo mínimo para un contador de referencias */
typedef volatile int refcount_t;

/* Estructura para la lista de grupos suplementarios (simplificada) */
struct group_info {
    refcount_t usage;
    int ngroups;
    /* Para simplificar, usamos un arreglo de tamaño 1.
       En una versión real se usaría un arreglo flexible. */
    kgid_t gid[1];
};

/* Estructura de credenciales (versión simplificada) */
struct cred {
    atomic_long_t usage;
    kuid_t uid;   /* UID real */
    kgid_t gid;   /* GID real */
    kuid_t suid;  /* UID guardado */
    kgid_t sgid;  /* GID guardado */
    kuid_t euid;  /* UID efectivo */
    kgid_t egid;  /* GID efectivo */
    kuid_t fsuid; /* UID para operaciones VFS */
    kgid_t fsgid; /* GID para operaciones VFS */
    unsigned securebits;
    kernel_cap_t cap_inheritable; /* capacidades heredables */
    kernel_cap_t cap_permitted;   /* capacidades permitidas */
    kernel_cap_t cap_effective;   /* capacidades efectivas */
    kernel_cap_t cap_bset;        /* capacidad límite */
    kernel_cap_t cap_ambient;     /* conjunto de capacidades ambient */
    struct group_info *group_info;/* grupos suplementarios */
    /* Omite otros campos (como keyrings o security) para simplificar */
};

/* Funciones en línea mínimas para manejo de referencias en group_info */
static inline struct group_info *get_group_info(struct group_info *gi)
{
    gi->usage++;
    return gi;
}

#define put_group_info(gi) \
    do { if (--((gi)->usage) == 0) { /* liberar memoria si fuera necesario */ } } while (0)

/* Funciones mínimas para manejo de referencias en cred */

static inline struct cred *get_new_cred_many(struct cred *cred, int nr)
{
    cred->usage += nr;
    return cred;
}

static inline struct cred *get_new_cred(struct cred *cred)
{
    return get_new_cred_many(cred, 1);
}

static inline const struct cred *get_cred_many(const struct cred *cred, int nr)
{
    struct cred *nonconst_cred = (struct cred *)cred;
    if (!cred)
        return cred;
    nonconst_cred->usage += nr;
    return cred;
}

static inline const struct cred *get_cred(const struct cred *cred)
{
    return get_cred_many(cred, 1);
}

static inline void put_cred_many(const struct cred *cred, int nr)
{
    struct cred *nonconst_cred = (struct cred *)cred;
    if (nonconst_cred) {
        nonconst_cred->usage -= nr;
        if (nonconst_cred->usage <= 0) {
            /* Aquí se liberaría la memoria, si fuera necesario */
        }
    }
}

static inline void put_cred(const struct cred *cred)
{
    put_cred_many(cred, 1);
}

/* Definiciones para acceder a las credenciales actuales.
 * Se espera que 'current' ya esté definido en kernel_compat.h (simulado).
 */
#define current_cred() \
    (rcu_dereference_protected(current->cred, 1))
#define get_current_cred() (get_cred(current_cred()))

#endif /* _LINUX_CRED_H */
