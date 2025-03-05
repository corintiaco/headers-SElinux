#ifndef KERNEL_COMPAT_H
#define KERNEL_COMPAT_H

// Redefinir atributos y secciones que no son necesarios en espacio de usuario.
#ifndef __section
#  define __section(x)
#endif

#ifndef __init
#  define __init
#endif

#ifndef __initdata
#  define __initdata
#endif

#ifndef __initconst
#  define __initconst
#endif

#ifndef __exit
#  define __exit
#endif

#ifndef __cold
#  define __cold
#endif

#ifndef __latent_entropy
#  define __latent_entropy
#endif

#ifndef __noinitretpoline
#  define __noinitretpoline
#endif

#ifndef __cplusplus
#  include <stdbool.h>
#endif

// Redefinir rcu_dereference_protected para entorno de usuario.
#ifndef rcu_dereference_protected
#define rcu_dereference_protected(p, c) (p)
#endif

// Definir "current" para simular la tarea actual en espacio de usuario.
#ifndef current
struct task_struct {
    struct cred *cred;
    struct cred *real_cred;
    /* Agrega otros campos si es necesario para tus pruebas */
};
static struct task_struct current_task = {0};
#define current (&current_task)
#endif

// Macros para inicializar los tipos de credenciales.
#ifndef KUIDT_INIT
#define KUIDT_INIT(val) ((kuid_t){ (val) })
#endif

#ifndef KGIDT_INIT
#define KGIDT_INIT(val) ((kgid_t){ (val) })
#endif

// Si no estamos en modo REAL_KERNEL, definimos valores simulados.
#ifndef REAL_KERNEL

// Si el header ya declara 'selinux_path', definimos 'selinux_mnt' como alias:

#ifndef selinux_mnt
#define selinux_mnt selinux_path
#endif
static int has_selinux_config = 1;
#endif

#endif // KERNEL_COMPAT_H
